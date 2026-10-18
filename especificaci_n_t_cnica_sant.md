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

*   **Línea Modificada (`+`):** Si el primer byte de la línea es `+`, significa "Esta línea cambió". Los caracteres que siguen al `+` sobrescriben la línea actual.
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