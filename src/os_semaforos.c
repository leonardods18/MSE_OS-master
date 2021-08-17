/*
 * os_semaforos.c
 * Examen ISO 2021 Maestría en sistemas embebidos
 * Autor: Del Sancio, Leonardo
 */


/*==================[inclusions]=============================================*/

#include "os_semaforos.h"
#include "os.h"

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
