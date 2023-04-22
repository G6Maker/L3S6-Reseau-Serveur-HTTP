#include "adresse_internet.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

adresse_internet *adresse_internet_new_aux(int flag, const char *nom,
    uint16_t port) {
  // on va utiliser getaddrinfo pour voir si l'adresse existe deja
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = flag | AI_NUMERICSERV; // empeche la résolution de service
  char string_port[7];
  sprintf(string_port, "%d", (int) port);
  struct addrinfo *result;
  int r = getaddrinfo(nom, string_port, &hints, &result);
  if (r != 0) {
    perror("getaddrinfo");
    // ou fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    return NULL;
  }
  adresse_internet *adresse = malloc(sizeof(*adresse));
  /*strcpy(adresse->nom,nom);
   * strcpy(adresse->service,string_port);*/
  memcpy(&(adresse->sock_addr), result->ai_addr, (size_t) result->ai_addrlen);
  freeaddrinfo(result);
  getnameinfo((const struct sockaddr *) (&adresse->sock_addr),
      sizeof(adresse->sock_addr), adresse->nom, _DNS_NAME_MAX_SIZE,
      adresse->service, _DNS_NAME_MAX_SIZE, 0);
  return adresse;
}

adresse_internet *adresse_internet_new(const char *nom, uint16_t port) {
  return adresse_internet_new_aux(0, nom, port);
}

adresse_internet *adresse_internet_any(uint16_t port) {
  return adresse_internet_new_aux(AI_PASSIVE, NULL, port);
}

adresse_internet *adresse_internet_loopback(uint16_t port) {
  return adresse_internet_new_aux(0, "127.0.0.1", port);
}

void adresse_internet_free(adresse_internet *adresse) {
  free(adresse);
}

int adresse_internet_get_info(adresse_internet *adresse,
    char *nom_dns, int taille_dns, char *nom_port, int taille_port) {
  if (nom_dns == NULL || nom_port == NULL) {
    return -1;
  }
  strncpy(nom_dns, adresse->nom, (size_t) taille_dns);
  strncpy(nom_port, adresse->service, (size_t) taille_port);
  return 0;
}

int adresse_internet_get_ip(const adresse_internet *adresse, char *ip,
    int taille_ip) {
  struct sockaddr_in *sock_addr = (struct sockaddr_in *) &adresse->sock_addr;
  const char *ipc = inet_ntoa(sock_addr->sin_addr);
  strncpy(ip, ipc, (size_t) taille_ip);
  return 0;
}

uint16_t adresse_internet_get_port(const adresse_internet *adresse) {
  struct sockaddr_in *sock_addr = (struct sockaddr_in *) &adresse->sock_addr;
  return ntohs(sock_addr->sin_port);
}

int adresse_internet_get_domain(const adresse_internet *adresse) {
  struct sockaddr_in *sock_addr = (struct sockaddr_in *) &adresse->sock_addr;
  return sock_addr->sin_family;
}

int adresse_internet_to_sockaddr(const adresse_internet *adresse,
    struct sockaddr *addr) {
  memcpy(addr, (struct sockaddr *) &adresse->sock_addr,
      sizeof(adresse->sock_addr));
  return 0;
}

int sockaddr_to_adresse_internet(const struct sockaddr *addr,
    adresse_internet *adresse) {
  /*printf("sockaddr_to_adresse_internet : %d\n",sock_addr->sin_addr.s_addr);
   * printf("sockaddr_to_adresse_internet ntohs : %s\n",
   * inet_ntoa(sock_addr->sin_addr));
   * printf("sockadd_to port : %d\n", ntohs(sock_addr->sin_port));*/
  //adresse = malloc(sizeof(*adresse));
  //if (adresse == NULL) {
    //perror("adresse_internet_new");
    //return -1;
  //}
  memcpy(&(adresse->sock_addr), addr, sizeof(struct sockaddr));
  socklen_t len = sizeof(struct sockaddr);
  if (getnameinfo(addr, len, adresse->nom, _DNS_NAME_MAX_SIZE, adresse->service,
                _SERVICE_NAME_MAX_SIZE, NI_NUMERICHOST | NI_NUMERICSERV) == 0)
    printf("host=%s, serv=%s\n", adresse->nom, adresse->service);
  return 0;
}
