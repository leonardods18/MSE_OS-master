/*
 * os_semaforos.h
 * Examen ISO 2021 Maestr�a en sistemas embebidos
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

/*==================[typedef]================================================*/

typedef struct {
	uint32_t tomado;
	TaskParameters_t * tarea;
} Semaforo_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void semaforo_init(Semaforo_t * t);

void semaforo_take(Semaforo_t * t);

void semaforo_give(Semaforo_t * t);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_SEMAFORO_H_ */
