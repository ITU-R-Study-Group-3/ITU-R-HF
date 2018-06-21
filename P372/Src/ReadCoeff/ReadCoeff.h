#define GETXF2		1		// foF2
#define GETXFM3		2		// M(3000)F2
#define GETXE		4		// foE
#define GETXESU		8		// foE 10%
#define GETXES		16		// foE 50%
#define GETXESL		32		// foE 90%
#define GETXHPO1		64		// h'F, F2 (Used in ITS78)
#define	GETXHPO2		128		// h'F, F2 (Used in P.1239)
#define	GETXHP		256		// h'F
#define	GETFAKP		512		// Atmospheric parameter
#define	GETFAKABP	1024		// Atmospheric parameter
#define	GETDUD		2048		// Atmospheric parameter
#define	GETFAM		4096		// Atmospheric parameter
#define	GETSYS1		8192		// sys1() (Used in ITS78)
#define	GETSYS2		16384	// sys2() (Used in REC533)
#define	GETPERR		32768	// 
#define	GETF2D		65536	// MUF statistics 
#define	GETPKO		131072	// MUF statistics 
#define	GETSLP		262144	// MUF statistics 
#define	GETCCR		524288	// MUF statistics 

struct IonoCoeff {
	char name[256];		// File header name
						// Variable name line		
	int		*if2;		// if2(10)			   3 -    4
	double	***xf2;		// xf2(13,76,2)		   6 -  401
	int		*ifm3;		// ifm3(10)			 403 -  404	
	double	***xfm3;		// xfm3(9,49,2)		 406 -  582
	int		*ie;			// ie(10)			 584 -  585
	double	***xe;		// xe(9,22,2)		 587 -  666
	int		*iesu;		// iesu(10)			 668 -  669
	double	***xesu;		// xesu(5,55,2)		 671 -  780
	int		*ies;		// ies(10)			 782 -  783
	double	***xes;		// xes(7,61,2)		 785 -  955
	int		*iesl;		// iesl(10)			 957 -  958
	double	***xesl;		// xesl(5,55,2)		 960 - 1069
	int		*ihpo1;		// ihpo1(10)		1071 - 1072
	double	***xhpo1;	// xhpo1(13,29,2)	1074 - 1224
	int		*ihpo2;		// ihpo2(10)		1226 - 1227
	double	***xhpo2;	// xhpo2(9,55,2)	1229 - 1426
	int		*ihp;		// ihp(10)			1428 - 1429
	double	***xhp;		// xhp(9,37,2)		1431 - 1564
	double	***fakp;		// fakp(29,16,6)	1566 - 2122
	double	**fakabp;	// fakabp(2,6)		2124 - 2126
	double	***dud;		// dud(5,12,5)		2128 - 2187
	double	**fam;		// fam(14,12)		2189 - 2222
	double	***sys1;		// sys1(9,16,6)		2224 - 2396
	double	***sys2;		// sys2(9,16,6)		2398 - 2570
	double	***perr;		// perr(9,4,6)		2572 - 2615
	double	***f2d;		// f2d(16,6,6)		2617 - 2732
	double	***pko;		// pko(8,7,6)		2734 - 2801
	double	***slp;		// slp(8,7,6)		2803 - 2870
	double	***ccr;		// ccr(8,7,6)		2872 - 2939
};

// ReadCoeff.c prototypes
void ReadCoeff(struct IonoCoeff *Coeff, int month, long What2Read);
void FreeCoeffArrays(struct IonoCoeff *Coeff);
void NullCoeffArrays(struct IonoCoeff *Coeff);