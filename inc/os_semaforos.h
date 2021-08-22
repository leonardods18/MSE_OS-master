/*
 * os_semaforos.h
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */


#ifndef _OS_SEMAFORO_H_
#define _OS_SEMAFORO_H_

/*==================[inclusions]=============================================*/

#include "os_tareas.h"

#include <stdint.h>

/*==================[macros]=================================================*/

/** invalid event definition TODO: redefina esta macro a su criterio */
#define INVALID_EVENT NULL
#define QUEUE_HEAP_SIZE		64			//cantidad de bytes reservados por cada cola definida
/*==================[typedef]================================================*/

typedef struct {
	uint32_t tomado;
	TaskParameters_t * tarea;
} Semaforo_t;

/*==================[external data declaration]==============================*/
struct _cola  {
	uint8_t data[QUEUE_HEAP_SIZE];
	TaskParameters_t* tarea_asociada;
	uint16_t indice_head;
	uint16_t indice_tail;
	uint16_t size_elemento;
};
//==================================[Colas]==========================
typedef struct _cola osCola;
   

void os_ColaInit(osCola* cola, uint16_t datasize);

void os_ColaWrite(osCola* cola, void* dato);

void os_ColaRead(osCola* cola, void* dato);

/*==================[external functions declaration]=========================*/

void semaforo_init(Semaforo_t * t);

void semaforo_take(Semaforo_t * t);

void semaforo_give(Semaforo_t * t);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_SEMAFORO_H_ */
