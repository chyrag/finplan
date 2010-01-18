/*
 * $Id$
 *
 * finance.c:
 *
 * Little more description of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>

int
process_file(char *filename)
{
	int fd;
	int buf[BUFSIZ];
	int lines = 0;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		warn("unable to open %s for reading", filename);
		return -1;
	}

	memset(buf, 0, BUFSIZ);
	while (read(fd, buf, BUFSIZ) > 0) {
		printf("%s\n", buf);
		lines++;
	}
	printf("Processed %d lines\n", lines);

	close(fd);
	return 0;
}

int
display_finance(int verbose)
{
	return 0;
}
