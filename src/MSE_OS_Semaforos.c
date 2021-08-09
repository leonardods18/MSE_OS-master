#include "MSE_OS_Semaforos.h"
#include "MSE_OS_Tareas.h"

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
void os_SemaforoInit(osSemaforo* sem)  {
	sem->tomado = true;
	sem->tarea_asociada = NULL;
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
void os_SemaforoTake(osSemaforo* sem)  {
	bool Salir = false;
	tarea* tarea_actual;

	/*
	 * La estructura control_OS solo puede ser accedida desde el archivo core, por lo que
	 * se provee una funcion para obtener la tarea actual (equivale a acceder a
	 * control_OS.tarea_actual)
	 */
	tarea_actual = os_getTareaActual();

	if (tarea_actual->estado == TAREA_RUNNING)  {

		/*
		 * En el caso de que otra tarea desbloquee por error la tarea que se acaba de
		 * bloquear con el semaforo (en el caso que este tomado) el bloque while se
		 * encarga de volver a bloquearla hasta tanto no se haga un give
		 */
		while (!Salir)  {

			/*
			 * Si el semaforo esta tomado, la tarea actual debe bloquearse y se
			 * mantiene un puntero a la estructura de la tarea actual, la que
			 * recibe el nombre de tarea asociada. Luego se hace un CPU yield
			 * dado que no se necesita mas el CPU hasta que se libere el semaforo.
			 *
			 * Si el semaforo estaba libre, solamente se marca como tomado y se
			 * retorna
			 */
			if(sem->tomado)  {
				tarea_actual->estado = TAREA_BLOCKED;
				sem->tarea_asociada = tarea_actual;
				os_CpuYield();
			}
			else  {
				sem->tomado = true;
				Salir = true;
			}
		}
	}
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
void os_SemaforoGive(osSemaforo* sem)  {
	tarea* tarea_actual;

	tarea_actual = os_getTareaActual();

	/*
	 * Por seguridad, se deben hacer varios checkeos antes de hacer un give sobre
	 * el semaforo. En el caso de que se den todas las condiciones, se libera y se
	 * actualiza la tarea correspondiente a estado ready.
	 */

	if (tarea_actual->estado == TAREA_RUNNING &&
			sem->tomado == true &&
			sem->tarea_asociada != NULL)  {

		sem->tomado = false;
		sem->tarea_asociada->estado = TAREA_READY;
	}
}
