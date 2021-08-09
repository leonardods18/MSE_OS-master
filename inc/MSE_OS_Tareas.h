#ifndef _MSE_OS_TAREAS_H_
#define _MSE_OS_TAREAS_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>

/*==================[macros]=================================================*/

#define STACK_SIZE 256
#define TASK_NAME_SIZE				10	
#define MAX_TASK_COUNT				10	

/*==================[definicion de datos para el OS]=================================*/

#define ERR_OS_CANT_TAREAS		-1
#define ERR_OS_SCHEDULING		-2


/********************************************************************************
 * Definicion de los estados posibles para las tareas
 *******************************************************************************/

enum _estadoTarea  {
	TAREA_READY,
	TAREA_RUNNING,
   TAREA_BLOCKED // tarea bloqueada
};

typedef enum _estadoTarea estadoTarea;



/********************************************************************************
 * Definicion de la estructura para cada tarea
 *******************************************************************************/
struct _tarea  {
	uint32_t stack[STACK_SIZE/4];
	uint32_t stack_pointer;
	void *entry_point;
	uint8_t id;
	estadoTarea estado;
	uint8_t prioridad;
	uint32_t ticks_bloqueada;
};

typedef struct _tarea tarea;

#endif /* #ifndef _OS_TAREAS_H_ */