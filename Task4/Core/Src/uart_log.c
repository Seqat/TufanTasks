/* uart_log.c
 * STM32F103C8T6 — Hafta 4: Araç Sensör Panosu
 * UART yardımcı fonksiyon gövdeleri
 */

#include "uart_log.h"

/* ── UART_Print ──────────────────────────────────────────────────── */
void UART_Print(UART_HandleTypeDef *huart, const char *msg)
{
    if (huart == NULL || msg == NULL) return;

    uint16_t len = (uint16_t)strlen(msg);
    if (len == 0) return;

    /* HAL_UART_Transmit: blocking (polling) gönderim */
    HAL_UART_Transmit(huart, (uint8_t *)msg, len, UART_TX_TIMEOUT_MS);
    /*
     * NOT: Dönüş değeri burada kontrol edilmiyor.
     * Hata yönetimi gerekirse:
     *   if (HAL_UART_Transmit(...) != HAL_OK) { ... }
     * şeklinde main.c tarafında ele alınabilir.
     */
}

/* ── UART_PrintFloat ─────────────────────────────────────────────── */
void UART_PrintFloat(UART_HandleTypeDef *huart, const char *label,
                     float val, uint8_t decimals)
{
    char buf[UART_BUF_SIZE];

    /*
     * snprintf: tampon taşmasına karşı güvenli sprintf
     * NOT: float desteği için linker'a -u _printf_float eklenmelidir.
     *      CubeMX Makefile projesinde:
     *        Makefile -> LDFLAGS satırına -u _printf_float ekleyin.
     */
    switch (decimals)
    {
        case 1:
            snprintf(buf, sizeof(buf), "%s%.1f\r\n", label, val);
            break;
        case 2:
            snprintf(buf, sizeof(buf), "%s%.2f\r\n", label, val);
            break;
        default:
            snprintf(buf, sizeof(buf), "%s%.3f\r\n", label, val);
            break;
    }

    UART_Print(huart, buf);
}

/* ── UART_PrintUint ──────────────────────────────────────────────── */
void UART_PrintUint(UART_HandleTypeDef *huart, const char *label,
                    uint32_t val)
{
    char buf[UART_BUF_SIZE];
    snprintf(buf, sizeof(buf), "%s%lu\r\n", label, (unsigned long)val);
    UART_Print(huart, buf);
}
