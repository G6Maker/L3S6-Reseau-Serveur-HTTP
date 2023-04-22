adresse_internet_dir = ../adresse_internet/
socketTCP_dir = ../socketTCP/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -DxNATIMPHIGH \
  -D_POSIX_C_SOURCE=200112L \
  -D_XOPEN_SOURCE=700 \
  -DxDINPUT -DxDOUTPUT \
  -pthread \
  -I$(adresse_internet_dir) \
  -I$(socketTCP_dir) \
  -g \
  -DRECTERM
LDFLAGS = -pthread -lrt
vpath %.c $(adresse_internet_dir) $(socketTCP_dir)
vpath %.h $(adresse_internet_dir) $(socketTCP_dir)
objects = adresse_internet.o socketTCP.o serveur.o
executable = serveur

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

tar:
	$(MAKE) clean
	tar -zcf "$(CURDIR)_$(shell date +'%d.%m.%y-%Hh%M').tar.gz" *.c makefile

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

adresse_internet.o: adresse_internet.c adresse_internet.h adresse_internet_type.h
socketTCP.o: socketTCP.c socketTCP.h adresse_internet.h
serveur.o: serveur.c serveur.h socketTCP.h adresse_internet.h
