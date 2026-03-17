/* sensor.c
 * STM32F103C8T6 — Hafta 4: Araç Sensör Panosu
 * ADC okuma ve dönüşüm fonksiyon gövdeleri
 */

#include "sensor.h"

/* ── sensor_read ─────────────────────────────────────────────────── */
HAL_StatusTypeDef sensor_read(ADC_HandleTypeDef *hadc, SensorData *data)
{
    HAL_StatusTypeDef status;

    /* Güvenlik: pointer kontrolü */
    if (hadc == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    /* 1) ADC dönüşümünü başlat */
    status = HAL_ADC_Start(hadc);
    if (status != HAL_OK)
    {
        data->valid = 0;
        return status;
    }

    /* 2) Dönüşümün tamamlanmasını bekle (zaman aşımı: 100 ms) */
    status = HAL_ADC_PollForConversion(hadc, ADC_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        data->valid = 0;
        HAL_ADC_Stop(hadc);
        return status;   /* HAL_TIMEOUT durumunda çağıran taraf uyarı basar */
    }

    /* 3) Ham değeri oku */
    data->raw = HAL_ADC_GetValue(hadc);

    /* 4) Dönüşümler */
    data->voltage = (data->raw / ADC_MAX_VALUE) * ADC_VREF;
    data->percent = (data->raw / ADC_MAX_VALUE) * 100.0f;

    /* 5) Aralık kontrolü — donanım hatası tespiti */
    if (data->voltage < 0.0f || data->voltage > ADC_VREF)
    {
        data->valid = 0;
    }
    else
    {
        data->valid = 1;
    }

    HAL_ADC_Stop(hadc);
    return HAL_OK;
}
