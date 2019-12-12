#include <stdio.h>

#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/fon.h"		/* Primitives de la boite a outils */
#include "../lib/array_list.h"
#include "../lib/flash.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define SIZE_RECV 32

struct server_instance {
	struct sockaddr_in *p_adr_serv;
	int socket;
	array_list_t *clients;
	int max_socket;
	Flash_Instance *flash;
};

struct server_instance *create_instance(int socket,
					struct sockaddr_in *p_adr_serv)
{
	struct server_instance *instance =
	    malloc(sizeof(struct server_instance));
	instance->p_adr_serv = p_adr_serv;
	instance->socket = socket;
	instance->clients = init_array_list();
	instance->max_socket = socket;
	instance->flash = malloc(sizeof(Flash_Instance));
	instance->flash->users = init_array_list();
	return instance;
}

int add_client(struct server_instance *instance)
{
	int new_sock = h_accept(instance->socket, instance->p_adr_serv);
	add_element(instance->clients, (void *)new_sock);
	instance->max_socket =
	    new_sock > instance->max_socket ? new_sock : instance->max_socket;
	printf("new socket %d count %d\n", new_sock,
	       instance->clients->current_index);
	return new_sock;
}

void remove_client(struct server_instance *instance, int socket)
{
	remove_element(instance->clients, (void *)socket);
	h_close(socket);
	printf("lost socket %d count %d\n", socket,
	       instance->clients->current_index);
	if (instance->max_socket == socket) {
		int max = 0;
		for (int i = 0; i < instance->clients->current_index; ++i)
			max = max > (int) get(instance->clients,i) ? max : (int)get(instance->clients, i);
	}
}

void
respond_client(int socket, char *str_recv,
	       int ret, Flash_Instance *f)
{
	//"%d: %d", code cmd, id user,
	/*
	Actions:
	1 Login/SignIn
	2 subscribe
	3 unsubscribe
	4 publish
	5 list
	6 logout
	*/
	// printf("%s\n", str_recv);
	// char *buff = malloc(sizeof(char) * 5);
	// sprintf(buff, "%d", socket);
	// h_writes(get(instance->clients, socket), buff, 5);

	int cmd_id;
	int usr_id;
	char *cmd_arg = malloc(sizeof(char) * 20);

	sscanf(str_recv, "%d;%d;", &cmd_id, &usr_id);
	strcpy(cmd_arg, &str_recv[12]);

	switch (cmd_id) {
		case 1:
			login(usr_id, cmd_arg, socket, f);
			break;
		case 2:
			subscribe(usr_id, cmd_arg, f);
			break;
		case 3:
			unsubscribe(usr_id, cmd_arg, f);
			break;
		case 4:
			publish(usr_id, cmd_arg, f);
			break;
		case 5:
			list_sub(usr_id, f);
			break;
		case 6:
			logout(usr_id, f);
			break;
	}
}

void serveur_appli(char *service)
/* procedure correspondant au traitement du client de votre application */
{
	printf("%s\n", service);

	struct sockaddr_in *p_adr_serv;
	int sockServeur = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(SERVICE_DEFAUT, SERVEUR_DEFAUT, SOCK_STREAM, &p_adr_serv);

	h_bind(sockServeur, p_adr_serv);
	h_listen(sockServeur, 1);

	char *str_recv = malloc(sizeof(char) * SIZE_RECV + 1);
	fd_set rdfs;

	struct server_instance *instance =
	    create_instance(sockServeur, p_adr_serv);

	while (1) {
		printf("loop\n");
		FD_ZERO(&rdfs);
		FD_SET(instance->socket, &rdfs);

		for (int i = 0; i < instance->clients->current_index; ++i)
			FD_SET((int)get(instance->clients, i), &rdfs);

		if ((select(instance->max_socket + 1, &rdfs, NULL, NULL, NULL))
		    < 0) {
			perror("select()");
			exit(errno);
		}
		if (FD_ISSET(instance->socket, &rdfs)) {
			printf("listening server\n");
			int new_sock = add_client(instance);
			h_writes(new_sock, "ping", 5);
		} else {
			printf("listening clients\n");
			for (int i = 0; i < instance->clients->current_index; ++i) {
				int curr_socket = (int) get(instance->clients, i);
				if (FD_ISSET(curr_socket, &rdfs)) {
					int ret = h_reads(curr_socket, str_recv, SIZE_RECV);
					if (ret > 0) {
						respond_client(curr_socket, str_recv, ret, instance->flash);
					} else {
						remove_client(instance, curr_socket);
					}
				}
			}
		}
	}
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
