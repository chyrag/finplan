/*
 * finance.h: declarations for finance functions
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#ifndef __FINANCE_H_
#define __FINANCE_H_

#define DEFAULT_TIMELINE	12	/* months */

/*
 * Monthly expense
 */
struct monthlyexp {
	enum month	month;
	uint32_t	year;
	uint32_t	expenses;
	uint32_t	savings;
};

/*
 * Expense type
 */
enum expensetype {
	MONTHLY,
	ANNUAL,
	BUDGETARY,
}; 

/*
 * Options for annual expenses
 */
struct annualexpopt {
	enum month month; /* which month is this expense due? */
};

/*
 * Options for goals
 */
struct goal {
	enum month month; /* which month/year is this goal due? */
	int year;
};

/*
 * Expenses
 */
struct expense {
	enum expensetype exptype;
	union {
		struct annualexpopt aeo;
		struct goal g;
	} opts;
	char *comment;
	uint32_t amount;
};

int 			process_file(SLIST_ENTRY(expense), int, char *);
int 			display_finance(struct monthlyexp *, int , int);
struct monthlyexp 	*init_expenselist(int);
void 			cleanup_expenselist(struct monthlyexp *);

#endif /* __FINANCE_H_ */
