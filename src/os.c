/*
 * os.c
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */

/*==================[inclusions]=============================================*/

#include "os.h"
#include <string.h>

/*==================[macros and definitions]=================================*/

#define ePrioIDLE	   0
#define sinAsignar	   -1

/*==================[internal data declaration]==============================*/

int32_t puntero_proxima_tarea = 0;
TaskParameters_t tareas[maximas_tareas];

int32_t puntero_proxima_tarea_prioMAX = 0;
int32_t tareasPrioMAX[maximas_tareas];

int32_t puntero_proxima_tarea_prioMED = 0;
int32_t tareasPrioMED[maximas_tareas];
//
int32_t puntero_proxima_tarea_prioMIN = 0;
int32_t tareasPrioMIN[maximas_tareas];

uint32_t * contexto_next_task_sp = 0;

int32_t current_task = sinAsignar;

TaskParameters_t tareaIDLE;

uint32_t contador_tick = 0;

/*==================[internal functions declaration]=========================*/

static void initHardware(void);

void __attribute__((weak)) task_return_hook(void * ret_val);

void init_stack(uint32_t stack[], uint32_t stack_size, uint32_t * sp,
		task_type entry_point, void * arg);

uint32_t get_next_context(uint32_t current_sp);

void * task_idle(void * arg);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	// Se le da la menor prioridad de interrupcion.
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/*************************************************************************************************
	 *  @brief Hook de retorno de tareas
     *
     *  @details
     *   Esta funcion no deberia accederse bajo ningun concepto, porque ninguna tarea del OS
     *   debe retornar. Si lo hace, es un comportamiento anormal y debe ser tratado.
     *
	 *  @param none
	 *
	 *  @return none.
***************************************************************************************************/
void __attribute__((weak)) task_return_hook(void * ret_val) {
	while (1) {
		__WFI();
	}
}

/*************************************************************************************************
	 *  @brief Inicializa las tareas que correran en el OS.
     *
     *  @details
     *   Inicializa una tarea para que pueda correr en el OS implementado.
     *   Es necesario llamar a esta funcion para cada tarea antes que inicie
     *   el OS.
     *
	 *  @param *entryPoint		Puntero a la tarea que se desea inicializar.
	 *  @param *task			Puntero a la estructura de control que sera utilizada para
	 *  						la tarea que se esta inicializando.
	 *  @return     None.
***************************************************************************************************/

void init_stack(uint32_t stack[], uint32_t stack_size_b, uint32_t * sp,
		task_type entry_point, void * arg) {

	bzero(stack, stack_size_b);

	stack[stack_size_b / 4 - XPSR] = 1 << 24; /* xPSR.T = 1 		*/ // Posición 1
	stack[stack_size_b / 4 - PC_REG] = (uint32_t) entry_point; /* Program Counter	*/ // Posición 2
	stack[stack_size_b / 4 - LR] = (uint32_t) task_return_hook; /* LR				*/ // Apunta a la dirección de retorno
	stack[stack_size_b / 4 - 8] = (uint32_t) arg; /* R0				*/ // Para salvar el argunmento	
	stack[stack_size_b / 4 - 9] = 0xFFFFFFF9; /* Valor de retorno de excepcion a cargar en el LR.	0xFFFFFFF9 / 0xFFFFFFF0 / 0xFFFFFFF1 */

	*sp = (uint32_t) &(stack[stack_size_b / 4 - 17]);// ahora tengo que sumar uno mas al stack por el LR IRO
}

uint32_t get_next_context(uint32_t current_sp) {
	
	if (tareaIDLE.estado == eRunning) {
		tareaIDLE.sp = current_sp;
		tareaIDLE.estado = eReady;
	} else if (current_task != sinAsignar) { // no es el main
		tareas[current_task].sp = current_sp;
		switch (tareas[current_task].estado) {
		case eRunning: // Situacion normal
			tareas[current_task].estado = eReady;
			break;
		case eWaiting: // Esperando un Delay o Semaforo
			
			break;
		case eReady:
			
			break;
		default:
			while (1) {
				__WFI();
			}
		}
	}

	
	puntero_proxima_tarea_prioMAX = 0;
	puntero_proxima_tarea_prioMED = 0;
	puntero_proxima_tarea_prioMIN = 0;

	int indiceTarea = current_task;
	int t;
	for (t = 0; t < puntero_proxima_tarea; ++t) {

		// Round-Robin 
		indiceTarea = (indiceTarea + 1) % (puntero_proxima_tarea);

		if (tareas[indiceTarea].estado == eReady) {
			switch (tareas[indiceTarea].prioridad) {
			case ePrioMAX:
				tareasPrioMAX[puntero_proxima_tarea_prioMAX++] = indiceTarea;
				break;
			case ePrioMED:
				tareasPrioMED[puntero_proxima_tarea_prioMED++] = indiceTarea;
				break;
			case ePrioMIN:
				tareasPrioMIN[puntero_proxima_tarea_prioMIN++] = indiceTarea;
				break;
			default:
				while (1) {
					__WFI();
				}
			}
		}
	}

	current_task = sinAsignar;

	
	if (current_task == sinAsignar && puntero_proxima_tarea_prioMAX > 0)
		current_task = tareasPrioMAX[0];

	if (current_task == sinAsignar && puntero_proxima_tarea_prioMED > 0)
		current_task = tareasPrioMED[0];

	if (current_task == sinAsignar && puntero_proxima_tarea_prioMIN > 0)
		current_task = tareasPrioMIN[0];
	
	if (current_task == sinAsignar) { // idle
		contexto_next_task_sp = &tareaIDLE.sp;
		tareaIDLE.estado = eRunning;
	} else {
		contexto_next_task_sp = &tareas[current_task].sp;
		tareas[current_task].estado = eRunning;
	}


	return *contexto_next_task_sp;
}

void SysTick_Handler(void) {
	
	contador_tick++;
	int t;
	for (t = 0; t < puntero_proxima_tarea; ++t)
		if (tareas[t].delay > 0)
			if (--tareas[t].delay <= 0)
				tareas[t].estado = eReady;

	schedule();
}

void schedule() {
	

	// Aseguramos que se ejecuten todas las interrupciones en el pipeline
	__ISB();
	// Aseguramos que se completen todos los accesos a memoria
	__DSB();

	// Activo PendSV para llevar a cabo el cambio de contexto
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void * task_idle(void * arg) {
	while (1) {
		__WFI();
	}
}

/*==================[external functions definition]==========================*/
/*************************************************************************************************
	 *  @brief Inicializa el OS.
     *
     *  @details
     *   Inicializa el OS seteando la prioridad de PendSV como la mas baja posible. Es necesario
     *   llamar esta funcion antes de que inicie el sistema. Es mandatorio llamarla luego de
     *   inicializar las tareas
     *
	 *  @param 		None.
	 *  @return     None.
***************************************************************************************************/
void initOS(void) {

	initHardware();
	
	tareaIDLE.entry_point = (task_type) task_idle;
	tareaIDLE.stack_size_b = STACK_SIZE_B;
	tareaIDLE.prioridad = ePrioIDLE;
	tareaIDLE.arg = 0;
	tareaIDLE.estado = eReady;

	init_stack(tareaIDLE.stack, tareaIDLE.stack_size_b, &tareaIDLE.sp,
			tareaIDLE.entry_point, tareaIDLE.arg);
	
	bzero(tareasPrioMAX, maximas_tareas);
	bzero(tareasPrioMED, maximas_tareas);
	bzero(tareasPrioMIN, maximas_tareas);
}

void osTaskCreate(task_type entry_point, uint32_t stack_size_b,
		eTaskPrioridades prioridad, void * arg) {
	if (puntero_proxima_tarea < maximas_tareas) {
		tareas[puntero_proxima_tarea].entry_point = entry_point;
		tareas[puntero_proxima_tarea].stack_size_b = stack_size_b;
		tareas[puntero_proxima_tarea].prioridad = prioridad;
		tareas[puntero_proxima_tarea].arg = arg;

		tareas[puntero_proxima_tarea].estado = eReady;

		init_stack(tareas[puntero_proxima_tarea].stack,
				tareas[puntero_proxima_tarea].stack_size_b,
				&tareas[puntero_proxima_tarea].sp,
				tareas[puntero_proxima_tarea].entry_point,
				tareas[puntero_proxima_tarea].arg);

		puntero_proxima_tarea++;
	}
}

void task_delay(uint32_t tick) {
	
	if (tick > 0) {
		tareas[current_task].delay = tick;
		tareas[current_task].estado = eWaiting;
	}
	schedule();
}

uint32_t get_tick(void) {
	return contador_tick;
}

TaskParameters_t * get_current_task(void){
	return &tareas[current_task];
}

void ini_SeccionCritica(void){
	__disable_irq();
}
void fin_SeccionCritica(void){
	__enable_irq();
}

/*==================[end of file]============================================*/

