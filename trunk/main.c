/*
 * $Id$
 *
 * Finance planner
 *
 * Little more description of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <err.h>

#include "finance.h"

static char *progname;

/*
 * Function to print the usage of the program.
 */
void
usage (void) {
	printf("Usage: %s [-h] [-v] [-t months] file ...\n", progname);
}

int
main (int argc, char **argv)
{
	int verbose = 0;
	int ch;
	char *ptr;
	int months;
	struct monthexp *expenses;

	/*
	 * Find the program name to be used for error messages and usage
	 * function
	 */
	progname = (ptr = rindex(argv[0], '/'))? ptr + 1 : argv[0];

	/*
	 * getopt magic
	 */
	while ((ch = getopt(argc, argv, "hvt:")) != -1) {
		switch (ch) {
			case 'v':
				verbose = 1;
				break;
			case 't':
				months = atoi(optarg);
				break;
			case 'h':
			default:
				usage();
				goto graceful_exit;
				break;
		}
	}
	argc -= optind;
	argv += optind;

	if (!argc) {
		/*
		 * No files to process?
		 */
		usage();
		goto graceful_exit;
	}

	/*
	 * t months, huh?
	 */
	expenses = init_expenses(months);
	if (!expenses) {
		errx(1, "unable to allocate memory");
	}

	/*
	 * Main input file processing ...
	 */
	for (ch = 0; ch < argc; ch++) {
		if (process_file(expenses, months, argv[ch]) < 0) {
			return 1;
		}
	}

	/*
	 * Display results
	 */
	if (display_finance(expenses, months, verbose) < 0) {
		return 1;
	}

	cleanup_expenses(expenses);

graceful_exit:
	return 0;
}
