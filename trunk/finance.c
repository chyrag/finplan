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
uint32_t	initialcapital = 0;
uint32_t	intendedsavings = 0;
uint32_t	monthlyincome = 0;
uint32_t	funds = 0;

int
get_curr_month(int *m, int *y)
{
	struct tm      *tm;
	time_t		now;

	if (!m || !y) {
		return -1;
	}
	now = time(NULL);
	tm = localtime(&now);

	if (tm) {
		*m = tm->tm_mon + 1;	/* jan = 1, feb = 2, ... */
		*y = tm->tm_year + 1900;
		return 0;
	}
	return -1;
}

int
display_finances(struct monthlyexp *exp, int months, int verbose)
{
	int		m;

	/*
	 * We'll have prettier display later.
	 */
	for (m = 0; m < months; m++) {
		if (verbose) {
			printf("%s %d (E: %d) (S: %d)\n",
			       monthname(exp[m].month), exp[m].year,
			       exp[m].expenses, exp[m].funds);
		} else {
			printf("%s %d %d\n",
			       monthname(exp[m].month), exp[m].year,
			       exp[m].expenses);
		}
	}

	return 0;
}

struct monthlyexp *
init_expenselist(int months, int cur_mon, int cur_year)
{
	struct monthlyexp *exp;
	int		m         , y;

	exp = malloc(months * sizeof(struct monthlyexp));
	if (exp) {
		for (m = 0, y = 0; m < months; m++) {
			/*
			 * Initialize figures for next month onwards..
			 */
			exp[m].month = ((cur_mon + m) % MONTHS_PER_YEAR) + 1;
			if (exp[m].month == JAN) {
				y++;
			}
			exp[m].year = cur_year + y;
			exp[m].expenses = 0;
			exp[m].funds = 0;
		}
	}
	return exp;
}

int
calculate_expenses(struct monthlyexp *exp, struct expense_hdr *headp,
		   int months, int cur_mon, int cur_year)
{
	int		i         , rm, ry;
	struct expense *e;

	/*
	 * Funds that we have at disposal
	 */
	funds = initialcapital;

	/*
	 * For each month, go over the expense list, and add to the expenses
	 */
	ry = cur_year;
	for (i = 0; i < months; i++) {
		rm = ((cur_mon + i) % MONTHS_PER_YEAR) + 1;
		if (rm == JAN) {
			ry++;
		}
		/*
		 * Add the salary to the funds
		 */
		funds += monthlyincome;

		/*
		 * Go over the expense list for the month
		 */
		STAILQ_FOREACH(e, headp, next) {
			switch (e->exptype) {
			case BUDGETARY:
			case MONTHLY:
				exp[i].expenses += e->amount;
				break;
			case ANNUAL:
				if (e->opts.aeo.month == rm) {
					exp[i].expenses += e->amount;
				}
				break;
			case ONETIME:
				if ((e->opts.oeo.month == rm) &&
				    (e->opts.oeo.year == ry)) {
					exp[i].expenses += e->amount;
				}
				break;
			default:
				break;
			}
		}

		/*
		 * Check if we have enough funds to cover this month's
		 * expenses
		 */
		if (funds < exp[i].expenses) {
			display_finances(exp, i + 1, 1);
			err(1, "expenses for %s %d exceeds the funds"
			    " (need %d; have %d)\n",
			    monthname(rm), ry,
			    exp[i].expenses, funds);
			return -1;
		}
		/*
		 * Deduct the expenses of the month from the savings
		 */
		funds -= exp[i].expenses;

		/*
		 * We add the intended monthly savings to the overall
		 * savings, but also deduct that from the income, and so...
		 * they cancel each other.
		 */
		exp[i].funds = funds;
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
#if NOT_YET			/* need to debug why this is not valid */
		free(e->comment);
#endif
		STAILQ_REMOVE_HEAD(headp, next);
		free(e);
	}

	/*
	 * ... and the calculations.
	 */
	free(expenses);
}
