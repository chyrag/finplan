/*
 * finance.c: main guts of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <err.h>
#include <sys/types.h>
#include <sys/queue.h>

#include "finance.h"

/*
 * Global variables
 */
uint32_t initialcapital;
uint32_t intendedsavings;

int
get_month_year(int *m, int *y)
{
	struct tm *tm;
	time_t now;

	if (!m || !y) {
		return -1;
	}

	now = time(NULL);
	tm = localtime(&now);

	if (tm) {
		*m = tm->tm_mon + 1; /* jan = 1, feb = 2, ... */
		*y = tm->tm_year + 1900;
		return 0;
	}

	return -1;
}

int
display_finances(struct monthlyexp *exp, int months, int verbose)
{
	int m;

	/*
	 * We'll have prettier display later.
	 */
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

	/*
	 * Find the current month/year.
	 */
	if (get_month_year(&cur_mon, &cur_year) < 0) {
		return NULL;
	}

	exp = malloc(months * sizeof(struct monthlyexp));
	if (exp) {
		for (m = 0, y = 0; m < months; m++) {
			/*
			 * Initialize figures for next month onwards..
			 */
			exp[m].month = ((cur_mon + m) % MONTHS_PER_YEAR)+1;
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
calculate_expenses(struct monthlyexp * exp, int months,
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
cleanup_expenselist(struct expense_hdr *headp,
		struct monthlyexp *expenses)
		
{
	struct expense *e;

	/*
	 * Free the expenses ...
	 */
	while (!STAILQ_EMPTY(headp)) {
		e = STAILQ_FIRST(headp);
		free(e->comment);
		STAILQ_REMOVE_HEAD(headp, next);
		free(e);
	}

	/*
	 * ... and the calculations.
	 */
	free(expenses);
}
