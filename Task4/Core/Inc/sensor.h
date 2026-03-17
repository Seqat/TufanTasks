/* sensor.h
 * STM32F103C8T6 — Hafta 4: Araç Sensör Panosu
 * ADC okuma fonksiyon prototipleri ve sabitler
 */

#ifndef SENSOR_H
#define SENSOR_H

#include "stm32f1xx_hal.h"

/* ── Sabitler ────────────────────────────────────────────────────── */
#define ADC_MAX_VALUE    4095.0f   /* 12-bit ADC maks. değeri          */
#define ADC_VREF         3.3f      /* Referans gerilimi (V)             */
#define ADC_TIMEOUT_MS   100U      /* PollForConversion zaman aşımı    */

/* ── Veri Yapısı ─────────────────────────────────────────────────── */
typedef struct {
    uint32_t raw;       /* Ham ADC değeri (0–4095)               */
    float    voltage;   /* Hesaplanan gerilim (V)                */
    float    percent;   /* Yüzde temsili (%)                     */
    uint8_t  valid;     /* 1: geçerli okuma, 0: hata             */
} SensorData;

/* ── Fonksiyon Prototipleri ──────────────────────────────────────── */

/**
 * @brief  Potansiyometre bağlı ADC kanalından tek ölçüm alır.
 *         Ham değeri gerilim (V) ve yüzdeye (%) dönüştürür.
 * @param  hadc   CubeMX tarafından oluşturulan ADC handle'ı (&hadc1)
 * @param  data   Sonuçların yazılacağı SensorData pointer'ı
 * @retval HAL_OK | HAL_TIMEOUT | HAL_ERROR
 */
HAL_StatusTypeDef sensor_read(ADC_HandleTypeDef *hadc, SensorData *data);

#endif /* SENSOR_H */
