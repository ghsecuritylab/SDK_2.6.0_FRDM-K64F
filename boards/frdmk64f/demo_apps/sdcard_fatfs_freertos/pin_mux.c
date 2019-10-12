/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v5.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 5.0.0
board: FRDM-K64F
pin_labels:
- {pin_num: '73', pin_signal: CMP1_IN1/PTC3/LLWU_P7/SPI0_PCS1/UART1_RX/FTM0_CH2/CLKOUT/I2S0_TX_BCLK, label: 'J1[16]', identifier: DEBUG_UART_RX}
- {pin_num: '76', pin_signal: PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/FTM0_CH3/FB_AD11/CMP1_OUT, label: 'J2[4]', identifier: DEBUG_UART_TX}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '4', peripheral: SDHC, signal: CMD, pin_signal: ADC0_DM2/ADC1_SE7a/PTE3/SPI1_SIN/UART1_RTS_b/SDHC0_CMD/TRACE_D1/SPI1_SOUT, open_drain: disable, drive_strength: high,
    pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '2', peripheral: SDHC, signal: 'DATA, 0', pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN, open_drain: disable,
    drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '1', peripheral: SDHC, signal: 'DATA, 1', pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT, open_drain: disable,
    drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '6', peripheral: SDHC, signal: 'DATA, 2', pin_signal: PTE5/SPI1_PCS2/UART3_RX/SDHC0_D2/FTM3_CH0, open_drain: disable, drive_strength: high, pull_select: up,
    pull_enable: enable, passive_filter: disable}
  - {pin_num: '5', peripheral: SDHC, signal: 'DATA, 3', pin_signal: PTE4/LLWU_P2/SPI1_PCS0/UART3_TX/SDHC0_D3/TRACE_D0, open_drain: disable, drive_strength: high,
    pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '3', peripheral: SDHC, signal: DCLK, pin_signal: ADC0_DP2/ADC1_SE6a/PTE2/LLWU_P1/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/TRACE_D2, open_drain: disable, drive_strength: high,
    pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '7', peripheral: GPIOE, signal: 'GPIO, 6', pin_signal: PTE6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/FTM3_CH1/USB_SOF_OUT, open_drain: disable, drive_strength: low,
    pull_select: down, pull_enable: disable}
  - {pin_num: '62', peripheral: UART0, signal: RX, pin_signal: PTB16/SPI1_SOUT/UART0_RX/FTM_CLKIN0/FB_AD17/EWM_IN}
  - {pin_num: '63', peripheral: UART0, signal: TX, pin_signal: PTB17/SPI1_SIN/UART0_TX/FTM_CLKIN1/FB_AD16/EWM_OUT_b}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTB16 (pin 62) is configured as UART0_RX */
    PORT_SetPinMux(BOARD_DEBUG_UART_RX_PORT, BOARD_DEBUG_UART_RX_PIN, kPORT_MuxAlt3);

    /* PORTB17 (pin 63) is configured as UART0_TX */
    PORT_SetPinMux(BOARD_DEBUG_UART_TX_PORT, BOARD_DEBUG_UART_TX_PIN, kPORT_MuxAlt3);

    /* PORTE0 (pin 1) is configured as SDHC0_D1 */
    PORT_SetPinMux(BOARD_SDHC0_D1_PORT, BOARD_SDHC0_D1_PIN, kPORT_MuxAlt4);

    PORTE->PCR[0] =
        ((PORTE->PCR[0] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE1 (pin 2) is configured as SDHC0_D0 */
    PORT_SetPinMux(BOARD_SDHC0_D0_PORT, BOARD_SDHC0_D0_PIN, kPORT_MuxAlt4);

    PORTE->PCR[1] =
        ((PORTE->PCR[1] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE2 (pin 3) is configured as SDHC0_DCLK */
    PORT_SetPinMux(BOARD_SDHC0_DCLK_PORT, BOARD_SDHC0_DCLK_PIN, kPORT_MuxAlt4);

    PORTE->PCR[2] =
        ((PORTE->PCR[2] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE3 (pin 4) is configured as SDHC0_CMD */
    PORT_SetPinMux(BOARD_SDHC0_CMD_PORT, BOARD_SDHC0_CMD_PIN, kPORT_MuxAlt4);

    PORTE->PCR[3] =
        ((PORTE->PCR[3] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE4 (pin 5) is configured as SDHC0_D3 */
    PORT_SetPinMux(BOARD_SDHC0_D3_PORT, BOARD_SDHC0_D3_PIN, kPORT_MuxAlt4);

    PORTE->PCR[4] =
        ((PORTE->PCR[4] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE5 (pin 6) is configured as SDHC0_D2 */
    PORT_SetPinMux(BOARD_SDHC0_D2_PORT, BOARD_SDHC0_D2_PIN, kPORT_MuxAlt4);

    PORTE->PCR[5] =
        ((PORTE->PCR[5] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | (uint32_t)(kPORT_PullUp)

         /* Passive Filter Enable: Passive input filter is disabled on the corresponding pin. */
         | PORT_PCR_PFE(kPORT_PassiveFilterDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: High drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_HighDriveStrength));

    /* PORTE6 (pin 7) is configured as PTE6 */
    PORT_SetPinMux(BOARD_SDHC_CD_PORT, BOARD_SDHC_CD_PIN, kPORT_MuxAsGpio);

    PORTE->PCR[6] =
        ((PORTE->PCR[6] &
          /* Mask bits to zero which are setting */
          (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ODE_MASK | PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

         /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the corresponding PE
          * field is set. */
         | PORT_PCR_PS(kPORT_PullDown)

         /* Pull Enable: Internal pullup or pulldown resistor is not enabled on the corresponding pin. */
         | PORT_PCR_PE(kPORT_PullDisable)

         /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
         | PORT_PCR_ODE(kPORT_OpenDrainDisable)

         /* Drive Strength Enable: Low drive strength is configured on the corresponding pin, if pin is
          * configured as a digital output. */
         | PORT_PCR_DSE(kPORT_LowDriveStrength));

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/