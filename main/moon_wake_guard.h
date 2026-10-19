#pragma once
#include <stdbool.h>
#include <stdint.h>
/* Controller-owned: discard the whole wake/cancel gesture, including delayed clicks. */
typedef struct { bool active; uint64_t released_since; } moon_wake_guard_t;
static inline void moon_wake_guard_arm(moon_wake_guard_t *g) {
    g->active=true; g->released_since=0;
}
static inline bool moon_wake_guard_poll(moon_wake_guard_t *g, bool released, uint64_t now) {
    if (!g->active) return false;
    if (!released) g->released_since=0;
    else if (!g->released_since) g->released_since=now;
    else if (now-g->released_since>=300) g->active=false;
    return true;
}
