# Laboratorio_1_SD

Asignatura:
- Sistemas Distribuidos y paralelos

Profesor:
- Miguel carcamo

Participantes: 
- Thomas Riffo 21134817-8
- Isidora Oyanedel 21168603-0

## Requisitos
- Compilador C++ con OpenMP:
  - g++/clang++ con soporte `-fopenmp` y C++17.
  - Nota: si tu g++ es antiguo y falla `std::filesystem`, enlaza con `-lstdc++fs` o usa g++ 9+.
- Python 3 (para gráficas) con:
  - numpy, matplotlib, pillow

Instalación rápida de python deps:
```bash
pip3 install numpy matplotlib pillow 
```
## Compilación
```bash
# Compilar
make

# Limpiar artefactos
make clean
```

luego llamamos ./wave_propagation

## Paso preliminar: 

+ Hay que ir a esta dirección: /mnt/c/Users/thoma/Documents/Materias/Sistemas distribuidos/Ejercicios uCPP/Lab1/Laboratorio_1_SD$

+ Debemos asegurarnos de tener todos los archivos dentro de la misma carpeta.

+ Parametros de tiempo de ejecución, todos son opcionales:
    - `schedule_type`, es un entero. 0 = static, 1 = dynamic, 2 = guided
    - `chunk_size`, es un entero > 0.
    - `-collapse`, es un string.

## INTRUCCIONES DE EJECUCION:

1. Hacemos un "make" para compilar los archivos necesarios

2. Si queremos eliminar todo, simplemente hacemos un "make clean"

3. Para ejecutar el codigo simplemente hacemos un "./wave_propagation", aquí hay que tener varias consideraciones.

    3.1. Si queremos cambiar los parametros del laboratorio tenemos que ir al main y simplemente cambiarlos con lo que queramos.

    3.2  Si queremos hacer uso de los diferentes metodos de sincronización y de chunks. Escribimos en consola una de los siguientes comandos
        - ./wave_propagation 0      (static)
        - ./wave_propagation 1 4    (dynamic)
        - ./wave_propagation 2 8    (guided)
    
    3.3 Si se quiere ejecutar el codigo con dimensiones distintas se tiene que cambiar el parametro de entrada del metodo "initializeRegularNetwork" que se encuentra en el main:
        - para 1D: initializeRegularNetwork(1)
        - para 2D: initializeRegularNetwork(2, int largo, int ancho)

    3.4 Si la ejecución es 2D, podemos elegir si queremos que haga un collapse o no, para elegir como ejecutarlo, se pone el siguiente comando:
        - ./wave_propagation 2 8
        - ./wave_propagation 2 8 -collapse
    
Ejemplos:

```bash
# Serial (por defecto usa static si no pasas nada)
./wave_propagation

# Paralelo con static
./wave_propagation 0

# Paralelo con dynamic y chunk=4
./wave_propagation 1 4

# Paralelo con guided y chunk=8
./wave_propagation 2 8

# 2D con collapse(2) (si la malla es 2D en el main)
./wave_propagation 2 8 -collapse
```
    3.5 En el main se puede cambiar el tipo de fuente externa que se quiere usar, hay 4 fuentes distintas a usar: 
        - "constexpr SourcePreset kSource = SourcePreset::Zero;"
        - "constexpr SourcePreset kSource = SourcePreset::Fixed;"
        - "constexpr SourcePreset kSource = SourcePreset::Random;"
        - "constexpr SourcePreset kSource = SourcePreset::Sine;"

4. Si se quieren obtener los calculos de rendimiento, simplemente se tiene que agregar "-benchmark" al comando "./wave_propagation", de la siguiente manera: 
    - ./wave_propagation -benchmark

5. Si se quiere graficar y ver una animación que permita apreciar el comportamiento de la onda en python, ejecutamos el siguiente comando.
    - python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif
    - python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "wave evolution.dat" --output wave_2d.gif

Ejemplos:
```bash
# 1D (guarda datos/wave_1d.gif)
python3 graficar_resultados.py --mode 1d --outdir datos

# 2D (ancho x alto deben coincidir con la malla usada en C++)
python3 graficar_resultados.py --mode 2d --width 10 --height 10 --outdir datos

# Usando un archivo específico
python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "datos/wave evolution.dat" --outdir datos --output onda_2d.gif
```

6. Con los resultados obtenidos de benchmark con el paso número 5, podemos graficar eso con el codigo de python llamado analisis.py, simplemente ejecutamos:
    - python3 analisis.py


## Consideraciones

- Los resultados obtenidos se guardan en la carpeta llamada datos, aquí se guardan los archivos .dat y los graficos que se crean en la ejecución de los codigos de python.

Cuando se define el tipo de fuente externa, tenemos lo siguiente. 
- Zero: S_i(t) = 0 para todos los nodos
- Fixed: S_i(t) = kFixedValue para todos los nodos (o vector fijo si prefieres)
- Random: S_i(t) = valor aleatorio fijo distinto por nodo (en [kRandMin, kRandMax])
- Sine: S_i(t) = kAmp · sin(kOmega · t) (igual para todos los nodos)

El tiempo `t` avanza automaticamente por cada paso.

## Solución a errores comunes:
- FileNotFoundError: asegúrate de que el archivo exista en `datos/` y que `--width * --height == N` (número de nodos).

## Benchmarks de performance
El modo benchmark explora combinaciones de `schedule × chunk × threads`, promedia 10 repeticiones y calcula speedup, eficiencia y barras de error.

Ejecutar:
```bash
./wave_propagation -benchmark
```

Salida en `datos/`:
- `benchmark results.dat` — tabla completa del grid
- `scaling analysis.dat` — mejor combinación por número de threads

Gráficas de performance:
```bash
# Genera datos/performance plots.png
python3 analisis.py

# Personalizar (si lo deseas)
python3 analisis.py --infile "datos/benchmark results.dat" --outdir datos --outfile "performance plots.png"
```
