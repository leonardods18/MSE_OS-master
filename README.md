# Sistema medidor de flancos

Examen MSE 2021
Implementar un medidor de flancos para la materia IMPLEMENTACIÓN DE SISTEMAS OPERATIVOS. 

## Requisitos ð

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

## Aclaraciones ð

Importante:

 *  	 Si se presiona una tecla y se suelta sin presionar la otra, el sistema enviará un mensaje de error por UART.
 *  	 Si se presiona una tecla, pasando un tiempo t se presiona la otra, luego una de las 2 se suelta y vuelve a presionar sin soltar la otra, el sistema enviará mensaje de error por UART.

 
## Pruebas de funcionamiento âï¸

Imágenes del funcionamiento:


## Construido ð ï¸

Autor: Del Sancio Leonardo Daniel.
Profesor: Sanchez Gonzalo.

![funcionamiento](https://user-images.githubusercontent.com/59117988/129645980-e02eec4b-4a3a-4ad5-b3f8-5941bbe9ccd5.JPG)

