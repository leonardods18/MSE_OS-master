# Sistema medidor de tiempo para flancos

Examen MSE 2021
IMPLEMENTACIÓN DE SISTEMAS OPERATIVOS. 

## Requisitos 

Implemente un sistema que mida la diferencia de tiempos entre flancos positivos y negativos
generados por dos pulsos, cuyas ocurrencias se solapen temporalmente, segÃºn se muestra en la
figura. Cada caso de solapamiento tendrá un led específico asociado, el cual se encenderá
inmediatamente luego de que los dos botones dejen de ser presionados. El tiempo en que el led
correspondiente estará encendido será la suma de los tiempos entre flancos ascendentes y
descendentes respectivamente.

Asimismo, deberá enviar por la UART un mensaje asociado al evento ocurrido (al dejar de
presionar ambos botones) el cual indique el color de led que se ha encendido, tiempo total que
se enciende y tiempos t1 y t2 segÃºn se muestra en los ejemplos a continuación:
Led Verde encendido:\n\r
\t Tiempo encendido: 578 ms
\t Tiempo entre flancos descendentes: 315 ms \n\r
\t Tiempo entre flancos ascendentes: 263 ms \n\r

Led Azul encendido:\n\r
\t Tiempo encendido: 1869 ms
\t Tiempo entre flancos descendentes: 873 ms \n\r
\t Tiempo entre flancos ascendentes: 996 ms \n\r

## Aclaraciones 

Importante:

 *  	 Si se presiona una tecla y se suelta sin presionar la otra, el sistema enviará un mensaje de error por UART.
 *  	 Si se presiona una tecla, pasando un tiempo t se presiona la otra, luego una de las 2 se suelta y vuelve a presionar sin soltar la otra, el sistema enviará mensaje de error por UART.

 
## Pruebas de funcionamiento

Imágenes del funcionamiento:


![funcionamiento](https://user-images.githubusercontent.com/59117988/129646457-48af79c9-59cd-4e57-946e-85dc598c185b.JPG)


## Construido

Autor: Del Sancio Leonardo Daniel.

Profesor: Sanchez Gonzalo.

