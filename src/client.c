#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>

#include "../lib/fon.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define SIZE_RECV 30

#define CLEAR_STDIN                                 \
	{                                               \
		int c;                                      \
		while ((c = getchar()) != '\n' && c != EOF) \
			;                                       \
	}

char *psd;
size_t login_size = 7;
size_t arg_size = 21;
size_t cmd_size = 2;

typedef struct
{
	char cmd_id[2];
	char login[7];
	char arg[21];
} Message;

void display_help()
{
	printf("Les différentes commandes disponibles sont : \n");
	printf("\t - s <login> : S'abonner à un nouvel utilisateur de pseudo <login> \n");
	printf("\t - u <login> : Se désabonner d'un utilisateur de pseudo <login>\n");
	printf("\t - p <message>: Publier un nouveau message de contenu <message>\n");
	printf("\t - l : Afficher la liste des personnes auxquelles on est abonné\n");
	printf("\t - h : Afficher cette aide \n");
	printf("\t - e : Quitter l'application \n");
}

void display_msg(Message *m)
{
	printf("--------------------\n");
	printf("[%s] : \n", m->login);
	printf("\t%s\n", m->arg);
	printf("--------------------\n");
}

void send_to_server(int socket, char* cmdline)
{
	char *buffer = malloc(sizeof(char) * SIZE_RECV);
	Message *msg = malloc(sizeof(Message));
	//getline(&cmd, &cmd_size, stdin);

	switch (cmdline[0])
	{
	case 's':
		strcpy(msg->cmd_id, "2");
		strcpy(msg->login, psd);
		strncpy(msg->arg, cmdline+2,6);
		break;
	case 'u':
		strcpy(msg->cmd_id, "3");
		strcpy(msg->login, psd);
		strncpy(msg->arg, cmdline+2,6);
		break;
	case 'l':
		strcpy(msg->cmd_id, "5");
		strcpy(msg->login, psd);
		break;
	case 'p':
		strcpy(msg->cmd_id, "4");
		strcpy(msg->login, psd);
		strncpy(msg->arg, cmdline+2,20);
		break;
	case 'h':
		display_help();
		break;
	case 'e':
		strcpy(msg->cmd_id, "6");
		strcpy(msg->login, psd);
		break;
	default:
		cmdline[1] = 'h';
		printf("Commande inconnue !\n");
		break;
	}
	if (cmdline[1] != 'h')
	{
		memcpy((void *)buffer, (void *)msg, sizeof(Message));
		h_writes(socket, buffer, SIZE_RECV); // On envoie la requête au serveur
		sleep(1);
	}
}

void display_serveur(char *buffer, int numsockclient)
{
	//printf("display server !\n");
	Message *msg = malloc(sizeof(Message));
	memcpy((void *)msg, (void *)buffer, sizeof(Message));
	int id = atoi(msg->cmd_id);
	if (id < 0)
	{
		printf("Erreur (%d)\n", id);
		id = id * (-1);
	}
	//printf("id : %d\n", id);
	switch (id)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		printf("\t -%s\n", msg->arg);
		break;
	case 6:
		printf("\t -%s\n", msg->arg);
		h_close(numsockclient);
		exit(0);
		break;
	case 7:
		display_msg(msg);
		break;
	default:
		break;
	}
}

void client_appli(char *serveur, char *service)
{
	printf("%s: %s\n", serveur, service);

	struct sockaddr_in *p_adr_client;
	int numsockclient = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(SERVICE_DEFAUT, NULL, SOCK_STREAM, &p_adr_client);

	//Connexion
	h_connect(numsockclient, p_adr_client);
	char *str_recv = malloc(sizeof(char) * SIZE_RECV);

	//Login
	printf("Bonjour et bienvenue sur FlashTweet !\n");
	printf("Veuillez saisir votre nom d'utilisateur : \n");
	psd = NULL;
	getline(&psd, &login_size, stdin);
	psd[6] = '\0';

	Message *msg = malloc(sizeof(Message));
	strcpy(msg->cmd_id, "1");
	strcpy(msg->login, psd);
	memcpy((void *)str_recv, (void *)msg, sizeof(Message));
	h_writes(numsockclient, str_recv, SIZE_RECV);
	h_reads(numsockclient, str_recv, SIZE_RECV);

	memcpy((void *)msg, (void *)str_recv, sizeof(Message));

	if (strcmp(msg->cmd_id, "-1") == 0)
	{
		printf("Erreur de Login\n");
		h_close(numsockclient);
		exit(-1);
	}

	printf("--------------------\n");
	printf("Avant de pouvoir taper une commande, vous devez appuyer sur la touce Entrée de votre clavier.\n");
	printf("Pour afficher l'aide avec les commandes que vous pouvez effectuer, tapez h\n");
	printf("--------------------\n");

	while (1)
	{
		fd_set rdfs;

		FD_ZERO(&rdfs);
		FD_SET(numsockclient, &rdfs);
		FD_SET(STDIN_FILENO, &rdfs);
		int fdmax = numsockclient;

		int err = select(fdmax + 1, &rdfs, NULL, NULL, NULL);
		switch (err)
		{
		case 0:
			/* timeout */
			break;

		case -1:
			/* error */
			puts("error");
			break;

		default:

			if (FD_ISSET(STDIN_FILENO, &rdfs))
			{
				CLEAR_STDIN;
				printf("%s>",psd);
				fgets(str_recv,SIZE_RECV,stdin);
				send_to_server(numsockclient, str_recv);
			}

			if (FD_ISSET(numsockclient, &rdfs))
			{
				int ret = h_reads(numsockclient, str_recv, SIZE_RECV);
				if (ret > 0)
				{
					display_serveur(str_recv, numsockclient);
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{

	char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
	char *service = SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 1: /* arguments par defaut */
		printf("serveur par defaut: %s\n", serveur);
		printf("service par defaut: %s\n", service);
		break;
	case 2: /* serveur renseigne  */
		serveur = argv[1];
		printf("service par defaut: %s\n", service);
		break;
	case 3: /* serveur, service renseignes */
		serveur = argv[1];
		service = argv[2];
		break;
	default:
		printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur, service);
}
