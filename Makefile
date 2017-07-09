SRC1=	xlobj.c xllist.c xlcont.c xlbfun.c
SRC2=	xldmem.c xleval.c xlfio.c xlftab.c xlglob.c xlio.c xlisp.c xljump.c \
	xlmath.c xlprin.c xlread.c xlinit.c
SRC3=	xlsetf.c xlstr.c xlsubr.c xlsym.c xlsys.c xlbind.c xldbug.c
SRCS=	$(SRC1) $(SRC2) $(SRC3) xlisp.h

OBJS=	xlbfun.o xlbind.o xlcont.o xldbug.o xldmem.o xleval.o xlfio.o \
	xlftab.o xlglob.o xlinit.o xlio.o xlisp.o xljump.o xllist.o xlmath.o \
	xlobj.o xlprin.o xlread.o xlsetf.o xlstr.o xlsubr.o xlsym.o xlsys.o 
MISC=	Makefile fact.lsp init.lsp object.lsp prolog.lsp trace.lsp \
	xlstub.c.NOTUSED 

CFLAGS=	-O

xlisp: $(OBJS)
	cc -o xlisp $(CFLAGS) $(OBJS)

$(OBJS): xlisp.h

rcs: $(SRCS)
	rcs -l $?
	touch rcs

lint:
	lint -ach $(SRCS)

new: clean
	rm -f xlisp
	make xlisp

clean:
	rm -f *.o

shar: $(SRCS) $(MISC)
	shar -c -v xlisp.doc > xlisp1.shar
	shar -c -v $(SRC1) > xlisp2.shar
	shar -c -v $(SRC2) > xlisp3.shar
	shar -c -v $(SRC3) $(MISC) > xlisp4.shar
