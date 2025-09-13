/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file bsp_eth.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/ 

#ifndef __BSP_ETH_H__
#define __BSP_ETH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_eth.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"

/** ETH MAC address macro definition **/
#define MAC_ADDR0                               ((uint8_t)0x10U)
#define MAC_ADDR1                               ((uint8_t)0x32U)
#define MAC_ADDR2                               ((uint8_t)0x54U)
#define MAC_ADDR3                               ((uint8_t)0x76U)
#define MAC_ADDR4                               ((uint8_t)0x89U)
#define MAC_ADDR5                               ((uint8_t)0xBAU)

#define ETH_RX_BUFFER_SIZE                      (1536UL)



#define PHY_USE_DM9162EP

/** ETH External PHY Macro Definition **/
#ifdef  PHY_USE_DM9162EP
#define PHY_ADDR                                ((uint16_t)0x00U)

#define PHY_BCR                                 ((uint16_t)0x00U)   /* Basic Control Register */
#define PHY_BSR                                 ((uint16_t)0x01U)   /* Basic Status Register */

#define PHY_RESET                               ((uint16_t)0x8000U) /* PHY Reset */
#define PHY_LOOPBACK                            ((uint16_t)0x4000U) /* Select loop-back mode */
#define PHY_FULLDUPLEX_100M                     ((uint16_t)0x2100U) /* Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M                     ((uint16_t)0x2000U) /* Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M                      ((uint16_t)0x0100U) /* Set the full-duplex mode at 10 Mb/s */
#define PHY_HALFDUPLEX_10M                      ((uint16_t)0x0000U) /* Set the half-duplex mode at 10 Mb/s */
#define PHY_AUTONEGOTIATION                     ((uint16_t)0x1000U) /* Enable auto-negotiation function */
#define PHY_RESTART_AUTONEGOTIATION             ((uint16_t)0x0200U) /* Restart auto-negotiation function */
#define PHY_POWERDOWN                           ((uint16_t)0x0800U) /* Select the power down mode */
#define PHY_ISOLATE                             ((uint16_t)0x0400U) /* Isolate PHY from MII */

#define PHY_AUTONEGO_COMPLETE                   ((uint16_t)0x0020U) /* Auto-Negotiation process completed */
#define PHY_LINKED_STATUS                       ((uint16_t)0x0004U) /* Valid link established */
#define PHY_JABBER_DETECTION                    ((uint16_t)0x0002U) /* Jabber condition detected */

#define PHY_SDSR                                ((uint16_t)0x00U)   /* The offset of the status register indicating speed and duplex mode in auto-negotiation mode */

#define PHY_SPEED_MASK                          ((uint16_t)0x2000U) /* PHY Speed mask */
#define PHY_DUPLEX_MASK                         ((uint16_t)0x0100U) /* PHY Duplex mask */
#endif /* PHY_USE_DM9162EP */

#ifdef  PHY_USE_LAN8720
#define PHY_ADDR                                ((uint16_t)0x00U)

#define PHY_BCR                                 ((uint16_t)0x00U)   /* Basic Control Register */
#define PHY_BSR                                 ((uint16_t)0x01U)   /* Basic Status Register */

#define PHY_RESET                               ((uint16_t)0x8000U) /* PHY Reset */
#define PHY_LOOPBACK                            ((uint16_t)0x4000U) /* Select loop-back mode */
#define PHY_FULLDUPLEX_100M                     ((uint16_t)0x2100U) /* Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M                     ((uint16_t)0x2000U) /* Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M                      ((uint16_t)0x0100U) /* Set the full-duplex mode at 10 Mb/s */
#define PHY_HALFDUPLEX_10M                      ((uint16_t)0x0000U) /* Set the half-duplex mode at 10 Mb/s */
#define PHY_AUTONEGOTIATION                     ((uint16_t)0x1000U) /* Enable auto-negotiation function */
#define PHY_RESTART_AUTONEGOTIATION             ((uint16_t)0x0200U) /* Restart auto-negotiation function */
#define PHY_POWERDOWN                           ((uint16_t)0x0800U) /* Select the power down mode */
#define PHY_ISOLATE                             ((uint16_t)0x0400U) /* Isolate PHY from MII */

#define PHY_AUTONEGO_COMPLETE                   ((uint16_t)0x0020U) /* Auto-Negotiation process completed */
#define PHY_LINKED_STATUS                       ((uint16_t)0x0004U) /* Valid link established */
#define PHY_JABBER_DETECTION                    ((uint16_t)0x0002U) /* Jabber condition detected */

#define PHY_SDSR                                ((uint16_t)0x1FU)   /* The offset of the status register indicating speed and duplex mode in auto-negotiation mode */

#define PHY_SPEED_MASK                          ((uint16_t)0x0008U) /* PHY Speed mask */
#define PHY_DUPLEX_MASK                         ((uint16_t)0x0010U) /* PHY Duplex mask */
#endif /* PHY_USE_LAN8720 */

#ifdef  PHY_USE_SR8201F
#define PHY_ADDR                                ((uint16_t)0x01U)

#define PHY_BCR                                 ((uint16_t)0x00U)   /* Basic Control Register */
#define PHY_BSR                                 ((uint16_t)0x01U)   /* Basic Status Register */

#define PHY_RESET                               ((uint16_t)0x8000U) /* PHY Reset */
#define PHY_LOOPBACK                            ((uint16_t)0x4000U) /* Select loop-back mode */
#define PHY_FULLDUPLEX_100M                     ((uint16_t)0x2100U) /* Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M                     ((uint16_t)0x2000U) /* Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M                      ((uint16_t)0x0100U) /* Set the full-duplex mode at 10 Mb/s */
#define PHY_HALFDUPLEX_10M                      ((uint16_t)0x0000U) /* Set the half-duplex mode at 10 Mb/s */
#define PHY_AUTONEGOTIATION                     ((uint16_t)0x1000U) /* Enable auto-negotiation function */
#define PHY_RESTART_AUTONEGOTIATION             ((uint16_t)0x0200U) /* Restart auto-negotiation function */
#define PHY_POWERDOWN                           ((uint16_t)0x0800U) /* Select the power down mode */
#define PHY_ISOLATE                             ((uint16_t)0x0400U) /* Isolate PHY from MII */

#define PHY_AUTONEGO_COMPLETE                   ((uint16_t)0x0020U) /* Auto-Negotiation process completed */
#define PHY_LINKED_STATUS                       ((uint16_t)0x0004U) /* Valid link established */
#define PHY_JABBER_DETECTION                    ((uint16_t)0x0002U) /* Jabber condition detected */

#define PHY_SDSR                                ((uint16_t)0x00U)   /* The offset of the status register indicating speed and duplex mode in auto-negotiation mode */

#define PHY_SPEED_MASK                          ((uint16_t)0x2000U) /* PHY Speed mask */
#define PHY_DUPLEX_MASK                         ((uint16_t)0x0100U) /* PHY Duplex mask */
#endif /* PHY_USE_SR8201F */



void ETH_BSP_GPIOInit(ETH_InfoType* pInfo);
void ETH_BSP_NVICInit(ETH_InfoType* pInfo);
void ETH_BSP_ClockCmd(FunctionalStatus Cmd);
ErrorStatus ETH_BSP_Init(void);
void ETH_BSP_DeInit(void);



#ifdef __cplusplus
}
#endif

#endif /*__BSP_ETH_H__ */
