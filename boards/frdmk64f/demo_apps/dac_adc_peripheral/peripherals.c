/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v5.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 4.0.0
board: FRDM-K64F
functionalGroups:
- name: BOARD_InitPeripherals
  called_from_default_init: true
  id_prefix: BOARD_
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * ADC16 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'ADC16'
- type: 'adc16'
- mode: 'ADC'
- type_id: 'adc16_7d827be2dc433dc756d94a7ce88cbcc5'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'ADC0'
- config_sets:
  - fsl_adc16:
    - adc16_config:
      - referenceVoltageSource: 'kADC16_ReferenceVoltageSourceVref'
      - clockSource: 'kADC16_ClockSourceAsynchronousClock'
      - enableAsynchronousClock: 'true'
      - clockDivider: 'kADC16_ClockDivider8'
      - resolution: 'kADC16_ResolutionSE12Bit'
      - longSampleMode: 'kADC16_LongSampleDisabled'
      - enableHighSpeed: 'false'
      - enableLowPower: 'false'
      - enableContinuousConversion: 'false'
    - adc16_channel_mux_mode: 'kADC16_ChannelMuxA'
    - adc16_hardware_compare_config:
      - hardwareCompareModeEnable: 'false'
    - doAutoCalibration: 'false'
    - trigger: 'false'
    - hardwareAverageConfiguration: 'kADC16_HardwareAverageDisabled'
    - enable_irq: 'true'
    - adc_interrupt:
      - IRQn: 'ADC0_IRQn'
      - enable_priority: 'false'
      - enable_custom_name: 'true'
      - handler_custom_name: 'BOARD_ADC16_IRQ_HANDLER_FUNC'
    - adc16_channels_config:
      - 0:
        - enableDifferentialConversion: 'false'
        - channelNumber: 'SE.23'
        - enableInterruptOnConversionCompleted: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
adc16_channel_config_t BOARD_ADC16_channelsConfig[1] = {
    {.channelNumber = 23U, .enableDifferentialConversion = false, .enableInterruptOnConversionCompleted = true}};
const adc16_config_t BOARD_ADC16_config            = {.referenceVoltageSource     = kADC16_ReferenceVoltageSourceVref,
                                           .clockSource                = kADC16_ClockSourceAsynchronousClock,
                                           .enableAsynchronousClock    = true,
                                           .clockDivider               = kADC16_ClockDivider8,
                                           .resolution                 = kADC16_ResolutionSE12Bit,
                                           .longSampleMode             = kADC16_LongSampleDisabled,
                                           .enableHighSpeed            = false,
                                           .enableLowPower             = false,
                                           .enableContinuousConversion = false};
const adc16_channel_mux_mode_t BOARD_ADC16_muxMode = kADC16_ChannelMuxA;
const adc16_hardware_average_mode_t BOARD_ADC16_hardwareAverageMode = kADC16_HardwareAverageDisabled;

void BOARD_ADC16_init(void)
{
    /* Enable interrupt ADC0_IRQn request in the NVIC */
    EnableIRQ(ADC0_IRQn);
    /* Initialize ADC16 converter */
    ADC16_Init(BOARD_ADC16_PERIPHERAL, &BOARD_ADC16_config);
    /* Make sure, that software trigger is used */
    ADC16_EnableHardwareTrigger(BOARD_ADC16_PERIPHERAL, false);
    /* Configure hardware average mode */
    ADC16_SetHardwareAverage(BOARD_ADC16_PERIPHERAL, BOARD_ADC16_hardwareAverageMode);
    /* Configure channel multiplexing mode */
    ADC16_SetChannelMuxMode(BOARD_ADC16_PERIPHERAL, BOARD_ADC16_muxMode);
}

/***********************************************************************************************************************
 * DAC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'DAC'
- type: 'dac'
- mode: 'basic'
- type_id: 'dac_a54f338a6fa6fd273bc89d61f5a3b85e'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'DAC0'
- config_sets:
  - fsl_dac:
    - dac_config:
      - referenceVoltageSource: 'kDAC_ReferenceVoltageSourceVref2'
      - enableLowPowerMode: 'false'
    - dac_enable: 'true'
    - dac_value: '0'
    - quick_selection: 'QS_DAC_1'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const dac_config_t BOARD_DAC_config = {.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2,
                                       .enableLowPowerMode     = false};

void BOARD_DAC_init(void)
{
    /* Initialize DAC converter */
    DAC_Init(BOARD_DAC_PERIPHERAL, &BOARD_DAC_config);
    /* Output value of DAC. */
    DAC_SetBufferValue(BOARD_DAC_PERIPHERAL, 0U, 0U);
    /* Make sure the read pointer is set to the start */
    DAC_SetBufferReadPointer(BOARD_DAC_PERIPHERAL, 0U);
    /* Enable DAC output */
    DAC_Enable(BOARD_DAC_PERIPHERAL, true);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
    /* Initialize components */
    BOARD_ADC16_init();
    BOARD_DAC_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
    BOARD_InitPeripherals();
}
