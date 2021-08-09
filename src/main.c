/*
* Profesor: Gonzalo Sanchez.
* Alumno: Del Sancio, Leonardo.
* main.c
 */

/*==================[inclusions]=============================================*/

#include "main.h"

#include "board.h"

#include "MSE_OS_Core.h"

#include "sapi.h"  

#include "MSE_OS_API.h"

#include "MSE_OS_Semaforos.h"

#include "perifericos.h"

#include "MSE_OS_Tareas.h"


/*==================[macros and definitions]=================================*/

#define MILISEC		1000
#define tiempo		10000000
#define PRIORIDAD_0		0
#define PRIORIDAD_1		1
#define PRIORIDAD_2		2
#define PRIORIDAD_3		3

/*==================[Global data declaration]==============================*/


tarea g_sTarea1, g_sTarea2,g_sTarea3; // le damos prioridad 0
tarea g_sTarea4, g_sTarea5,g_sTarea6; // prioridad 1
tarea g_sTarea7, g_sTarea8,g_sTarea9; // prioridad 3

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[macros]=================================================*/


/*==================[internal functions definition]==========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void)  {
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / MILISEC);		//systick de 1ms
}


/*==================[Definicion de tareas para el OS]==========================*/
void tarea1(void)  {
	
	while (1) {
      uint32_t l = 0;
	for(l=0;l<10000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 1\r\n" );   
   Board_LED_Toggle(LED_1);
	}
}

void tarea2(void)  {
	
   	while (1) { 
      uint32_t l = 0;
	for(l=0;l<12000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 2\r\n" );  
  Board_LED_Toggle(LED_2); 
	}
}

void tarea3(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<15000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 3\r\n" );   
   Board_LED_Toggle(LED_3);
	}
}

void tarea4(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<18000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 4\r\n" );   
   Board_LED_Toggle(LED_VERDE);
	}
}

void tarea5(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<20000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 5\r\n" ); 
 Board_LED_Toggle(LED_ROJO);  
	}
}

void tarea6(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<22000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 6\r\n" );   
   Board_LED_Toggle(LED_AZUL);   
	}
}

void tarea7(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<25000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 7\r\n" );   
	}
}

void tarea8(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<27000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 8\r\n" );   
	}
}

void tarea9(void)  {
	
	while (1) {
	   uint32_t l = 0;
	for(l=0;l<30000000;l++)  
   {
   }
   uartWriteString( UART_USB, "Tarea 9\r\n" );   
	}
}


/*============================================================================*/

int main(void)  {

	initHardware();
   //boardConfig		 (							);
   uartConfig		 ( UART_USB, 115200			);
   

   os_InitTarea(tarea1, &g_sTarea1,PRIORIDAD_0);
	os_InitTarea(tarea2, &g_sTarea2,PRIORIDAD_0);
	os_InitTarea(tarea3, &g_sTarea3,PRIORIDAD_0);
   os_InitTarea(tarea4, &g_sTarea4,PRIORIDAD_1);
	os_InitTarea(tarea5, &g_sTarea5,PRIORIDAD_1);
	os_InitTarea(tarea6, &g_sTarea6,PRIORIDAD_1);
	os_InitTarea(tarea7, &g_sTarea7,PRIORIDAD_3);
   os_InitTarea(tarea8, &g_sTarea8,PRIORIDAD_3);
   os_InitTarea(tarea9, &g_sTarea9,PRIORIDAD_3);


	os_Init();

	while (1) {
    
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
