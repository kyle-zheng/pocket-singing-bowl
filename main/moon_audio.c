#include "moon_audio.h"
#include "moon_mix.h"
#include "moon_volume.h"
#include "esp_log.h"
#include "bsp_audio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdatomic.h>
#include <string.h>

extern const uint8_t moon_pcm_start[] asm("_binary_moon_bowls_pcm_start");
#define FRAMES 128000U
#define VOICES 4
#define CHUNK MOON_MIX_FRAMES
static QueueHandle_t commands;
static atomic_bool ready,stop_requested,stopped;
static atomic_uint requested_volume;
typedef struct { unsigned note; } command_t;
typedef struct { unsigned note, position, old_note, old_position, fade; bool active; } voice_t;
static int16_t sample(unsigned note, unsigned pos) {
    const uint8_t *p=moon_pcm_start+2*(note*FRAMES+pos);
    return (int16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8));
}
static void audio_task(void *arg) {
    (void)arg;
    if(bsp_audio_init()!=ESP_OK || bsp_audio_set_format(16000,16,1)!=ESP_OK) {
        atomic_store(&ready,false); atomic_store(&stopped,true); vTaskDelete(NULL); return;
    }
    bsp_audio_set_volume(MOON_CODEC_VOLUME_MAX);
    voice_t voices[VOICES]={0};
    int16_t pcm[CHUNK]; int32_t mixed[CHUNK]; int gain=0;
    moon_limiter_t limiter; moon_limiter_reset(&limiter);
    atomic_store(&ready,true);
    for(;;) {
        if(atomic_load(&stop_requested)) {atomic_store(&ready,false);atomic_store(&stopped,true);vTaskDelete(NULL);return;}
        bool active=false;
        for(unsigned v=0;v<VOICES;v++) active|=voices[v].active;
        command_t cmd;
        /* Idle blocks on the command queue; silent DMA tail prevents old audio looping. */
        if(xQueueReceive(commands,&cmd,active?0:pdMS_TO_TICKS(100))==pdTRUE) {
            ESP_LOGI("moon_audio","Play note=%u volume=%u",cmd.note,atomic_load(&requested_volume));
            unsigned slot=VOICES;
            for(unsigned v=0;v<VOICES;v++) if(!voices[v].active) {slot=v;break;}
            if(slot==VOICES) {
                slot=0;
                for(unsigned v=1;v<VOICES;v++) if(voices[v].position>voices[slot].position) slot=v;
            }
            voice_t old=voices[slot];
            voices[slot]=(voice_t){.note=cmd.note,.active=true,
                .old_note=old.note,.old_position=old.position,.fade=old.active?320:0};
            active=true;
        }
        if(!active) continue;
        unsigned target=moon_volume_gain_q8(atomic_load(&requested_volume));
        int ceiling=MOON_MIX_CEILING;
        for(unsigned v=0;v<VOICES;v++) if(voices[v].active) {
            int c=moon_note_ceiling(voices[v].note);
            if(c<ceiling) ceiling=c;
            if(voices[v].fade) {
                c=moon_note_ceiling(voices[v].old_note);
                if(c<ceiling) ceiling=c;
            }
        }
        for(unsigned i=0;i<CHUNK;i++) {
            gain=moon_gain_step(gain,target);
            int32_t mix=0;
            for(unsigned v=0;v<VOICES;v++) if(voices[v].active) {
                int32_t current=sample(voices[v].note,voices[v].position++);
                if(voices[v].fade) {
                    int32_t old=voices[v].old_position<FRAMES?sample(voices[v].old_note,voices[v].old_position++):0;
                    current=(current*(320-(int)voices[v].fade)+old*(int)voices[v].fade)/320;
                    voices[v].fade--;
                }
                mix+=current;
                if(voices[v].position==FRAMES) voices[v].active=false;
            }
            mixed[i]=(int32_t)((int64_t)mix*gain/(100*256));
        }
        moon_limit_block_at_ceiling(&limiter,mixed,pcm,target==0,ceiling);
        if(bsp_audio_write(pcm,sizeof(pcm))!=ESP_OK) {
            atomic_store(&ready,false); (void)bsp_audio_sleep(); atomic_store(&stopped,true); vTaskDelete(NULL); return;
        }
        active=false;
        for(unsigned v=0;v<VOICES;v++) active|=voices[v].active;
        if(!active) {
            memset(mixed,0,sizeof(mixed));
            /* Flush the delayed final block before clearing the DMA tail. */
            moon_limit_block(&limiter,mixed,pcm,atomic_load(&requested_volume)==0);
            if(bsp_audio_write(pcm,sizeof(pcm))!=ESP_OK) {
                atomic_store(&ready,false); (void)bsp_audio_sleep(); atomic_store(&stopped,true); vTaskDelete(NULL); return;
            }
            moon_limiter_reset(&limiter);
            memset(pcm,0,sizeof(pcm));
            /* More than the I2S DMA ring: leave zeroes before blocking again. */
            for(int i=0;i<16;i++) if(bsp_audio_write(pcm,sizeof(pcm))!=ESP_OK) {
                atomic_store(&ready,false); (void)bsp_audio_sleep(); atomic_store(&stopped,true); vTaskDelete(NULL); return;
            }
        }
    }
}
bool moon_audio_start(void) {
    commands=xQueueCreate(8,sizeof(command_t));
    if(!commands) return false;
    if(xTaskCreate(audio_task,"moon_audio",5120,NULL,5,NULL)!=pdPASS) {
        vQueueDelete(commands); commands=NULL; return false;
    }
    return true;
}
bool moon_audio_stop(unsigned timeout_ms) {
    if(!commands) return true;
    atomic_store(&stop_requested,true);
    for(unsigned waited=0;waited<timeout_ms;waited+=10) {
        if(atomic_load(&stopped)) return true;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return atomic_load(&stopped);
}
bool moon_audio_ready(void) {return atomic_load(&ready);}
void moon_audio_volume(unsigned volume) {atomic_store(&requested_volume,moon_volume_clamp(volume));}
bool moon_audio_strike(unsigned note,unsigned volume) {
    if(!commands || !moon_audio_ready() || note>6) return false;
    moon_audio_volume(volume);
    if (!moon_volume_clamp(volume)) return true;
    command_t cmd={.note=note};
    return xQueueSend(commands,&cmd,0)==pdTRUE;
}
