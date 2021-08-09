/*
* Profesor: Gonzalo Sanchez.
* Alumno: Del Sancio, Leonardo.
* MSE_OS_Core.h
 */

#ifndef ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_
#define ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "MSE_OS_Tareas.h"


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
 * 			Valores necesarios para registros del stack frame inicial
 ***********************************************************************************/

#define INIT_XPSR 	1 << 24				//xPSR.T = 1
#define EXEC_RETURN	0xFFFFFFF9			//retornar a modo thread con MSP, FPU no utilizada

//----------------------------------------------------------------------------------


/************************************************************************************
 * 						Definiciones varias
 ***********************************************************************************/
#define STACK_FRAME_SIZE			8
#define FULL_STACKING_SIZE 			17	//16 core registers + valor previo de LR


/*==================[definicion codigos de error de OS]=================================*/

#define MAX_PRIORITY		0			
#define MIN_PRIORITY		3			

#define PRIORITY_COUNT		(MIN_PRIORITY-MAX_PRIORITY)+1	//cantidad de prioridades asignables

/********************************************************************************
 * Definicion de los estados posibles de nuestro OS
 *******************************************************************************/

enum _estadoOS  {
	OS_NORMAL_RUN,
	OS_FROM_RESET
};

typedef enum _estadoOS estadoOS;



/********************************************************************************
 * Definicion de la estructura de control para el sistema operativo
 *******************************************************************************/
struct _osControl  {
		void *listaTareas[MAX_TASK_COUNT];			//array de punteros a tareas
	int32_t error;								//variable que contiene el ultimo error generado
	uint8_t cantidad_Tareas;					//cantidad de tareas definidas por el usuario
	uint8_t cantTareas_prioridad[PRIORITY_COUNT];	//cada posicion contiene cuantas tareas tienen la misma prioridad

	estadoOS estado_sistema;					//Informacion sobre el estado del OS
	bool cambioContextoNecesario;				//Esta bandera indica si el scheduler determino un cambio de contexto

	tarea *tarea_actual;				//definicion de puntero para tarea actual
	tarea *tarea_siguiente;			//definicion de puntero para tarea siguiente
};
typedef struct _osControl osControl;





#endif /* ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_ */
