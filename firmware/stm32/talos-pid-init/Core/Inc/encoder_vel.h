#ifndef ENCODER_VEL_H
#define ENCODER_VEL_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct {
    TIM_HandleTypeDef *htim;
    int16_t prev_count;
    float   ticks_per_rev;
    float   wheel_radius_m;
} EncoderVel;

static inline void EncoderVel_Init(EncoderVel *e,
                                  TIM_HandleTypeDef *htim,
                                  float ticks_per_rev,
                                  float wheel_radius_m)
{
    e->htim = htim;
    e->ticks_per_rev = ticks_per_rev;
    e->wheel_radius_m = wheel_radius_m;
    e->prev_count = (int16_t)__HAL_TIM_GET_COUNTER(htim);
}

static inline void EncoderVel_Reset(EncoderVel *e)
{
    e->prev_count = (int16_t)__HAL_TIM_GET_COUNTER(e->htim);
}

static inline float EncoderVel_UpdateTicksPerSec(EncoderVel *e, float dt_sec)
{
    if (dt_sec <= 0.0f) return 0.0f;

    int16_t now = (int16_t)__HAL_TIM_GET_COUNTER(e->htim);
    int16_t delta = (int16_t)(now - e->prev_count);
    e->prev_count = now;

    return ((float)delta) / dt_sec;
}

static inline float EncoderVel_UpdateRadPerSec(EncoderVel *e, float dt_sec)
{
    float ticks_per_sec = EncoderVel_UpdateTicksPerSec(e, dt_sec);
    float rev_per_sec = ticks_per_sec / e->ticks_per_rev;
    return rev_per_sec * (2.0f * 3.14159265358979323846f);
}

static inline float EncoderVel_UpdateMetersPerSec(EncoderVel *e, float dt_sec)
{
    float rad_per_sec = EncoderVel_UpdateRadPerSec(e, dt_sec);
    return rad_per_sec * e->wheel_radius_m;
}

#endif // ENCODER_VEL_H
