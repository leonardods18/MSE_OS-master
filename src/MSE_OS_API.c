/*
* Profesor: Gonzalo Sanchez.
* Alumno: Del Sancio, Leonardo.
* MSE_OS_API.c
 */

#include "MSE_OS_API.h"

void os_Delay(uint32_t ticks)  {
	tarea* tarea_actual;
   
	tarea_actual = os_getTareaActual();
   
	if (tarea_actual->estado == TAREA_RUNNING && ticks > 0)  {

		tarea_actual->ticks_bloqueada = ticks;

		while (tarea_actual->ticks_bloqueada > 0)  { // queda atrapada si se vuelve a ejecutar la tarea antes que termine el período de bloqueo
			tarea_actual->estado = TAREA_BLOCKED;
			os_CpuYield();
		}
	}
}
