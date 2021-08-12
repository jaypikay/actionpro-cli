# includes and libs
INCS = -I. -I/usr/include
LIBS = -L/usr/lib -lusb-1.0

# flags
CFLAGS = -std=c99 -pedantic -Wall -Os ${INCS}
LDFLAGS = -s ${LIBS}

# compiler and linker
CC = cc
