[English version] (https://gitlab.pervasive.it.uc3m.es/aptel/sockets2_concurrent_servers/blob/master/README_EN.md)

# Servidores concurrentes con el API de sockets

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

<img src="https://gitlab.pervasive.it.uc3m.es/aptel/sockets2_concurrent_servers/raw/master/img/process.png" width="500px">

En esta práctica dispone de todos los ficheros necesarios para probar un servidor concurrente. Para descargarlo, usa el siguiente comando:

 ```
 git -c http.sslVerify=false clone https://gitlab.pervasive.it.uc3m.es/aptel/sockets2_concurrent_servers.git
 ```

> Si tienes dificultades para entender qué sucede en el caso de threads o hilos:

> Puesto que en esta práctica usaremos procesos en lugar de hilos, es fundamental que comprendas cómo funcionan. En la carpeta `processes` dispones de un ejemplo de uso de procesos.  Descomprímelo, compila (make) y ejecuta ./processes. Observa qué pasa con la variable test.

> Las funciones principales para gestionar los hilos son:
* pthread_create() permite crear un nuevo hilo de ejecución.
* pthread_join() hace que un hilo espere por otro.
* pthread_mutex_lock() (y similares) permite que dos o más hilos accedan sincronizadamente a un recurso común.
* pthread_exit() termina el hilo.

> En la carpeta `threads` dispones de un ejemplo del uso de threads, puedes compilarlo (make) y ejecutarlo ./threads. Observa qué mensaje imprime cada hilo. 

Otras estrategias que utilizan los servidores de alto rendimiento incluyen tener servidores prearrancados entre los que van distribuyendo las peticiones según van llegando, para evitar el tiempo de espera hasta que se crea el hilo o proceso. Por lo general, los servidores TCP son concurrentes, para poder servir simultáneamente a varios clietnes (y los servidores UDP secuenciales).

A partir de ahora, vamos a utilizar el servidor de eco concurrente (TCPEchod) y el mismo cliente de eco de la práctica anterior (rebautizado a TCPEcho) que encontrarás en la carpeta psockets2.

Para compilar el archivo haremos lo mismo que en la práctica de servidores secuenciales:

```
make clean
make
```

Si examinas el código del servidor concurrente, verás que el servidor TCPEchod después de recibir una conexión (accept) arranca un proceso con `fork`. El proceso hijo (creado como resultado de la llamada a `fork` identificable al comprobar `fork()==0`) cierra su referencia al descriptor del socket pasivo heredado del padre (`msock`), y ejecuta el servicio `TCPechod()`. El proceso padre (que diferenciamos con `fork()!=0) cierra el descriptor del socket `asock` devuelto por `accept()` (al que está conectado el cliente), y vuelve a la ejecución de `accept()` a la espera de nuevas conexiones.

### 1. Compila:

```
make clean
make
```

ejecuta el servidor en el puerto 8XXX (recuerda que sumamos las tres últiamas cifras de la IP de tu máquina a 8000 para evitar colisiones entre grupos):

```
TCPechod 8xxx 
```

y en otra ventana, ejecuta el cliente:

```
./TCPecho <server host> 8xxx
```

y observa su comportamiento.

### 2. Con tcpdump examine el tráfico con origen/destino en el puerto 8XXX, y mientras, desde otras máquina, lanza dos clientes para ver el tráfico intercambiado.

### 3. ¿Observas en esta versión del servidor el problema que tenía el servidor secuencial original en el apartado 3 de la práctica anterior? ¿Por qué? Utilice el comando `netstat` para observar las conexiones establecidas y ver los estdos y puertos efímeros de las mismas. Por ejemplo:

```
netstat -tn
netstat -putan
```

### 4. ¿Observa en esta versión del servidor el problema que tenía el servidor secuencial original en el apartado 5? ¿Por qué?

## Señales

Cuando un proceso es iniciado, se puede cambiar su "curso" (pausarlo, reanudarlo, cancelarlo, etc.) o un proceso padre puede ser notificado de la finalización de procesos hijos utilizando SEÑALES. Las señales permiten que los procesos se comuniquen entre si, y el kernel también pueda comunicarse con ellos.

Las señales son denotadas por: `SIG{NAME}`, donde es el nombre que se le da a la señal.

Hay un conjunto de señales ante las cuales todos los procesos se comportan igual, otras ante las que procesos distintos se comportarán de manera distinta, e incluso habrá procesos que ignoren determinadas señales porque la probabilidad de que ocurran es insignificante. Sin embargo, hay señales que no pueden ignorarse (por ejemplo, porque terminan un proceso), ya que son comunes a todos los procesos. En esta práctica, veremos algunas de las más importantes (o usadas).

### 5. Observa la línea del nuevo servidor que hay a continuación de la creación del socket: 

```c
 (void) signal(SIGCHLD, reaper);
```

y la función a la que hace referencia:

```c
void reaper(int sig){
  int status;
  while (wait3(&status, WNOHANG, (struct rusage *)0) > 0)
} 
```

el cliente:

Esto es un manejador (handler) de la señal `SIGCHILD`, que se ejecuta cuando dicha señal
es recibida. SIGCHILD es una señal que lanza el sistema para indicar al proceso padre
que un hijo ha terminado. La función signal registra el manejador de forma que cuando se
reciba esta señal, se ejecute el manejador. El manejador permite que el sistema libere
todos los recursos que tiene el proceso hijo. Cuando uno de los procesos hijos muere, se
queda en estado zombie hasta que el padre recoja la señal SIGCHILD. La función reaper
permite que esto se realice sin bloquear al proceso padre mediante la llamada wait3 (es
una variante de la llamada wait), y mediante WNOHANG, controlamos el caso de que algún
proceso reciba una señal que no vaya destinada a él (porque no la haya generado uno de
sus procesos hijos). 

Prueba lo siguiente: compila dos instancias del servidor concurrente, uno con la llamada a
signal y otro sin dicha llamada. ¿Qué diferencias observa entre ambas versiones
examinando la tabla de procesos (con el comando ps x) después de que un cliente cierre
su conexión? 

NOTA: El comando ps nos permite ver los procesos que se están ejecutando. La opción x
permite mostrar los procesos del usuario actual. 

Otro manejador muy importante es el de la señal SIGPIPE, que en el caso de sockets indica que
la conexión se ha roto (recibe un RST). El manejador de SIGPIPE permite incluir el código
necesario para tratar esta excepción (incrementar el número de fallos observados, intentar
reestablecer la conexión, cerrar descriptores auxiliares que ya no son necesarios, etc.). Debe de
estar especificado en el protocolo que siguen cliente y servidor. 

### 6. Incluya un manejador de SIGPIPE en el servidor de eco, siguiendo el ejemplo del manejador
de SIGCHILD. Experimente y anote las ocasiones en que observa que se ejecuta el manejador. 

Usa `kill -l` para ver las señales disponibles y `kill -signal pid` para mandar la señal indicada al proceso especificado.
