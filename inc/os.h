/*
 * os.h
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */

#ifndef OS_H_
#define OS_H_


/*==================[inclusions]=============================================*/

#include "board.h"
#include "os_tareas.h"


/************************************************************************************
 * 	Posiciones dentro del stack de los registros que lo conforman
 ***********************************************************************************/

#define XPSR			1
#define PC_REG			2
#define LR				3
#define R12				4
#define R3				5
#define R2				6
#define R1				7
#define R0				8
#define LR_PREV_VALUE	9
#define R4				10
#define R5				11
#define R6				12
#define R7				13
#define R8				14
#define R9				15
#define R10 			16
#define R11 			17

//----------------------------------------------------------------------------------
/************************************************************************************
 * 						Definiciones varias
 ***********************************************************************************/
#define STACK_FRAME_SIZE			8
#define FULL_STACKING_SIZE 			17	//16 core registers + valor previo de LR

#define TASK_NAME_SIZE				10	//tamaño array correspondiente al nombre
#define MAX_TASK_COUNT				8	//cantidad maxima de tareas para este OS

#define MAX_PRIORITY		0			//maxima prioridad que puede tener una tarea
#define MIN_PRIORITY		3			//minima prioridad que puede tener una tarea

#define PRIORITY_COUNT		(MIN_PRIORITY-MAX_PRIORITY)+1	//cantidad de prioridades asignables

#define QUEUE_HEAP_SIZE		64			//cantidad de bytes reservados por cada cola definida

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void initOS(void);
void osTaskCreate(task_type entry_point, uint32_t stack_size_b, eTaskPrioridades prioridad, void * arg);
void task_delay( uint32_t systick);
uint32_t get_tick(void);
TaskParameters_t * get_current_task(void);
void schedule(void);
void ini_SeccionCritica(void);
void fin_SeccionCritica(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/

#endif /* OS_H_ */
