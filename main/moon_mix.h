#pragma once
#include <stdint.h>
#include <string.h>

static inline int moon_gain_step(int current, unsigned target) {
    if (!target) return 0;
    int delta=(int)target-current;
    return current+(delta>64?64:delta < -64?-64:delta);
}

#define MOON_MIX_FRAMES 256
#define MOON_MIX_CEILING 12845 /* Largest provisional per-note ceiling. */
#define MOON_MIX_UNITY 32768
/* One block (16 ms) lookahead. No per-sample nonlinear waveshaping. */
typedef struct {
    int32_t pending[MOON_MIX_FRAMES];
    int bound, boundary;
} moon_limiter_t;
static inline void moon_limiter_reset(moon_limiter_t *s) {
    memset(s,0,sizeof(*s));
    s->bound=s->boundary=MOON_MIX_UNITY;
}
static inline void moon_limit_block_at_ceiling(moon_limiter_t *s, const int32_t *input,
                                    int16_t *output, int muted, int ceiling) {
    if(muted) {
        moon_limiter_reset(s);
        memset(output,0,MOON_MIX_FRAMES*sizeof(*output));
        return;
    }
    int32_t peak=0;
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) {
        int32_t a=input[i]<0?-input[i]:input[i];
        if(a>peak) peak=a;
    }
    int bound=peak>ceiling ?
        (int)((int64_t)ceiling*MOON_MIX_UNITY/peak):MOON_MIX_UNITY;
    /* Both ends are bounded by the pending block's peak. The next block
       constrains this block's end, so attenuation precedes a loud onset. */
    int end=s->bound<bound?s->bound:bound;
    if(end>s->boundary+2048) end=s->boundary+2048; /* >=256 ms full recovery */
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) {
        int gain=s->boundary+(end-s->boundary)*(int)(i+1)/MOON_MIX_FRAMES;
        output[i]=(int16_t)((int64_t)s->pending[i]*gain/MOON_MIX_UNITY);
        s->pending[i]=input[i];
    }
    s->bound=bound;
    s->boundary=end;
}

static inline void moon_limit_block(moon_limiter_t *s, const int32_t *input,
                                    int16_t *output, int muted) {
    moon_limit_block_at_ceiling(s,input,output,muted,MOON_MIX_CEILING);
}
