# Compilar proyecto:
cmake -B build -S . -G "Ninja"
cmake --build build

# 🎬 Proyecto Milo: Conversor y Reproductor de Video a ASCII

Milo es un motor escrito en C++ que transforma videos estándar (MP4, AVI, etc.) en arte ASCII de alta eficiencia. En lugar de procesar y renderizar texto en tiempo real con altos costos de CPU, Milo compila los videos en un formato binario propietario (`.milo`) utilizando compresión RLE y cálculo de gradientes. Luego, utiliza un visor de alto rendimiento basado en **SFML e ImGui** para reproducirlos de forma fluida.

## ✨ Funcionalidades Principales

* **Codificación por Gradientes (OpenCV):** Analiza cada fotograma mediante matrices de Sobel para detectar bordes y gradientes, asignando el carácter ASCII óptimo (`.`, `-`, `|`, `@`, etc.) según la intensidad y dirección de la luz.
* **Formato Binario Propietario (`.milo`):** Los videos se empaquetan en archivos binarios optimizados mediante una cabecera estricta (`HeaderLILA`) que almacena metadatos (columnas, filas, FPS) y evita sobrecargas de memoria (padding).
* **Compresión RLE (Run-Length Encoding):** Reduce drásticamente el tamaño del archivo `.milo` al comprimir secuencias de caracteres ASCII repetidos utilizando una sintaxis de control hexadecimal.
* **Renderizado Acelerado por GPU:** Utiliza ImGui (configurado con índices de 32 bits) montado sobre SFML para dibujar fotogramas enteros de texto plano en pantalla sin colapsar, escalando dinámicamente la fuente para que el video siempre encaje perfectamente en la ventana.
* **Gestor de Archivos Integrado:** Interfaz de línea de comandos (CLI) interactiva que escanea y administra automáticamente los videos convertidos dentro de la carpeta aislada `videos_milo/`.

---

## 🛠️ Requisitos Previos e Instalación

Para compilar y ejecutar este proyecto, necesitas las siguientes herramientas y bibliotecas:

1. **C++17 o superior** (necesario para `<filesystem>` y características modernas).
2. **CMake** (Sistema de construcción).
3. **OpenCV** (Para la extracción de fotogramas y filtros de imagen).
4. **SFML** (Gestión de ventanas y contextos gráficos).
5. **ImGui + ImGui-SFML** (Renderizado de interfaz y texto masivo). 
   > **⚠️ Importante para ImGui:** Para evitar el error de *Assertion failed (16-bit indices)* al renderizar miles de caracteres, asegúrate de haber descomentado la línea `#define ImDrawIdx unsigned int` en tu archivo `imconfig.h`.


# Especificación del Formato de Video ASCII (.milo)

**Versión:** 1.0
**Extensión:** `.milo`
**Arquitectura:** Binario estructurado con carga útil (payload) de texto.

---

## 1. Estructura General del Archivo

Todo archivo `.milo` sigue un flujo secuencial estricto. Está compuesto por una cabecera global en el byte cero, seguida de una lista de paquetes, donde cada paquete representa un fotograma individual, cada fotograma posee su propia cabezera..

`[ CABECERA GLOBAL ]` -> `[ FrameHedear 0 (I-Frame) ][ FOTOGRAMA 0 (I-Frame) ]` -> `[ FrameHedear 1 (I-Frame) ][ FOTOGRAMA 1 (P-Frame) ]` -> `...`

---

## 2. Cabecera Principal (Main Header)

Se encuentra siempre al principio del archivo. Ocupa un tamaño fijo de **14 bytes**. Se lee de forma binaria.

| Byte Offset | Tamaño | Tipo de Dato | Nombre | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| 0 | 5 bytes | `char[5]` | `Firma` | Número mágico. Siempre debe ser `milo`. Si no es esto, abortar lectura. |
| 4 | 2 bytes | `uint16_t` | `Columnas` | Ancho del video en caracteres (ej. 120). |
| 6 | 2 bytes | `uint16_t` | `Filas` | Alto del video en caracteres (ej. 40). |
| 8 | 1 byte | `uint8_t` | `FPS` | Tasa de fotogramas por segundo (ej. 30). |
| 9 | 4 bytes | `uint32_t` | `TotalFrames` | (Opcional) Cantidad total de fotogramas para calcular barra de progreso. |

---

## 3. Cabecera de Fotograma (Frame Header)

Antes de leer el texto de un fotograma, se debe leer su cabecera. Esto indica cómo el reproductor debe interpretar los datos que siguen.

| Byte Offset | Tamaño | Tipo de Dato | Nombre | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| 0 | 4 byte | `4 bytes` | `id` | representa un identificado de frame |
| 1 | 4 bytes | `uint32_t` | `PayloadSize` | Tamaño exacto en bytes del texto del fotograma que sigue a continuación. |

---

## 4. Codificación y Cadenas de Control

El bloque de datos (Payload) de cada fotograma contiene los caracteres ASCII y utiliza marcadores (cadenas/caracteres especiales) para controlar el flujo de renderizado.

### Marcadores Estándar
*   **Fin de Línea (EOL):** `\n` (Byte `0x0A`). Indica que el reproductor debe saltar a la siguiente fila en la pantalla.
*   **Fin de Fotograma (EOF):** No requiere un carácter especial. El reproductor sabrá que el fotograma terminó cuando haya leído exactamente la cantidad de bytes dictada por el `PayloadSize` de su cabecera.

### Compresión Temporal (Fotogramas Tipo 'P')
Para evitar guardar fotogramas repetidos, los fotogramas tipo `P` usan marcadores al inicio de cada línea para indicar qué hacer respecto al fotograma anterior guardado en memoria.

*   **Línea Idéntica (`C`):** Si el primer byte de una línea es `C`, significa "Copia esta línea entera del fotograma en Referencia". El reproductor simplemente salta a la siguiente fila sin procesar caracteres. 
    *   *Ejemplo de carga útil:* `C\n` (Pesa solo 2 bytes en lugar de los 120 bytes de la línea original).

*   **Línea Modificada# Compilar proyecto:
cmake -B build -S . -G "Ninja"
cmake --build build

# 🎬 Proyecto Milo: Conversor y Reproductor de Video a ASCII

Milo es un motor escrito en C++ que transforma videos estándar (MP4, AVI, etc.) en arte ASCII de alta eficiencia. En lugar de procesar y renderizar texto en tiempo real con altos costos de CPU, Milo compila los videos en un formato binario propietario (`.milo`) utilizando compresión RLE y cálculo de gradientes. Luego, utiliza un visor de alto rendimiento basado en **SFML e ImGui** para reproducirlos de forma fluida.

## ✨ Funcionalidades Principales

- **Codificación por Gradientes Nativa:** Analiza cada fotograma calculando matrices de Sobel directamente sobre la memoria RAM para detectar bordes y gradientes, asignando el carácter ASCII óptimo (`.`, `-`, `|`, `@`, etc.) según la intensidad y dirección de la luz.
- **Extracción Ligera con FFmpeg (Pipes):** Prescinde de librerías pesadas como OpenCV. Utiliza subprocesos estándar de C (`popen`) para capturar el flujo de píxeles puros desde FFmpeg y escalar la imagen internamente usando `stb_image_resize2`.
- **Formato Binario Propietario (`.milo`):** Los videos se empaquetan en archivos binarios optimizados mediante cabeceras estrictas (`HeaderLILA` y `framehdr`) protegidas contra alineación de memoria (padding).
- **Compresión RLE (Run-Length Encoding):** Reduce drásticamente el tamaño del archivo `.milo` al comprimir secuencias de caracteres ASCII repetidos utilizando caracteres de control hexadecimal.
- **Renderizado Acelerado por GPU:** Utiliza ImGui (configurado con índices de 32 bits) montado sobre SFML para dibujar fotogramas enteros de texto plano en pantalla sin colapsar, escalando dinámicamente la fuente para que el video siempre encaje perfectamente en la ventana sin desbordar.
- **Gestor de Archivos Integrado:** Interfaz de línea de comandos (CLI) interactiva que escanea y administra automáticamente los videos convertidos dentro de la carpeta aislada `videos_milo/`.

---

## 🛠️ Requisitos Previos e Instalación para Desarrolladores

Para compilar y trabajar sobre este proyecto, necesitas las siguientes herramientas:

1. **C++17 o superior** (necesario para `<filesystem>` y características modernas).
2. **CMake** (Sistema de construcción) y un generador como Ninja o MinGW Makefiles.
3. **FFmpeg y FFprobe:** Los ejecutables `ffmpeg.exe` y `ffprobe.exe` deben estar instalados en el sistema (PATH) o ubicados en el mismo directorio que el ejecutable de Milo para extraer los fotogramas vía tuberías.
4. **SFML** (Gestión de ventanas y contextos gráficos).
5. **ImGui + ImGui-SFML** (Renderizado de interfaz y texto masivo). 
   > **⚠️ Importante para ImGui:** Para evitar el error de *Assertion failed (16-bit indices)* al renderizar miles de caracteres, asegúrate de haber descomentado la línea `#define ImDrawIdx unsigned int` en tu archivo `imconfig.h`.

---

# Especificación del Formato de Video ASCII (.milo)

**Versión:** 1.0  
**Extensión:** `.milo`  
**Arquitectura:** Binario estructurado empaquetado a nivel de byte (1-byte alignment via `#pragma pack`).

---

## 1. Estructura General del Archivo

Todo archivo `.milo` sigue un flujo secuencial estricto. Está compuesto por una cabecera global en el byte cero, seguida de una lista de paquetes, donde cada paquete representa un fotograma individual. Cada fotograma posee su propia cabecera descriptiva.

`[ Cabecera Principal ]` -> `[ FrameHeader 0 ][ Payload ASCII 0 ]` -> `[ FrameHeader 1 ][ Payload ASCII 1 ]` -> `...`

---

## 2. Cabecera Principal (HeaderLILA)

Se encuentra siempre al principio del archivo. Ocupa un tamaño fijo exacto de **10 bytes**. 

| Byte Offset | Tamaño | Tipo de Dato | Nombre | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| 0 | 5 bytes | `char[5]` | `Firma` | Número mágico. Siempre debe ser `milo\0`. Si no es esto, abortar lectura. |
| 5 | 2 bytes | `uint16_t` | `Columnas` | Ancho del video en caracteres (escalado). |
| 7 | 2 bytes | `uint16_t` | `Filas` | Alto del video en caracteres (escalado). |
| 9 | 1 byte | `uint8_t` | `FPS` | Tasa de fotogramas por segundo. |

---

## 3. Cabecera de Fotograma (framehdr)

Antes de leer el texto de un fotograma, se debe leer en binario su cabecera estructural de **11 bytes**. Esto indica cómo el reproductor debe interpretar y gestionar la memoria de los datos que siguen.

| Byte Offset | Tamaño | Tipo de Dato | Nombre | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| 0 | 4 bytes | `int` | `id` | Identificador numérico secuencial del frame. |
| 4 | 1 byte | `bool` | `soy_copiado` | Flag que indica si este frame reutiliza información estructural de otro. |
| 5 | 4 bytes | `int` | `id_copiado` | El ID del frame del cual se están heredando filas enteras (-1 si no aplica). |
| 9 | 2 bytes | `uint16_t` | `tamaño` | Tamaño exacto en bytes del texto ASCII (Payload) que sigue a continuación. |

---

## 4. Codificación y Cadenas de Control (Payload)

El bloque de datos de cada fotograma contiene los caracteres ASCII y utiliza marcadores (bytes de control) para optimizar el tamaño en disco y dictar saltos al renderizador.

### Marcadores Estándar
-   **Fin de Línea (EOL):** `\n` (Byte `0x0A`). Indica al reproductor que la fila terminó.
-   **Fin de Fotograma (EOF/Transición):** `\x04` (Byte End of Transmission). Marca explícitamente la finalización del string ASCII para el fotograma actual.

### Compresión Temporal (Fotogramas de Referencia)
- No implementado

### Compresión RLE (Letras Repetidas)
Las secuencias de más de 5 caracteres idénticos se colapsan usando caracteres de control del sistema ASCII original:
-   **Sintaxis:** `[\x02][Carácter_repetido][Cantidad_en_char][\x03]`
-   *Ejemplo interno:* Si el fondo es negro y hay 10 espacios seguidos (` (`+`):** Si el primer byte de la línea es `+`, significa "Esta línea cambió". Los caracteres que siguen al `+` sobrescriben la línea actual.
    *   *Ejemplo de carga útil:* `+###  @@  ##\n`
* **Caractere repetidos :** se prepresnetan con (inicialziador,caracter,veces que se repite, finalizador) un ejemplo es  `!@5|`

---

## 5. Ejemplo de Flujo de Lectura (Cudicódigo)

1. Leer 13 bytes iniciales. Validar "milo".
2. Bucle infinito:
   a. Leer 1 byte para `TipoFrame` (I o P).
   b. Leer 4 bytes para saber el `PayloadSize`.
   c. Extraer `PayloadSize` bytes a un *string*.
   d. Si es `I`: Imprimir tal cual.
   e. Si es `P`: Leer línea por línea; si empieza con `=`, mantener línea anterior.
   f. Mostrar en pantalla (SFML) y esperar tiempo (1000/FPS).