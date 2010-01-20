/*
 * utils.c: Small utility functions
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"


/*
 * Get the expense amount from the line
 *
 * This function assumes that the expense line in the input would always look
 * like:
 * C:options:comment:amount
 */

int
get_last_field(char *line)
{
	char *ptr = strrchr(line, SEPERATOR);

	if (ptr) {
		return atoi(ptr+1);
	}

	return 0;
}

int
get_option_month(char *line)
{
}

int
get_month_year (int *m, int *y)
{
	struct tm *tm;
	time_t now;

	now = time(NULL);
	tm = localtime(&now);

	if (m) {
		*m = tm->tm_mon;
	}

	if (y) {
		*y = tm->tm_year + 1900;
	}

	return 0;
}


static const char *monthnames[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

const char *
monthname (int n)
{
	if (n < 0 || n >= MONTHS_PER_YEAR) {
		return NULL;
	}

	return monthnames[n];
}
