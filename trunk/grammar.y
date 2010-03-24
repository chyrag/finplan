/*
 * Finance planner
 *
 * Little more description of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <err.h>
#include <sys/queue.h>
#include "finance.h"

static char *progname;
static struct expense_hdr headp;
extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;
%}

%union {
	int number;
	char c;
	char *string;
}

%token NEWLINE SEP
%token <string> COMMENT
%token <string> MONTH
%token <c>      CHAR
%token <number> AMOUNT

%%

lines: /* empty */
	| lines line
	;

line:	emptyline
	| initialcapitalorsavings
	| monthlyexpense
	| annualexpense
	;

emptyline: NEWLINE

initialcapitalorsavings: CHAR SEP AMOUNT
	{
		switch ($1) {
		case 'I':
			/*
			 * If we have got the initial capital via
			 * options, then use that, and ignore the one
			 * supplied in the input.
			 */
			initialcapital = (initialcapital)?:$3;
			break;
		case 'S':
			/*
			 * If we have got the intended savings via
			 * options, then use that, and ignore the one
			 * supplied in the input.
			 */
			intendedsavings = (intendedsavings)?:$3;
			break;
		}
	}
	;

monthlyexpense: CHAR SEP COMMENT SEP AMOUNT
	{
		struct expense *e;
		e = malloc(sizeof(struct expense));
		if (!e) {
			/* TODO */
			return -1;
		}
		switch ($1) {
		case 'B':
			e->exptype = BUDGETARY;
			break;
		case 'M':
			e->exptype = MONTHLY;
			break;
		default:
			return -1;
			break;
		}
		e->comment = trim($3);
		e->amount = $5;

		STAILQ_INSERT_TAIL(&headp, e, next);
	}
	;

annualexpense: CHAR SEP MONTH SEP COMMENT SEP AMOUNT
	{
		struct expense *e;
		e = malloc(sizeof(struct expense));
		if (!e) {
			/* TODO */
			return -1;
		}
		e->exptype = ANNUAL;
		e->opts.aeo.month = monthnumber($3);
		e->comment = trim($5);
		trim(e->comment);
		e->amount = $7;

		STAILQ_INSERT_TAIL(&headp, e, next);
	}
	;

%%

void
yyerror (const char * errstr)
{
	fprintf(stderr, "%s\n", errstr);
}

int
yywrap (void)
{
	return 1;
}

void
usage (void) {
	printf("Usage: %s [-h] [-v] [-t months] file ...\n", progname);
}

int
main (int argc, char **argv)
{
	int verbose, months, display;
	int ch, cmon, cyear;
	char *ptr;
	struct monthlyexp *expenselist;

	STAILQ_HEAD(expense_hdr, expense);
	verbose = 0;
	months = DEFAULT_TIMELINE;
	display = CONSOLE;

	/*
	 * Find the program name to be used for error messages and usage
	 * function
	 */
	progname = (ptr = rindex(argv[0], '/'))? ptr + 1 : argv[0];

	/*
	 * getopt magic
	 */
	while ((ch = getopt(argc, argv, "hvd:t:i:s:")) != -1) {
		switch (ch) {
			case 'v':
				verbose = 1;
				break;
			case 'd':
				display = CONSOLE;
				break;
			case 't':
				months = atoi(optarg);
				break;
			case 'i':
				initialcapital = atoi(optarg);
				break;
			case 's':
				intendedsavings = atoi(optarg);
				break;
			case 'h':
			default:
				usage();
				return 0;
		}
	}
	argc -= optind;
	argv += optind;

#if _DOESNT_WORK_YET
	/*
	 * If provided with the input file, then use that. Only process the
	 * first file; ignore the rest.
	 */
	if (argc) {
		yyin = fopen(argv[1], "r");
	}
#endif

	STAILQ_INIT(&headp);
	/*
	 * Call the parser to fill up the expense list
	 */
	yyparse();

	/*
	 * Get the current month/year
	 */
	if (get_curr_month(&cmon, &cyear) < 0) {
		errx(1, "unable to find current month/year");
	}

	/*
	 * FIXME: Step 2: Validate input
	 */


	/*
	 * Step 3: Expenses array
	 */
	expenselist = init_expenselist(months, cmon, cyear);
	if (!expenselist) {
		errx(1, "unable to allocate memory");
	}

	/*
	 * Step 4: Main expense processing
	 */
	if (calculate_expenses(expenselist, &headp, months, cmon) < 0) {
		errx(1, "error processing expenses");
	}

	/*
	 * Step 5: Display results
	 */
	if (display_finances(expenselist, months, verbose) < 0) {
		return 1;
	}

	/*
	 * Step 6: Cleanup
	 */
	cleanup_expenselist(&headp, expenselist);

	if (argc) {
		fclose(yyin);
	}

	return 0;
}
