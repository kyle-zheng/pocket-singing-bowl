#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MOON_BATTERY_UNKNOWN (-1)
#define MOON_BATTERY_READING (-2)
/* A plausibility check against the CW2017 ADC range, not a voltage-to-SOC curve. */
typedef struct {
    int displayed, candidate;
    unsigned count;
    uint64_t pending_since, candidate_since, last_sample;
    bool sampled;
} moon_battery_filter_t;
static inline void moon_battery_filter_init(moon_battery_filter_t *f, uint64_t now) {
    *f=(moon_battery_filter_t){.displayed=MOON_BATTERY_READING,
        .candidate=-1,.pending_since=now};
}
static inline int moon_battery_filter_sample(moon_battery_filter_t *f, int soc, int mv, uint64_t now) {
    if(f->sampled && (now<f->last_sample || now-f->last_sample<1000)) return f->displayed;
    f->sampled=true;f->last_sample=now;
    if(soc<0 || soc>100 || mv<2500 || mv>4700) {
        f->displayed=MOON_BATTERY_UNKNOWN;f->candidate=-1;f->count=0;
        f->pending_since=now;
        return f->displayed;
    }
    /* Normal small changes need no cosmetic smoothing; crossing low-battery needs confirmation. */
    if(f->displayed>=0 && abs(soc-f->displayed)<=2 &&
       (soc<=15)==(f->displayed<=15) && (soc==0)==(f->displayed==0)) {
        f->displayed=soc;return soc;
    }
    if(f->displayed>=0) {f->pending_since=now;f->candidate=-1;f->count=0;}
    if(f->candidate<0 || abs(soc-f->candidate)>2 ||
       (soc<=15)!=(f->candidate<=15) || (soc==0)!=(f->candidate==0)) {
        f->candidate=soc;f->count=1;f->candidate_since=now;
    } else f->count++;
    /* Zero after reset is valid numerically but may still be provisional. Never substitute 100%. */
    if(f->count>=3 && now-f->candidate_since>=(soc==0?30000U:2000U)) {
        f->displayed=soc;return soc;
    }
    f->displayed=now-f->pending_since>=30000?MOON_BATTERY_UNKNOWN:MOON_BATTERY_READING;
    return f->displayed;
}
