### Proximas Tareas 26/09
-  Listo : Arreglar el reproductor pq se meclan los fotogramas en el reproductor (nose cual es el problema)
- Algoritmo para hacer mas liviano el archivo donde guardo los fotogramas .milo

## AlgoritmoS para Reducri el peso del archivo .milo

1. Codificación Predictiva (Delta Encoding / P-Frames)
Teoría temporal. En un video a 30 FPS, la diferencia visual entre el fotograma 1 y el fotograma 2 suele ser minúscula; la mayor parte del fondo permanece estática. Esta estrategia se basa en la premisa de que guardar la pantalla completa una y otra vez es un desperdicio masivo. La teoría dicta que, después de guardar un fotograma completo (I-Frame), los siguientes fotogramas (P-Frames) solo deben contener una lista de las "coordenadas y valores" que cambiaron respecto al instante anterior. Si un personaje se mueve, el archivo solo registra los píxeles nuevos del personaje y los píxeles del fondo que se restauran, ignorando por completo el resto del lienzo.

- "Tengo 2 Frame n y n+1"
- Voy a comprar cadenas de string o matrices??
Nesesito Asignarles Prioirdad.
- comprar lineas enteras //soy igual al anteror
    -voy a usar un frame principal para comparar varios contra el, cunado no haya nada igual al frame de referenci, cambio el frame de referencia. EL FRAME DE REFERENCIA NUNCA DEBE ESTAR CAMBIADO, DEBE TENER EL LARO Y ANCHO QUE INDICA EL HEADERLILA.
- comprar columnas enteras //soy igual al enterior
- comprar linea de caracteres iguales //aca guardo &*100
- comprar columnas de caracteres iguales
- comprar matrices.

Necesito cambiar la forma en la que leo, ahora no leo siempre col*fila cantidad de memoria., necesito contar \n

######################################################################################################################
Por ahora
- comparo filas de Frame y Guardo UN "\n" (Co)
- comparo letras repetidas y escribo "!a5"
- el terminador de un frame es "|"
######################################################################################################################

2. Run-Length Encoding (RLE - Codificación por longitud de pistas)
Teoría espacial. Esta es la forma más pura de explotar la repetición de datos. En el arte ASCII, es extremadamente común tener grandes bloques del mismo carácter (por ejemplo, el cielo negro representado por cientos de espacios en blanco consecutivos). La teoría del RLE establece que, en lugar de registrar "espacio, espacio, espacio, espacio...", el algoritmo simplemente registra un multiplicador y el valor: "400 veces espacio". Esto colapsa áreas homogéneas masivas en apenas un par de bytes.

3. Codificación de Entropía (Algoritmo de Huffman)
Teoría estadística. Basada en la teoría de la información de Claude Shannon. Se fundamenta en que no todos los caracteres aparecen con la misma frecuencia. En un texto normal (o un video ASCII), un "espacio" o un punto aparecerán muchísimo más que el símbolo @. La codificación de entropía abandona la idea de que todos los caracteres deben pesar lo mismo. Le asigna identificadores muy cortos (a nivel de bits) a los caracteres más comunes, y códigos más largos a los caracteres raros. Al final, el peso global del archivo colapsa matemáticamente porque lo que más se repite es lo que menos pesa.

4. Empaquetado de Bits (Bit-Packing / Paletización)
Teoría de optimización de ancho de banda. En informática estándar, un carácter o letra ocupa siempre 1 byte completo (8 bits), lo que permite hasta 256 símbolos diferentes. Sin embargo, si tu video ASCII utiliza una paleta de solo 8 o 16 caracteres distintos, usar 8 bits enteros por letra deja una enorme cantidad de bits "vacíos" e inútiles. La teoría del bit-packing propone alterar la estructura de almacenamiento a nivel máquina para usar únicamente los bits estrictamente necesarios (por ejemplo, 4 bits para representar 16 símbolos), logrando cortar el peso del archivo exactamente a la mitad desde la raíz, sin importar el contenido del video.