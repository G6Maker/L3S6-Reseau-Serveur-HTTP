#ifndef ADRESSEINTERNETTYPE_H_
#define ADRESSEINTERNETTYPE_H_

#define _DNS_NAME_MAX_SIZE 256
#define _SERVICE_NAME_MAX_SIZE 20

struct _adresse_internet_struct{
  struct sockaddr_storage sock_addr;
  char nom[_DNS_NAME_MAX_SIZE];
  char service[_SERVICE_NAME_MAX_SIZE];
};

#endif // ADRESSEINTERNETTYPE_H_
