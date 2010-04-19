/*
 * finance.h: declarations for finance functions
 * 
 * Chirag Kantharia <chirag@kantharia.in>
 * Jan 2010
 */

#ifndef __FINANCE_H_
#define __FINANCE_H_

#define DEFAULT_TIMELINE	12	/* months */
#define MONTHS_PER_YEAR		12

#define REQ_INFO_MSG		"(need %d, have %d, savings %d)"
#define NOSAVINGS_ERR_MSG	"warning: not enough funds to save %d for %s %d"
#define USESAVINGS_ERR_MSG	"warning: would need to borrow %d from savings for %s %d"
#define NOHOPE_ERR_MSG		"error: expenses for %s %d exceeds funds\n"


extern uint32_t	initialcapital;
extern uint32_t	intendedsavings;
extern uint32_t	monthlyincome;

/*
 * Possible outcomes of triaging expenses
 */
enum {
	EOK = 0,

	/*
	 * Expenses are high, but we can borrow from savings and
	 * survive.
	 */
	EUSESAVINGS,

	/*
	 * Expenses are high, but we can borrow from savings and
	 * survive.
	 */
	ENOSAVINGS,

	/*
	 * Expenses are high and we cant survive even if we borrow
	 * from savings.
	 */
	ENOHOPE,
};

enum month {
	INVALID,
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

static const char *monthnames[] = {
	"Invalid month",
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

static inline const char *
monthname(int n)
{
	if (n < JAN || n > DEC) {
		return NULL;
	}
	return monthnames[n];
}

static inline char *
uppercase(char *name)
{
	int		i         , len;

	len = strlen(name);
	for (i = 0; i < len; i++) {
		if (name[i] >= 'a' && name[i] <= 'z') {
			name[i] += 'A' - 'a';
		}
	}

	return name;
}

static inline char *
trim(char *string)
{
	int		i         , j, len;
	char           *p;

	if ((len = strlen(string)) == 0) {
		return string;
	}
	for (i = 0; i < len && string[i] == ' '; i++);
	for (j = len - 1; j > 0 && string[j] == ' '; j--);

	len = j - i + 1;
	if (!(p = malloc(len + 1))) {
		return p;
	}
	strncpy(p, &string[i], len);
	p[len] = '\0';

	return p;
}

static inline enum month
monthnumber(char *monthname)
{
	/*
	 * Keeping it simple and stupid right now. Later on, if the function
	 * is being called several times, then, we can hash on the monthname
	 * and keep an array of hashes.
	 */
#define CMP(m)  if (!strncmp(uppercase(monthname), #m, 3)) { \
			return m; \
		}

	CMP(JAN);
	CMP(FEB);
	CMP(MAR);
	CMP(APR);
	CMP(MAY);
	CMP(JUN);
	CMP(JUL);
	CMP(AUG);
	CMP(SEP);
	CMP(OCT);
	CMP(NOV);
	CMP(DEC);

	return INVALID;
}

/*
 * Monthly expense
 */
struct monthlyexp {
	enum month	month;	/* jan = 1, feb = 2, ... */
	uint32_t	year;
	uint32_t	expenses;
	uint32_t	funds;
	uint32_t	savings;
};

/*
 * Expense type
 */
enum expensetype {
	INVALIDEXP,
	MONTHLY,
	ANNUAL,
	BUDGETARY,
	ONETIME,
};

static const char *expensenames[] = {
	"Invalid",
	"Monthly",
	"Annual",
	"Budgetary",
	"One time",
};

static inline const char *
expensename(enum expensetype exptype)
{
	if (exptype < 0 || exptype > BUDGETARY) {
		exptype = 0;
	}
	return expensenames[exptype];
}

/*
 * Options for annual expenses
 */
struct annualexpopt {
	enum month	month;	/* which month is this expense due? */
};

/*
 * Options for one time expenses
 */
struct onetimeexpopt {
	enum month	month;	/* month, ... */
	uint16_t	year;	/* ... year when this expense due? */
};

#if 0
/*
 * Options for goals
 */
struct goal {
	enum month	month;	/* which month/year is this goal due? */
	int		year;
};
#endif

/*
 * Expenses
 */
struct expense {
	enum expensetype exptype;
	union {
		struct annualexpopt aeo;
		struct onetimeexpopt oeo;
#if 0
		struct goal	g;
#endif
	}		opts;
	char           *comment;
	uint32_t	amount;

			STAILQ_ENTRY  (expense) next;
};
STAILQ_HEAD(expense_hdr, expense);

/*
 * Display options
 */
enum displaytype {
	CONSOLE = 1,
	GNUPLOT,
	X11,
};

int		get_curr_month(int *, int *);
struct monthlyexp *init_expenselist(int, int, int);
int		calculate_expenses(int, struct monthlyexp *, struct expense_hdr *, int, int, int);
int		display_finances(int, struct monthlyexp *, int, int);
void		cleanup_expenselist(struct expense_hdr *, struct monthlyexp *);

#endif				/* __FINANCE_H_ */
