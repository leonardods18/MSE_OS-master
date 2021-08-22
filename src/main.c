/*
 * main.c
 * IMPORTANTE: Leer archivo READMI.md 
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo 
 */

/*==================[inclusions]=============================================*/

#include "main.h"
#include "os.h"

#include "os_semaforos.h"
#include "perifericosIRQ.h"
#include "ciaaUART.h"

#include <stdint.h>
#include <stdlib.h>

/*==================[macros and definitions]=================================*/
/*==================[macros and definitions]=================================*/

#define MILISEC		1000
#define tiempo		10000000
#define PRIORIDAD_0		0
#define PRIORIDAD_1		1
#define PRIORIDAD_2		2
#define PRIORIDAD_3		3



enum estados_pulsadores {
         stSUELTO, 
      stPRESIONADO
};

/*==================[internal data declaration]==============================*/

struct _dataLed{
	uint8_t Led;				// led a encender puede ser LEDS_VERDE, LEDS_ROJO ,
								// LEDS_AMARILLO o LEDS_RGB_AZUL
	uint16_t delta_suma;

};

typedef struct _dataLed dataLed;

typedef struct {
	pulsadores_e pulsador;
	uint8_t estado;
	uint8_t flanco;
	uint32_t tiempo_inicio_flanco_ascendente;
	uint32_t tiempo_inicio_flanco_descendente;
	uint8_t error;
} Pulsadores_t;

static Pulsadores_t pulsador[_PULSADORES_HABILITADOS_];

char str1[_LONG_LINEA_UART_];

/*==================[internal functions declaration]=========================*/

static void pulsador_inicializar(Pulsadores_t * p, pulsadores_e n);

void * tarea_estoyVivo(void * arg);

void * tarea_actualizar_pulsador_1(void * arg);
void * tarea_actualizar_pulsador_2(void * arg);
void analizar_pulsadores(void);
void * tarea_notificar_UART(void * arg);

/*==================[internal data definition]===============================*/

Semaforo_t s_actualizar_pulsador_1;
Semaforo_t s_actualizar_pulsador_2;
Semaforo_t s_notificar_UART;

osCola bufferLed;	

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void GPIO6_IRQHandler(void) {       // INTERRUPCION GPIO6

	pulsador[0].flanco = Periferico_Get_Pulsador(pulsador[0].pulsador);
	if (pulsador[0].flanco)
		if (pulsador[0].tiempo_inicio_flanco_descendente == 0)
			pulsador[0].tiempo_inicio_flanco_descendente = get_tick();
		else
			pulsador[0].error = 1;
	else
		if (pulsador[0].tiempo_inicio_flanco_ascendente == 0)
			pulsador[0].tiempo_inicio_flanco_ascendente = get_tick();
		else
			pulsador[0].error = 1;

	semaforo_give(&s_actualizar_pulsador_1);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(6)); // INT0 (canal 0 -> hanlder GPIO0)
	NVIC_ClearPendingIRQ(PIN_INT6_IRQn);
}

void GPIO7_IRQHandler(void) {          // INTERRUPCION GPIO7

	pulsador[1].flanco = Periferico_Get_Pulsador(pulsador[1].pulsador);
	if (pulsador[1].flanco)
		if (pulsador[1].tiempo_inicio_flanco_descendente == 0)
			pulsador[1].tiempo_inicio_flanco_descendente = get_tick();
		else
			pulsador[1].error = 1;
	else
		if (pulsador[1].tiempo_inicio_flanco_ascendente == 0)
			pulsador[1].tiempo_inicio_flanco_ascendente = get_tick();
		else
			pulsador[1].error = 1;

	semaforo_give(&s_actualizar_pulsador_2);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(7)); // INT0 (canal 0 -> hanlder GPIO0)
	NVIC_ClearPendingIRQ(PIN_INT7_IRQn);
}

static
void pulsador_inicializar(Pulsadores_t * p, pulsadores_e n) {
	p->pulsador = n;
	p->estado = stSUELTO;
	p->flanco = 0;
	p->tiempo_inicio_flanco_ascendente = 0;
	p->tiempo_inicio_flanco_descendente = 0;
	p->error = 0;
}

void * tarea_estoyVivo(void * arg) {
	while (1) {		

		Board_LED_Toggle(LED_1);
		task_delay(1000);
	}
}

void * tarea_actualizar_pulsador_1(void * arg) {
	while (1) {

		semaforo_take(&s_actualizar_pulsador_1);

		// Detectar antirrebotes (no hace falta porque esta por HW tambien)
		task_delay(50);

		uint8_t flanco = Periferico_Get_Pulsador(pulsador[0].pulsador);
		if (flanco == pulsador[0].flanco) {

			switch (pulsador[0].estado) {
			case stSUELTO:
				pulsador[0].estado = flanco ? stPRESIONADO : stSUELTO;
				break;
			case stPRESIONADO:
				pulsador[0].estado = flanco ? stPRESIONADO : stSUELTO;
				break;
			default:
				pulsador[0].estado = stSUELTO;
				break;
			}

			if (pulsador[0].estado == stSUELTO)
				analizar_pulsadores();
		}
	}
}

void * tarea_actualizar_pulsador_2(void * arg) {
	
   while (1) {

		semaforo_take(&s_actualizar_pulsador_2);

		// Detectar antirrebotes (no hace falta porque esta por HW tambien)
		task_delay(50);

		uint8_t flanco = Periferico_Get_Pulsador(pulsador[1].pulsador);
		if (flanco == pulsador[1].flanco) {

			switch (pulsador[1].estado) {
			case stSUELTO:
				pulsador[1].estado = flanco ? stPRESIONADO : stSUELTO;
				break;
			case stPRESIONADO:
				pulsador[1].estado = flanco ? stPRESIONADO : stSUELTO;
				break;
			default:
				pulsador[1].estado = stSUELTO;
				break;
			}

			if (pulsador[1].estado == stSUELTO)
				analizar_pulsadores();
		}
	}
}

void analizar_pulsadores(void){


	dataLed datoToLed;
	uint8_t statusLedAux;


	if (pulsador[0].estado == stSUELTO && pulsador[1].estado == stSUELTO){

		int tiempo_total = 0;
		int tiempo_entre_flancos_ascendentes = 0;
		int tiempo_entre_flancos_descendentes = 0;

		bzero(str1, _LONG_LINEA_UART_);

		char str2[_LONG_LINEA_UART_ / 2];
		bzero(str2, _LONG_LINEA_UART_ / 2);

		if (pulsador[0].error == 1								||
			pulsador[1].error == 1								||
			pulsador[0].tiempo_inicio_flanco_ascendente == 0	||
			pulsador[0].tiempo_inicio_flanco_descendente == 0	||
			pulsador[1].tiempo_inicio_flanco_ascendente == 0	||
			pulsador[1].tiempo_inicio_flanco_descendente == 0 	){

			strcpy(str1,"ERROR - Reintentar de nuevo por favor\r\n");   // si toco solo 1 botón y lo suelto, o bien si nunca suelto el primer botón 
		}
		else{
			if ( pulsador[0].tiempo_inicio_flanco_descendente <= pulsador[1].tiempo_inicio_flanco_descendente ){ // verde tambien puede ser rojo
				
				tiempo_entre_flancos_descendentes = pulsador[1].tiempo_inicio_flanco_descendente - pulsador[0].tiempo_inicio_flanco_descendente;
				if ( pulsador[0].tiempo_inicio_flanco_ascendente <= pulsador[1].tiempo_inicio_flanco_ascendente ){					
					strcpy(str1, "Led Verde encendido: \n\r");
					statusLedAux=LED_3;
               
               // escribo en la cola
               os_ColaWrite(&bufferLed,&datoToLed);               
					
               tiempo_entre_flancos_ascendentes = pulsador[1].tiempo_inicio_flanco_ascendente - pulsador[0].tiempo_inicio_flanco_ascendente;
				}else{					
					
               strcpy(str1, "Led Rojo encendido: \n\r");
					statusLedAux=LED_2;               
					tiempo_entre_flancos_ascendentes = pulsador[0].tiempo_inicio_flanco_ascendente - pulsador[1].tiempo_inicio_flanco_ascendente;
				}
			}else{
				
				tiempo_entre_flancos_descendentes = pulsador[0].tiempo_inicio_flanco_descendente - pulsador[1].tiempo_inicio_flanco_descendente; // azul o amarillo
				if ( pulsador[1].tiempo_inicio_flanco_ascendente <= pulsador[0].tiempo_inicio_flanco_ascendente ){					
					strcpy(str1, "Led Azul encendido: \n\r");
               statusLedAux=LED_AZUL;					
					tiempo_entre_flancos_ascendentes = pulsador[0].tiempo_inicio_flanco_ascendente - pulsador[1].tiempo_inicio_flanco_ascendente;
				}else{					
					strcpy(str1, "Led Amarillo encendido: \n\r");					
               statusLedAux=LED_1;
					tiempo_entre_flancos_ascendentes = pulsador[1].tiempo_inicio_flanco_ascendente - pulsador[0].tiempo_inicio_flanco_ascendente;
				}
			}
			tiempo_total =  tiempo_entre_flancos_ascendentes + tiempo_entre_flancos_descendentes;

         if(statusLedAux!=0){
			// Informo que se enciendan los led
			datoToLed.delta_suma=tiempo_total;
			datoToLed.Led=statusLedAux;			
            
         }
			sprintf(str2,"\t Tiempo encendido: %d ms\n\r", tiempo_total);
			strcat(str1, str2);
			sprintf(str2,"\t Tiempo entre flancos descendentes: %d ms\n\r", tiempo_entre_flancos_descendentes);
			strcat(str1, str2);
			sprintf(str2,"\t Tiempo entre flancos ascendentes: %d ms\n\r\n\r", tiempo_entre_flancos_ascendentes);
			strcat(str1, str2);
		}
		semaforo_give(&s_notificar_UART);

		pulsador[0].error = 0;
		pulsador[1].error = 0;
		pulsador[0].tiempo_inicio_flanco_ascendente = 0;
		pulsador[0].tiempo_inicio_flanco_descendente = 0;
		pulsador[1].tiempo_inicio_flanco_ascendente = 0;
		pulsador[1].tiempo_inicio_flanco_descendente = 0;
	}
}

void * tarea_notificar_UART(void * arg) {
	while (1) {
		semaforo_take(&s_notificar_UART);

		uartSend(str1, strlen(str1));
	}
}

void tareaLed(void)  {
	dataLed datoToLed;

	while (1) {
      //leo la cola
		os_ColaRead(&bufferLed,&datoToLed);
		Board_LED_Set(datoToLed.Led,TRUE);
		task_delay(datoToLed.delta_suma);
		Board_LED_Set(datoToLed.Led,FALSE);
	}
}


/*==================[external functions definition]==========================*/

int main(void) {
	Board_Init();
	Perifericos_Init();
	Pulsadores_Init();
	uartInit();

	pulsador_inicializar(&pulsador[0], PULSADOR_1);
	pulsador_inicializar(&pulsador[1], PULSADOR_2);

	char str[50];
	strcpy(str, "\n\r\n\rBIENVENIDO - SISTEMA MEDIDOR DE FLANCOS\r\n");
	uartSend(str, strlen(str));

	
	osTaskCreate(tarea_actualizar_pulsador_1 , STACK_SIZE_B, ePrioMED, (void*) 0x11223344);
	osTaskCreate(tarea_actualizar_pulsador_2 , STACK_SIZE_B, ePrioMED, (void*) 0x11223344);
	osTaskCreate(tarea_notificar_UART        , STACK_SIZE_B, ePrioMED, (void*) 0x11223344);

	semaforo_init(&s_actualizar_pulsador_1);
	semaforo_init(&s_actualizar_pulsador_2);
	semaforo_init(&s_notificar_UART);
	
   os_ColaInit(&bufferLed,sizeof(dataLed));
   
	initOS();

	while (1) {
		__WFI();
	}
}

/*==================[end of file]============================================*/
