/*
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
#include <sys/queue.h>

#include "utils.h"
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

	int months = DEFAULT_TIMELINE;
	struct monthlyexp *expenselist;

	SLIST_ENTRY(expense) exp;

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
	 * FIXME: Step 1: Read input
	 */
	for (ch = 0; ch < argc; ch++) {
		if (process_file(exp, months, argv[ch]) < 0) {
			return 1;
		}
	}

	/*
	 * FIXME: Step 2: Validate input
	 */


	/*
	 * Step 3: Expenses array
	 */
	expenselist = init_expenselist(months);
	if (!expenselist) {
		errx(1, "unable to allocate memory");
	}

	/*
	 * Step 4: Main expense processing
	 */

	/*
	 * Step 5: Display results
	 */
	if (display_finance(expenselist, months, verbose) < 0) {
		return 1;
	}

	/*
	 * Step 6: Cleanup
	 */
	cleanup_expenselist(expenselist);

graceful_exit:
	return 0;
}
