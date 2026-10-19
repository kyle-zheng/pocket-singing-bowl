#include "moon_ui.h"
#include "moon_battery_filter.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(moon_font_12);
LV_FONT_DECLARE(moon_font_14);
LV_FONT_DECLARE(moon_font_18);
LV_FONT_DECLARE(moon_font_28);
extern const lv_image_dsc_t moon_background;
static lv_obj_t *home, *settings, *theme, *pitch, *status, *help;
static lv_obj_t *heading, *battery_label, *rows[4], *values[4], *large, *foot, *sleep_help;
static const char *themes[] = {"安定 · 放松", "感受 · 流动", "自信 · 行动", "接纳 · 关怀", "表达 · 倾听", "专注 · 觉察", "静心 · 冥想"};
static const char *notes[] = {"C 调 · 海底轮", "D 调 · 脐轮", "E 调 · 太阳轮", "F 调 · 心轮", "G 调 · 喉轮", "A 调 · 眉心轮", "B 调 · 顶轮"};
static const char *names[] = {"敲钵间隔", "自动时长", "颂钵音量", "自动休眠"};
static lv_obj_t *label(lv_obj_t *parent, int x, int y, int w, const lv_font_t *font) {
    lv_obj_t *o = lv_label_create(parent);
    lv_obj_set_pos(o, x, y); lv_obj_set_width(o, w);
    lv_obj_set_style_text_font(o, font, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(0xeadcc0), 0);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_line_space(o, 2, 0);
    lv_label_set_text(o, "");
    return o;
}
static void visible(lv_obj_t *o, bool show) {
    if(show) lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
}
static void set_text(lv_obj_t *o, const char *s) {
    if(strcmp(lv_label_get_text(o), s)) lv_label_set_text(o, s);
}
void moon_ui_create(void) {
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x171817), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    home = lv_obj_create(screen); lv_obj_remove_style_all(home);
    lv_obj_set_size(home,240,320); lv_obj_remove_flag(home,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *image = lv_image_create(home); lv_image_set_src(image, &moon_background);
    theme = label(home, 20, 16, 200, &moon_font_18);
    pitch = label(home, 20, 245, 200, &moon_font_14);
    status = label(home, 20, 275, 200, &moon_font_12);
    help = label(home, 16, 266, 208, &moon_font_12);
    settings = lv_obj_create(screen); lv_obj_remove_style_all(settings);
    lv_obj_set_size(settings,240,320); lv_obj_remove_flag(settings,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(settings,lv_color_hex(0x171817),0);
    lv_obj_set_style_bg_opa(settings,LV_OPA_COVER,0);
    heading = label(settings,20,20,200,&moon_font_18);
    battery_label = label(settings,20,51,200,&moon_font_12);
    for(int i=0;i<4;i++) {
        rows[i] = label(settings,18,82+i*36,125,&moon_font_14);
        values[i] = label(settings,146,82+i*36,76,&moon_font_14);
    }
    large = label(settings,20,130,200,&moon_font_28);
    sleep_help = label(settings,14,187,212,&moon_font_12);
    lv_label_set_text(sleep_help,"闲置 10 分钟后自动休眠\n自动颂钵时不休眠");
    foot = label(settings,14,239,212,&moon_font_12);
}
static void value_text(unsigned row, uint32_t v, char *s, size_t n) {
    if(row==0) snprintf(s,n,"%lu 秒",(unsigned long)v);
    else if(row==1 && !v) snprintf(s,n,"不限时");
    else if(row==1) snprintf(s,n,"%lu 分钟",(unsigned long)(v/60));
    else if(row==3) snprintf(s,n,"%s",v?"开启":"关闭");
    else snprintf(s,n,"%lu%%",(unsigned long)v);
}
void moon_ui_update(const moon_model_t *m, uint64_t now, int battery, bool audio_ok, bool storage_ok) {
    char s[160]; visible(home,!m->settings); visible(settings,m->settings);
    if(!m->settings) {
        set_text(theme,themes[m->prefs.note]); set_text(pitch,notes[m->prefs.note]);
        const char *warning = !audio_ok ? "声音未就绪" : !storage_ok ? "设置未保存" : battery>=0 && battery<=15 ? "电量偏低" : NULL;
        uint64_t idle=now-m->activity_ms;
        bool show_help = idle < 10000 && !warning;
        visible(help,show_help); visible(status,!show_help);
        if(show_help) {
            const char *first = !m->automatic ? "短按确认键 手动颂钵" : m->paused ? "短按确认键 继续颂钵" : "短按确认键 暂停颂钵";
            snprintf(s,sizeof(s),"%s\n长按确认键 %s颂钵\n双击确认键 设置",first,m->automatic?"结束":"自动");
            set_text(help,s);
            /* One-second fade, without creating animation objects. */
            lv_obj_set_style_opa(help,idle<9000?255:(uint8_t)((10000-idle)*255/1000),0);
        } else {
            if(warning) snprintf(s,sizeof(s),"%s",warning);
            else if(!m->automatic) snprintf(s,sizeof(s),"手动颂钵");
            else if(!m->prefs.duration_s) snprintf(s,sizeof(s),"%s · 不限时",m->paused?"已暂停":"自动颂钵");
            else { unsigned sec=(unsigned)((m->remaining_ms+999)/1000); snprintf(s,sizeof(s),"%s · %u:%02u",m->paused?"已暂停":"自动颂钵",sec/60,sec%60); }
            set_text(status,s);
        }
        return;
    }
    set_text(heading,m->editing?names[m->row]:"设置");
    if(!storage_ok) snprintf(s,sizeof(s),"设置未保存");
    else if(battery==MOON_BATTERY_READING) snprintf(s,sizeof(s),"电量读取中%s",m->automatic?" · 已暂停":"");
    else if(battery<0) snprintf(s,sizeof(s),"电量未知%s",m->automatic?" · 已暂停":"");
    else snprintf(s,sizeof(s),"电量 %d%%%s",battery,m->automatic?" · 已暂停":"");
    set_text(battery_label,s);
    for(unsigned i=0;i<4;i++) {
        visible(rows[i],!m->editing); visible(values[i],!m->editing);
        snprintf(s,sizeof(s),"%s%s",i==m->row?"> ":"",names[i]); set_text(rows[i],s);
        value_text(i,i==0?m->prefs.interval_s:i==1?m->prefs.duration_s:i==2?m->prefs.volume:m->prefs.auto_sleep,s,sizeof(s)); set_text(values[i],s);
        lv_color_t color=lv_color_hex(i==m->row?0xf6dfad:0xaaa59a);
        lv_obj_set_style_text_color(rows[i],color,0); lv_obj_set_style_text_color(values[i],color,0);
    }
    visible(large,m->editing);
    visible(sleep_help,m->editing && m->row==3);
    if(m->editing) {
        value_text(m->row,m->draft,s,sizeof(s)); set_text(large,s);
        set_text(foot,(m->row==0 || m->row==2)?"短按上/下键 调整\n长按上/下键 快调\n短按确认键 保存\n长按确认键 取消":"短按上/下键 调整\n短按确认键 保存\n长按确认键 取消");
    } else set_text(foot,"短按上/下键 选择\n短按确认键 修改\n长按确认键 返回");
}
