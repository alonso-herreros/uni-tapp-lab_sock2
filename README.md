[English version] (https://gitlab.pervasive.it.uc3m.es/aptel/sockets1_sequential_servers/blob/master/README_EN.md)

# Servidores secuenciales con el API de sockets

### Material de soporte
-  On-line man pages: socket(2), socket(7), send(2), recv(2), read(2), write(2), setsockopt(2), fcntl(2), select(2), tcp(7), ip(7).
-  Guide to using sockets by Brian "Beej" Hall
-  Tcpdump manual
-  Chapters 6, 7 y 8 of "Linux Socket Programming" by Sean Walton, Sams Publishing Co. 2001
-  Fichero cheat sheet en proyecto anterior

### Prácticas con sockets
Descripción de las prácticas de sockets Las prácticas de sockets se dividen en tres partes:
1. Servidores secuenciales (cliente y servidor de eco, opciones de sockets, análisis con tcpdump, servidores de ficheros)
2. **Servidores concurrentes (procesos, hilos).**
3. Entrada/Salida I/O (manejadores de señales, mecanismos de polling, select)

## Servidores concurrentes 

La diferencia fundamental con los servidores secuenciales es que los concurrentes arrancan un
nuevo proceso (fork) o hilo de ejecución (pthread_create) para dar servicio a las
conexiones que se van recibiendo.

Como se puede ver en la Figura 1, un proceso tiene su propia zona de memoria, mientras que los hilos de ejecución (threads) comparten la misma zona de memoria y los recursos1. La elección de uno u otro depende de las ventajas y desventajas que aportan cada uno de ellos, pero pueden ser usados indistintamente (por ejemplo, en esta práctica utilizaremos procesos, pero podríamos utilizar hilos). 



En esta práctica dispone de todos los ficheros necesarios para probar un servidor concurrente. Para descargarlo, usa el siguiente comando:

 ```
 git -c http.sslVerify=false clone https://gitlab.pervasive.it.uc3m.es/aptel/sockets1_sequential_servers.git
 ```

> Si tienes dificultades para entender qué sucede en el caso de threads o hilos:

> Puesto que en esta práctica usaremos procesos en lugar de hilos, es fundamental que comprendas cómo funcionan. En la carpeta `processes` dispones de un ejemplo de uso de procesos.  Descomprímelo, compila (make) y ejecuta ./processes. Observa qué pasa con la variable test.

> Las funciones principales para gestionar los hilos son:
* pthread_create() permite crear un nuevo hilo de ejecución.
* pthread_join() hace que un hilo espere por otro.
* pthread_mutex_lock() (y similares) permite que dos o más hilos accedan sincronizadamente a un recurso común.
* pthread_exit() termina el hilo.

> En la carpeta `threads` dispones de un ejemplo del uso de threads, puedes compilarlo (make) y ejecutarlo ./threads. Observa qué mensaje imprime cada hilo. 

