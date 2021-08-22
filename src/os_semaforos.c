/*
 * os_semaforos.c
 * se implementan sermáforos y colas
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */


/*==================[inclusions]=============================================*/

#include "os_semaforos.h"
#include "os.h"
#include <string.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/*************************************************************************************************
	 *  @brief Inicializacion de un semaforo binario
     *
     *  @details
     *   Antes de utilizar cualquier semaforo binario en el sistema, debe inicializarse el mismo.
     *   Todos los semaforos se inicializan tomados
     *
	 *  @param		sem		Semaforo a inicializar
	 *  @return     None.
***************************************************************************************************/
void semaforo_init(Semaforo_t * t){
	t->tomado = 0;
}
/*************************************************************************************************
	 *  @brief Tomar un semaforo
     *
     *  @details
     *   Esta funcion es utilizada para tomar un semaforo cualquiera.
     *
	 *  @param		sem		Semaforo a tomar
	 *  @return     None.
***************************************************************************************************/
void semaforo_take(Semaforo_t * t){

	ini_SeccionCritica();

	if ( t->tomado == 0 ){
		t->tarea = get_current_task();

		t->tomado = 1;
		t->tarea->estado = eWaiting;

		schedule();
	}
	else{
		// semaforo tomado
	}
	fin_SeccionCritica();
}
/********************************************************************************
	 *  @brief Liberar un semaforo
     *
     *  @details
     *   Esta funcion es utilizada para liberar un semaforo cualquiera.
     *
	 *  @param		sem		Semaforo a liberar
	 *  @return     None.
 *******************************************************************************/
void semaforo_give(Semaforo_t * t){

	ini_SeccionCritica();

	if ( t->tomado == 1 ){
		t->tomado = 0;
		t->tarea->estado = eReady;

		schedule();
	}
	else{
		// semaforo no tomado
	}
	fin_SeccionCritica();
}

/*==================[end of file]============================================*/

void os_ColaInit(osCola* cola, uint16_t datasize)  {
	cola->indice_head = 0;
	cola->indice_tail = 0;
	cola->tarea_asociada = NULL;
	cola->size_elemento = datasize;
}

void os_ColaWrite(osCola* cola, void* dato)  {
  TaskParameters_t *tarea;
  uint16_t index_h;					
	uint16_t elementos_total;		
	
	index_h = cola->indice_head * cola->size_elemento;
	elementos_total = QUEUE_HEAP_SIZE / cola->size_elemento;
	

	if(((cola->indice_head == cola->indice_tail) && cola->tarea_asociada != NULL) &&
		cola->tarea_asociada->estado == eWaiting)  {
			cola->tarea_asociada->estado = eReady;
	}

	tarea = get_current_task();
	
	if(tarea->estado == eRunning)  {
		
		while((cola->indice_head + 1) % elementos_total == cola->indice_tail)  {
			
			tarea->estado = eWaiting;
			cola->tarea_asociada = tarea;
			os_CpuYield();
		}
		
		memcpy(cola->data+index_h,dato,cola->size_elemento);
		cola->indice_head = (cola->indice_head + 1) % elementos_total;
		cola->tarea_asociada = NULL;
	} 

}


void os_ColaRead(osCola* cola, void* dato)  
{
TaskParameters_t *tarea;
uint16_t elementos_total;		
	uint16_t index_t;					
	index_t = cola->indice_tail * cola->size_elemento;
	elementos_total = QUEUE_HEAP_SIZE / cola->size_elemento;	

	if((( (cola->indice_head + 1) % elementos_total == cola->indice_tail) &&
			cola->tarea_asociada != NULL) &&
			cola->tarea_asociada->estado == eWaiting)  {
		cola->tarea_asociada->estado = eReady;
	}

	
	tarea = get_current_task();
	if(tarea->estado == eRunning)  {
		

		while(cola->indice_head == cola->indice_tail)  {
			
			tarea->estado = eWaiting;
			cola->tarea_asociada = tarea;
			os_CpuYield();
		}

		memcpy(dato,cola->data+index_t,cola->size_elemento);
		cola->indice_tail = (cola->indice_tail + 1) % elementos_total;
		cola->tarea_asociada = NULL;
	}


   
}