# Laboratorio-Lenguajes-de-Programacion-Tarea-2

Repositorio del proyecto 1 del curso **Inteligencia Artificial I** de la **Universidad Simón Bolívar** durante el trimestre **Enero-Marzo 2026**. 

Autores: **David Díaz (20-10019) y Alan Argotte (19-10664)**  

## Estructura del repositorio e instrucciones de ejecución

### Estructura del repositorio

Este repositorio está compuesto por tres carpetas:  
- `additive_pdb`: contiene la implementación de IDA* con un par de PDBs que se pensaron originalmente aditivas.
- `admissible_pdb`: contiene la implementación de IDA* con una PDB admisible.
- `admissible_benchmarks`: contiene subcarpetas, cada una nombrada con el patrón utilizado por la PDB, con el costo en tiempo de la resolución de un suite de pruebas que consta de 20 instancias generadas aleatoriamente de Top-Spin.
- `non_admissible_pdb`: contiene la implementación de IDA* con una PDB no admisible.
- `entrega`: contiene la implementación de IDA* entregable que usa un par de PDBs no aditivas.

En las carpetas que contienen una implementación de IDA* encontrará las subcarpetas `./exe` y `./output`, que contienen los ejecutables de la implementación de IDA y de BFS para generar la PDB y de programas auxiliares tales como `automatic_test.cpp`, `print_database.cpp`, `random_instances` que cumplen funciones de debugeo y ejecución del suite de pruebas creado para el proyecto; también hallará pdb*.bin que contienen en formato binario las PDBs utilizadas por cada implementación y makefiles con las siguientes instrucciones:

- `all` ejecuta todas las instrucciones siguientes excepto `tests` (instrucción por defecto)
- `IDA` compila y genera el ejecutable de IDA*
- `database` compila y genera el ejecutable de BFS que crea la PDB usada por IDA*
- `generate_database` corre el ejecutable de BFS
- `tests` ejecuta el suite de pruebas. 

### Instrucciones de ejecución

La carpeta `entrega` contiene la implementación de IDA* con un par de PDBs no aditivas y que es el entregable para este proyecto. Para su ejecución, asegúrese de estar en la carpeta 
```
/entrega
```
Tenga en cuenta que los comandos del archivo make en esta carpeta son distintos:
- `all` ejecuta todas las instrucciones (instrucción por defecto)
- `compile_ida` compila y genera el ejecutable de IDA*
- `compile_databases` compila y genera el ejecutable de BFS que crea la PDB usada por IDA*
- `generate_databases` corre el ejecutable de BFS

Una vez en la carpeta, ejecute
```
make
```
O
```
make all
```
Para generar las PDBs (si no existen) y el ejecutable `ida` que es creado en la carpeta de trabajo. Una vez generado `ida`, ejecute
```
./ida <instancia> <archivo>
```
Donde `<instancia>` es una instancia a resolver del rompecabezas Top-Spin y `<archivo>` es un archivo de formato .txt donde son almacenados los movimientos realizados por IDA* para llegar a la solución. Por ejemplo,
```
./ida 7 6 2 3 8 1 5 9 10 11 12 4 ida.txt
```

Guarda los movimientos hechos por IDA* para llegar al nodo solución en el archivo ida.txt