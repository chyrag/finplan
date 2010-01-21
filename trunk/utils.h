/*
 * utils.h: declarations related to the utility functions
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#ifndef __UTILS_H_
#define __UTILS_H_

#define SEPERATOR		':'
#define MONTHS_PER_YEAR		12

/*
 * Months of the year
 */
enum month {
	JAN,
	FEB,
	MAR,
	APR,
	MAY,
	JUN,
	JUL,
	AUG,
	SEP,
	OCT,
	NOV,
	DEC,
};

int		get_last_field(char *);
int		get_option_month(char *);
int		get_month_year(int *, int *);
const char*	monthname(int n);

#endif /* __UTILS_H_ */
