#pragma once
#define MOON_VOLUME_MIN 0U
#define MOON_VOLUME_MAX 100U
#define MOON_VOLUME_DEFAULT 50U
#define MOON_VOLUME_STEP 5U
/* BSP 100 is its normal maximum output setting; no register-level overdrive. */
#define MOON_CODEC_VOLUME_MAX 100U
static inline unsigned moon_volume_clamp(unsigned volume) {
    return volume>MOON_VOLUME_MAX?MOON_VOLUME_MAX:volume;
}

/* Preserve 0..35%; compress the upper range to a provisional 70% of v0.1.6.
   75% was reported mostly clean, not measured distortion-free. */
static inline unsigned moon_volume_gain_q8(unsigned volume) {
    volume=moon_volume_clamp(volume);
    if(volume<=35) return volume*256U;
    return 35U*256U+(volume-35U)*35U*256U/65U;
}
/* Per-note ceilings also bound stacked tails to the provisional single-note
   maximum. Values are the v0.1.6 PCM peaks, not acoustic calibration. */
static inline int moon_note_ceiling(unsigned note) {
    static const unsigned peaks[7]={15073,15728,16384,17039,17694,18022,18350};
    return (int)(peaks[note<7?note:0]*70U/100U);
}
