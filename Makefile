PROG=	fin
SRCS=	\
	finance.c \
	grammar.y \
	token.l

CFLAGS = -Wall -g

OBJS = finance.o grammar.o token.o
GENFILES = token.c grammar.c y.tab.h
CLEANFILES = gpdata finplan.sh $(PROG) $(OBJS) $(GENFILES)
DISTCLEAN = cscope.*

all:		$(PROG)
$(PROG):	$(OBJS)
	$(CC) -o $@ $>

cscope:
	find . -name "*.[chyl]" > cscope.files
	cscope -buq -F cscope.files

clean:
	rm -f $(CLEANFILES)

distclean:
	rm -f $(CLEANFILES) $(DISTCLEAN)
