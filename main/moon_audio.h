#pragma once
#include <stdbool.h>
#include <stdint.h>
bool moon_audio_stop(unsigned timeout_ms);
bool moon_audio_start(void);
bool moon_audio_strike(unsigned note, unsigned volume);
bool moon_audio_ready(void);
void moon_audio_volume(unsigned volume);
