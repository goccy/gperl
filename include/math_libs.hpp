#define GPERL_ABS(x) do {								\
		switch (TYPE_CHECK(x)) {						\
		case 0:											\
			DOUBLE_init(reg[pc->dst], fabs(x.dvalue));	\
			break;										\
		case 1:											\
			INT_init(reg[pc->dst], abs(x.ivalue));		\
			break;										\
		default:										\
			fprintf(stdout, "ERROR!!: type error\n");	\
			break;										\
		}												\
	} while (0);

#define GPERL_ATAN2(x, y) do {									\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		double arg1 = (TYPE_CHECK(y)) ? y.ivalue : y.dvalue;	\
		DOUBLE_init(reg[pc->dst], atan2(arg0, arg1));			\
	} while (0);

#define GPERL_COS(x) do {										\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		DOUBLE_init(reg[pc->dst], cos(arg0));					\
	} while (0);

#define GPERL_EXP(x) do {										\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		DOUBLE_init(reg[pc->dst], exp(arg0));					\
	} while (0);

#define GPERL_HEX(x) do {										\
		/*DOUBLE_init(reg[pc->dst], hex(x.dvalue));*/			\
		fprintf(stdout, "TODO: coming soon\n");					\
		exit(EXIT_FAILURE);										\
	} while (0);

#define GPERL_INT(x) do {										\
		int v = (int)(TYPE_CHECK(x)) ? x.ivalue : x.dvalue;		\
		INT_init(reg[pc->dst], v);								\
	} while (0);

#define GPERL_LOG(x) do {										\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		DOUBLE_init(reg[pc->dst], log(arg0));					\
	} while (0);

#define GPERL_OCT(x) do {										\
		/*DOUBLE_init(reg[pc->dst], oct(x.dvalue));*/			\
		fprintf(stdout, "TODO: coming soon\n");					\
		exit(EXIT_FAILURE);										\
	} while (0);

#define GPERL_RAND(x) do {										\
		/*DOUBLE_init(reg[pc->dst], rand(x.dvalue));*/			\
		fprintf(stdout, "TODO: coming soon\n");					\
		exit(EXIT_FAILURE);										\
	} while (0);

#define GPERL_SIN(x) do {										\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		DOUBLE_init(reg[pc->dst], sin(arg0));					\
	} while (0);

#define GPERL_SQRT(x) do {										\
		double arg0 = (TYPE_CHECK(x)) ? x.ivalue : x.dvalue;	\
		DOUBLE_init(reg[pc->dst], sqrt(arg0));					\
	} while (0);

#define GPERL_SRAND(x) do {										\
		/*DOUBLE_init(reg[pc->dst], srand(x.dvalue));*/			\
		fprintf(stdout, "TODO: coming soon\n");					\
		exit(EXIT_FAILURE);										\
	} while (0);
