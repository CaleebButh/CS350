/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uart2echo.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define STATE_IDLE 0
#define STATE_O 1
#define STATE_ON 2
#define STATE_OF 3

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char input;
    uint8_t state = STATE_IDLE;
    UART2_Handle uart;
    UART2_Params uartParams;
    size_t bytesRead;
    uint32_t status = UART2_STATUS_SUCCESS;

    GPIO_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;
    uart = UART2_open(CONFIG_UART2_0, &uartParams);
    if (uart == NULL)
    {
        while (1) {}
    }

    while (1)
    {
        bytesRead = 0;
        while (bytesRead == 0)
        {
            status = UART2_read(uart, &input, 1, &bytesRead);
            if (status != UART2_STATUS_SUCCESS)
            {
                while (1) {}
            }
        }

        switch (state)
        {
            case STATE_IDLE:
                if (input == 'O' || input == 'o') state = STATE_O;
                break;
            case STATE_O:
                if (input == 'N' || input == 'n')
                {
                    state = STATE_ON;
                    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                }
                else if (input == 'F' || input == 'f') state = STATE_OF;
                else state = STATE_IDLE;
                break;
            case STATE_ON:
                if (input == 'O' || input == 'o') state = STATE_O;
                else state = STATE_IDLE;
                break;
            case STATE_OF:
                if (input == 'F' || input == 'f')
                {
                    state = STATE_IDLE;
                    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                }
                else state = STATE_IDLE;
                break;
        }
    }
}
