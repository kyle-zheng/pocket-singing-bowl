#include "moon_model.h"
#include <string.h>

static const uint32_t durations[] = {60, 180, 300, 600, 0};

moon_preferences_t moon_defaults(void) {
    return (moon_preferences_t){.version=2, .note=6, .interval_s=6,
                                .duration_s=180, .volume=MOON_VOLUME_DEFAULT};
}

bool moon_preferences_valid(const moon_preferences_t *p) {
    if (!p || p->version != 2 || p->auto_sleep > 1 || p->note > 6 || p->interval_s < 6 ||
        p->interval_s > 60 || p->volume > MOON_VOLUME_MAX || p->volume % MOON_VOLUME_STEP) return false;
    for (unsigned i=0; i<5; i++) if (p->duration_s == durations[i]) return true;
    return false;
}

bool moon_preferences_decode(moon_preferences_t *out, const void *data, size_t size) {
    moon_preferences_t loaded=moon_defaults();
    if (size!=5*sizeof(uint32_t) && size!=sizeof(loaded)) return false;
    memcpy(&loaded,data,size);
    if (size==5*sizeof(uint32_t)) {
        if (loaded.version!=1) return false;
        loaded.version=2; loaded.auto_sleep=0;
    }
    if (!moon_preferences_valid(&loaded)) return false;
    *out=loaded; return true;
}

bool moon_sleep_due(const moon_model_t *m, uint64_t now) {
    return m->prefs.auto_sleep && !(m->automatic && !m->paused && !m->settings) &&
           now>=m->activity_ms && now-m->activity_ms>=600000;
}

void moon_init(moon_model_t *m, const moon_preferences_t *prefs, uint64_t now) {
    memset(m, 0, sizeof(*m));
    moon_preferences_t loaded = prefs ? *prefs : moon_defaults();
    m->prefs = moon_preferences_valid(&loaded) ? loaded : moon_defaults();
    m->remaining_ms = m->prefs.duration_s * 1000ULL;
    m->next_ms = m->prefs.interval_s * 1000ULL;
    m->last_ms = m->activity_ms = now;
    m->repeat_key = -1;
    m->changed = true;
}

void moon_cancel_repeat(moon_model_t *m) { m->repeat_key = -1; }

static void adjust(moon_model_t *m, int direction) {
    /* Direction is +1 for UP while editing; list navigation is spatial. */
    if (!m->settings) {
        m->prefs.note = (m->prefs.note + (direction > 0 ? 6 : 1)) % 7;
        m->save = true;
    } else if (!m->editing) {
        int row = (int)m->row - direction;
        if (row >= 0 && row < 4) m->row = (uint8_t)row;
    } else if (m->row == 0) {
        int value = (int)m->draft + direction;
        if (value >= 6 && value <= 60) m->draft = (uint32_t)value;
    } else if (m->row == 1) {
        for (int i=0; i<5; i++) {
            if (m->draft == durations[i]) {
                int next = i + direction;
                if (next >= 0 && next < 5) m->draft = durations[next];
                break;
            }
        }
    } else if (m->row == 3) {
        m->draft = direction > 0 ? 1 : 0;
    } else {
        int value = (int)m->draft + (int)MOON_VOLUME_STEP * direction;
        if (value >= (int)MOON_VOLUME_MIN && value <= (int)MOON_VOLUME_MAX) m->draft = (uint32_t)value;
    }
    m->changed = true;
}

void moon_tick(moon_model_t *m, uint64_t now) {
    if (now < m->last_ms) return;
    uint64_t elapsed = now - m->last_ms;
    m->last_ms = now;
    if (m->repeat_key >= 0 && m->settings && m->editing && (m->row == 0 || m->row == 2) &&
        now >= m->repeat_due_ms) {
        adjust(m, m->repeat_key == MOON_UP ? 1 : -1);
        m->repeat_due_ms = now + (now - m->repeat_start_ms >= 1000 ? 35 : 120);
        m->activity_ms = now;
    }
    if (!m->automatic || m->paused || m->settings) return;
    if (m->prefs.duration_s && elapsed >= m->remaining_ms) {
        /* End has priority over the periodic strike: exactly one final sound. */
        m->remaining_ms = 0;
        m->automatic = false;
        m->activity_ms = now;
        m->strike = m->changed = true;
        return;
    }
    if (m->prefs.duration_s) m->remaining_ms -= elapsed;
    if (elapsed >= m->next_ms) {
        uint64_t interval = m->prefs.interval_s * 1000ULL;
        m->next_ms = interval - (elapsed - m->next_ms) % interval;
        m->strike = m->changed = true;
    } else m->next_ms -= elapsed;
}

void moon_input(moon_model_t *m, moon_key_t key, moon_event_t event, uint64_t now) {
    if (event == MOON_RELEASE && m->repeat_key == (int)key) moon_cancel_repeat(m);
    moon_tick(m, now);
    m->activity_ms = now;
    if (event == MOON_RELEASE) {
        if (m->repeat_key == (int)key) moon_cancel_repeat(m);
        return;
    }
    if (event == MOON_PRESS) {
        if (m->repeat_key != (int)key) moon_cancel_repeat(m);
        return;
    }
    if (key != MOON_OK) {
        if (event == MOON_CLICK || event == MOON_DOUBLE) {
            adjust(m, key == MOON_UP ? 1 : -1);
            if (event == MOON_DOUBLE) adjust(m, key == MOON_UP ? 1 : -1);
        } else if (event == MOON_LONG && m->settings && m->editing && (m->row == 0 || m->row == 2)) {
            adjust(m, key == MOON_UP ? 1 : -1);
            m->repeat_key = key;
            m->repeat_start_ms = now;
            m->repeat_due_ms = now + 120;
        }
        return;
    }
    moon_cancel_repeat(m);
    m->changed = true;
    if (m->settings) {
        if (event == MOON_LONG) {
            if (m->editing) m->editing = false;
            else m->settings = false;
        } else if (event == MOON_CLICK || event == MOON_DOUBLE) {
            /* A rapid pair in settings is one confirmation, never enter+save. */
            if (!m->editing) {
                m->editing = true;
                m->draft = m->row == 0 ? m->prefs.interval_s :
                           m->row == 1 ? m->prefs.duration_s :
                           m->row == 2 ? m->prefs.volume : m->prefs.auto_sleep;
            } else {
                if (m->row == 0) {
                    m->prefs.interval_s = m->draft;
                    m->next_ms = m->draft * 1000ULL;
                } else if (m->row == 1 && m->prefs.duration_s != m->draft) {
                    m->prefs.duration_s = m->draft;
                    m->remaining_ms = m->draft * 1000ULL;
                } else if (m->row == 2) m->prefs.volume = m->draft;
                else if (m->row == 3) m->prefs.auto_sleep = m->draft;
                m->editing = false;
                m->save = true;
            }
        }
    } else if (event == MOON_DOUBLE) {
        m->settings = true;
        m->row = 0;
        if (m->automatic) m->paused = true;
    } else if (event == MOON_LONG) {
        m->automatic = !m->automatic;
        m->paused = false;
        m->remaining_ms = m->prefs.duration_s * 1000ULL;
        m->next_ms = m->prefs.interval_s * 1000ULL;
        if (m->automatic) m->strike = true;
    } else if (event == MOON_CLICK) {
        if (m->automatic) m->paused = !m->paused;
        else m->strike = true;
    }
}
