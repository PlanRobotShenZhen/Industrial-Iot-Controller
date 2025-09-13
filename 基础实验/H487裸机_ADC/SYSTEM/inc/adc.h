
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_adc.h"
#include "n32h47x_48x_adc.h"
extern __IO uint16_t ADCConvertedValue;
void ADC_init(void);

#define DMA_CHANNEL_USED DMA1_CH1
#define DMA_IRQ_HANDLER  DMA1_Channel1_IRQHandler
#define DMA_IT_FLAG_TC   DMA1_INT_TXC1
#define DMA_IT_FLAG_GL   DMA1_INT_GLB1
#define DMA_IRQN         DMA1_Channel1_IRQn

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

