#ifndef SOCKET_TCP_H_
#define SOCKET_TCP_H_

#include "adresse_internet.h"
#include <stdint.h>
#include <netinet/ip.h> 
//#define _XOPEN_SOURCE 700

/*	Définition du type SocketTCP permettant de manipuler une socket BSD
	en mode connecté. */
typedef struct {
  int socket;
  adresse_internet *local;
  adresse_internet *distant;
  int isConnected;
  int isListening;
  int isBind;
} _socketTCP_struct;

typedef _socketTCP_struct socketTCP;

/*	Initialise la socket psocket (préalablement alouée)
	Valeur de retour 0 en cas de succèes, -1 sinon. */
extern int initSocketTCP(socketTCP *psocket);

/*	Se connecte sur une machine distante dont l'adresse et le port sont données en paramètre.
	Met a jour la structure osocket de type SocketTCP passée en paramètre.
	Retourne 0 si la connexion est établie, -1 sinon. */
extern int connectSocketTCP(socketTCP *osocket, const char *adresse, uint16_t port);

/*	Créer une socket d'ecoute et l'attache à l'adresse et au port donné en paramètre.
	Créer une file de connexions en entrée de taille SIZE_QUEUE sur SocketTCP.
	Retourne -1 si la socket n'est pas créer ou si l'attachement n'a pu avoir lieux,
	0 en cas de succès. */
extern int creerSocketEcouteTCP(socketTCP *isocket, uint16_t port);

/*	Attend une connexion sur la socket d'écoute secoute et met a jour la socket de
	service sservice lorsque la connexion est établie. Cette fonction est bloquante
	jusqu'à ce qu'une connexion soit établie.
	Retourne -1 en cas d'erreur, 0 sinon. */
extern int acceptSocketTCP(const socketTCP *secoute, socketTCP *sservice);


/*	Ecrit sur osocket un bloc d'octets buffer de taille length et retourne la taille
	des données écrites. */
extern int writeSocketTCP(const socketTCP *osocket, const void *buffer, size_t length);


/*	Lit sur nsocket un bloc d'octets buffer de taille au plus length et retourne la taille
	des données réellement lue. */
extern int readSocketTCP(const socketTCP *nsocket, void *buffer, size_t length);


/* 	Ferme la connexion dans les deux sens et libère l'espace éventuellement alloué par la
	SocketTCP. */
extern int closeSocketTCP(socketTCP *socket);


#endif // ADRESSE_INTERNET_H_
