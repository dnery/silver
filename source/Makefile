obj = main.o llist.o ilist.o sq0x88.o zobkey.o 0x88movegen.o algebraic.o

gch = stdafx.h.gch

all : $(obj)
	cc -o chessengine $(obj) -lm

main.o        : stdafx.h.gch sq0x88.h llist.h ilist.h 0x88movegen.h
sq0x88.o      : stdafx.h.gch sq0x88.h llist.h ilist.h 0x88movegen.h zobkey.h
0x88movegen.o : stdafx.h.gch sq0x88.h llist.h ilist.h 0x88movegen.h
algebraic.o   : stdafx.h.gch sq0x88.h llist.h ilist.h algebraic.h
zobkey.o      : stdafx.h.gch zobkey.h
llist.o       : llist.h
ilist.o       : ilist.h

stdafx.h.gch : stdafx.h
	cc	-c stdafx.h -g

.PHONY : clean
clean:
	-rm -f chessengine $(obj) $(gch)

run:
	./chessengine
