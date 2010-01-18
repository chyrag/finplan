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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <err.h>
#include <sys/types.h>
#include "finance.h"

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
process_file(struct monthexp *exp, int months, char *filename)
{
	FILE *fp;
	char buf[BUFSIZ];
	int i, lines = 0;
	int amount, savings, savedup = 0;

	fp = fopen(filename, "r");
	if (!fp) {
		return -1;
	}

	memset(buf, 0, BUFSIZ);
	while (fgets(buf, BUFSIZ, fp)) {
		switch (buf[0]) {
			case 'M':
			case 'B':
				/* monthly expense */
				/* handle monthly budgetary expense likewise
				   for now */
				amount = get_last_field(buf);
				if (amount < 0) {
					/* FIXME: error */
				}
				for (i = 0; i < months; i++) {
					exp[i].expenses += amount;
				}
				break;
			case 'A':
				/* annual expense */
				/* read further options */
				break;
			case 'U':
				/* upcoming planned expense */
				/* read further options */
				break;
			case 'S':
				/* intended savings */
				savings = get_last_field(buf);
				if (savedings < 0) {
					/* FIXME: error */
				}
				break;
			case 'I':
				/* initial capital */
				savedup = get_last_field(buf);
				if (savedup < 0) {
					/* FIXME: error */
				}
				break;
			case 'G':
				/* goals */
				/* read further options */
				break;
			default:
				warn("Unknown option %c", buf[0]);
				break;
		}
		lines++;
	}

	fclose(fp);
	return 0;
}

int
display_finance(struct monthexp *exp, int months, int verbose)
{
	int m;

	for (m = 0; m < months; m++) {
		printf("%s %d\n", monthnames[exp[m].month], exp[m].year);
		printf("Expenses: %d\n", exp[m].expenses);
		printf("Savings: %d\n", exp[m].savings);
		printf("\n");
	}
}

struct monthexp *
init_expenses(int months)
{
	struct monthexp *exp;
	struct tm *tm;
	int m, y;
	time_t now;

	exp = malloc(months * sizeof(struct monthexp));
	if (exp) {
		now = time(NULL);
		tm = localtime(&now);

		for (m = 0, y = 0; m < months; m++) {
			exp[m].month = (tm->tm_mon + m + 1) % MONTHS_PER_YEAR;
			if (exp[m].month == 0) {
				y++;
			}
			exp[m].year = tm->tm_year + 1900 + y;
			exp[m].expenses = 0;
			exp[m].savings = 0;
		}
	}

	return exp;
}

void
cleanup_expenses(struct monthexp *expenses)
{
	free(expenses);
}
