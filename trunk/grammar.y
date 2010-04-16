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
extern char *yytext;
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
%token <number> NUMBER

%%

lines: /* empty */
	| lines line
	;

line:	emptyline
	| initialcapitalorsavings
	| monthlyexpenseorincome
	| annualexpense
	| onetimeexpense
	;

emptyline: NEWLINE

initialcapitalorsavings: CHAR SEP NUMBER
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

monthlyexpenseorincome: CHAR SEP COMMENT SEP NUMBER
	{
		struct expense *e;
		e = malloc(sizeof(struct expense));
		if (!e) {
			/* TODO */
			return -1;
		}
		switch ($1) {
		case 'B':
			/*
			 * Income, salary...
			 */
			e->exptype = BUDGETARY;
			e->comment = trim($3);
			e->amount = $5;
			break;
		case 'M':
			/*
			 * Income, salary...
			 */
			e->exptype = MONTHLY;
			e->comment = trim($3);
			e->amount = $5;
			break;
		case 'C':
			/*
			 * Income, salary...
			 * (need to maintain a link list to keep track of incomes)
			 */
			monthlyincome += (monthlyincome)?:$5;
			break;
		default:
			return -1;
			break;
		}

		STAILQ_INSERT_TAIL(&headp, e, next);
	}
	;

annualexpense: CHAR SEP MONTH SEP COMMENT SEP NUMBER
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

onetimeexpense: CHAR SEP MONTH SEP NUMBER SEP COMMENT SEP NUMBER
	{
		struct expense *e;
		e = malloc(sizeof(struct expense));
		if (!e) {
			/* TODO */
			return -1;
		}
		e->exptype = ONETIME;
		e->opts.oeo.month = monthnumber($3);
		e->opts.oeo.year = $5;
		e->comment = trim($7);
		trim(e->comment);
		e->amount = $9;

		STAILQ_INSERT_TAIL(&headp, e, next);
	}
	;

%%

void
yyerror (const char * errstr)
{
	fprintf(stderr, "%s at %s\n", errstr, yytext);
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
	if (calculate_expenses(expenselist, &headp, months, cmon, cyear) < 0) {
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
