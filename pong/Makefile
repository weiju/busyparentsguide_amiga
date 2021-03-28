CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)

CFLAGS=-I$(NDK_INC) -c99 -O2 -I../include
LDFLAGS=-lamiga -lauto
EXES=pong

.PHONY : clean check
.SUFFIXES : .o .c .asm

all: $(EXES)

clean:
	rm -f *.o ../ratr0lib/*.o ../ptplayer/*.o $(EXES)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

pong: pong.o game_data.o ../ratr0lib/tilesheet.o ../ptplayer/ptplayer.o
	$(CC) $^ $(LDFLAGS) -o $@
