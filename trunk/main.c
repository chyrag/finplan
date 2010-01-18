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
#include <strings.h>
#include <unistd.h>

#include "finance.h"

static char *progname;

/*
 * Function to print the usage of the program.
 */
void
usage (void) {
	printf("Usage: %s [-h] [-v] file ...\n", progname);
}

int
main (int argc, char **argv)
{
	int verbose = 0;
	int ch;
	char *ptr;

	/*
	 * Find the program name to be used for error messages and usage
	 * function
	 */
	progname = (ptr = rindex(argv[0], '/'))? ptr + 1 : argv[0];

	/*
	 * getopt magic
	 */
	while ((ch = getopt(argc, argv, "hv")) != -1) {
		switch (ch) {
			case 'v':
				verbose = 1;
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
	 * Main input file processing ...
	 */
	for (ch = 0; ch < argc; ch++) {
		if (process_file(argv[ch]) < 0) {
			return 1;
		}
	}

	/*
	 * Display results
	 */
	if (display_finance(verbose) < 0) {
		return 1;
	}

graceful_exit:
	return 0;
}
