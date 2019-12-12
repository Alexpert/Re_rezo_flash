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
#define SIZE_RECV 30

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

typedef struct {
	char cmd_id[2];
	char login[7];
	char arg[21];
} Message;

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
	instance->flash = create_flash_instance();
	return instance;
}

int add_client(struct server_instance *instance)
{
	int new_sock = h_accept(instance->socket, instance->p_adr_serv);
	add_element(instance->clients, (void *)new_sock);
	instance->max_socket = MAX(instance->max_socket, new_sock);
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
			max = MAX(max, (int) get(instance->clients, i));
	}
}

void send_post_to(Post *p, User *dest, Flash_Instance *f) {

	if (dest->socket > 0) {
		Message msg;
		sprintf(msg.login, "%*s", 2, "4");
		sprintf(msg.login, "%*s", 6, getUser_from_id(p->author_id, f)->login);
		sprintf(msg.arg, "%*s", 20, p->content);
		h_writes(dest->socket, (char *) (&msg), sizeof(Message));
		remove_element(p->readers, (void *) dest->id);
		if (p->readers->current_index == 0)
			remove_element(f->posts, p);
			//destruct_post(p);
	}
}

void send_post(Post *p, Flash_Instance *f) {
	for (int i = 0; i < p->readers->current_index; ++i) {
		send_post_to(p, (User *)get(p->readers, i), f);
	}
}

void update_msg(int socket, char *login, Flash_Instance *f) {
	User *user =getUser_from_login(login, f);
	for (int i = 0; i < f->posts->current_index; ++i) {
		Post *p = get(f->posts, i);
		int j = 0;
		for (; j < p->readers->current_index && get(p->readers, j) != user; ++j);
		if (j != p->readers->current_index)
			send_post_to(p, user, f);
	}
}

void send_user_to(User *u, int socket) {
	Message msg;
	sprintf(msg.login, "%*s", 2, "6");
	sprintf(msg.arg, "%*s", 20, u->login);
	h_writes(socket, (char *) (&msg), sizeof(Message));
}

void send_list(array_list_t *list, int socket) {
	Message msg;
	sprintf(msg.login, "%*s", 2, "5");
	sprintf(msg.arg, "%*d", 20, list->current_index);
	h_writes(socket, (char *) (&msg), sizeof(Message));

	for (int i = 0; i < list->current_index; ++i)
		send_user_to((User *) get(list, i), socket);
}

void
respond_client(int socket, char *str_recv, struct server_instance *s)
{

	/*
	Actions:
	1 Login/SignIn -> fail/win
	2 subscribe -> fail/win
	3 unsubscribe -> fail/win
	4 publish -> fail/win
	5 list -> list
	6 logout -> fail/win
	*/

	Flash_Instance *f = s->flash;

	Message *msg = malloc(sizeof(Message));
	Message *res = malloc(sizeof(Message));
	Post *p;
	memcpy((void *) msg, (void *) str_recv, sizeof(Message));

	printf("cmd_id: %s\n", msg->cmd_id);
	printf("login: %s\n", msg->login);
	printf("arg: %s\n", msg->arg);

	switch (msg->cmd_id[0]) {
		case '1':
			if(login(msg->login, socket, f) == 0) {
				strcpy(res->cmd_id, "1");
				printf("login 1\n");
			} else {
				strcpy(res->cmd_id, "2");
				printf("login 2\n");
			}
			//update_msg(socket, msg->login, f);
			printf("update done\n");
			break;
		case '2':
			if(subscribe(msg->login, msg->arg, f) == 0) {
				strcpy(res->cmd_id, "1");
			} else {
				strcpy(res->cmd_id, "2");
			}
			break;
		case '3':
			if(unsubscribe(msg->login, msg->arg, f) == 0) {
				strcpy(res->cmd_id, "1");
			} else {
				strcpy(res->cmd_id, "2");
			}
			break;
		case '4':
			if(p = publish(msg->login, msg->arg, f)) {
				strcpy(res->cmd_id, "1");
			} else {
				strcpy(res->cmd_id, "2");
			}
			send_post(p, f);
			break;
		case '5':
			send_list(list_sub(msg->login, f), socket);
			break;
		case '6':
			if(logout(msg->login, f) == 0) {
				strcpy(res->cmd_id, "1");
			} else {
				strcpy(res->cmd_id, "2");
			}
			break;
	}

	char *str_res = malloc(sizeof(Message));
	memcpy((void *) str_res, (void *) res, sizeof(Message));
	h_writes(socket, str_res, sizeof(Message));
	printf("reply sent\n");
}

void serveur_appli(char *service)
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

		if ((select(instance->max_socket + 1, &rdfs, NULL, NULL, NULL)) < 0) {
			perror("select()");
			exit(errno);
		}
		if (FD_ISSET(instance->socket, &rdfs)) {
			printf("listening server\n");
			int new_sock = add_client(instance);
		} else {
			printf("listening clients\n");
			for (int i = 0; i < instance->clients->current_index; ++i) {
				int curr_socket = (int) get(instance->clients, i);
				if (FD_ISSET(curr_socket, &rdfs)) {
					int ret = h_reads(curr_socket, str_recv, SIZE_RECV);
					if (ret > 0) {
						respond_client(curr_socket, str_recv, instance);
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
