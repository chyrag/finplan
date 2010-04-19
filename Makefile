PROG=	fin
SRCS=	\
	finance.c \
	grammar.y \
	token.l

CFLAGS = -Wall -g

NO_MAN = yes

test:
	$(PROG) -v -i 75000 < input

cscope:
	find . -name "*.[chyl]" > cscope.files
	cscope -buq -F cscope.files

.include <bsd.prog.mk>
