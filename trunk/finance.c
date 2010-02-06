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
#include <sys/queue.h>

#include "utils.h"
#include "finance.h"

/*
 * Parse the expenses from the given filename and add it to the list of
 * expenses.
 */
int
process_file(struct expense_hdr *headp, char *filename)
{
	FILE *fp;
	char buf[BUFSIZ];
	int i, lines = 0;
	int amount;
	struct expense *e;

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
				amount = get_last_field(buf);
				if (amount < 0) {
					/* FIXME: error */
				}
				e = malloc(sizeof(struct expense));
				e->exptype = (buf[0] == 'M')?
					MONTHLY : BUDGETARY;
				e->amount = amount;
				STAILQ_INSERT_TAIL(headp, e, next);
				break;
			case 'S':
				/* savings */
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

int
process_expenses(struct monthlyexp * exp, int months,
		struct expense_hdr *headp)
{
	int i;
	struct expense *e;
	
	/*
	 * For each month, go over the expense list, and add to the expenses
	 */
	for (i = 0; i < months; i++) {
		STAILQ_FOREACH(e, headp, next) {
			switch (e->exptype) {
				case BUDGETARY:
				case MONTHLY:
					exp[i].expenses += e->amount;
					break;
				default:
					break;
			}
		}
	}
	return 0;
}

void
cleanup_expenselist(struct monthlyexp *expenses)
{
	free(expenses);
}
