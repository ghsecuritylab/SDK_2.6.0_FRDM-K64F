/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define SOPT5_UART0TXSRC_UART_TX 0x00u /*!<@brief UART 0 transmit data source select: UART0_TX pin */

/*! @name PORTB16 (number 62), U7[4]/UART0_RX
  @{ */
#define BOARD_DEBUG_UART_RX_PORT PORTB /*!<@brief PORT device name: PORTB */
#define BOARD_DEBUG_UART_RX_PIN 16U    /*!<@brief PORTB pin index: 16 */
                                       /* @} */

/*! @name PORTB17 (number 63), U10[1]/UART0_TX
  @{ */
#define BOARD_DEBUG_UART_TX_PORT PORTB /*!<@brief PORT device name: PORTB */
#define BOARD_DEBUG_UART_TX_PIN 17U    /*!<@brief PORTB pin index: 17 */
                                       /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

/*! @name PORTE24 (number 31), J2[20]/U8[4]/I2C0_SCL
  @{ */
#define BOARD_I2C_ACCEL_SCL_PORT PORTE /*!<@brief PORT device name: PORTE */
#define BOARD_I2C_ACCEL_SCL_PIN 24U    /*!<@brief PORTE pin index: 24 */
                                       /* @} */

/*! @name PORTE25 (number 32), J2[18]/U8[6]/I2C0_SDA
  @{ */
#define BOARD_I2C_ACCEL_SDA_PORT PORTE /*!<@brief PORT device name: PORTE */
#define BOARD_I2C_ACCEL_SDA_PIN 25U    /*!<@brief PORTE pin index: 25 */
                                       /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_I2C_ConfigurePins(void);

/*! @name PORTE24 (number 31), J2[20]/U8[4]/I2C0_SCL
  @{ */
#define BOARD_GPIO_ACCEL_SCL_GPIO GPIOE /*!<@brief GPIO device name: GPIOE */
#define BOARD_GPIO_ACCEL_SCL_PORT PORTE /*!<@brief PORT device name: PORTE */
#define BOARD_GPIO_ACCEL_SCL_PIN 24U    /*!<@brief PORTE pin index: 24 */
                                        /* @} */

/*! @name PORTE25 (number 32), J2[18]/U8[6]/I2C0_SDA
  @{ */
#define BOARD_GPIO_ACCEL_SDA_GPIO GPIOE /*!<@brief GPIO device name: GPIOE */
#define BOARD_GPIO_ACCEL_SDA_PORT PORTE /*!<@brief PORT device name: PORTE */
#define BOARD_GPIO_ACCEL_SDA_PIN 25U    /*!<@brief PORTE pin index: 25 */
                                        /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_GPIO_ConfigurePins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
