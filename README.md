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

Paso para procesar el codigo:

1. Debemos asegurarnos de tener todos los archivos dentro de la misma carpeta.

2. Hacemos un make para compilar los archivos necesarios

3. Si queremos reiniciar hacemos un make clean

4. Para ejecutar el codigo simplemente hacemos un ./wave_propagation, aquí hay que tener varias consideraciones.

    4.1. Si queremos cambiar los parametros del laboratorio tenemos que ir al main y simplemente cambiarlos.

    4.2  Si queremos usar los diferentes metodos de sincronización y de chunks. Escribimos en consola una de los siguientes comandos
        - ./wave_propagation 0
        - ./wave_propagation 1 4
        - ./wave_propagation 2 8
    
    4.3 Si se quiere ejecutar el codigo con dimensiones distintas se tiene que cambiar el parametro de entrada del metodo "initializeRegularNetwork":
        - para 1D: initializeRegularNetwork(1)
        - para 2D: initializeRegularNetwork(2, int largo, int ancho)

5. Si se quieren obtener los calculos de rendimiento, simplemente se tiene que agregar "-benchmark" al comando "./wave_propagation" 
    - ./wave_propagation -benchmark

6. Si se quiere ejecutar en python tenemos que aplicar los siguientes comandos.
    - python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif
    - python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "wave evolution.dat" --output wave_2d.gif



python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif