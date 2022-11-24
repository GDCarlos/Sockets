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

// #define PORT 40002

int main()
{
	int rc;

	/*
	if (argc < 2) {
		fprintf(stderr, "Usage: client <message>\n");
		exit(1);
	}
	*/
	
	char usuario[20];
	char contrasena[20];
	
	char direccionIP[15];
	int puerto;
	
	printf("\tPrograma cliente\n");
	printf("Ingresse la direccion IP del servidor: ");
	scanf("%s", direccionIP);
	printf("Ingrese el puerto: ");
	scanf("%d", &puerto);
	
	// printf("Direccion IP ingresada: %s\n", direccionIP);
	
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Could not create socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(puerto);
	inet_pton(AF_INET, direccionIP, &server_addr.sin_addr);

	rc = connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (rc < 0) {
		perror("No se puedo establecer la conexion con el servidor");
		exit(1);
	}
	
	printf("---Conexion establecida---\n");
	
	printf("Ingrese el nombre de usuario: ");
	scanf("%s", usuario);
	printf("Ingrese la contrasena: ");
	scanf("%s", contrasena);
	
	printf("Usuario: %s\n", usuario);
	printf("Constrasena: %s\n", contrasena);
	
	
	// Envia el nombre de usuario
	size_t len = strlen(usuario);
	char *msg = malloc(len + 3);
	strcpy(msg, usuario);
	strcat(msg, "\r\n");

	write(s, msg, strlen(msg) + 1);
	
	// Envia la contrasena
	len = strlen(contrasena);
	msg = malloc(len + 3);
	strcpy(msg, contrasena);
	strcat(msg, "\r\n");

	write(s, msg, strlen(msg) + 1);

	close(s);

	return 0;
}
