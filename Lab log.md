# Concurrent Servers with the sockets API - Lab experience log

This was done at home, using the UC3M VPN and ssh to connect to the virtual
machines.

## 1. Compile

Cloning and compilation were easy. The port used was 8990

2 terminals were open:

* One on `vit001`, running the server with

    ```bat
    ./TCPechod 8990
    ```

* One on `vit002`, running a client with

    ```bat
    ./TCPecho vit001.lab.it.uc3m.es 8990
    ```

## 2. Examine traffic with tcpdump

Two more terminals were open:

* Another one on `vit001` (total 2), running

    ```bat
    tcpdump port 8990
    ```

* A new client on `vit003`, running

    ```bat
    ./TCPecho vit001.lab.it.uc3m.es 8990
    ```

Both clients were successfully connected simultaneously and produced echo.
Traffic observed through `tcpdump` was normal. The outputs can be checked in
the logs folder

## 3. Sequential server section 3 problem

The sequential server from the previous lab had a server issue, which was
caused by the fact that the active sockets created for each connection were not
closed when the client that had started that connection sent the FIN segment.
Therefore, those active sockets were never closed, and the server did not
correctly sever the connections with the clients when it closed, preventing
another server to start on the same port. It was fixed by adding a call to
`close()` on the active socket dedicated to the client when the client closed
the connection.

In order to test this, a new terminal was open on `vit001` (total 3) to run
`netstat`

First, `netstat` was run while both connections were open. It showed the
passive socket (listening) and the two active sockets created (established):

```txt
[2024-10-08, 13:15:23] netstat -ptan | grep ":8[0-9]\+"
(Not all processes could be identified, non-owned process info
 will not be shown, you would have to be root to see it all.)
tcp        0      0 0.0.0.0:8990            0.0.0.0:*               LISTEN      151175/./TCPechod   
tcp        0      0 163.117.171.191:8990    163.117.171.192:36030   ESTABLISHED 151197/./TCPechod   
tcp        0      0 163.117.171.191:8990    163.117.171.193:38466   ESTABLISHED 183677/./TCPechod   
```

Then, the clients were killed, and `netstat` was run again to check the status
of the server's ports

```txt
[2024-10-08, 13:20:52] netstat -ptan | grep ":8[0-9]\+"
(Not all processes could be identified, non-owned process info
 will not be shown, you would have to be root to see it all.)
tcp        0      0 0.0.0.0:8990            0.0.0.0:*               LISTEN      151175/./TCPechod   
tcp        0      0 163.117.171.191:837     163.117.168.100:2049    TIME_WAIT   -                   
```

After the clients, the server was killed as well and `netstat` was run a third time

```txt
[2024-10-08, 13:21:02] netstat -ptan | grep ":8[0-9]\+"
(No info could be read for "-p": geteuid()=493990 but you should be root.)
tcp        0      0 163.117.171.191:831     163.117.168.100:2049    ESTABLISHED -                   
```

As we can see, the connections were closed correctly by the server. This can be
also seen in the tcpdump output: the server sends back a FIN,ACK in response to
the clients' FIN.

```txt
[2024-10-08, 13:20:37] tcpdump -l port 8990
tcpdump: verbose output suppressed, use -v[v]... for full protocol decode
listening on eth0, link-type EN10MB (Ethernet), snapshot length 262144 bytes
13:20:42.728632 IP vit003.lab.it.uc3m.es.38466 > vit001.clc-build-daemon: Flags [F.], seq 1077944704, ack 2705179984, win 64223, options [nop,nop,TS val 1403532967 ecr 1510792646], length 0
13:20:42.729251 IP vit001.clc-build-daemon > vit003.lab.it.uc3m.es.38466: Flags [F.], seq 1, ack 1, win 65142, options [nop,nop,TS val 1511195206 ecr 1403532967], length 0
13:20:42.729721 IP vit003.lab.it.uc3m.es.38466 > vit001.clc-build-daemon: Flags [.], ack 2, win 64222, options [nop,nop,TS val 1403532968 ecr 1511195206], length 0
13:20:44.520015 IP vit002.lab.it.uc3m.es.36030 > vit001.clc-build-daemon: Flags [F.], seq 896179996, ack 3023253300, win 64235, options [nop,nop,TS val 1261048347 ecr 1601013185], length 0
13:20:44.520604 IP vit001.clc-build-daemon > vit002.lab.it.uc3m.es.36030: Flags [F.], seq 1, ack 1, win 65154, options [nop,nop,TS val 1604415631 ecr 1261048347], length 0
13:20:44.520950 IP vit002.lab.it.uc3m.es.36030 > vit001.clc-build-daemon: Flags [.], ack 2, win 64234, options [nop,nop,TS val 1261048348 ecr 1604415631], length 0
``` 

After this, the server could be started again, and clients were able to connect

```txt
[2024-10-08, 13:21:19] ./TCPechod 8990
Parent: Waiting for incomming connections at port 8990
Parent: Incomming connection from 163.117.171.192 remote port 44554
Parent: Waiting for incomming connections at port 8990
	child 190462 : receiving data: 
asdf from vit002
```

## 4. Sequential server section 5 problem

The sequential server from the previous lab had issues when two clients tried
to connect simultaneously and send data. Only the first client to connect was
able to send data, as the server was sequential and only attended one
connection at a time. Furthermore, in section 6, if the second client was
killed before the first, when the server tried to attend to that connection,
as the second client was already dead, the connection was aborted, indicated
by the reset frames observed. 

In this case, the server can listen to and respond to two clients
simultaneously, as it forks the main process in order to attend to both
connections at the same time.

