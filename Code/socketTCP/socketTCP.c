#include "socketTCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define SIZE_QUEUE 5

int initSocketTCP(socketTCP *psocket) {
  if (psocket == NULL){
    return -1;
  }
  psocket->socket = -1;
  psocket->local = NULL;
  psocket->distant = NULL;
  psocket->isConnected = 0;
  psocket->isListening = 0;
  psocket->isBind = 0;
  return 0;
}

int connectSocketTCP(socketTCP *osocket, const char *adresse, uint16_t port){
  if (osocket == NULL || adresse == NULL){
    return -1;
  }
  osocket->distant = adresse_internet_new(adresse, port);
  osocket->socket = socket(((struct sockaddr *)(&(osocket->distant->sock_addr)))->sa_family, SOCK_STREAM,0);
  if (osocket->socket == -1){
    adresse_internet_free(osocket->distant);
    return -1;
  }
  if(connect(osocket->socket, (struct sockaddr *)((&osocket->distant->sock_addr)), sizeof(struct sockaddr)) == -1){
    adresse_internet_free(osocket->distant);
  }
  osocket->isConnected = 1;
  return 0;
}

int creerSocketEcouteTCP(socketTCP *isocket, uint16_t port){
  isocket->local = adresse_internet_any(port);
  if(isocket->local == NULL){
    return -1;
  }
  isocket->socket = socket(((struct sockaddr *)((&isocket->local->sock_addr)))->sa_family, SOCK_STREAM, 0);
  if (isocket->socket == -1){
    adresse_internet_free(isocket->local);
    return -1;
  }
  if (bind(isocket->socket, (struct sockaddr *)((&isocket->local->sock_addr)), sizeof(struct sockaddr))) {
    adresse_internet_free(isocket->local);
    return -1;
  }
  isocket->isBind = 1;
  if(listen(isocket->socket, SIZE_QUEUE) == -1){
    adresse_internet_free(isocket->local);
    return -1;
  }
  isocket->isListening = 1;
  return 0;
}

int acceptSocketTCP(const socketTCP *secoute, socketTCP *sservice){
  if (secoute == NULL || sservice == NULL){
    return -1;
  }
  initSocketTCP(sservice);
  struct sockaddr_storage so;
  socklen_t l = sizeof(so);
  sservice->socket = accept(secoute->socket, (struct sockaddr *)&so, &l);
  if (sservice->socket == -1){
    return -1;
  }
  adresse_internet *distant = malloc(sizeof(*distant));
  if(distant == NULL){
    perror("malloc");
    return -1;
  }
  if(sockaddr_to_adresse_internet((struct sockaddr *)&so, distant) == -1){
    return -1;
  }
  sservice->distant = distant;
  sservice->isConnected = 1;
  return 0;
}

int writeSocketTCP(const socketTCP *osocket, const void *buffer, size_t length){
  return (int)send(osocket->socket, buffer, length, 0);
}

int readSocketTCP(const socketTCP *nsocket, void *buffer, size_t length){
  return (int)recv(nsocket->socket, buffer, length, 0);
}

int closeSocketTCP(socketTCP *socket){
  close(socket->socket);
  if(socket->local != NULL){
    adresse_internet_free(socket->local);
  }
  if(socket->distant != NULL){
    adresse_internet_free(socket->distant);
  }
  free(socket);
  return 0;
}
