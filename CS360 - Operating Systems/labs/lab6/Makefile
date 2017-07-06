# Makefile: 
GCC = gcc 
CFLAGS = -DPOSIX_4D9 
LFLAGS = -lm -lpthread 
RM = rm -f
# Explanation of macros:
#     $< is any dependent file which is out of file1
#     $* is the target's basename (without extension)
#     $@ is the target's fullname
#
# add suffix .c since it is not a default with make utility 
.SUFFIXES:	.c .o
#
# implicit rule for compilation only:
.c.o:
	${GCC} -c ${CFLAGS} $<

OFILES=		dining.o 
OFILES2=	mutex_test.o 
OFILES3=	dotprod.o 

# dependencies 
#
all:	dining mutex_test dotprod 

dining:           $(OFILES)
		${GCC} ${CFLAGS} ${LFLAGS} $(OFILES) -o $@

mutex_test:           $(OFILES2)
		${GCC} ${CFLAGS} ${LFLAGS} $(OFILES2) -o $@

dotprod:           $(OFILES3)
		${GCC} ${CFLAGS} ${LFLAGS} $(OFILES3) -o $@

dining.o:		dining.c 
mutex_test.o:		mutex_test.c 
dotprod.o:		dotprod.c 

#
clean:
	${RM} *.o
	${RM} core
	${RM}  dotprod mutex_test dining 
