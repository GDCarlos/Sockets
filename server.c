//gcc server.c -o server -lpq -I /usr/include/postgresql

#include <unistd.h>
#include <stdio.h> // for perror()
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> // for inet_pton
#include <net/if.h>

#include </usr/include/postgresql/libpq-fe.h>

#define PUERTO 40002
#define BUFSIZE 2000

static void obtenerDireccionIP(unsigned char ip[15]);
static int buscarEnLaBD();

int main(){

	printf("\tPrograma servidor\n");

	unsigned char ip[15];
	int socket_servidor;
	int rc;
	char buffer[BUFSIZE];
	int done = 0;
	int salir = 0;
	
	char usuario[20];
	char contrasena[20];
	
	obtenerDireccionIP(ip);

	socket_servidor = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_servidor < 0) {
		perror("No se pudo conectar al socket: ");
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PUERTO);
	inet_pton(AF_INET, ip, &server_addr.sin_addr);

	rc = bind(socket_servidor, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (rc < 0) {
		perror("Couldn't bind server socket");
		exit(1);
	}

	rc = listen(socket_servidor, 5);
	if (rc < 0) {
		perror("Couldn't listen on server socket");
		exit(1);
	}

	printf("La direccion IP es: %s\n", inet_ntoa(server_addr.sin_addr));
	printf("Escuchando en el puerto: %d\n", (int) ntohs(server_addr.sin_port));

	while (!done) {
		int socket_cliente;
		struct sockaddr_in client_addr;
		socklen_t client_addr_size = sizeof(client_addr);

		socket_cliente = accept(socket_servidor, (struct sockaddr*) &client_addr, &client_addr_size);
		if (socket_cliente < 0) {
			perror("No se pudo aceptar la conexión");
			exit(1);
		}
		
		printf("\n\t---Cliente conectado---\n");
		
		while (!salir) {
			
			/*Lee osaurio*/
			if(recv(socket_cliente, buffer, 100, 0) < 0)
	  		{ //Comenzamos a recibir datos del cliente
			  //Si recv() recibe 0 el cliente ha cerrado la conexion. Si es menor que 0 ha habido algún error.
			    printf("Error al recibir los datos\n");
			    close(socket_cliente);
			    return 1;
			}
			else
			{
			    if (strcmp(buffer, "salir") == 0) {
			    	salir = 1;
			    }
			    strcpy(usuario, buffer);
			    bzero((char *)&buffer, sizeof(buffer));
			}
        	        printf("Nombre de usuario recibido: %s\n", usuario);
                
                
        	        // Recibe la contrasena
        		if(recv(socket_cliente, buffer, 100, 0) < 0)
			{ //Comenzamos a recibir datos del cliente
			  //Si recv() recibe 0 el cliente ha cerrado la conexion. Si es menor que 0 ha habido algún error.
				printf("Error al recibir los datos\n");
				close(socket_cliente);
				return 1;
			}
			else
			{
				if (strcmp(buffer, "salir") == 0) {
					salir = salir && 1;
					if (salir){
						done = 1;
					}
				
				}
				strcpy(contrasena, buffer);
				bzero((char *)&buffer, sizeof(buffer));
			}
        	        printf("Contraseña recibida: %s\n", contrasena);
	
			if (salir){
				printf("\n\t---Conexion terminada\n");			
			} else{
			
				
				if (buscarEnLaBD(usuario, contrasena) == 0){
					printf("---Datos correctos\n\n");
					/* Para el foco */
					send(socket_cliente, "Encender", 10, 0);
				
				}
				else{
					printf("---Datos incorrectos\n\n");
					send(socket_cliente, "Apagar", 10, 0);
				}		 
			}
			
		}
			
		close(socket_cliente);
		

		if (strcmp(buffer, "quit\r\n") == 0) {
			done = 1;
		}
	}

	close(socket_servidor);

	return 0;
}

static void obtenerDireccionIP(unsigned char ip[15])
{
    int fd;
    struct ifreq ifr;

    /*AF_INET - to define network interface IPv4*/
    /*Creating soket for it.*/
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;

    /*eth0 - define the ifr_name - port name
    where network attached.*/
    memcpy(ifr.ifr_name, "enp0s3", IFNAMSIZ - 1);

    /*Accessing network interface information by
    passing address using ioctl.*/
    ioctl(fd, SIOCGIFADDR, &ifr);
    /*closing fd*/
    close(fd);

    /*Extract IP Address*/
    strcpy(ip, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
}

static int buscarEnLaBD(char usuario[10], char contrasena[10]){
	PGconn *conn;
	PGresult *res;
	
	conn = PQsetdbLogin("localhost", "5432", NULL, NULL, "postgres", "postgres", "c03052002");
	
	int resultado;
	
	char consulta[1024];
	snprintf(consulta, sizeof(consulta), "SELECT * FROM proyectoTD.usuario WHERE nombre = '%s' AND contrasena = '%s'", usuario, contrasena);
	
	if(PQstatus(conn) != CONNECTION_BAD){
		res = PQexec(conn, consulta);
		// printf("---Estableciendo conexion con la base de datos---\n");
		if(res != NULL && PGRES_TUPLES_OK == PQresultStatus(res)){
			if (PQntuples(res) == 1){
				//printf("Datos correctos\n\n");
				resultado = 0;
			} else{
				//printf("Datos incorrectos\n\n");
				resultado = 1;
			}
			PQclear(res);
		}
	} else{
		printf("\n No se pudo conectar a la base de datos");
	}
	PQfinish(conn);
	return resultado;
}
