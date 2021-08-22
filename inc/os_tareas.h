/*
 * os_tareas.h
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */

#ifndef _OS_TAREAS_H_
#define _OS_TAREAS_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>

/*==================[macros]=================================================*/

#define maximas_tareas 10
#define STACK_SIZE_B   1024



/*==================[definicion codigos de error y warning de OS]=================================*/
#define ERR_OS_CANT_TAREAS		-1
#define ERR_OS_SCHEDULING		-2
#define ERR_OS_DELAY_FROM_ISR	-3

#define WARN_OS_QUEUE_FULL_ISR	-100
#define WARN_OS_QUEUE_EMPTY_ISR	-101

typedef void * ( * task_type)(void *);

typedef enum {
	eRunning = 0, 
	eReady, 
	eWaiting, 
} eTaskState;

enum _estadoOS  {
	OS_FROM_RESET,				//inicio luego de un reset
	OS_NORMAL_RUN,				//estado del sistema corriendo una tarea
	OS_SCHEDULING,				//el OS esta efectuando un scheduling
	OS_IRQ_RUN					//El OS esta corriendo un Handler

};

typedef enum {
	ePrioMAX = 1,
	ePrioMED,
	ePrioMIN,
} eTaskPrioridades;

typedef struct {
	task_type entry_point;
	uint32_t stack_size_b;
	eTaskPrioridades prioridad;
	void * arg;
	uint32_t stack[STACK_SIZE_B / 4]; 
	uint32_t sp;
	eTaskState estado;
	uint32_t delay;
} TaskParameters_t;

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_TAREAS_H_ */
