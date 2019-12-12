#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "../lib/fon.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define SIZE_RECV 30

#define CLEAR_STDIN { int c; while((c = getchar()) != '\n' && c != EOF); }

typedef struct
{
	char cmd_id[2];
	char login[7];
	char arg[21];
} Message;

void display_help()
{
	printf("Les différentes commandes disponibles sont : \n");
	printf("\t - s : S'abonner à un nouvel utilisateur \n");
	printf("\t - u : Se désabonner d'un utilisateur \n");
	printf("\t - p : Publier un nouveau message \n");
	printf("\t - h : Afficher cette aide \n");
	printf("\t - e : Quitter l'application \n");
}

void display_msg(Message *m)
{
	printf("[%s] : \n", m->login);
	printf("\t%s\n", m->arg);
	printf("--------------------\n");
}

void client_appli(char *serveur, char *service)
/* procedure correspondant au traitement du client de votre application */
{
	printf("%s: %s\n", serveur, service);
	size_t login_size = 7;

	struct sockaddr_in *p_adr_client;
	int numsockclient = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(SERVICE_DEFAUT, NULL, SOCK_STREAM, &p_adr_client);
	char *str_recv = malloc(sizeof(char) * SIZE_RECV);
	char *psd = NULL;

	//Connexion
	h_connect(numsockclient, p_adr_client);

	//Login
	printf("Bonjour et bienvenue sur FlashTweet !\n");
	printf("Veuillez saisir votre nom d'utilisateur : \n");
	getline(&psd, &login_size, stdin);
	psd[6] = '\0';

	Message *msg = malloc(sizeof(Message));
	strcpy(msg->cmd_id, "1");
	strcpy(msg->login, psd);
	memcpy((void *)str_recv, (void *)msg, sizeof(Message));
	h_writes(numsockclient, str_recv, SIZE_RECV);
	h_reads(numsockclient, str_recv, SIZE_RECV);

	memcpy((void *)msg, (void *)str_recv, sizeof(Message));

	if (strcmp(msg->cmd_id, "2") == 0)
	{
		printf("Erreur lors de la connexion\n");
		h_close(numsockclient);
		exit(-1);
	}
	printf("Pour afficher l'aide avec les commandes que vous pouvez effectuer, tapez h\n");
	char cmd;
	char *args = NULL;
	char *login = NULL;
	size_t arg_size = 21;
	int nb;
	do
	{
		// h_reads(numsockclient, str_recv, SIZE_RECV);
		// printf("msg recieved\n");
		// memcpy((void *)msg, (void *)str_recv, sizeof(Message));
		// if (strcmp(msg->cmd_id, "3") == 0)
		// {
		// 	nb = atoi(msg->arg);
		// 	for (int i = 0; i < nb; i++)
		// 	{
		// 		h_reads(numsockclient, str_recv, SIZE_RECV);
		// 		memcpy((void *)msg, (void *)str_recv, sizeof(Message));
		// 		display_msg(msg);
		// 	}
		// }
		// printf("%s>", psd);
		// cmd = getc(stdin);
		switch (cmd)
		{
		case 's':
			printf("A qui souhaitez-vous vous abonner ? : \n");
			CLEAR_STDIN;
			getline(&login, &login_size, stdin);
			login[6] = '\0';
			strcpy(msg->cmd_id, "2");
			strcpy(msg->login, psd);
			strcpy(msg->arg, login);
			break;
		case 'u':
			printf("De qui souhaitez-vous vous désabonner ? : \n");
			CLEAR_STDIN;
			getline(&login, &login_size, stdin);
			login[6] = '\0';
			strcpy(msg->cmd_id, "3");
			strcpy(msg->login, psd);
			strcpy(msg->arg, login);
			break;
		case 'l':
			strcpy(msg->cmd_id, "5");
			strcpy(msg->login, psd);
			break;
		case 'p':
			printf("Saisissez le message à poster : \n");
			CLEAR_STDIN;
			getline(&args, &arg_size , stdin);
			args[strlen(args)-1] = '\0';
			strcpy(msg->cmd_id, "4");
			strcpy(msg->login, psd);
			strcpy(msg->arg, args);
			break;
		case 'h':
			display_help();
			break;
		case 'e':
			strcpy(msg->cmd_id, "6");
			strcpy(msg->login, psd);
			break;
		default:
			cmd = 'h';
			printf("Commande inconnue !\n");
			break;
		}
		if (cmd != 'h')
		{
			memcpy((void *)str_recv, (void *)msg, sizeof(Message));
			h_writes(numsockclient, str_recv, SIZE_RECV); // On envoie la requête au serveur
			sleep(1);
			h_reads(numsockclient, str_recv, SIZE_RECV); //On récupère la réponse
			memcpy((void *)msg, (void *)str_recv, sizeof(Message));

			switch (cmd)
			{
			case 's':
				if (strcmp(msg->cmd_id, "1") == 0)
				{
					printf("L'abonnement a été effectué\n");
				}
				else
				{
					printf("Erreur : l'abonnement a échoué \n");
				}
				break;
			case 'u':
				if (strcmp(msg->cmd_id, "1") == 0)
				{
					printf("Le désabonnement a été effectué\n");
				}
				else
				{
					printf("Erreur : le désabonnement a échoué \n");
				}
				break;
			case 'p':
				printf("%s\n",msg->cmd_id);
				if (strcmp(msg->cmd_id, "1") == 0)
				{
					printf("Le message a été publié\n");
				}
				else
				{
					printf("Erreur : la publication a échoué \n");
				}
				break;
			case 'e':
				if (strcmp(msg->cmd_id, "1") == 0)
				{
					printf("Déconnexion en cours...\n");
					h_close(numsockclient);
					exit(0);
				}
				else
				{
					printf("Erreur : la déconnexion a échoué \n");
				}
				break;
			case 'l':
				if (strcmp(msg->cmd_id, "5") == 0)
				{
					nb = atoi(msg->arg);
					printf("Liste des abonnements : \n");
					for (int i = 0; i < nb; i++)
					{
						h_reads(numsockclient, str_recv, SIZE_RECV); //On récupère la réponse
						memcpy((void *)msg, (void *)str_recv, sizeof(Message));
						printf("\t %s\n", msg->login);
					}
				}
				break;
			default:
				break;
			}
		}
	} while (1);
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
