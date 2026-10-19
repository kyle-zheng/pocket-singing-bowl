#pragma once
#include "moon_model.h"
void moon_ui_create(void);
void moon_ui_update(const moon_model_t *m, uint64_t now, int battery, bool audio_ok, bool storage_ok);
