/*
 * bno055.h — Minimal BNO055 IMU driver for STM32 HAL
 *
 * Uses I2C in blocking mode. Operates in IMU mode (accel + gyro fusion,
 * no magnetometer). Outputs Euler pitch (deg) and Gyro X (deg/s).
 *
 * Axis mapping (validated on bench, Phase 2):
 *   Euler Pitch  = rotation about BNO055 X-axis
 *   Gyro X       = angular rate about BNO055 X-axis  ← correct pitch rate axis
 *   Gyro Y       = angular rate about BNO055 Y-axis  ← NOT pitch rate (orthogonal)
 *
 * Mounting guidance (for chassis assembly):
 *   Mount the BNO055 so its X-axis (the edge where PS0 sits) is parallel to
 *   the wheel axle (left-right axis of the robot).
 *   - Robot leans forward → Euler Pitch should go positive.
 *   - Verify sign after first assembly; add PITCH_SIGN = -1 in Phase 3 if needed.
 */

#pragma once
#include "stm32f4xx_hal.h"

/* Default I2C address (COM3/ADR pin low). HAL expects 8-bit (7-bit << 1). */
#define BNO055_I2C_ADDR     (0x28U << 1U)

/* Selected register addresses */
#define BNO055_REG_CHIP_ID      0x00U
#define BNO055_REG_UNIT_SEL     0x3BU
#define BNO055_REG_OPR_MODE     0x3DU
#define BNO055_REG_PWR_MODE     0x3EU
#define BNO055_REG_SYS_TRIGGER  0x3FU
#define BNO055_REG_GYR_X_LSB    0x14U   /* Gyro X — pitch rate axis (same axis as Euler Pitch) */
#define BNO055_REG_GYR_Y_LSB    0x16U   /* Gyro Y — NOT the pitch rate axis */
#define BNO055_REG_EUL_PITCH_LSB 0x1EU  /* Euler pitch LSB */

/* Operation modes */
#define BNO055_MODE_CONFIG  0x00U
#define BNO055_MODE_IMU     0x08U   /* Accel + Gyro fusion; no mag calibration needed */

#define BNO055_CHIP_ID      0xA0U

typedef struct {
    I2C_HandleTypeDef *hi2c;
} BNO055;

/* ---------- internal helpers ---------- */

static inline HAL_StatusTypeDef _bno_write(BNO055 *dev, uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(dev->hi2c, BNO055_I2C_ADDR,
                             reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 20);
}

static inline HAL_StatusTypeDef _bno_read(BNO055 *dev, uint8_t reg,
                                          uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(dev->hi2c, BNO055_I2C_ADDR,
                            reg, I2C_MEMADD_SIZE_8BIT, buf, len, 20);
}

/* ---------- public API ---------- */

/*
 * BNO055_Init — configure and enter IMU fusion mode.
 *
 * Call once after HAL_Init and I2C init. Blocks ~700 ms for chip startup.
 * Returns HAL_OK on success, HAL_ERROR if chip not found or comms fail.
 *
 * If your MCU resets without a power cycle the chip is already up; the
 * 700 ms wait is harmless in that case.
 */
static inline HAL_StatusTypeDef BNO055_Init(BNO055 *dev, I2C_HandleTypeDef *hi2c)
{
    dev->hi2c = hi2c;
    uint8_t val;

    /* Give chip time to boot from power-on */
    HAL_Delay(700);

    /* Verify chip ID */
    if (_bno_read(dev, BNO055_REG_CHIP_ID, &val, 1) != HAL_OK || val != BNO055_CHIP_ID)
        return HAL_ERROR;

    /* Enter CONFIG mode (required before changing settings) */
    if (_bno_write(dev, BNO055_REG_OPR_MODE, BNO055_MODE_CONFIG) != HAL_OK) return HAL_ERROR;
    HAL_Delay(25);

    /* Units: degrees for Euler, dps for Gyro, m/s² for Accel, Celsius (all 0) */
    if (_bno_write(dev, BNO055_REG_UNIT_SEL, 0x00) != HAL_OK) return HAL_ERROR;

    /* Normal power mode */
    if (_bno_write(dev, BNO055_REG_PWR_MODE, 0x00) != HAL_OK) return HAL_ERROR;
    HAL_Delay(10);

    /* Enter IMU fusion mode */
    if (_bno_write(dev, BNO055_REG_OPR_MODE, BNO055_MODE_IMU) != HAL_OK) return HAL_ERROR;
    HAL_Delay(20);  /* Datasheet: 19 ms mode switch delay */

    return HAL_OK;
}

/*
 * BNO055_GetPitch — Euler pitch angle in degrees.
 *
 * Range: -180 to +180 deg. Sign convention must be validated by tilting
 * the robot forward and confirming the reading direction (Phase 2 task).
 * Returns 0.0 on I2C error.
 */
static inline float BNO055_GetPitch(BNO055 *dev)
{
    uint8_t buf[2];
    if (_bno_read(dev, BNO055_REG_EUL_PITCH_LSB, buf, 2) != HAL_OK)
        return 0.0f;
    int16_t raw = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
    return (float)raw / 16.0f;
}

/*
 * BNO055_GetGyroX — Gyro X angular rate in degrees/second.
 *
 * This is the pitch rate axis — the same physical axis as Euler Pitch.
 * Validated on bench (Phase 2): Gyro Y did NOT respond to pitch-direction
 * tilts; Gyro X is the correct axis.
 * Sign convention (positive = ?) must be confirmed after chassis assembly.
 * Returns 0.0 on I2C error.
 */
static inline float BNO055_GetGyroX(BNO055 *dev)
{
    uint8_t buf[2];
    if (_bno_read(dev, BNO055_REG_GYR_X_LSB, buf, 2) != HAL_OK)
        return 0.0f;
    int16_t raw = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
    return (float)raw / 16.0f;
}
