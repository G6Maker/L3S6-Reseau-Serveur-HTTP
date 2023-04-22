#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "adresse_internet.h"

#define TAILLE  (80*sizeof(char))
int main(/*int argc, char *argv[]*/) {
	//if (argc != 2) {
		//return EXIT_FAILURE;
	//}
	adresse_internet *x;
	x = adresse_internet_new("www.google.fr",80);
	//x = adresse_internet_new(argv[1],80);

	char *nom_dns = malloc(80*sizeof(char));
	char *nom_port = malloc(80*sizeof(char));
	char *ip = malloc(TAILLE);

	adresse_internet_get_ip(x,ip,TAILLE);
	adresse_internet_get_info(x,nom_dns,80*sizeof(char),nom_port,80*sizeof(char));
	struct sockaddr_in *test;
	test = ((struct sockaddr_in *)&x->sock_addr);
  adresse_internet *p = malloc(sizeof(*p));
  sockaddr_to_adresse_internet((struct sockaddr *)test, p);
	printf("%s\n%s\n", x->nom, x->service);
	printf("ip = %s\n", ip);
	printf("%s\n%s\nsin_family : %d\n sin_port %d\n %d\n valeur convertie : %s\n",nom_dns,nom_port,test->sin_family,htons(test->sin_port), test->sin_addr.s_addr, inet_ntoa(test->sin_addr));
  free(p);
  free(nom_dns);
  free(nom_port);
  free(ip);
  adresse_internet_free(x);
}
