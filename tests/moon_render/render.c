#include "lvgl.h"
#include "moon_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
static uint16_t frame[240*320];
static uint8_t draw[240*40*2] __attribute__((aligned(4)));
static void flush(lv_display_t *display,const lv_area_t *area,uint8_t *pixels) {
    for(int y=area->y1;y<=area->y2;y++) for(int x=area->x1;x<=area->x2;x++) {
        frame[y*240+x]=(uint16_t)pixels[0]|((uint16_t)pixels[1]<<8);pixels+=2;
    }
    lv_display_flush_ready(display);
}
static void save(const char *name) {
    lv_refr_now(NULL);
    FILE *f=fopen(name,"wb");assert(f);fprintf(f,"P6\n240 320\n255\n");
    for(int i=0;i<240*320;i++) {uint16_t p=frame[i];uint8_t rgb[]={((p>>11)*255)/31,(((p>>5)&63)*255)/63,((p&31)*255)/31};fwrite(rgb,1,3,f);}
    fclose(f);
}
int main(void) {
    lv_init();lv_display_t *d=lv_display_create(240,320);
    lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(d,draw,NULL,sizeof(draw),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d,flush);
    moon_ui_create();moon_model_t m;moon_init(&m,NULL,0);
    moon_ui_update(&m,0,80,true,true);save("home-help.ppm");
    moon_ui_update(&m,11000,80,true,true);save("home-quiet.ppm");
    m.settings=true;moon_ui_update(&m,11000,80,true,true);save("settings.ppm");
    m.editing=true;m.draft=60;moon_ui_update(&m,11000,80,true,true);save("interval.ppm");
    m.row=1;m.draft=0;moon_ui_update(&m,11000,80,true,true);save("unlimited.ppm");
    m.row=2;m.draft=0;moon_ui_update(&m,11000,80,true,true);save("volume-mute.ppm");
    m.row=3;m.draft=0;moon_ui_update(&m,11000,80,true,true);save("sleep-off.ppm");
    m.draft=1;moon_ui_update(&m,11000,80,true,true);save("sleep-on.ppm");
    m.editing=false;moon_ui_update(&m,11000,80,true,true);save("settings-sleep.ppm");
    moon_ui_update(&m,11000,-2,true,true);save("battery-reading.ppm");
    moon_ui_update(&m,11000,-1,true,true);save("battery-unknown.ppm");
    for(unsigned note=0;note<7;note++) {m.settings=false;m.prefs.note=note;moon_ui_update(&m,11000,-1,true,true);lv_refr_now(NULL);}
    puts("Moon UI render: PASS (240x320, 24KB LVGL heap)");
    return 0;
}
