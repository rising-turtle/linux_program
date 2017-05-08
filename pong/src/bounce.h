/* bounce.h			*/

/* some settings for the game	*/

#define	BLANK		' '
#define	DFL_SYMBOL	'o'
#define	TOP_ROW		4
#define	BOT_ROW 	21
#define	LEFT_EDGE	9
#define	RIGHT_EDGE	70
#define	X_INIT		20		/* starting col		*/
#define	Y_INIT		10		/* starting row		*/
#define	TICKS_PER_SEC	50		/* affects speed	*/

#define	X_TTM		5
#define	Y_TTM		8
#define X_MAX_TTM       8
#define Y_MAX_TTM       5

#define MISSING         -1
#define BOUNCE          1
#define NO_BOUNCE       0

/** the ping pong ball **/

struct ppball {
		int	y_pos, x_pos,
			y_ttm, x_ttm,
			y_ttg, x_ttg,
			y_dir, x_dir;
		char	symbol ;

	} ;
