PROG=	fin
SRCS=	\
	finance.c \
	grammar.y \
	token.l

NO_MAN = yes

test:
	$(PROG) -v -t 24 < input

.include <bsd.prog.mk>
