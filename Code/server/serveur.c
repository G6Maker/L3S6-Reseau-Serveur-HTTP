#include "socketTCP.h"
#include "adresse_internet.h"
#include "serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>

#define SIZE_QUEUE 5

#define TAILLE 80

#define PAQUET_SIZE 1024

#define SERVER "HOMEMADE"

#define SOURCE "./sources"

#define LOGPATH "./log/log"

#define CTYPE "text/HTML"

#define MAX_HTTP_SIZE 12500

#define PORT 80

#define METHODE_REQUETE "GET"


socketTCP *s = NULL;

enum {
  OK = 200,
  NOT_FOUND = 404,
  NOT_IMPLEMENTED = 501,
  BAD_REQUEST = 400,
  NOT_MODIFIED = 304
};

int main(void) {
  setSignaux();
  socketTCP *se = malloc(sizeof(socketTCP));
  if (se == NULL) {
    perror("malloc");
    return -1;
  }
  if (initSocketTCP(se) == -1) {
    perror("initSocketTCP");
    free(se);
    return -1;
  }
  if (creerSocketEcouteTCP(se, PORT) != 0) {
    perror("creerSocketEcouteTCP");
    free(se);
    return -1;
  }
  s = se;
  pthread_t ph;
  //definition des attributs des Threads
  int errnum;
  pthread_attr_t attr;
  if ((errnum = pthread_attr_init(&attr)) != 0) {
    fprintf(stderr, "pthread_attr_init: %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
  if ((errnum
        = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
    fprintf(stderr, "pthread_attr_setdetachstate: %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
  do {
	socketTCP ss;
    initSocketTCP(&ss);
    if (acceptSocketTCP(se, &ss) == -1) {
      perror("acceptSocketTCP");
    } //bloquant
    socketTCP *ssf = malloc(sizeof(socketTCP));
    if (ssf == NULL) {
      perror("malloc");
      adresse_internet_free(ss.distant);
      continue;
    }
    memcpy(ssf, &ss, sizeof(socketTCP));
    if ((errnum = pthread_create(&ph, &attr, hand, ssf)) != 0) {
      fprintf(stderr, "pthread_create: %s\n", strerror(errnum));
    }
  } while (1);
  closeSocketTCP(se);
  free(se);
  exit(EXIT_SUCCESS);
  return 0;
}

char *format_header(int code, const char *ctype) {
  char *result = malloc(500);
  if (result == NULL) {
    perror("malloc");
    return NULL;
  }
  char ectype[100];
  strcpy(ectype, ctype);
  //avoir l'heure/date
  time_t timestamp = time(NULL);
  struct tm *pTime = gmtime(&timestamp);
  char buffer[TAILLE];
  strftime(buffer, TAILLE, "%a, %d %b %Y %X %Z", pTime);
  char strcode[50];
  char content[300];
  content[0] = '\0';
  switch (code) {
    case OK:
      strcpy(strcode, "OK");
      break;
    case NOT_FOUND:
      strcpy(strcode, "Not Found");
      strcpy(content, "<!DOCTYPE html><body><h1>ERROR 404</h1></body>");
      strcpy(ectype, CTYPE);
      break;
    case NOT_IMPLEMENTED:
      strcpy(strcode, "Not Implemented");
      break;
    case BAD_REQUEST:
      strcpy(strcode, "Bad Request");
      break;
    default:
      strcpy(strcode, "");
      break;
  }
  sprintf(result,
      "HTTP/1.0 %d %s\r\nDate: %s\r\nServer: %s\r\nContent-Type: %s\r\n\r\n%s",
      code, strcode, buffer, SERVER, ectype,
      content);
  return result;
}

void *hand(void *ss) {
  int retval = timeout(((socketTCP *)ss)->socket);
  switch (retval) {
    case -1:
      perror("select()");
      return 0;
    case 0:   //si pas pret a lire alors closeSocketTCP(ss);
      printf("Timeout.\n");
      closeSocketTCP(ss);
      return 0;
  }
  char *buffer = malloc(MAX_HTTP_SIZE);
  if (buffer == NULL) {
    perror("malloc");
    closeSocketTCP(ss);
  }
  readSocketTCP(ss, buffer, MAX_HTTP_SIZE);
  //analyser ici buffer
  char *uri = extract_uri(buffer);
  char *ext = get_extension(uri);
  char *ctype = get_MIME_type(ext);
  char *date = extract_date(buffer);
  int error = check_400(buffer);
  if (error == 0) {
    if (is_modify_since(date, uri) == 1) {
      send_page(ss, uri, ctype);
    } else {
      char *buf = format_header(NOT_MODIFIED, "NONE");
      writeSocketTCP(ss, buf, strlen(buf));
      free(buf);
    }
  } else {
    char *buf = format_header(error, "NONE");
    writeSocketTCP(ss, buf, strlen(buf));
    free(buf);
  }
  free(buffer);
  free(uri);
  free(ext);
  free(ctype);
  free(date);
  closeSocketTCP(ss);
  return 0;
}

char *get_extension(const char *uri) {
  char *ptr = (char *) uri;
  char *res;
  size_t i = 0;
  ptr = ptr + strlen(uri);
  while (*ptr != '.' && ptr != uri) {
    if (*ptr == '/') {
      res = malloc(1);
      *res = '\0';
      return res;
    }
    ptr -= 1;
    i++;
  }
  res = malloc(i + 1);
  strncpy(res, ptr + 1, i);
  *(res + i) = '\0';
  return res;
}

char *get_MIME_type(const char *ext) {
  char *res;
  if (!strcmp(ext, "plain") || !strcmp(ext, "html")
      || !strcmp(ext, "css") || !strcmp(ext, "js")) {
    res = malloc(strlen("text/") + strlen(ext) + 1);
    strcpy(res, "text/");
    strcat(res, ext);
    return res;
  } else if (!strcmp(ext, "gif") || !strcmp(ext, "png")
      || !strcmp(ext, "jpeg") || !strcmp(ext, "jpg") || !strcmp(ext, "bmp")
      || !strcmp(ext, "webp") || !strcmp(ext, "ico")) {
    res = malloc(strlen("image/") + strlen(ext) + 1);
    strcpy(res, "image/");
    strcat(res, ext);
    return res;
  } else if (!strcmp(ext, "midi") || !strcmp(ext, "mpeg")
      || !strcmp(ext, "webm") || !strcmp(ext, "ogg")
      || !strcmp(ext, "wav")) {
    res = malloc(strlen("audio/") + strlen(ext) + 1);
    strcpy(res, "audio/");
    strcat(res, ext);
    return res;
  } else if (!strcmp(ext, "webm") || !strcmp(ext, "ogg")) {
    res = malloc(strlen("video/") + strlen(ext) + 1);
    strcpy(res, "video/");
    strcat(res, ext);
    return res;
  } else if (!strcmp(ext, "php")) {
    res = malloc(strlen("text/html") + 1);
    strcpy(res, "text/html");
    return res;
  } else {
    res = malloc(strlen("text/plain") + 1);
    strcpy(res, "text/plain");
    return res;
  }
}

int send_page(socketTCP *ss, const char *path, const char *ctype) {
  if (is_path_valide(path) == 1) {
    //ouvrir le fichier dans path
    int f = open(path, O_RDONLY);
    //si trouvé alors:
    if (f != -1) {
      ssize_t sc;
      //appeler format_header(int code) et send()
      char buffer[PAQUET_SIZE];
      if ((sc = read(f, buffer, PAQUET_SIZE - 1)) > 0) {
        char *buf = format_header(OK, ctype);
        writeSocketTCP(ss, buf, strlen(buf));
        free(buf);
        writeSocketTCP(ss, buffer, (size_t) sc);
        //boucle tant qu'on peux read alors
        while ((sc = read(f, buffer, PAQUET_SIZE - 1)) > 0) {
          //envoyer ce qu'on a lu via ss
          writeSocketTCP(ss, buffer, (size_t) sc);
        }
        if (sc == -1) {
          perror("read");
          return -1;
        }
      } else {
        char *buf = format_header(NOT_FOUND, ctype);
        writeSocketTCP(ss, buf, strlen(buf));
        free(buf);
      }
      close(f);
    } else { //sinon envoyer header avec 404
      char *buf = format_header(NOT_FOUND, ctype);
      writeSocketTCP(ss, buf, strlen(buf));
      free(buf);
    }
  } else {
    char *buf = format_header(BAD_REQUEST, ctype);
    writeSocketTCP(ss, buf, strlen(buf));
    free(buf);
  }
  return 0;
}

char *extract_uri(const char *r) {
  char *ptr = (char *) r;
  char *uri = NULL;
  while (*ptr != ' ' && *ptr != '\0') {
    ptr += 1;
  }
  ptr += 1;
  size_t i = 0;
  if (*ptr == '/') {
    char *tmp = ptr;
    while (*ptr != ' ') {
      i++;
      ptr += 1;
    }
    uri = malloc(i + 1 + strlen(SOURCE));
    if (uri == NULL) {
      perror("malloc");
      return NULL;
    }
    strcpy(uri, SOURCE);
    strncpy((uri + strlen(SOURCE)), tmp, i);
    *(uri + strlen(SOURCE) + i) = '\0';
  } else {
    uri = malloc(1 + strlen(SOURCE));
    strcpy(uri, SOURCE);
    *(uri + strlen(SOURCE)) = '\0';
  }
  return uri;
}

char *extract_date(const char *r) {
  char *ptr = strstr(r, "If-Modified-Since : ");
  if (ptr == NULL) {
    return NULL;
  }
  ptr += strlen("If-Modified-Since : ");
  char *tmp = ptr;
  size_t i = 0;
  while (*tmp != '\n' && *tmp != '\0') {
    tmp += 1;
    i++;
  }
  char *result = malloc(i + 1);
  strncpy(result, ptr, i);
  *(result + i) = '\0';
  return result;
}

int is_modify_since(const char *date, const char *uri) {
  if (date == NULL) {
    return 1;
  }
  struct stat filestat;
  stat(uri, &filestat);
  printf("%s", ctime(&filestat.st_mtime));
  struct tm tm;
  strptime(date, "%a, %d %b %Y %X %Z", &tm);
  time_t rdate = mktime(&tm);
  return (difftime(rdate, filestat.st_mtime) < 0) ? 1 : 0;
}

int check_400(const char *r) {
  char *ptr = (char *) r;
  while (*ptr != ' ') {
    if (*ptr == '\0') {
      return BAD_REQUEST;
    }
    ptr += 1;
  }
  ptr += 1;
  if (*ptr != '/') {
    return BAD_REQUEST;
  }
  while (*ptr != ' ') {
    if (*ptr == '\0') {
      return BAD_REQUEST;
    }
    ptr += 1;
  }
  ptr += 1;
  size_t i = 0;
  while (*ptr != '/') {
    if (*ptr == '\0') {
      return BAD_REQUEST;
    }
    ptr += 1;
    i++;
  }
  char temp[i + 1];
  strncpy(temp, ptr - i, i);
  if (check_get(r) != 0) {
    return NOT_IMPLEMENTED;
  }
  return 0;
}

int check_get(const char *r) {
  char *ptr = (char *) r;
  size_t i = 0;
  while (*ptr != ' ' && *ptr != '\0') {
    i++;
    ptr += 1;
  }
  char temp[i + 1];
  strncpy(temp, r, i);
  temp[i] = '\0';
  if (strcmp(temp, METHODE_REQUETE) == 0) {
    return 0;
  } else {
    return NOT_IMPLEMENTED;
  }
}

int is_path_valide(const char *p) {
  char *ptr = (char *) p;
  int nb = 0;
  int nf = 0;
  while (*ptr != '\0') {
    if (*ptr == '/' && *ptr != '\0' && *(ptr + 1) != '\0') {
      ptr += 1;
      if (*ptr == '.' && *(ptr + 1) != '\0' && *(ptr + 1) == '.') {
        ++nb;
        ptr += 2;
      } else {
        ++nf;
      }
    } else {
      ptr += 1;
    }
  }
  return nb <= nf;
}

char *extract_ctype(const char *r) {
  char *ptr = strstr(r, "Accept: ");
  char *ctype;
  if (ptr == NULL) {
    ctype = malloc(strlen(CTYPE) + 1);
    if (ctype == NULL) {
      return NULL;
    }
    strcpy(ctype, CTYPE);
  } else {
    ptr += strlen("Accept: ");
    char *tmp = ptr;
    size_t i = 0;
    while (*tmp != ',' && isspace(*tmp) == 0 && *tmp != '\0') {
      i++;
      tmp += 1;
    }
    ctype = malloc(i + 1);
    if (ctype == NULL) {
      return NULL;
    }
    strncpy(ctype, ptr, i);
    *(ctype + i) = '\0';
  }
  return ctype;
}

int make_log(const char *b, socketTCP *ss){
  int fd = open(LOGPATH, O_CREAT | O_APPEND | O_SYNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd == -1){
    perror("open");
    return -1;
  }
  time_t timestamp = time(NULL);
  struct tm *pTime = gmtime(&timestamp);
  char buffer[TAILLE];
  strftime(buffer, TAILLE, "%a, %d %b %Y %X %Z", pTime);
  if (write(fd, buffer, strlen(buffer)) == -1){
    perror("write");
    return -1;
  }
  char buf[MAX_HTTP_SIZE*3];
  sprintf(buf, "\ndistant:\n\tnom: %s\n\n\tservice: %s\nreceive:\n", ss->distant->nom,
    ss->distant->service);
  if (write(fd, buf, strlen(buf)) == -1){
    perror("write");
    return -1;
  }
  if (write(fd, b, strlen (b)) == -1){
    perror("write");
    return -1;
  }
  close(fd);
  return -1;
}

int timeout(int fd) {
  fd_set rfds;
  struct timeval tv;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  int fdmax = fd + 1;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  int retval = select(fdmax, &rfds, NULL, NULL, &tv);
  return retval;
}

void setSignaux() {
  //gestion des signaux
  struct sigaction action;
  action.sa_handler = gestionnaire;
  action.sa_flags = SA_SIGINFO;
  struct sigaction noting;
  noting.sa_handler = NULL;
  noting.sa_flags = SA_SIGINFO;
  if (sigfillset(&action.sa_mask) == -1) {
    perror("sigfillset");
    exit(EXIT_FAILURE);
  }
  if (sigfillset(&noting.sa_mask) == -1) {
    perror("sigfillset");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGPIPE, &noting, NULL) == -1){
    perror("sigaction1");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGINT, &action, NULL) == -1) {
    perror("sigaction1");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGTERM, &action, NULL) == -1) {
    perror("sigaction3");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGQUIT, &action, NULL) == -1) {
    perror("sigaction4");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGTSTP, &action, NULL) == -1) {
    perror("sigaction5");
    exit(EXIT_FAILURE);
  }
}

//fonction gestionnaire pour les signaux
void gestionnaire(int signum) {
  if (signum < 0) {
    printf("\nProblème sur la gestion des signaux.\n");
  }
  closeSocketTCP(s);
  printf("\nMerci\n");
  exit(EXIT_SUCCESS);
}
