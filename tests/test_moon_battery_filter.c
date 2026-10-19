#include "moon_battery_filter.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    moon_battery_filter_t f;
    moon_battery_filter_init(&f,0);
    /* Reported regression: provisional 0% after waking, followed by the real 100%. */
    for(uint64_t t=0;t<10000;t+=1000)
        assert(moon_battery_filter_sample(&f,0,4180,t)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4180,10000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4180,11000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4180,12000)==100);
    assert(moon_battery_filter_sample(&f,0,4170,42000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4170,43000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4170,44000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,100,4170,45000)==100);
    /* Real low and zero remain displayable; this is not an always-full filter. */
    assert(moon_battery_filter_sample(&f,10,3400,75000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,10,3400,76000)==MOON_BATTERY_READING);
    assert(moon_battery_filter_sample(&f,10,3400,77000)==10);
    for(uint64_t t=78000;t<108000;t+=1000)
        assert(moon_battery_filter_sample(&f,0,3000,t)<0);
    assert(moon_battery_filter_sample(&f,0,3000,108000)==0);
    /* Bus failures or invalid voltages are unknown; repeated fast polls are not evidence. */
    assert(moon_battery_filter_sample(&f,-1,3000,109000)==MOON_BATTERY_UNKNOWN);
    assert(moon_battery_filter_sample(&f,50,-1,110000)==MOON_BATTERY_UNKNOWN);
    assert(moon_battery_filter_sample(&f,101,3800,111000)==MOON_BATTERY_UNKNOWN);
    assert(moon_battery_filter_sample(&f,50,3800,112000)<0);
    for(uint64_t t=112001;t<113000;t++) assert(moon_battery_filter_sample(&f,50,3800,t)<0);
    assert(moon_battery_filter_sample(&f,50,3800,113000)<0);
    assert(moon_battery_filter_sample(&f,50,3800,114000)==50);
    moon_battery_filter_init(&f,0);
    for(uint64_t t=0;t<=31000;t+=1000) {
        int value=moon_battery_filter_sample(&f,t%2000?100:0,4000,t);
        assert(value==(t>=30000?MOON_BATTERY_UNKNOWN:MOON_BATTERY_READING));
    }
    puts("Moon battery: PASS (wake zero recovery, real low/zero, invalid reads, timed confirmation)");
}
