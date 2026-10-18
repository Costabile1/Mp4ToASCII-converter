### 30/09
- hoy me decidi por usar la libreria IMUGUI para dibujar los fotogramas, esta es mucho mas rapido ya que lo hace a bajo nivel y no procesa los caracteres como un texto real, solo los imprime en pantalla
- Las Metricas que usabamos para medir la relacion entre el tiempo de procesado y de dibujado pasaron de unas 500 => lo cual ahcia todo muy lento, a ahora estar en torno a las 15 y 100, lo cual es una mejora sustancial.
- tambien preguntandole a gemini ahora agreuge unos calculos para que la reproduccion de video siempre se acople a al ventana SFML que le damos.
- Ya que dentro de todo funciona ahora el proximo paso es hacer la conexion con una web para que pueda usarse libremente.
- O sino la otra opcion es rompert todo y hacer que se pueda mantener el color y se dibujen los caracters con color.

### 29/09
![alt text](image.png)
Resolvi un re problema que tenia, es que hacia una variable arreglo char, le ponia todos los caracteres que debia tener, pero no le ponia un terminador \0, y el problema era que depsues cuando asginaba ese arreglo al string principal, C se ponia a buscar el \0 en toda la memoria yeso causaba que se llende de basura.
Ahora se recorta lso repetidos y se muestra bastante bien, pero por alguan forma esun poquito raro, no tan Bien como cuando no lo comprimia
- Aun asi los videos pesados y mas largos se hacen re lentos, no me sirve. por que tampoco hago el archivo mucho mas liviano
- El problema parase ser cuando tengo frame que no tiene caractereres iguales seguidos, entonces itero por unmonton de caracetes y voy metiendo de a uno en el strig principal
- probando medir los tiempo de ejecucion entre el procesamiento de caracteres repetidos y lo que tarda la ventana en mostrar todos los caracteres de cada frame, nos damos cuenta que la ventana tarda al rededor de 500 veces mas en mostrar los caracteres que el codigo en crear el frame. aca tenemos el cuello de botella.
- Para solucionar esto tenemos 3 opciones:
- 1. usar otro metodo de sfml que no sea crear texto de alto nivel
- 2. usar la libreria imgui que en teoria es mas rapida para esto
- 3. abandonar smfl y usar la api nativa de windows.
Vamos a empezar desde el 1 y vamos hacia el 3 si no encontramos soluciones.

### 27/09
- voy a cambair todo, ahora no hay mas ancho y largo, por que lo voy cambiando mucho, ahora se itera de una char por char y a la mierda,nhasta buscar un char que finaliza todo el frame.
- Ya cambie toda la de buscar caracteres iguales
- Me falta cambiar la que busca filas frames iguales, aunque se me ocurre que es apuede quedar asi pq precisamente busca filas iguales de frame no modificados.

Problemas:
- Estoy haciendo el reproductor pero teno problemas
- los frames pueden llear !a10\nC\n
y estoy buscando indicde de C en el anteriro
Y LO QUE TENGO QUE AHCER ES CONTAR CANTIDAD DE \N, estoy al horno.

Ya arreglue varias cosas
ahora en el entorno de prueba funcinna correctamete
Solucione el problema que tenia mas arriba sobre /n, 
pero ahora tengo un problema que salra la excepcion cuando ejecute, nose si es el acceder en un fram[i] que no corresponde o es stoi, por que un momento verifi que que mierda tenia la varibale operador, que debria ller !a osea oeprador y una letra y tenia operador y numero, osea faltaba el caracter o letra ascii

--Problemas con el tema de agarrar correctamwente los caracteres que indican si hay cracteres repetidos.

- Finalmente hice que funcione el comprimidor que hacia que las letras repetidas las guarde mejor
- PERO al mostrar el video el calculo era lento y hacia que el video vaya mucho, no, Muchisimo mas lento.
- ademas la compresion tampoco es que hacia que el video no pesara nada, seguia pesando bastante y ademas el video se hace lentisimo.

Opciones para la proxima:
- primeo decodificar todo, y despues mostrarlo, ocupando muchisima memoria
- 


### 26/09
- Ahora ahora tenemos el lector de video frame a frame, lo guarda correctamente en un archivo .milo
- El reprodcutor abre correctamente una ventana sfml y muetra el video en caracteres ASCII
- Actualmente implemente 2 cabeceras, una genral para todo el archivo
- otra especifica de cada frame, que indica cosas como si tiene caracteres repetidos que se omitieron por espacio o filas simialres a otro frame y donde y demas
- Implemente las funciones que buscan lineas iguales en frame continuos tomando uno como referencia (no esta probado en un video real aun, nose como se va a comportar cuando cambien los frame)
- implemente la funcion de reducir caracteres repetidos
- Estas 2 fucniones funcionan en un entorno de pruebas, falta probar en frames de un video real.