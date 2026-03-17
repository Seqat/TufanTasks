/* uart_log.h
 * STM32F103C8T6 — Hafta 4: Araç Sensör Panosu
 * UART yardımcı fonksiyon prototipleri
 *
 * NEDEN kendi fonksiyonlarımızı yazıyoruz?
 *   Standart printf() bir işletim sistemi ve dinamik bellek yönetimi
 *   gerektirir. STM32 gibi bare-metal sistemlerde bunlar yoktur.
 *   HAL_UART_Transmit() yalnızca byte dizisi gönderebildiğinden,
 *   sprintf() ile tampona formatladıktan sonra bu fonksiyonla iletiriz.
 */

#ifndef UART_LOG_H
#define UART_LOG_H

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

/* ── Sabitler ────────────────────────────────────────────────────── */
#define UART_TX_TIMEOUT_MS  100U    /* HAL_UART_Transmit zaman aşımı */
#define UART_BUF_SIZE       128U    /* Tampon boyutu (byte)           */

/* ── Fonksiyon Prototipleri ──────────────────────────────────────── */

/**
 * @brief  Sabit karakter dizisini UART üzerinden gönderir.
 * @param  huart  UART handle'ı (&huart1)
 * @param  msg    Gönderilecek null-terminated string
 */
void UART_Print(UART_HandleTypeDef *huart, const char *msg);

/**
 * @brief  Etiket + float değerini formatlanmış şekilde UART'a gönderir.
 *         Örnek çıktı:  "Gerilim        : 2.20 V\r\n"
 * @param  huart     UART handle'ı
 * @param  label     Satır başı etiketi (örn. "Gerilim        : ")
 * @param  val       Gönderilecek float değer
 * @param  decimals  Ondalık basamak sayısı (1–3)
 */
void UART_PrintFloat(UART_HandleTypeDef *huart, const char *label,
                     float val, uint8_t decimals);

/**
 * @brief  Etiket + uint32 değerini UART'a gönderir.
 * @param  huart   UART handle'ı
 * @param  label   Satır başı etiketi
 * @param  val     Gönderilecek tam sayı değer
 */
void UART_PrintUint(UART_HandleTypeDef *huart, const char *label,
                    uint32_t val);

#endif /* UART_LOG_H */
