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
 * Global variables set via expense input file
 */
uint32_t	initialcapital = 0;
uint32_t	intendedsavings = 0;
uint32_t	monthlyincome = 0;

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
display_finances(int displaytype, struct monthlyexp *exp, int months, int verbose)
{
	int		m;
	FILE		*fp;

	switch (displaytype) {
	case CONSOLE:
		for (m = 0; m < months; m++) {
			/*
			 * E: Expenses
			 * S: Savings
			 * AF: Available funds
			 */
			printf("%s %d | E: %8d | S: %8d | AF: %8d\n",
					monthname(exp[m].month), exp[m].year,
					exp[m].expenses, exp[m].savings,
					exp[m].funds);
		}
		break;
	case GNUPLOT:
		/*
		 * FIXME: Use a temp file
		 * FIXME: Use seperate functions
		 */
		fp = fopen("gpdata", "w");
		if (!fp) {
			return -1;
		}
		for (m = 0; m < months; m++) {
			fprintf(fp, "\"%s %d\" %d %d %d\n", monthname(exp[m].month),
					exp[m].year, exp[m].expenses, exp[m].savings,
					exp[m].funds);
		}
		fclose(fp);
		break;
	default:
		printf("Display type not yet implemented\n");
		break;
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
			exp[m].savings = 0;
		}
	}
	return exp;
}

static int
triage_expense (uint32_t funds, uint32_t savings, struct monthlyexp *exp)
{
	/*
	 * We can meet the expenses without stretching.
	 */
	if (exp->expenses + intendedsavings < funds) {
		return EOK;
	}

	/*
	 * If the expenses exceed the funds...
	 * ... then, lets check if we can forgo the savings for this
	 * month, and manage.
	 */
	if (exp->expenses < funds) {
		return ENOSAVINGS;
	}

	/*
	 * If the expenses exceed the funds, but we can borrow from
	 * savings, and save the world!
	 */
	if (exp->expenses < funds + savings) {
		return EUSESAVINGS;
	}

	/*
	 * If none of the above matches, then expenses should be more
	 * than we can manage using available funds + savings.
	 */
	return ENOHOPE;
}

int
calculate_expenses(int display, struct monthlyexp *exp, struct expense_hdr *headp,
		   int months, int cur_mon, int cur_year)
{
	int		i, rm, ry;
	uint32_t	funds, savings, bamt;
	struct expense *e;

	/*
	 * Funds that we have at disposal
	 */
	funds = initialcapital;
	savings = 0;

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
		 * We have calculated the expenses for this month.
		 * Check if we have enough funds to cover this month's
		 * expenses plus the intended savings
		 */

		switch (triage_expense(funds, savings, &exp[i])) {
		case EOK:
			/*
			 * Add monthly savings as an expense
			 */
			exp[i].expenses += intendedsavings;
			savings         += intendedsavings;
			exp[i].savings	= savings;
			funds 		-= exp[i].expenses;
			exp[i].funds 	= funds;
			break;

		case ENOSAVINGS:
			/*
			 * Sadly, we can't afford to save the intended
			 * amount this month.
			 */
			warnx(NOSAVINGS_ERR_MSG, intendedsavings,
					monthname(rm), ry);
			exp[i].savings 	= savings;
			funds 		-= exp[i].expenses;
			exp[i].funds 	= funds;
			break;

		case EUSESAVINGS:
			/*
			 * We would need to use the savings to manage
			 * expenses this month.
			 */
			bamt = exp[i].expenses - funds;
			warnx(USESAVINGS_ERR_MSG, bamt,
					monthname(rm), ry);
			savings 	-= bamt;
			funds		= 0;
			exp[i].savings 	= savings;
			exp[i].funds 	= funds;
			break;

		case ENOHOPE:
			warnx(NOHOPE_ERR_MSG REQ_INFO_MSG,
					monthname(rm), ry,
					exp[i].expenses, funds,
					savings);
			display_finances(display, exp, i + 1, 1);
			return -1;

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
