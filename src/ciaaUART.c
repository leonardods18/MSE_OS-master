#include "ciaaUART.h"
#include "os.h"

uint8_t rxbuf[UART_BUF_SIZE];
uint8_t txbuf[UART_BUF_SIZE];

RINGBUFF_T rrb;
RINGBUFF_T trb;

void UART2_IRQHandler(void)
{
	// Función de interrupcion tanto para enviar como para recibir

	Chip_UART_IRQRBHandler(LPC_USART2, &rrb, &trb);
	
}


void uartInit(void)
{
	/* UART2 (USB-UART) */
	Chip_UART_Init(LPC_USART2);
	Chip_UART_SetBaud(LPC_USART2, 115200);

	Chip_UART_TXEnable(LPC_USART2);

	Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              
	Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); 
	Chip_UART_IntEnable(LPC_USART2, UART_IER_RBRINT);

 
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);

	RingBuffer_Init(&rrb, rxbuf, 1, UART_BUF_SIZE);
	RingBuffer_Init(&trb, txbuf, 1, UART_BUF_SIZE);
}

uint32_t uartSend(void * data, int datalen)
{
	return Chip_UART_SendRB(LPC_USART2, &trb, data, datalen);
}

uint32_t uartRecv(void * data, int datalen)
{
	return Chip_UART_ReadRB(LPC_USART2, &rrb, data, datalen);
}
