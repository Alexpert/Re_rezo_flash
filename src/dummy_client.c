#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/fon.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

typedef struct {
	char cmd_id[2];
	char login[7];
	char arg[21];
} Message;

void client_appli(char *serveur, char *service)
/* procedure correspondant au traitement du client de votre application */
{
	printf("%s: %s\n", serveur, service);

	struct sockaddr_in *p_adr_client;
	int serv = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(SERVICE_DEFAUT, NULL, SOCK_STREAM, &p_adr_client);

	h_connect(serv, p_adr_client);
	char *str_recv = malloc(sizeof(char) * 5);

  Message *recm = malloc(sizeof(Message));
  Message *senm = malloc(sizeof(Message));

	do {
    sscanf(senm->cmd_id, "cmd_id: %1s");
    sscanf(senm->login, "login: %1s");
    sscanf(senm->arg, "arg: %1s");
	}
	while (1);
}

int main(int argc, char *argv[])
{

	char *serveur = SERVEUR_DEFAUT;	/* serveur par defaut */
	char *service = SERVICE_DEFAUT;	/* numero de service par defaut (no de port) */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc) {
	case 1:		/* arguments par defaut */
		printf("serveur par defaut: %s\n", serveur);
		printf("service par defaut: %s\n", service);
		break;
	case 2:		/* serveur renseigne  */
		serveur = argv[1];
		printf("service par defaut: %s\n", service);
		break;
	case 3:		/* serveur, service renseignes */
		serveur = argv[1];
		service = argv[2];
		break;
	default:
		printf
		    ("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur, service);
}
