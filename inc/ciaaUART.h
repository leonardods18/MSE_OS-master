
 
#ifndef CIAAUART_H_
#define CIAAUART_H_

#include "board.h"
#include "string.h"

#define UART_BUF_SIZE	512
#define UART_RX_FIFO_SIZE 16

void uartInit(void);
uint32_t uartSend(void * data, int datalen);
uint32_t uartRecv(void * data, int datalen);

#endif /* CIAAUART_H_ */
