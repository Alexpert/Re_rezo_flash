#include <stdio.h>

#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/fon.h"		/* Primitives de la boite a outils */
#include "../lib/array_list.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

struct server_instance {
	int socket;
	array_list_t *clients;
	int max_socket;
};

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

	// int nb_clients = 0;
	// int max_clients = 5;
	// int max_socket = numsockserveur;
	// int *clients = malloc(sizeof(int) * max_clients);

	struct server_instance *instance = malloc(sizeof(struct server_instance));
	instance->socket = numsockserveur;
	instance->clients = init_array_list();
	instance->max_socket = numsockserveur;

	while(1) {
		FD_ZERO(&rdfs);
		FD_SET(instance->socket, &rdfs);

		for (int i = 0; i < instance->clients->current_index; ++i)
			FD_SET((int) get(instance->clients, i), &rdfs);

	  if((select(instance->max_socket + 1, &rdfs, NULL, NULL, NULL)) < 0) {
			perror("select()");
	    exit(errno);
	  }
		if (FD_ISSET(instance->socket, &rdfs)) {
			int new_sock = h_accept(instance->socket, p_adr_serv);
			add_element(instance->clients, (void *) new_sock);
			instance->max_socket = new_sock > instance->max_socket ? new_sock : instance->max_socket;
			printf("nb_clients: %d\n", instance->clients->current_index);
			h_writes(new_sock, "ping", 5);
		} else {
			for (int i = 0; i < instance->clients->current_index; ++i) {
				if (FD_ISSET((int) get(instance->clients, i), &rdfs)) {
					h_reads(get(instance->clients, i), str_recv, 5);
					printf("%s\n", str_recv);
					char *buff = malloc(sizeof(char) * 5);
					sprintf(buff, "%d", i);
					h_writes(get(instance->clients, i), buff, 5);
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
