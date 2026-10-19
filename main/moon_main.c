#include "moon_model.h"
#include "moon_battery_filter.h"
#include "moon_wake_guard.h"
#include "bsp_audio.h"
#include "bsp_i2c.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "moon_ui.h"
#include "moon_audio.h"
#include "bsp_display.h"
#include "bsp_button.h"
#include "bsp_battery.h"
#include "bsp_pins.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdatomic.h>

static const char *TAG="moon_bowl";
static QueueHandle_t button_queue,save_queue;
static atomic_bool button_overflow,storage_ok;
static atomic_int battery_soc=MOON_BATTERY_READING;
static bool nvs_ready;
static atomic_bool storage_stop,storage_stopped;
typedef struct {moon_key_t key;moon_event_t event;} button_message_t;
static uint64_t millis(void) {return (uint64_t)esp_timer_get_time()/1000;}
static void button_callback(bsp_btn_t key,bsp_btn_ev_t event,void *user) {
    (void)user;
    button_message_t msg;
    switch(key) {case BSP_BTN_UP:msg.key=MOON_UP;break;case BSP_BTN_DOWN:msg.key=MOON_DOWN;break;case BSP_BTN_OK:msg.key=MOON_OK;break;default:return;}
    switch(event) {
        case BSP_BTN_PRESS:msg.event=MOON_PRESS;break;
        case BSP_BTN_CLICK:msg.event=MOON_CLICK;break;
        case BSP_BTN_DOUBLE:msg.event=MOON_DOUBLE;break;
        case BSP_BTN_LONG:msg.event=MOON_LONG;break;
        case BSP_BTN_RELEASE:msg.event=MOON_RELEASE;break;
        default:return;
    }
    if(xQueueSend(button_queue,&msg,0)!=pdTRUE) atomic_store(&button_overflow,true);
}
static void storage_task(void *arg) {
    (void)arg;
    bool battery_available=bsp_battery_init()==ESP_OK;
    uint64_t next_battery=battery_available?0:millis()+30000;
    if(!battery_available) atomic_store(&battery_soc,MOON_BATTERY_UNKNOWN);
    moon_battery_filter_t battery_filter;
    moon_battery_filter_init(&battery_filter,millis());
    moon_preferences_t pending;
    bool dirty=false; uint64_t save_at=0;
    for(;;) {
        if(atomic_load(&storage_stop) && !dirty) {atomic_store(&storage_stopped,true);vTaskDelete(NULL);return;}
        moon_preferences_t next;
        if(xQueueReceive(save_queue,&next,pdMS_TO_TICKS(100))==pdTRUE) {
            pending=next;dirty=true;save_at=millis()+700;
        }
        uint64_t now=millis();
        if(dirty && now>=save_at) {
            nvs_handle_t handle;
            esp_err_t err=nvs_ready?nvs_open("moon_bowl",NVS_READWRITE,&handle):ESP_FAIL;
            if(err==ESP_OK) {
                err=nvs_set_blob(handle,"preferences",&pending,sizeof(pending));
                if(err==ESP_OK) err=nvs_commit(handle);
                nvs_close(handle);
            }
            atomic_store(&storage_ok,err==ESP_OK);
            if(err==ESP_OK) dirty=false;
            else {save_at=now+5000;ESP_LOGW(TAG,"Preferences not saved: %s",esp_err_to_name(err));}
        }
        if(now>=next_battery) {
            if(!battery_available) {battery_available=bsp_battery_init()==ESP_OK;now=millis();}
            int soc=battery_available?bsp_battery_soc():-1;
            int mv=battery_available?bsp_battery_mv():-1;
            int display=moon_battery_filter_sample(&battery_filter,soc,mv,now);
            atomic_store(&battery_soc,display);
            ESP_LOGI(TAG,"Battery raw=%d%% voltage=%dmV display=%d",soc,mv,display);
            next_battery=millis()+(display>=0 || !battery_available?30000:1000);
        }
    }
}
/* Terminal handoff: all bus users must acknowledge stopping before peripherals sleep. */
static void enter_sleep(void) {
    atomic_store(&storage_stop,true);
    for(unsigned i=0;i<200 && !atomic_load(&storage_stopped);i++) vTaskDelay(pdMS_TO_TICKS(10));
    if(!atomic_load(&storage_stopped) || !moon_audio_stop(2000)) {
        ESP_LOGE(TAG,"Sleep worker stop failed; restarting safely");esp_restart();
    }
    int level=0;
    if(bsp_button_prepare_deep_sleep(&level)!=ESP_OK ||
       esp_deep_sleep_enable_gpio_wakeup(1ULL<<BSP_BTN_GPIO,ESP_GPIO_WAKEUP_GPIO_LOW)!=ESP_OK) {
        ESP_LOGE(TAG,"Sleep wake setup failed; restarting");esp_restart();
    }
    /* A late key press will wake immediately; startup consumes that gesture. */
    /* Keep CW2017 measuring across MCU sleep: restarting it discards SOC estimation state. */
    esp_err_t audio=bsp_audio_sleep();
    esp_err_t pins=bsp_audio_prepare_deep_sleep();
    esp_err_t bus=bsp_i2c_prepare_deep_sleep();
    if(audio!=ESP_OK || pins!=ESP_OK || bus!=ESP_OK) {
        ESP_LOGE(TAG,"Peripheral sleep failed; restarting");esp_restart();
    }
    if(!bsp_lvgl_lock(1000)) esp_restart();
    if(bsp_display_prepare_deep_sleep()!=ESP_OK) esp_restart();
    ESP_LOGI(TAG,"Entering automatic deep sleep");
    esp_deep_sleep_start();
    esp_restart();
}
void app_main(void) {
    ESP_LOGI(TAG,"电子颂钵疗愈 v0.1.8");
    moon_preferences_t prefs=moon_defaults();
    esp_err_t err=nvs_flash_init();
    /* Never erase another app's NVS to recover space/version errors. */
    nvs_ready=err==ESP_OK; atomic_store(&storage_ok,nvs_ready);
    if(nvs_ready) {
        nvs_handle_t handle;
        if(nvs_open("moon_bowl",NVS_READONLY,&handle)==ESP_OK) {
            moon_preferences_t loaded;size_t size=sizeof(loaded);
            if(nvs_get_blob(handle,"preferences",&loaded,&size)==ESP_OK)
                (void)moon_preferences_decode(&prefs,&loaded,size);
            nvs_close(handle);
        }
    }
    button_queue=xQueueCreate(24,sizeof(button_message_t));
    save_queue=xQueueCreate(1,sizeof(moon_preferences_t));
    if(!button_queue || !save_queue) {ESP_LOGE(TAG,"Queue allocation failed");return;}
    if(bsp_display_init()!=ESP_OK || !bsp_lvgl_init()) {ESP_LOGE(TAG,"Display unavailable");return;}
    if(!bsp_lvgl_lock(1000)) {ESP_LOGE(TAG,"Display lock failed");return;}
    moon_ui_create();bsp_lvgl_unlock();
    bsp_display_backlight(65);
    moon_model_t model;moon_init(&model,&prefs,millis());
    ESP_LOGI(TAG,"Preferences note=%u volume=%u",(unsigned)prefs.note,(unsigned)prefs.volume);
    moon_audio_volume(prefs.volume);
    if(!moon_audio_start()) ESP_LOGE(TAG,"Audio task unavailable");
    if(xTaskCreate(storage_task,"moon_storage",4096,NULL,2,NULL)!=pdPASS) {atomic_store(&storage_ok,false);atomic_store(&storage_stopped,true);}
    err=bsp_button_init(button_callback,NULL);
    if(err!=ESP_OK) {ESP_LOGE(TAG,"Buttons unavailable: %s",esp_err_to_name(err));bsp_display_backlight(15);return;}
    uint64_t last_ui=0;unsigned brightness=65;
    uint64_t fade_start=0;unsigned fade_brightness=0;
    moon_wake_guard_t guard={0};
    /* Also protect power-on/recovery with a held key. */
    moon_wake_guard_arm(&guard);
    for(;;) {
        button_message_t msg;
        bool event=xQueueReceive(button_queue,&msg,pdMS_TO_TICKS(20))==pdTRUE;
        uint64_t now=millis();
        int key_mv=bsp_button_read_mv();
        bool released=key_mv>=BSP_BTN_MV_RELEASED_MIN;
        if(fade_start && (event || !released)) {
            fade_start=0;model.activity_ms=now;model.changed=true;
            moon_wake_guard_arm(&guard);
        }
        bool guarded=moon_wake_guard_poll(&guard,released,now);
        if(guarded) {xQueueReset(button_queue);model.activity_ms=now;}
        else if(event) moon_input(&model,msg.key,msg.event,now);
        if(!released) model.activity_ms=now;
        if(atomic_exchange(&button_overflow,false)) {moon_cancel_repeat(&model);xQueueReset(button_queue);}
        if(model.repeat_key>=0) {
            int mv=bsp_button_read_mv();
            if(mv<0 || mv>=BSP_BTN_MV_RELEASED_MIN) moon_cancel_repeat(&model);
        }
        moon_tick(&model,now);
        if(model.strike) {(void)moon_audio_strike(model.prefs.note,model.prefs.volume);model.strike=false;}
        if(model.save) {
            moon_audio_volume(model.prefs.volume);
            xQueueOverwrite(save_queue,&model.prefs);model.save=false;
        }
        unsigned target=now-model.activity_ms>(model.automatic?20000U:60000U)?18:65;
        if(!fade_start && !guard.active && released && atomic_load(&storage_ok) && moon_sleep_due(&model,now)) {
            fade_start=now;fade_brightness=brightness;
        }
        if(fade_start) {
            uint64_t elapsed=now-fade_start;
            target=elapsed>=1500?0:(unsigned)(fade_brightness*(1500-elapsed)/1500);
            if(brightness!=target) {brightness=target;bsp_display_backlight(brightness);}
            if(elapsed>=1500) enter_sleep();
        } else if(brightness!=target) {brightness=brightness<target?brightness+1:brightness-1;bsp_display_backlight(brightness);}
        if(model.changed || now-last_ui>=100) {
            if(bsp_lvgl_lock(30)) {
                moon_ui_update(&model,now,atomic_load(&battery_soc),moon_audio_ready(),atomic_load(&storage_ok));
                bsp_lvgl_unlock();model.changed=false;last_ui=now;
            }
        }
    }
}
