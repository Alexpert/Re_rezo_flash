#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "flash.h"

Flash_Instance *create_flash_instance() {
	Flash_Instance *f_i = malloc(sizeof(Flash_Instance));
	f_i->users = init_array_list();
	f_i->posts = init_array_list();
	f_i->next_usr_id = 0;
	return f_i;
}

User *getUser_from_id(int id, Flash_Instance * f)
{
	int i = 0;
	while (i < f->users->current_index
	       && ((User *) get(f->users, i))->id != id)
		++i;
	if (i == f->users->current_index)
		return NULL;

	return (User *) get(f->users, i);
}

User *getUser_from_login(char *login, Flash_Instance * f)
{
	int i = 0;
	while (i < f->users->current_index && strcmp(((User *) get(f->users, i))->login, login) != 0) {
			printf("user: %ld\n", strlen(((User *)get(f->users, i))->login));
			printf("user: %ld\n", strlen(login));
			++i;
	}
	if (i == f->users->current_index)
		return NULL;

	return (User *) get(f->users, i);
}

int login(char *login, int socket, Flash_Instance * f)
{
	User *user = getUser_from_login(login, f);

	if (user == NULL) {	//ajout nouvel utilisateur
		if (login == NULL || strlen(login) != 6)
			return -1;

		User *newUser = malloc(sizeof(User));

		strcpy(newUser->login, login);
		newUser->id = ++(f->next_usr_id);
		newUser->socket = socket;
		newUser->following = init_array_list();
		newUser->followers = init_array_list();

		add_element(f->users, newUser);
	} else {		//connecter utilisateur
		user->socket = socket;
	}

	return 0;
}

int subscribe(char *follower, char *following, Flash_Instance * f)
{
	printf("%s subs %s", follower, following);
	User *follower_struct = (User *) getUser_from_login(follower, f);
	User *following_struct = (User *) getUser_from_login(following, f);

	if (follower_struct == NULL) {
		printf("%s not found", follower);
		return -1;
	}
	if (following_struct == NULL) {
		printf("%s not found", following);
		return -1;
	}

	add_element(follower_struct->following, (void *)following_struct);
	add_element(following_struct->followers, (void *)follower_struct);

	return 0;
}

int unsubscribe(char *follower, char *following, Flash_Instance * f)
{
	User *follower_struct = (User *) getUser_from_login(follower, f);
	User *following_struct = (User *) getUser_from_login(following, f);

	if (follower_struct == NULL || following_struct == NULL)
		return -1;

	element_t *e =
	    remove_element(following_struct->followers, follower_struct);

	if (e == NULL)
		return -1;

	e = remove_element(follower_struct->following, following_struct);

	if (e == NULL)
		return -1;

	return 0;
}

Post *publish(char *login, char *message, Flash_Instance * f)
{
	User *publisher = getUser_from_login(login, f);

	if (strlen(message) > 20 || !publisher) {
		printf("publish fail\n");
		return NULL;
	}

	Post *post = malloc(sizeof(Post));
	add_element(f->posts, post);

	post->readers = init_array_list();
	for (int i = 0; i < publisher->followers->current_index; ++i) {
		add_element(post->readers, get(publisher->followers, i));
	}
	strcpy(post->content, message);
	post->author_id = publisher->id;

	return post;
}

array_list_t *list_sub(char *login, Flash_Instance * f)
{
	User *user_struct = (User *) getUser_from_login(login, f);

	return user_struct == NULL ? NULL : user_struct->following;
}

int logout(char *login, Flash_Instance * f)
{
	User *user_struct = (User *) getUser_from_login(login, f);
	if (user_struct == NULL)
		return -1;
	user_struct->socket = -1;
	return 0;
}
