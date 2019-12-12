#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "flash.h"

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
	while (i < f->users->current_index
	       && strcmp(((User *) get(f->users, i))->login, login))
		++i;
	if (i == f->users->current_index)
		return NULL;

	return (User *) get(f->users, i);
}

int login(int id, char *login, int socket, Flash_Instance * f)
{
	int i = 0;
	while (i < f->users->current_index
	       && ((User *) get(f->users, i))->id != id)
		++i;

	if (i == f->users->current_index) {	//ajout nouvel utilisateur
		if (login == NULL || strlen(login) != 6)
			return -1;

		User *newUser = malloc(sizeof(User));

		strcpy(newUser->login, login);
		newUser->id = id;
		newUser->socket = socket;
		newUser->posts = init_array_list();
		newUser->following = init_array_list();
		newUser->followers = init_array_list();

		add_element(f->users, newUser);
	} else {		//connecter utilisateur
		((User *) get(f->users, i))->socket = socket;
	}

	return 0;
}

int subscribe(int follower, char *following, Flash_Instance * f)
{
	User *follower_struct = (User *) getUser_from_id(follower, f);
	User *following_struct = (User *) getUser_from_login(following, f);

	if (follower_struct == NULL || following_struct == NULL)
		return -1;

	add_element(follower_struct->following, (void *)following_struct);
	add_element(following_struct->followers, (void *)follower_struct);

	return 0;
}

int unsubscribe(int follower, char *following, Flash_Instance * f)
{
	User *follower_struct = (User *) getUser_from_id(follower, f);
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

int publish(int id, char *message, Flash_Instance * f)
{
	User *publisher = getUser_from_id(id, f);

	if (strlen(message) > 20 || !publisher)
		return -1;

	Post *post = malloc(sizeof(Post));
	add_element(publisher->posts, (void *)post);

	post->readers = init_array_list();
	ensure_capacity(post->readers, publisher->followers->current_index);
	memcpy(post->readers->data, publisher->followers->data,
	       publisher->followers->current_index);
	post->readers->current_index = publisher->followers->current_index;
	strcpy(post->content, message);
	post->author_id = id;

	return 0;
}

array_list_t *list_sub(int id, Flash_Instance * f)
{
	User *user_struct = (User *) getUser_from_id(id, f);

	return user_struct == NULL ? NULL : user_struct->following;
}

int logout(char *user, Flash_Instance * f)
{
	User *user_struct = (User *) getUser_from_login(user, f);
	if (user_struct == NULL)
		return -1;
	user_struct->socket = -1;
	return 0;
}
