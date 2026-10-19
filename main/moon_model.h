#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "moon_volume.h"

typedef enum { MOON_UP, MOON_DOWN, MOON_OK } moon_key_t;
typedef enum { MOON_PRESS, MOON_CLICK, MOON_DOUBLE, MOON_LONG, MOON_RELEASE } moon_event_t;
typedef struct {
    uint32_t version, note, interval_s, duration_s, volume, auto_sleep;
} moon_preferences_t;
typedef struct {
    moon_preferences_t prefs;
    bool automatic, paused, settings, editing;
    uint8_t row;
    uint32_t draft;
    uint64_t remaining_ms, next_ms, last_ms, activity_ms;
    int repeat_key;
    uint64_t repeat_start_ms, repeat_due_ms;
    bool strike, save, changed;
} moon_model_t;

moon_preferences_t moon_defaults(void);
bool moon_preferences_decode(moon_preferences_t *out, const void *data, size_t size);
bool moon_sleep_due(const moon_model_t *m, uint64_t now);
bool moon_preferences_valid(const moon_preferences_t *prefs);
void moon_init(moon_model_t *m, const moon_preferences_t *prefs, uint64_t now);
void moon_tick(moon_model_t *m, uint64_t now);
void moon_input(moon_model_t *m, moon_key_t key, moon_event_t event, uint64_t now);
void moon_cancel_repeat(moon_model_t *m);
