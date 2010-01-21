/*
 * finance.c: main guts of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include "utils.h"
#include "finance.h"

int
process_file(SLIST_ENTRY(expense) exp, int months, char *filename)
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
			case '#':
				/* skip comment */
				break;
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
				if (savings < 0) {
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
				/*
				 * skip lines which are not handled as yet
				 */
				break;
		}
		lines++;
	}

	fclose(fp);
	return 0;
}

int
display_finance(struct monthlyexp *exp, int months, int verbose)
{
	int m;

	for (m = 0; m < months; m++) {
		printf("%s %d\n", monthname(exp[m].month), exp[m].year);
		printf("Expenses: %d\n", exp[m].expenses);
		printf("Savings: %d\n", exp[m].savings);
		printf("\n");
	}
}

struct monthlyexp *
init_expenselist(int months)
{
	struct monthlyexp *exp;
	int cur_mon, cur_year, m, y;

	if (get_month_year(&cur_mon, &cur_year) < 0) {
		return NULL;
	}

	exp = malloc(months * sizeof(struct monthlyexp));
	if (exp) {
		for (m = 0, y = 0; m < months; m++) {
			exp[m].month = (cur_mon + m + 1) % MONTHS_PER_YEAR;
			if (exp[m].month == 0) {
				y++;
			}
			exp[m].year = cur_year + y;
			exp[m].expenses = 0;
			exp[m].savings = 0;
		}
	}

	return exp;
}

void
cleanup_expenselist(struct monthlyexp *expenses)
{
	free(expenses);
}
