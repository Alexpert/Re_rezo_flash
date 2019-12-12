#ifndef FLASH_H
#define FLASH_H
#include "array_list.h"

typedef struct {
	int id;
	char login[6];
	int socket;		//-1 if notconnected
	array_list_t *following;
	array_list_t *followers;
} User;

typedef struct {
	int author_id;
	char content[20];
	array_list_t *readers;
} Post;

typedef struct {
	array_list_t *users;
	array_list_t *posts;
	int next_usr_id;
} Flash_Instance;

Flash_Instance *create_flash_instance();

/*
Retourne l'utilisateur ayant l'id id, NULL sinon
*/
User *getUser_from_id(int id, Flash_Instance * f);
User *getUser_from_login(char *login, Flash_Instance * f);

/*
Login crée l'utilisateur de nom login si celui ci n'existe pas et le connecte
Si il existe, "connecte" l'utilisateur
-1 en cas d'erreur 0 sinon
*/
int login(char *login, int socket, Flash_Instance * f);

/*
Abonne sub à l'utilisateur canal
-1 en cas d'erreur 0 sinon
*/
int subscribe(char *follower, char *following, Flash_Instance * f);

/*
Desbonne sub de l'utilisateur canal
-1 en cas d'erreur 0 sinon
*/
int unsubscribe(char *follower, char *following, Flash_Instance * f);

/*
Publie un message
-1 en cas d'erreur 0 sinon
*/
Post *publish(char *author, char *message, Flash_Instance * f);

/*
Liste les abonnements
-1 en cas d'erreur 0 sinon
*/
array_list_t *list_sub(char *user, Flash_Instance * f);

/*
"Déconnecte" l'utilisateur
-1 en cas d'erreur 0 sinon
*/
int logout(char *user, Flash_Instance * f);

#endif
