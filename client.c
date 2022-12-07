#include <unistd.h>
#include <stdio.h> // for perror()
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> // for inet_pton

#include <string.h>

static void foco(char resultado[10]);

int main()
{
	char buffer[10];
	int rc;
	char resultado[10];
	
	char usuario[20];
	char contrasena[20];
	
	char direccionIP[15];
	int puerto;
	
	int salir = 0;
	
	printf("\tPrograma cliente\n");
	printf("Ingrese la direccion IP del servidor: ");
	scanf("%s", direccionIP);
	printf("Ingrese el puerto: ");
	scanf("%d", &puerto);
	
	
	int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_servidor < 0) {
		perror("Could not create socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(puerto);
	inet_pton(AF_INET, direccionIP, &server_addr.sin_addr);

	rc = connect(socket_servidor, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (rc < 0) {
		perror("No se puedo establecer la conexion con el servidor");
		exit(1);
	}
	
	printf("\n\t---Conexion establecida---\n");
	printf("***Para terminar la conexion, escriba salir como nombre de usuario y contrasena\n\n");
	
  	
	while (!salir) {
	
		printf("Ingrese el nombre de usuario: ");
		scanf("%s", usuario);
		printf("Ingrese la contrasena: ");
		scanf("%s", contrasena);
		
		if (strcmp(usuario, "salir") == 0 && strcmp(contrasena, "salir") == 0) {
			salir = 1;
		}
	
		send(socket_servidor, usuario, 100, 0); //envio de usuario
		send(socket_servidor, contrasena, 100, 0); //envio de contrasena
	
	
		// Recibe la confirmacion
		if(recv(socket_servidor, buffer, 10, 0) < 0)
		{ //Comenzamos a recibir datos del cliente
			printf("Error al recibir los datos\n");
			close(socket_servidor);
			return 1;
		}
		else
		{
			strcpy(resultado, buffer);
			bzero((char *)&buffer, sizeof(buffer));
			foco(resultado);
		}
	
	}
	
	printf("\n\t---Conexion terminada\n");
	close(socket_servidor);

	return 0;
}

static void foco(char resultado[10]){
	int value = strcmp(resultado, "Encender");
	
	if (value == 0){
		int status = system("usbrelay HURTM_1=1 > /dev/null 2>&1");
		printf("---Datos correctos\n\n");
		sleep(4);
		status = system("usbrelay HURTM_1=0 > /dev/null 2>&1");
	}
	
	value = strcmp(resultado, "Apagar");
	if (value == 0)
	{
		int status = system("usbrelay HURTM_1=0 > /dev/null 2>&1");
		printf("---Datos incorrectos\n\n");
	}
}
