#include "moon_mix.h"
#include "moon_volume.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    moon_limiter_t s;
    int32_t in[MOON_MIX_FRAMES]; int16_t out[MOON_MIX_FRAMES];
    moon_limiter_reset(&s);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) in[i]=10000;
    moon_limit_block(&s,in,out,0);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]==0);
    moon_limit_block(&s,in,out,0);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]==10000);
    /* Abrupt worst-case four-voice overload must be anticipated, never wrap. */
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) in[i]=4*32767;
    moon_limit_block(&s,in,out,0);
    for(unsigned i=1;i<MOON_MIX_FRAMES;i++) assert(abs(out[i]-out[i-1])<40);
    moon_limit_block(&s,in,out,0);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]>0 && out[i]<=MOON_MIX_CEILING);
    moon_limit_block(&s,in,out,1);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]==0);
    memset(in,0,sizeof(in)); moon_limit_block(&s,in,out,0);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]==0); /* no delayed unmute burst */
    /* Deterministic stress including changing signs/levels and release. */
    unsigned rng=42;
    for(unsigned block=0;block<2000;block++) {
        for(unsigned i=0;i<MOON_MIX_FRAMES;i++) {
            rng=rng*1664525U+1013904223U;
            in[i]=(int)(rng%262137U)-131068;
            if(block%8==0) in[i]/=20;
        }
        moon_limit_block(&s,in,out,0);
        for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(abs(out[i])<=MOON_MIX_CEILING);
    }
    memset(in,0,sizeof(in));
    for(int i=0;i<20;i++) moon_limit_block(&s,in,out,0);
    assert(s.boundary==MOON_MIX_UNITY);
    assert(moon_gain_step(100*256,0)==0 && moon_gain_step(0,35*256)==64);
    assert(moon_volume_clamp(101)==100 && moon_volume_clamp(0)==0);
    assert(MOON_VOLUME_DEFAULT==50);
    /* Retain quiet/default levels and provide strictly increasing 5% steps. */
    for(unsigned volume=0;volume<=35;volume++) assert(moon_volume_gain_q8(volume)==volume*256U);
    for(unsigned volume=1;volume<=100;volume++) assert(moon_volume_gain_q8(volume)>moon_volume_gain_q8(volume-1));
    assert(moon_volume_gain_q8(100)==70*256U);
    assert(moon_volume_gain_q8(200)==70*256U);
    for(unsigned note=0;note<7;note++) {
        moon_limiter_reset(&s);
        for(unsigned i=0;i<MOON_MIX_FRAMES;i++) in[i]=4*32767;
        moon_limit_block_at_ceiling(&s,in,out,0,moon_note_ceiling(note));
        moon_limit_block_at_ceiling(&s,in,out,0,moon_note_ceiling(note));
        for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]<=moon_note_ceiling(note));
    }
    /* Lowering the next block's ceiling must not truncate the delayed block. */
    moon_limiter_reset(&s);
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) in[i]=12000;
    moon_limit_block_at_ceiling(&s,in,out,0,moon_note_ceiling(6));
    moon_limit_block_at_ceiling(&s,in,out,0,moon_note_ceiling(0));
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]<=moon_note_ceiling(6));
    moon_limit_block_at_ceiling(&s,in,out,0,moon_note_ceiling(0));
    for(unsigned i=0;i<MOON_MIX_FRAMES;i++) assert(out[i]<=moon_note_ceiling(0));
    /* Run actual embedded PCM through the production limiter: all seven notes,
       four overlapping starts, 6 s automatic overlap, and mixed notes. */
    FILE *f=fopen("assets/music/moon-bowls.pcm","rb"); assert(f);
    int16_t *bank=malloc(7*128000*sizeof(*bank)); assert(bank);
    assert(fread(bank,sizeof(*bank),7*128000,f)==7*128000); fclose(f);
    for(unsigned volume=0;volume<=100;volume+=(volume==0?35:65))
    for(unsigned note=0;note<8;note++) for(unsigned spacing=3200;spacing<=96000;spacing+=92800) {
        moon_limiter_reset(&s);
        for(unsigned pos=0;pos<128000+3*spacing+MOON_MIX_FRAMES;pos+=MOON_MIX_FRAMES) {
            for(unsigned i=0;i<MOON_MIX_FRAMES;i++) {
                in[i]=0;
                for(unsigned v=0;v<4;v++) {
                    int p=(int)(pos+i)-(int)(v*spacing);
                    if(p>=0 && p<128000) in[i]+=bank[(note<7?note:v*2)*128000+p];
                }
            }
            int ceiling=moon_note_ceiling(note<7?note:0);
            for(unsigned i=0;i<MOON_MIX_FRAMES;i++) in[i]=(int32_t)((int64_t)in[i]*moon_volume_gain_q8(volume)/(100*256));
            moon_limit_block_at_ceiling(&s,in,out,volume==0,ceiling);
            for(unsigned i=0;i<MOON_MIX_FRAMES;i++) {
                assert(abs(out[i])<=ceiling);
                if(volume==0) assert(out[i]==0);
            }
        }
    }
    free(bank);
    puts("Moon mixer: PASS (lookahead, overload, smooth gain, immediate mute, release, real PCM overlap)");
}
