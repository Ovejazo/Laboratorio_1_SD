# Laboratorio_1_SD

Asignatura:
- Sistemas Distribuidos y paralelos

Profesor:
- Migue carcamo

Participantes: 
- Thomas Riffo 21134817-8
- Isidora Oyanedel 21168603-0

Paso preliminar: 

+ Hay que ir a esta dirección: /mnt/c/Users/thoma/Documents/Materias/Sistemas distribuidos/Ejercicios uCPP/Lab1/Laboratorio_1_SD$

INTRUCCIONES DE EJECUCION:

1. Debemos asegurarnos de tener todos los archivos dentro de la misma carpeta.

2. Hacemos un "make" para compilar los archivos necesarios

3. Si queremos eliminar todo, simplemente hacemos un "make clean"

4. Para ejecutar el codigo simplemente hacemos un "./wave_propagation", aquí hay que tener varias consideraciones.

    4.1. Si queremos cambiar los parametros del laboratorio tenemos que ir al main y simplemente cambiarlos con lo que queramos.

    4.2  Si queremos hacer uso de los diferentes metodos de sincronización y de chunks. Escribimos en consola una de los siguientes comandos
        - ./wave_propagation 0      (static)
        - ./wave_propagation 1 4    (dynamic)
        - ./wave_propagation 2 8    (guided)
    
    4.3 Si se quiere ejecutar el codigo con dimensiones distintas se tiene que cambiar el parametro de entrada del metodo "initializeRegularNetwork" que se encuentra en el main:
        - para 1D: initializeRegularNetwork(1)
        - para 2D: initializeRegularNetwork(2, int largo, int ancho)

    4.4 Si la ejecución es 2D, podemos elegir si queremos que haga un collapse o no, para elegir como ejecutarlo, se pone el siguiente comando:
        - ./wave_propagation 2 8
        - ./wave_propagation 2 8 -collapse

5. Si se quieren obtener los calculos de rendimiento, simplemente se tiene que agregar "-benchmark" al comando "./wave_propagation", de la siguiente manera: 
    - ./wave_propagation -benchmark

6. Si se quiere graficar y ver una animación que permita apreciar el comportamiento de la onda en python, ejecutamos el siguiente comando.
    - python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif
    - python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "wave evolution.dat" --output wave_2d.gif

7. Con los resultados obtenidos de benchmark con el paso número 5, podemos graficar eso con el codigo de python llamado analisis.py, simplemente ejecutamos:
    - python3 analisis.py



python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif