# Práctica 2 — Simulador de una Máquina de Turing determinista multicinta

## 1. Información del proyecto

* **Alumno:** Daniel Palenzuela Álvarez alu0101140469
* **Asignatura:** Complejidad Computacional
* **Curso:** 2026/27
* **Práctica 2:** Programar un simulador de una Máquina de Turing determinista
* **Repositorio Github:** [https://github.com/alu0101140469/pract2-complej](https://github.com/alu0101140469/pract2-complej)

El objetivo del proyecto es implementar un simulador reutilizable de Máquinas de Turing (MT). El programa lee la definición de una máquina desde un fichero de configuración, valida sus componentes y permite ejecutar distintas cadenas de entrada. Para cada entrada informa de si la ejecución se ha detenido en un estado de aceptación y muestra el contenido de cada cinta, con la posición de la cabeza marcada entre corchetes.

## 2. Variantes implementadas y funcionamiento general

Las variantes elegidas para esta práctica son:

- **Escritura y movimientos simultáneos:** en cada paso se obtienen primero los símbolos leídos bajo todas las cabezas. A continuación se escriben los nuevos símbolos en todas las cintas, se mueven todas las cabezas y se actualiza el estado.
- **Movimientos:** solo se permiten `L` (izquierda) y `R` (derecha). No se admite el movimiento estacionario `S`.
- **Cinta infinita en ambas direcciones:** las posiciones se representan con coordenadas con signo, por lo que la cabeza puede desplazarse a izquierda o derecha del origen.
- **Entrada desde fichero o teclado:** se pueden procesar varias cadenas mediante `-in` o introducirlas por teclado.

El flujo general del programa es:

```text
Fichero de configuración
          |
          v
ConfigurationParser
          |
          v
MachineDefinition -> validación de la definición
          |
          v
TuringMachine + Tape(s)
          |
          v
Leer símbolos -> buscar transición -> escribir -> mover -> cambiar estado
          |
          v
¿Existe una transición aplicable?
       /             \
     Sí               No
     |                 |
     v                 v
Siguiente paso    Comprobar si el estado actual pertenece a F
                       |
                       v
              Mostrar resultado y cintas
```

La ejecución finaliza únicamente cuando no existe una transición para el estado y los símbolos leídos. La entrada se acepta si el estado en ese momento pertenece al conjunto de estados de aceptación `F` y en caso contrario, se rechaza. El parámetro opcional `--max-steps` permite interrumpir una ejecución como medida de seguridad, pero alcanzar ese límite **no** se considera una parada normal de la MT.

## 3. Compilación, pruebas y limpieza

Se necesita un compilador de C++ con soporte de C++17, `make` y Python 3 para las pruebas de integración de la línea de comandos, y el simulador no necesita bibliotecas externas.

Desde el directorio raíz del proyecto:

```sh
make
```

El ejecutable se genera en `build/tm_simulator`.

Para compilar y ejecutar las pruebas automatizadas:

```sh
make test
```

Las pruebas incluyen comprobaciones de las dos máquinas, entradas válidas e inválidas, casos límite, una máquina de dos cintas y pruebas de la interfaz de línea de comandos.

Para eliminar los objetos y ejecutables generados:

```sh
make clean
```

## 4. Ejecución del programa

### 4.1. Entradas desde fichero

```sh
./build/tm_simulator -config machines/MT1_lenguaje.txt -in tests/inputs_mt1.txt
./build/tm_simulator -config machines/MT2_conteo.txt -in tests/inputs_mt2.txt
```

El argumento `-config` es obligatorio, y el argumento `-in` es opcional, ya que cuando no se proporciona las cadenas se solicitan por teclado.

### 4.2. Entradas por teclado

```sh
./build/tm_simulator -config machines/MT1_lenguaje.txt
```

Introduce una cadena por línea. Una línea vacía representa la cadena vacía y `:q` termina la lectura. Cuando se emplea `-in`, cada línea no vacía representa una entrada, las líneas que empiezan por `#` se ignoran y la cadena `""` representa la cadena vacía.

### 4.3. Límite opcional de pasos

```sh
./build/tm_simulator -config machines/MT2_conteo.txt -in tests/inputs_mt2.txt --max-steps 1000000
```

Si se alcanza el límite, el programa informa de que la ejecución ha sido interrumpida y no clasifica la entrada como aceptada o rechazada por parada normal.

### 4.4. Salida

Para cada entrada se muestran:

- La cadena procesada.
- El estado en el que se detuvo la ejecución o el estado actual si se alcanzó el límite.
- El número de transiciones ejecutadas.
- Si la cadena ha sido aceptada, rechazada o si la ejecución ha sido interrumpida.
- El contenido de todas las cintas y la posición de cada cabeza.

La cabeza se representa poniendo el símbolo leído entre corchetes, por ejemplo, `ab[c]d` indica que la cabeza está sobre `c`. Si una cinta tiene un contenido muy extenso, el método de visualización evita imprimir miles de celdas y muestra un aviso con la posición de la cabeza.

## 5. Formato de los ficheros de configuración

El fichero de configuración contiene las siguientes líneas, ignorando las líneas vacías y las líneas completas cuyo primer carácter no blanco sea `#`.

```text
# Comentarios
q1 q2 q3 ...       # conjunto Q
 a1 a2 a3 ...      # alfabeto de entrada Sigma
A1 A2 A3 ...       # alfabeto de cinta Gamma
q1                 # estado inicial
.                  # símbolo blanco
q2 q3              # conjunto de estados de aceptación F
1                  # número de cintas
...                # transiciones, una por línea
```

La línea del conjunto de estados de aceptación puede contener varios estados. En este simulador, un `-` en esa línea representa un conjunto de aceptación vacío.

Para una MT con `k` cintas, cada transición tiene `2 + 3k` campos:

```text
estado_origen lectura_1 ... lectura_k estado_destino escritura_1 ... escritura_k movimiento_1 ... movimiento_k
```

Para una cinta, por ejemplo:

```text
q0 a q1 X R
```

Para dos cintas:

```text
qCopy a . qCopy a a R R
```

En este último caso, se leen `a` y `.` en las dos cintas, se pasa a `qCopy`, se escribe `a` en ambas y se mueven ambas cabezas a la derecha. Cada símbolo de cinta debe ser un solo carácter, y cada transición debe especificar la lectura, escritura y movimiento de todas las cintas.

### 5.1. Símbolo reservado `.`

El carácter `.` se reserva en las configuraciones y **no puede pertenecer al alfabeto de entrada ni aparecer en las cadenas de entrada**. En las configuraciones de este proyecto se utiliza para representar el blanco de la cinta. No debe utilizarse como un símbolo literal de una cadena que se quiera procesar.

### 5.2. Validaciones

Antes de ejecutar la máquina, el programa comprueba, entre otras cosas, que:

- El conjunto de estados `Q` no esté vacío y el estado inicial pertenezca a `Q`.
- El número de cintas sea positivo.
- El símbolo blanco pertenezca a `Gamma` y no pertenezca a `Sigma`.
- `Sigma` esté contenido en `Gamma` y no incluya `.`.
- Todos los estados de aceptación pertenezcan a `Q`.
- Los estados y símbolos utilizados en las transiciones estén definidos.
- Cada transición contenga exactamente el número de campos correspondiente al número de cintas.
- Los movimientos sean exclusivamente `L` o `R`.
- No existan dos transiciones con la misma combinación de estado y símbolos leídos, para preservar el determinismo.
- Cada cadena de entrada solo contenga símbolos de `Sigma`.

Si el fichero no se puede abrir, la definición no es válida o la entrada contiene un símbolo prohibido, el programa muestra un error.

## 6. Estructura del proyecto

```text
Practica2_MT/
├── include/
│   └── turing/
│       ├── ConfigurationParser.h
│       ├── MachineDefinition.h
│       ├── Tape.h
│       ├── Transition.h
│       └── TuringMachine.h
├── src/
│   ├── ConfigurationParser.cpp
│   ├── MachineDefinition.cpp
│   ├── Tape.cpp
│   ├── Transition.cpp
│   ├── TuringMachine.cpp
│   └── main.cpp
├── machines/
│   ├── MT1_lenguaje.txt
│   └── MT2_conteo.txt
│   └── examples/
│       ├── EJemplo1_MT.txt
│       ├── Ejemplo2_MT.txt
│       └── MT_multicinta_demo.txt
├── tests/
│   ├── inputs_mt1.txt
│   ├── inputs_mt2.txt
│   ├── inputs_multitape.txt
│   ├── invalid_input.txt
│   ├── test_cli.py
│   └── test_turing_machine.cpp
├── tools/
│   └── generate_graphs.py
├── docs/
│   └── graphs/
│       ├── MT1_lenguaje.dot
│       ├── MT1_lenguaje.png
│       ├── MT1_lenguaje.svg
│       ├── MT2_conteo.dot
│       ├── MT2_conteo.png
│       └── MT2_conteo.svg
├── libs/
│   └── .gitkeep
├── build/
│   └── .gitkeep
├── .gitignore
├── Makefile
└── README.md
```

## 7. Descripción de los ficheros y las clases

### 7.1. `src/main.cpp`

Es el punto de entrada del programa y coordina el resto de componentes. Procesa los argumentos de la línea de comandos, carga la configuración mediante `ConfigurationParser`, crea la máquina, obtiene las entradas desde fichero o teclado, reinicia la máquina para cada cadena y muestra el resultado. También gestiona los errores mediante excepciones y mensajes por consola.

Las opciones reconocidas son:

- `-config <fichero>`: obligatorio; contiene la definición de la MT.
- `-in <fichero>`: opcional; contiene las cadenas de entrada.
- `--max-steps <n>`: opcional; establece un límite de seguridad para la ejecución.
- `-h` o `--help`: muestra la ayuda.

### 7.2. `include/turing/Transition.h` y `src/Transition.cpp`

El fichero declara dos estructuras relacionadas con las transiciones:

- **`TransitionKey`** identifica una transición mediante el estado actual y un vector con los símbolos que leen todas las cabezas. Su `operator<` compara ambos elementos y permite utilizar la clave en un `std::map`.
- **`Transition`** almacena el estado destino (`nextState`), los símbolos que se escriben (`writeSymbols`) y los movimientos (`movements`). Los vectores mantienen el orden de las cintas.

Esta separación permite buscar la transición usando el estado y la lectura, y obtener como resultado el conjunto de operaciones que se deben ejecutar.

### 7.3. `include/turing/MachineDefinition.h` y `src/MachineDefinition.cpp`

La clase **`MachineDefinition`** representa la definición formal de la MT. Almacena el conjunto de estados `Q`, los alfabetos `Sigma` y `Gamma`, el estado inicial, el símbolo blanco, los estados de aceptación, el número de cintas y la función de transición.

Los métodos `addState`, `addInputSymbol`, `addTapeSymbol`, `addAcceptingState` y `addTransition` permiten construir la definición. Los métodos de consulta (`states()`, `tapeAlphabet()`, `tapeCount()`, `isAccepting()` y otros) dan acceso a sus datos.

`findTransition()` busca la transición asociada a un estado y vector de lecturas. `validateInput()` comprueba que una cadena pertenezca al alfabeto de entrada y rechaza el carácter reservado `.`. `validate()` verifica la consistencia de la definición completa, incluidos los estados, alfabetos, movimientos y determinismo.

### 7.4. `include/turing/ConfigurationParser.h` y `src/ConfigurationParser.cpp`

La clase **`ConfigurationParser`** lee el fichero de texto y genera un objeto `MachineDefinition`. Su método principal, `parseFile(path)`, abre el fichero, elimina líneas vacías y comentarios, interpreta las siete líneas iniciales de la definición y lee las transiciones restantes.

Para una máquina multicinta calcula cuántos campos debe tener cada transición a partir del número de cintas. Una vez leída la información, llama a `validate()` antes de devolver la definición. Así se mantiene separado el análisis del formato del código que ejecuta la máquina.

### 7.5. `include/turing/Tape.h` y `src/Tape.cpp`

La clase **`Tape`** representa una cinta infinita en ambas direcciones y su cabeza de lectura/escritura. Utiliza `headPosition_`, una coordenada con signo para la posición de la cabeza, y `cells_`, un `std::map` que almacena las celdas no blancas.

Esta representación dispersa evita reservar memoria para una cinta infinita: las posiciones que no están almacenadas se interpretan como blancas. Sus métodos principales son:

- `read()`: lee el símbolo bajo la cabeza.
- `symbolAt(position)`: consulta una posición concreta.
- `write(symbol)`: escribe en la posición actual; si se escribe el blanco, elimina la celda del mapa.
- `move(direction)`: mueve la cabeza a izquierda o derecha.
- `headPosition()`: devuelve la posición actual.
- `render()`: produce una representación de la cinta y marca la celda actual con corchetes.

La clase también comprueba que el movimiento sea válido y detecta los límites del tipo numérico usado para representar las posiciones.

### 7.6. `include/turing/TuringMachine.h` y `src/TuringMachine.cpp`

La clase **`TuringMachine`** es el motor de ejecución. Contiene una `MachineDefinition`, el estado actual, un vector de cintas y un indicador de parada.

- `reset(input)` valida la entrada, crea las cintas vacías, escribe la cadena en la primera cinta y deja la cabeza en el primer símbolo. El resto de las cintas comienzan vacías y todas las cabezas parten del origen.
- `currentTransition()` recoge el símbolo que lee cada cabeza y busca la transición que corresponde a la combinación del estado y todas las lecturas.
- `performStep()` aplica la transición: primero escribe en todas las cintas, después mueve todas las cabezas y finalmente actualiza el estado. Las lecturas se recogen antes de modificar las cintas, de modo que el paso implementa la variante simultánea.
- `run(maxSteps)` repite la ejecución hasta que no haya transición o se alcance el límite opcional. Devuelve un `RunResult` con el estado de parada, la aceptación, el número de pasos y la indicación de si se alcanzó el límite.

La aceptación se evalúa cuando la máquina se detiene normalmente: si el estado actual está en `F`, la entrada se acepta; de lo contrario, se rechaza. El límite opcional no altera esta definición de parada.

### 7.7. Ficheros de pruebas

- **`tests/test_turing_machine.cpp`:** ejecuta pruebas de las dos máquinas, comprueba entradas aceptadas y rechazadas, verifica la máquina multicinta y los ejemplos proporcionados, y comprueba que `.` se rechaza como símbolo de entrada. También recorre exhaustivamente las 255 cadenas sobre `{a,b}` de longitud entre 0 y 7 para comprobar los resultados de las dos máquinas.
- **`tests/test_cli.py`:** prueba el ejecutable completo desde la consola con configuraciones y ficheros de entrada, y verifica que se informen resultados y errores esperados.
- **`tests/inputs_mt1.txt` y `tests/inputs_mt2.txt`:** contienen las entradas de prueba para las dos máquinas principales.
- **`tests/inputs_multitape.txt`:** contiene la entrada para la máquina de demostración multicinta.
- **`tests/invalid_input.txt`:** contiene una entrada inválida con el carácter reservado `.`.

### 7.8. `docs/` y `tools/generate_graphs.py`

El directorio `docs/graphs/` contiene los grafos de las dos MT en formatos PNG y SVG, además de sus fuentes `.dot`.

`tools/generate_graphs.py` genera los grafos a partir de los ficheros de configuración usando Graphviz (`dot`). Para regenerarlos, con Graphviz instalado, se ejecuta:

```sh
python3 tools/generate_graphs.py
```

### 7.10. `machines/examples/`

- **`examples/Ejemplo1_MT.txt`:** ejemplo que reconoce cadenas binarias con un número impar de ceros.
- **`examples/Ejemplo2_MT.txt`:** ejemplo que duplica un número representado en unario.
- **`examples/MT_multicinta_demo.txt`:** máquina de dos cintas que copia la entrada de la primera cinta a la segunda. Se incluye para comprobar el formato extendido de las transiciones y la ejecución multicinta.

## 8. Máquinas de Turing de la práctica

### 8.1. MT1 — Reconocimiento del lenguaje

- **Configuración:** `machines/MT1_lenguaje.txt`.
- **Grafo:** `docs/graphs/MT1_lenguaje.png` y `docs/graphs/MT1_lenguaje.svg`.
- **Fuente Graphviz:** `docs/graphs/MT1_lenguaje.dot`.
- **Número de cintas:** 1.
- **Estado inicial:** `q0`.
- **Estado de aceptación:** `qAccept`.
- **Alfabeto de entrada:** `Sigma = {a, b}`.
- **Alfabeto de cinta:** `Gamma = {., a, b, X, Y}`.

La máquina reconoce el lenguaje:

\[
L = \{a^n b^m \mid m \geq n,\ n > 0\}.
\]

La estrategia es marcar cada `a` procesada con `X` y buscar una `b` todavía no emparejada para marcarla con `Y`. Después vuelve hacia la zona inicial y repite el proceso. Cuando ya no quedan `a` por procesar, comprueba que el resto esté compuesto únicamente por marcas `Y` o por `b` sin emparejar. Si falta una `b` para alguna `a`, la cadena está vacía, aparece una `a` después de las `b` o la estructura no cumple el lenguaje, la máquina no alcanza el estado de aceptación.

Ejemplos:

| Entrada | Resultado |
|---|---|
| `ab` | Aceptada |
| `aabbb` | Aceptada |
| `aab` | Rechazada: hay menos `b` que `a` |
| `b` | Rechazada: no hay ninguna `a` |
| `aba` | Rechazada: las `a` no preceden a todas las `b` |

### 8.2. MT2 — Conteo unario de `b` seguido del conteo unario de `a`

- **Configuración:** `machines/MT2_conteo.txt`.
- **Grafo:** `docs/graphs/MT2_conteo.png` y `docs/graphs/MT2_conteo.svg`.
- **Fuente Graphviz:** `docs/graphs/MT2_conteo.dot`.
- **Número de cintas:** 1.
- **Estado inicial:** `qInit`.
- **Estado de aceptación:** `qAccept`.
- **Alfabeto de entrada:** `Sigma = {a, b}`.
- **Alfabeto de cinta:** `Gamma = {., a, b, c, d, A, B, C, D, E, 0, 1, :}`.

La máquina recibe una cadena formada por `a` y `b` y la transforma en el número de `b` representado en unario, seguido de un blanco, seguido del número de `a` representado en unario. El símbolo `1` representa una unidad. Si no aparece ninguno de los símbolos de un tipo, el contador correspondiente se representa con `0`.

Para hacerlo, utiliza símbolos auxiliares para marcar caracteres procesados y un marcador temporal para localizar el final de la entrada. Construye las zonas de conteo, elimina el contenido original y sustituye el separador temporal por el blanco. La cabeza termina al principio del resultado.

El blanco se muestra como `.` en las salidas del simulador, por lo que las siguientes representaciones describen la cinta final:

| Entrada | Cinta final | Significado |
|---|---|---|
| `abbabaabb` | `[1]1111.1111` | Cinco `b` y cuatro `a` |
| `aa` | `[0].11` | Cero `b` y dos `a` |
| `bb` | `[1]1.0` | Dos `b` y cero `a` |
| `a` | `[0].1` | Cero `b` y una `a` |
| `b` | `[1].0` | Una `b` y cero `a` |
| Cadena vacía | `[0].0` | Cero `b` y cero `a` |

En la notación de salida, los corchetes indican la posición de la cabeza. Por ejemplo, `[1]1111.1111` contiene cinco unos a la izquierda del blanco y cuatro a su derecha.

## 9. Pruebas y comprobaciones

Las pruebas se ejecuta con:

```sh
make test
```

Se prueban la aceptación y el rechazo de cadenas de MT1, las salidas de MT2 para casos normales y límite (incluida la cadena vacía), la máquina de dos cintas, los dos ejemplos proporcionados y el rechazo del símbolo reservado `.`. También se comprueba el uso de `-config` y `-in` desde la interfaz de consola.
