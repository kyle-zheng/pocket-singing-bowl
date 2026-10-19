#include "moon_model.h"
#include "moon_wake_guard.h"
#include <assert.h>
#include <stdio.h>

static void enter_interval(moon_model_t *m, uint64_t now) {
    moon_input(m, MOON_OK, MOON_DOUBLE, now);
    moon_input(m, MOON_OK, MOON_CLICK, now);
    assert(m->settings && m->editing && m->row == 0);
}

int main(void) {
    moon_model_t m;
    moon_init(&m, NULL, 0);
    assert(m.prefs.interval_s == 6 && m.prefs.note == 6 && m.prefs.volume == 50 && !m.automatic);
    enter_interval(&m, 0);
    assert(!m.strike);
    moon_input(&m, MOON_UP, MOON_LONG, 500);
    for (uint64_t t=500; t<5000; t+=20) moon_tick(&m, t);
    assert(m.draft == 60);
    moon_input(&m, MOON_UP, MOON_RELEASE, 5000);
    moon_tick(&m, 6000);
    assert(m.repeat_key == -1);
    moon_input(&m, MOON_OK, MOON_LONG, 6000);
    assert(!m.editing && m.settings && m.prefs.interval_s == 6);
    moon_input(&m, MOON_OK, MOON_LONG, 6000);
    assert(!m.settings && !m.automatic);

    moon_input(&m, MOON_OK, MOON_LONG, 6000);
    assert(m.automatic && m.strike);
    m.strike = false;
    moon_tick(&m, 11999); assert(!m.strike);
    moon_tick(&m, 12000); assert(m.strike);
    m.strike = false;
    uint64_t remaining = m.remaining_ms;
    moon_input(&m, MOON_OK, MOON_DOUBLE, 12000);
    moon_tick(&m, 90000);
    assert(m.paused && m.remaining_ms == remaining && !m.strike);
    moon_input(&m, MOON_OK, MOON_LONG, 90000);
    moon_input(&m, MOON_DOWN, MOON_CLICK, 90000);
    assert(m.prefs.note == 0 && m.remaining_ms == remaining);
    moon_input(&m, MOON_OK, MOON_CLICK, 90000);
    moon_tick(&m, 90000 + remaining);
    assert(!m.automatic && m.strike && m.remaining_ms == 0);
    m.strike = false;
    moon_tick(&m, 300000); assert(!m.strike);

    moon_preferences_t prefs = moon_defaults();
    prefs.duration_s = 0; prefs.note = 6; prefs.interval_s = 17;
    moon_init(&m, &prefs, 0);
    assert(!m.automatic && m.prefs.note == 6 && m.prefs.interval_s == 17);
    moon_input(&m, MOON_OK, MOON_LONG, 0);
    m.strike = false;
    moon_tick(&m, 24ULL*60*60*1000);
    assert(m.automatic && m.strike && m.remaining_ms == 0);
    assert(m.next_ms > 0 && m.next_ms <= 17000);
    moon_input(&m, MOON_OK, MOON_CLICK, m.last_ms);
    m.strike = false; moon_tick(&m, m.last_ms + 100000);
    assert(m.paused && !m.strike);

    moon_init(&m, NULL, 0); enter_interval(&m, 0);
    for (int i=0;i<100;i++) moon_input(&m, MOON_DOWN, MOON_CLICK, 0);
    assert(m.draft == 6);
    for (int i=0;i<11;i++) moon_input(&m, MOON_UP, MOON_CLICK, 0);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    assert(m.prefs.interval_s == 17 && m.save);
    prefs = m.prefs; prefs.interval_s = 61;
    assert(!moon_preferences_valid(&prefs));
    moon_init(&m, &prefs, 0); assert(m.prefs.interval_s == 6);
    /* Volume endpoints, 5% steps, save/cancel, persistence and persistent mute. */
    moon_init(&m, NULL, 0);
    assert(m.prefs.volume == 50);
    moon_input(&m, MOON_OK, MOON_DOUBLE, 0);
    moon_input(&m, MOON_DOWN, MOON_DOUBLE, 0);
    assert(m.row == 2);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    for (int i=0;i<30;i++) moon_input(&m, MOON_UP, MOON_CLICK, 0);
    assert(m.draft == 100);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    prefs = m.prefs;
    moon_init(&m, &prefs, 0);
    assert(m.prefs.volume == 100 && !m.automatic);
    moon_input(&m, MOON_OK, MOON_DOUBLE, 0);
    moon_input(&m, MOON_DOWN, MOON_DOUBLE, 0);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    for (int i=0;i<30;i++) moon_input(&m, MOON_DOWN, MOON_CLICK, 0);
    assert(m.draft == 0);
    moon_input(&m, MOON_OK, MOON_LONG, 0);
    assert(m.prefs.volume == 100);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    for (int i=0;i<20;i++) moon_input(&m, MOON_DOWN, MOON_CLICK, 0);
    moon_input(&m, MOON_OK, MOON_CLICK, 0);
    assert(m.prefs.volume == 0);
    prefs=m.prefs; prefs.volume=0; prefs.interval_s=17;
    assert(moon_preferences_valid(&prefs));
    moon_init(&m,&prefs,0);
    assert(m.prefs.volume==0 && m.prefs.interval_s==17);
    prefs.volume=105; assert(!moon_preferences_valid(&prefs));
    prefs.volume=6; assert(!moon_preferences_valid(&prefs));
    for (unsigned v=0;v<=100;v+=5) {prefs.volume=v;assert(moon_preferences_valid(&prefs));}
    moon_init(&m,NULL,0);
    moon_input(&m,MOON_OK,MOON_DOUBLE,0);
    moon_input(&m,MOON_DOWN,MOON_DOUBLE,0);
    moon_input(&m,MOON_OK,MOON_CLICK,0);
    moon_input(&m,MOON_UP,MOON_LONG,500);
    uint32_t before=m.draft;
    moon_input(&m,MOON_UP,MOON_RELEASE,m.repeat_due_ms);
    moon_tick(&m,4000);
    assert(m.draft==before && m.repeat_key==-1);
    moon_input(&m,MOON_UP,MOON_LONG,4500);
    for(uint64_t t=4500;t<8500;t+=20) moon_tick(&m,t);
    assert(m.draft==100);
    moon_input(&m,MOON_UP,MOON_RELEASE,8500);
    moon_input(&m,MOON_DOWN,MOON_LONG,9000);
    for(uint64_t t=9000;t<13000;t+=20) moon_tick(&m,t);
    assert(m.draft==0);
    moon_input(&m,MOON_OK,MOON_CLICK,13000);
    moon_tick(&m,14000);
    assert(m.prefs.volume==0 && m.repeat_key==-1 && !m.editing);
    prefs=m.prefs;moon_init(&m,&prefs,15000);
    assert(m.prefs.volume==0);
    /* Sleep defaults, option persistence, old settings migration and idle boundaries. */
    moon_init(&m,NULL,0);
    assert(!m.prefs.auto_sleep && !moon_sleep_due(&m,3600000));
    moon_input(&m,MOON_OK,MOON_DOUBLE,0);
    for(int i=0;i<3;i++) moon_input(&m,MOON_DOWN,MOON_CLICK,0);
    assert(m.row==3);
    moon_input(&m,MOON_OK,MOON_CLICK,0);
    moon_input(&m,MOON_UP,MOON_CLICK,0);
    moon_input(&m,MOON_OK,MOON_LONG,0);
    assert(!m.prefs.auto_sleep);
    moon_input(&m,MOON_OK,MOON_CLICK,0);
    moon_input(&m,MOON_UP,MOON_CLICK,0);
    moon_input(&m,MOON_OK,MOON_CLICK,0);
    assert(m.prefs.auto_sleep && m.save);
    prefs=m.prefs;moon_init(&m,&prefs,100);
    assert(!moon_sleep_due(&m,600099) && moon_sleep_due(&m,600100));
    moon_input(&m,MOON_UP,MOON_PRESS,600100);
    assert(!moon_sleep_due(&m,600101));
    prefs.duration_s=0;moon_init(&m,&prefs,0);
    moon_input(&m,MOON_OK,MOON_LONG,0);
    moon_tick(&m,3600000);assert(!moon_sleep_due(&m,3600000));
    moon_input(&m,MOON_OK,MOON_CLICK,3600000);
    assert(!moon_sleep_due(&m,4199999) && moon_sleep_due(&m,4200000));
    prefs.duration_s=600;moon_init(&m,&prefs,0);
    moon_input(&m,MOON_OK,MOON_LONG,0);
    moon_tick(&m,600000);
    assert(!m.automatic && !moon_sleep_due(&m,600000));
    assert(!moon_sleep_due(&m,1199999) && moon_sleep_due(&m,1200000));
    uint32_t legacy[]={1,4,17,0,0};
    assert(moon_preferences_decode(&prefs,legacy,sizeof(legacy)));
    assert(prefs.version==2 && prefs.note==4 && prefs.interval_s==17 && prefs.volume==0 && !prefs.auto_sleep);
    prefs.auto_sleep=1;
    moon_preferences_t restored;
    assert(moon_preferences_decode(&restored,&prefs,sizeof(prefs)) && restored.auto_sleep==1);
    prefs.auto_sleep=2;assert(!moon_preferences_valid(&prefs));
    assert(!moon_preferences_decode(&restored,legacy,sizeof(legacy)-1));
    moon_wake_guard_t guard={0};moon_wake_guard_arm(&guard);
    assert(moon_wake_guard_poll(&guard,false,10000) && guard.active);
    assert(moon_wake_guard_poll(&guard,true,10020) && guard.active);
    assert(moon_wake_guard_poll(&guard,true,10200) && guard.active);
    assert(moon_wake_guard_poll(&guard,false,10210) && guard.active);
    assert(moon_wake_guard_poll(&guard,true,10300) && guard.active);
    assert(moon_wake_guard_poll(&guard,true,10600) && !guard.active);
    assert(!moon_wake_guard_poll(&guard,true,10620));
    puts("Moon model: PASS (timing, no duplicate ending, unlimited, repeat, cancel/save, defaults)");
    return 0;
}
