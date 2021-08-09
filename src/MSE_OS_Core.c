/*
* Profesor: Gonzalo Sanchez.
* Alumno: Del Sancio, Leonardo.
* MSE_OS_Core.h
 */

#include "MSE_OS_Core.h"



/*==================[definicion de variables globales]=================================*/

static osControl control_OS;
static tarea tareaIdle;

/*==================[definicion de prototipos static]=================================*/
static void initTareaIdle(void);
static void setPendSV(void);
static void ordenarPrioridades(void);
static int32_t partition(tarea** arr, int32_t l, int32_t h);


/*==================[hooks]=================================*/


void __attribute__((weak)) returnHook(void)  {    // no debe entrar acá
	while(1);
}



void __attribute__((weak)) tickHook(void)  {
	__asm volatile( "nop" );
}

void __attribute__((weak)) errorHook(void *caller)  {  // 
	/*
	 * Revisar el contenido de control_OS.error para obtener informacion. Utilizar os_getError()
	 */
	while(1);
}


void __attribute__((weak)) idleTask(void)  {   //Esta tarea se ejecuta solamente cuando todas las demas tareas estan en estado bloqueado.
                                                //   Puede ser redefinida por el usuario.
	while(1)  {
		__WFI();
	}
}


int32_t os_getError(void)  {   // Ultimo error ocurrido dentro del OS.
	return control_OS.error;
}

/*==================[definicion de funciones de OS]=================================*/

void os_InitTarea(void *entryPoint, tarea *task, uint8_t prioridad)  { //Se necesita llamar a esta función para cada tarea antes de iniciar el sistema operativo.
	static uint8_t id = 0;				//el id sera correlativo a medida que se generen mas tareas



	if(control_OS.cantidad_Tareas < MAX_TASK_COUNT)  {

		task->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
		task->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;		//direccion de la tarea (ENTRY_POINT)
		task->stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;			//Retorno de la tarea (no deberia darse)


		task->stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;

		task->stack_pointer = (uint32_t) (task->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

	
		task->entry_point = entryPoint;
		
      task->id = id;
		
      task->estado = TAREA_READY;
		
      task->prioridad = prioridad;
	
      control_OS.listaTareas[id] = task;
		
      control_OS.cantidad_Tareas++;
		
      control_OS.cantTareas_prioridad[prioridad]++;

		id++;
	}

	else {
		
      control_OS.error = ERR_OS_CANT_TAREAS;
		
      errorHook(os_InitTarea);  // si se excede las tareas, actualiza el último error generado.
	}
}


void os_Init(void)  {  // se setea la prioridad del PendSV como la prioridad más baja. Función importante

	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);


	initTareaIdle();

	
   control_OS.estado_sistema = OS_FROM_RESET;
	
   control_OS.tarea_actual = NULL;
	
   control_OS.tarea_siguiente = NULL;


	for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)  {
		if(i>=control_OS.cantidad_Tareas)
			control_OS.listaTareas[i] = NULL;
	}

	ordenarPrioridades();
}


static void initTareaIdle(void)  {                                // se toman entrypoint fijos. La tarea especial se inicializa en 255 (0xFF)
	tareaIdle.stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
	tareaIdle.stack[STACK_SIZE/4 - PC_REG] = (uint32_t)idleTask;		//direccion de la tarea (ENTRY_POINT)
	tareaIdle.stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;			//Retorno de la tarea (no deberia darse)


	tareaIdle.stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;
	tareaIdle.stack_pointer = (uint32_t) (tareaIdle.stack + STACK_SIZE/4 - FULL_STACKING_SIZE);


	tareaIdle.entry_point = idleTask;
	tareaIdle.id = 0xFF;
	tareaIdle.estado = TAREA_READY;
	tareaIdle.prioridad = 0xFF;
}



static void scheduler(void)  {                   //  implementacion de scheduler del tipo Round-Robin
	static uint8_t indicePrioridad[PRIORITY_COUNT];		//indice de tareas a ejecutar segun prioridad
	uint8_t indiceArrayTareas = 0;
	uint8_t prioridad_actual = MAX_PRIORITY;			//Maxima prioridad al iniciar
	uint8_t cantBloqueadas_prioActual = 0;
	bool salir = false;
	uint8_t cant_bloqueadas = 0;



	if (control_OS.estado_sistema == OS_FROM_RESET)  {
		control_OS.tarea_actual = (tarea*) &tareaIdle;
		control_OS.cambioContextoNecesario = true;
		memset(indicePrioridad,0,sizeof(uint8_t) * PRIORITY_COUNT);
	}
	else {


		while(!salir)  {


			indiceArrayTareas = 0;


			if(control_OS.cantTareas_prioridad[prioridad_actual] > 0)  {


				indicePrioridad[prioridad_actual] %= control_OS.cantTareas_prioridad[prioridad_actual];


				for (int i=0; i<prioridad_actual;i++) {
					indiceArrayTareas += control_OS.cantTareas_prioridad[i];
				}
				indiceArrayTareas += indicePrioridad[prioridad_actual];


				if ( ((tarea*)control_OS.listaTareas[indiceArrayTareas])->ticks_bloqueada == 0 &&
					 ((tarea*)control_OS.listaTareas[indiceArrayTareas])->estado == TAREA_BLOCKED )  {

					((tarea*)control_OS.listaTareas[indiceArrayTareas])->estado = TAREA_READY;
				}

				switch (((tarea*)control_OS.listaTareas[indiceArrayTareas])->estado) {

				case TAREA_READY:
					control_OS.tarea_siguiente = (tarea*) control_OS.listaTareas[indiceArrayTareas];
					control_OS.cambioContextoNecesario = true;
					indicePrioridad[prioridad_actual]++;
					salir = true;
					break;

				case TAREA_BLOCKED:
					cant_bloqueadas++;
					cantBloqueadas_prioActual++;
					indicePrioridad[prioridad_actual]++;
					if (cant_bloqueadas == control_OS.cantidad_Tareas)  {
						control_OS.tarea_siguiente = &tareaIdle;
						control_OS.cambioContextoNecesario = true;
						salir = true;
					}
					else {
						if(cantBloqueadas_prioActual == control_OS.cantTareas_prioridad[prioridad_actual])  {
							cantBloqueadas_prioActual = 0;
							indicePrioridad[prioridad_actual] = 0;
							prioridad_actual++;
						}
					}
					break;

				case TAREA_RUNNING:
					indicePrioridad[prioridad_actual]++;
					control_OS.cambioContextoNecesario = false;
					salir = true;
					break;
	
				default:
					control_OS.error = ERR_OS_SCHEDULING;
					errorHook(scheduler);
				}
			}
			else {
				indicePrioridad[prioridad_actual] = 0;
				prioridad_actual++;
			}
		}
	}
}




static void setPendSV(void)  {   //Setea la bandera comrrespondiente para que se ejucute PendSV

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	__ISB();
	__DSB();
}

void SysTick_Handler(void)  {   //se llama al scheduler y luego de determinarse cual es la tarea siguiente a ejecutar
	uint8_t i;


	i = 0;
	while (control_OS.listaTareas[i] != NULL)  {

		if( ((tarea*)control_OS.listaTareas[i])->ticks_bloqueada > 0 )
			((tarea*)control_OS.listaTareas[i])->ticks_bloqueada--;

		i++;
	}

	scheduler();

	if(control_OS.cambioContextoNecesario)
		setPendSV();

	tickHook();
}


uint32_t getContextoSiguiente(uint32_t sp_actual)  {
	uint32_t sp_siguiente;



	if (control_OS.estado_sistema == OS_FROM_RESET)  {
		sp_siguiente = control_OS.tarea_actual->stack_pointer;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
		control_OS.estado_sistema = OS_NORMAL_RUN;
	}

	
	else {
		control_OS.tarea_actual->stack_pointer = sp_actual;

		if (control_OS.tarea_actual->estado == TAREA_RUNNING)
			control_OS.tarea_actual->estado = TAREA_READY;

		sp_siguiente = control_OS.tarea_siguiente->stack_pointer;

		control_OS.tarea_actual = control_OS.tarea_siguiente;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
	}

	control_OS.cambioContextoNecesario = false;

	return sp_siguiente;
}


void os_CpuYield(void)  {
	scheduler();
	if(control_OS.cambioContextoNecesario)
		setPendSV();
}

tarea* os_getTareaActual(void)  {  //puntero a la estructura de la tarea actual.
	return control_OS.tarea_actual;
}


static void ordenarPrioridades(void)  {  //Ordena los punteros a las estructuras del tipo tarea 
	// Create an auxiliary stack
	int32_t stack[MAX_TASK_COUNT];

	// initialize top of stack
	int32_t top = -1;
	int32_t l = 0;
	int32_t h = control_OS.cantidad_Tareas - 1;

	// push initial values of l and h to stack (indices a estructuras de tareas)
	stack[++top] = l;
	stack[++top] = h;

	// Keep popping from stack while is not empty
	while (top >= 0) {
		// Pop h and l
		h = stack[top--];
		l = stack[top--];

		// Set pivot element at its correct position
		// in sorted array
		int32_t p = partition(control_OS.listaTareas, l, h);

		// If there are elements on left side of pivot,
		// then push left side to stack
		if (p - 1 > l) {
			stack[++top] = l;
			stack[++top] = p - 1;
		}

		// If there are elements on right side of pivot,
		// then push right side to stack
		if (p + 1 < h) {
			stack[++top] = p + 1;
			stack[++top] = h;
		}
	}
}


/*************************************************************************************************
	 *  @brief Ordena tareas de mayor a menor prioridad.
     *
     *  @details
     *   Funcion de soporte para ordenarPrioridades. No debe llamarse fuera de mencionada
     *   funcion.
     *
	 *  @param 	arr		Puntero a la lista de punteros de estructuras de tareas a ordenar
	 *  @param	l		Inicio del vector a ordenar (puede ser un subvector)
	 *  @param	h		Fin del vector a ordenar (puede ser un subvector)
	 *  @return     	Retorna la posicion del pivot necesario para el algoritmo
***************************************************************************************************/
static int32_t partition(tarea** arr, int32_t l, int32_t h)  {
	tarea* x = arr[h];
	tarea* aux;
	int32_t i = (l - 1);

	for (int j = l; j <= h - 1; j++) {
		if (arr[j]->prioridad <= x->prioridad) {
			i++;
			aux = arr[i];
			arr[i] = arr[j];
			arr[j] = aux;
		}
	}
	aux = arr[i+1];
	arr[i+1] = arr[h];
	arr[h] = aux;

	return (i + 1);
}



