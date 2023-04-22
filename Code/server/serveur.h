#ifndef SERVEUR_H_
#define SERVEUR_H_



void setSignaux();
void gestionnaire(int signum);
int timeout(int fd);

/* "L'executable" du thread"*/
void *hand(void *se);

int make_log(const char *b, socketTCP *ss);
char *format_header(int code, const char *ctype);
int send_page(socketTCP *ss, const char *path, const char *ctype);

/*	Verifie si la requetes est valide et revoie 0
	si la requetes est incorrect renvoie un flags */
int is_good_request(char *r);

/*	Extrait l'URI de la requete, alloue une zone memoire de la taille de l'URI + 1
	et renvoie l'addresse contenant l'URI + '\0' */
char *extract_uri(const char *r);

/*	Extrait la date de la requete si elle est présente alloue une zone mémoire de
	la taille de la date + 1.
	Renvoie la date en chaine de caractere suivie de '\0', NULL sinon. */
char *extract_date(const char *r);


int is_modify_since(const char *date, const char *uri);

/* 	Prend une requète et revoie BAD_REQUEST ou NOT_IMPLEMENTED si l'un des cas
 	est trouvé, si la requete est acceptée renvoie 0. */
int check_400(const char *r);

/* Prend l'uri et recupère sont extension (.x). */
char *get_extension(const char *uri);

/* Prend l'extention d'une URI et renvoie le type MIME de l'extension. */
char *get_MIME_type(const char *ext);

/*	Prend une requetes et renvoie 0 si la methode est GET sinon une valeure != 0. */
int check_get(const char *r);

/*	Vérifie si l'URI est valide, avec la restriction de ne pas sortir du dossier
 	source. renvoie 0 si la requetes est valide, 1 sinon.*/
int is_path_valide(const char *p);

/* Extrait le content type(ctype) de la requetes reçu. */
char *extract_ctype(const char *r);

#endif
