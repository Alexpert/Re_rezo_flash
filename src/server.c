#include <stdio.h>

#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/fon.h"		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"



void serveur_appli(char *service)
/* procedure correspondant au traitement du client de votre application */
{
	printf("%s\n", service);

  struct sockaddr_in *p_adr_serv;
	int numsockserveur = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(SERVICE_DEFAUT, SERVEUR_DEFAUT, SOCK_STREAM, &p_adr_serv);

	h_bind(numsockserveur, p_adr_serv);
	h_listen(numsockserveur, 1);

  char *str_recv = malloc(sizeof(char) * 5);
	fd_set rdfs;

	int nb_clients = 0;
	int max_clients = 5;
	int max_socket = numsockserveur;
	int *clients = malloc(sizeof(int) * max_clients);

	while(1) {
		FD_ZERO(&rdfs);
		FD_SET(numsockserveur, &rdfs);
		for (int i = 0; i < nb_clients; ++i) {
			FD_SET(clients[i], &rdfs);
		}

	  if((select(max_socket + 1, &rdfs, NULL, NULL, NULL)) < 0) {
			perror("select()");
	    exit(errno);
	  }
		if (FD_ISSET(numsockserveur, &rdfs)) {
			int numsockclient = h_accept(numsockserveur, p_adr_serv);
			clients[nb_clients++] = numsockclient;
			max_socket = numsockclient > max_socket ? numsockclient : max_socket;
			printf("nb_clients: %d\n", nb_clients);
			h_writes(numsockclient, "ping", 5);
		} else {
			for (int i = 0; i < nb_clients; ++i) {
				if (FD_ISSET(clients[i], &rdfs)) {
					h_reads(clients[i], str_recv, 5);
					printf("%s\n", str_recv);
					char *buff = malloc(sizeof(char) * 5);
					sprintf(buff, "%d", i);
					h_writes(clients[i], buff, 5);
				}
			}
		}
	}

	// do {
	// 	h_writes(numsockclient, "ping", 5);	//On envoi le mot à trouver (avec les tirets)
	// 	h_reads(numsockclient, str_recv, 5);	// On récupère la saisie du client
  //   printf("%s\n", str_recv);
	// } while (1);
}

int main(int argc, char *argv[])
{

	char *service = SERVICE_DEFAUT;	/* numero de service par defaut */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc) {
	case 1:
		printf("defaut service = %s\n", service);
		break;
	case 2:
		service = argv[1];
		break;

	default:
		printf("Usage:serveur service (nom ou port) \n");
		exit(1);
	}

  serveur_appli(service);
}
