# proyecto Concurrencia
proyecto de concurrencia y sistemas distribuidos basado en 
el proyecto de Omar Aflak (https://gist.github.com/OmarAflak/aca9d0dc8d583ff5a5dc16ca5cdda86a), añadiendo componentes 
de OpenMP para paralelizar la aplicacion del filtro gaussiano.

# Instrucciones de Compilación y Correr Programa
<pre>
-g++ -o exec proyecto.cpp -lpng -fopenmp

-./exec
</pre>

# Cambiar de Imagen
para cambiar de imagen solo hay que cambiar la ruta en el archivo ``proyecto.cpp``, linea 172
