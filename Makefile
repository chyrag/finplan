# $Id$

PROG=	fin
SRCS=	main.c finance.c

CFLAGS+= -O2 -g

NO_MAN = yes

test:
	$(PROG) -v -t 24 input

.include <bsd.prog.mk>
