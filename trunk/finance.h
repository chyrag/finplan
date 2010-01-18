/*
 * $Id$
 *
 * finance.h:
 *
 * Little more description of the program
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#ifndef __FINANCE_H_
#define __FINANCE_H_

#define DEFAULT_TIMELINE	12	/* months */
#define MONTHS_PER_YEAR		12

#define SEPERATOR		':'

struct monthexp {
	uint8_t		month;
	uint32_t	year;
	uint32_t	expenses;
	uint32_t	savings;
};

static const char *monthnames[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

int process_file(struct monthexp *exp, int months, char *filename);
int display_finance(struct monthexp *exp, int months, int verbose);
struct monthexp *init_expenses(int months);
void cleanup_expenses(struct monthexp *expenses);

#endif /* __FINANCE_H_ */
