# ACTIONPRO Makefile

include config.mk

PRG = actionpro
SRC = main.c xusb.c usbms.c
OBJ = ${SRC:.c=.o}
BIN = ${OBJ:.o=}

all: options ${PRG}

options:
	@echo ${PRG} compile options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

${PRG}: ${OBJ}
	${CC} -o ${PRG} ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${PRG} ${OBJ}

purge: clean
	@echo removing old config.h
	@rm -f config.h

.PHONY: all options clean purge
