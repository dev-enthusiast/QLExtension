/****************************************************************
**
**	window.h	- Window system master header file 
**
**	Copyright 1989-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/window/src/window.h,v 1.44 2006/11/27 20:17:23 khodod Exp $
**
****************************************************************/

#ifndef IN_WINDOW_H
#define IN_WINDOW_H

#include <window/base.h>

#if !defined( TRUE )
#	define TRUE    1
#	define FALSE   0
#endif



/*
**	Define window function return codes
*/

#define WERROR      -1		/* Indicates an abnormal return from a function	*/
#define WNOERR      0		/* Indicates a normal return from a function	*/
#define	WRETRY		1		/* Used within error handler to indicate retry	*/


/*
**  Define maximum input buffer size
*/

#define MAXINBUF    133


/*
**  Define maximum number of I/O fields
*/

#define MAXFIELDS   128


/*
**  Define devices which work with the window system
*/

#define AUTOMATIC   0x80		/* Automatic monitor type selection		*/
#define CGA_40x25	0x01		/* 40 x 25 CGA color mode				*/    
#define CGA_80x25	0x02		/* 80 x 25 CGA color mode				*/
#define EGA_40x12	0x03		/* 40 x 12 EGA color mode				*/
#define	EGA_40x25	0x04		/* 40 x 25 EGA color mode				*/
#define	EGA_80x25	0x05		/* 80 x 25 EGA color mode				*/
#define EGA_80x35	0x06 		/* 80 x 35 EGA color mode				*/
#define EGA_80x43	0x07		/* 80 x 43 EGA color mode				*/
#define MONOCHROME  0x08		/* 80 x 25 monochrome mode				*/
#define	VGA_80x50	0x09		/* 80 x 50 VGA color mode				*/
#define	VGA_80x60	0x0a		/* 80 x 60 VGA color mode				*/ 
#define	VGA_80x100	0x0b		/* 80 x 100 VGA color mode				*/
#define	USER_MODE	0x0f		/* User char set, resolution is required*/


/*
**  Define window end parameters
*/

#define FREE        0			/* Free a windows allocated memory		*/
#define NOFREE      1			/* Don't free a windows memory			*/



/*
**  Define colors & monochrome constants
*/

#define GSWIN_BLACK           0
#define GSWIN_BLUE            1
#define GSWIN_GREEN           2
#define GSWIN_CYAN            3
#define GSWIN_RED             4
#define GSWIN_MAGENTA         5
#define GSWIN_BROWN           6
#define GSWIN_WHITE           7
#define GSWIN_DARK_GREY       8
#define GSWIN_BRIGHT_BLUE     9
#define GSWIN_BRIGHT_GREEN    10
#define GSWIN_BRIGHT_CYAN     11
#define GSWIN_BRIGHT_RED      12
#define GSWIN_BRIGHT_MAGENTA  13
#define GSWIN_YELLOW          14
#define GSWIN_BRIGHT_WHITE    15

#define GSWIN_COLOR(f,b)  ((f)|(b)<<4)  /* Convert colors into a byte       */
/*
**	This is for backwards compatibility only
*/
#define COLOR(f,b)  GSWIN_COLOR((GSWIN_##f),(GSWIN_##b))

#define UNDERLINE       0x01        /* Underline attribute              */
#define INVERSE         0x70        /* Inverse video                    */
#define BLINK           0x80        /* Blinking video field             */
#define VNORMAL         0x07        /* Normal video attribute           */
#define VINTENSE        0x0F        /* Intense video                    */


/*
**  Define window control flags
*/

#define ICURSOR		0x00000001L		/* Input cursor enabled (columns)   */
#define LCURSOR		0x00000002L		/* Line cursor enabled              */
#define FCURSOR		0x00000004L		/* Field cursor enabled             */
#define DISPLAY		0x00000008L		/* Window is displayable            */
#define	SHADOW		0x00000010L		/* Window has a shadow				*/
#define T_LEFT		0x00000020L		/* Title left justified             */
#define T_RIGHT		0x00000040L		/* Title right justified            */
#define ATR_ON		0x00000080L		/* Attributes enabled for window    */
#define ARROWS		0x00000100L		/* Arrow display enabled for window */
#define HEADLINE	0x00000200L		/* Headline border enabled          */
#define TAILLINE	0x00000400L		/* Bottome line border enabled      */
#define SIDELINE	0x00000800L		/* Sideline borders enabled         */
#define NOALLOC		0x00001000L		/* User supplied window data table  */
#define ALLOCATE	0x00002000L		/* Allocate all memory at once      */
#define	CCURSOR		0x00004000L		/* Character cursor enabled			*/
#define	SCROLL		0x00008000L		/* Enable automatic scrolling		*/
#define	AUTOPOS		0x00010000L		/* Automatic makevis after puts		*/
#define	MOUSEADJH	0x00020000L		/* Enable horizontal mouse adjusts	*/
#define	MOUSEADJV	0x00040000L		/* Enable vertical mouse adjusts	*/
#define	MOUSEADJM	0x00080000L		/* Enable movement mouse adjusts	*/
#define	ICURSORL	0x00100000L		/* Input cursor enabled (lines)		*/
#define	ICURSORR	0x00200000L		/* Input cursor enabled (region)	*/
#define	WORDWRAP	0x00400000L		/* Wrap words when putting to windows */

#define T_CENTER	0x00000000L		/* Title centered (default)         */

#define	MOUSEADJ	(MOUSEADJH | MOUSEADJV | MOUSEADJM)	/* All mouse adj*/
#define BORDER		(HEADLINE | TAILLINE | SIDELINE) /* All borders on	*/


/*
**	Define input system control flags
*/

#define	INSERT_MODE	0x0001			/* Start input in insert mode		*/
#define	MODE_CHANGE	0x0002			/* Allow toggle insert/normal mode	*/
#define	FLOW_THRU	0x0004			/* Flow through data entry			*/
#define	NO_FILL_RET	0x0008			/* Allow return even if mustfill	*/
#define	AUTO_CLEAR	0x0010			/* Clear field at 1st sign of input	*/


/*
**	Define input system status flags
*/

#define	UNFILLED	0x0001			/* On - MUSTFILL returned premature	*/
#define	ABORTED		0x0002			/* On - Entry aborted				*/
#define	MODIFIED	0x0004			/* On - Data in field is modified	*/


/*
**	Define template control flags
*/

#define	UPPERCASE	0x0001			/* Uppercase conversion flag		*/
#define	LOWERCASE	0x0002			/* Lowercase conversion flag		*/
#define	MUSTFILL	0x0004			/* Character must be filled			*/


/*
**  Define synchronization flags
*/

#define SYNC_X      0x0001          /* Synchronize in x axis            */
#define SYNC_Y      0x0002          /* Synchronize in y axis            */
#define SYNC_CURSOR 0x0004          /* Cursor is also synchronized      */

#define SYNC_BOTH   (SYNC_X | SYNC_Y)	/* Synchronize in both axis		*/


/*
**	Define beeping parameters
*/

#define	BEEP_OFF	0x0100			/* Turns beeping off				*/
#define	BEEP_ON		0x0200			/* Turns beeping on					*/
#define	BEEP_LOW	0x0012			/* Makes low beeping noise			*/
#define	BEEP_MED	0x0006			/* Makes medium beeping noise		*/
#define	BEEP_HIGH	0x0003			/* Makes high pitched beeping noise	*/


/*
**  Define structure for a window
*/

struct WINDOW;

typedef void (*W_MOUSE_PROC)( WINDOW *w, int x, int y, int buttons );
typedef int  (*W_KEY_PROC)( int Key, int Ungot );

struct WINDOW
{
	WINDOW
			*prev,		  		/* Ptr to prev window in list	*/
			*next,				/* Ptr to next window in list   */
			*sync_x,			/* Synchronized x window        */
			*sync_y,			/* Synchronized y window        */
			*link;				/* Linked screen position window*/

	long	control;			/* Window control flags			*/

    char    **tbl,              /* Table of window data         */
            **atr,              /* Table of attribute data      */
            *freetbl,           /* Free pointer if ALLOCATED    */
            *freeatr;           /* Free pointer if ALLOCATED    */

	const char
            *title,             /* Title of window              */
            *subtit;            /* Subtitle of window           */

    char    at_window,          /* Window character attribute	*/
            at_fcursor,         /* Field cursor attribute		*/
            at_lcursor,        	/* Line cursor attribute		*/
            at_icursor,         /* Input cursor attribute		*/
            at_border,			/* Border attribute             */
			at_title,			/* Title attribute				*/
			at_subtitle,		/* Subtitle attribute			*/
			at_arrows,			/* Arrows attribute				*/
			at_shadow,			/* Attribute of shadow			*/
			at_prompt,			/* Field prompt attribute		*/
			at_data;			/* Field data attribute			*/

    int     *fld;               /* Table of field locations     */

    int     wx_lo,              /* X low coordinate             */
            wy_lo,              /* Y low coordinate             */
            wx_hi,              /* X high coordinate            */
            wy_hi;              /* Y high coordinate            */

    int     wx_lim,             /* Internal limits              */
            wy_lim,             /* taking borders into account  */
            wx_him,             /* High limits                  */
            wy_him;             /*                              */

    int     x_pos,              /* X position in virtual window */
            y_pos,              /* Y position in virtual window */
            x_cur,              /* X position of write cursor   */
            y_cur,              /* Y position of write cursor   */
            f_cursor,           /* Current field cursor pos     */
            l_cursor,           /* Current line cursor pos      */
            i_cur_x1,           /* Input cursor x start         */
            i_cur_x2,           /* Input cursor x end           */
            i_cur_y1,           /* Input cursor y start			*/
			i_cur_y2,			/* Input cursor y end			*/
			c_cur_typ,			/* Character cursor type		*/
            sync,               /* Synchronized cursor flags    */
            width,              /* Virtual width of the window  */
            length,             /* Length in lines of data table*/
            last,               /* Last line in the data table  */
            status;             /* Open status indicator        */

    char    bc_top,             /* Top border character         */
            bc_bottom,          /* Bottom border char           */
            bc_lside,           /* Left side border char        */
            bc_rside,           /* Right side border char       */
            bc_uplf,            /* Upper left corner char       */
            bc_uprt,            /* Upper right corner char      */
            bc_dnlf,            /* Lower left corner char       */
            bc_dnrt,            /* Lower right corner char      */
			bc_shadow;			/* Shadow character				*/

	W_MOUSE_PROC
			mouse_func;			/* Window mouse function		*/

	void	*user_ptr;			/* For the user to use			*/

};


/*
**	Define mouse button info
*/

#define W_MOUSE_BN1		0x0001		/* Button 1 depressed */
#define W_MOUSE_BN2		0x0002		/* Button 2 depressed */
#define W_MOUSE_BN3		0x0004		/* Button 3 depressed */



/*
**	Define keyproc control parameters
*/

#define	W_KP_FRONT		1		/* Add keyproc to front of list	*/
#define	W_KP_BACK		2		/* Add keyproc to end of list	*/
#define	W_KP_DELETE		3		/* Delete keyproc from list		*/


/*
**	Define keyproc structure
*/

struct KEYPROC
{
	KEYPROC
			*next;

	W_KEY_PROC
			func;

};


/*
**  Declare global variable structure
*/

struct W_GLOBAL
{
    WINDOW  *w_cur,				/* Current window pointer			*/
			*w_lst;				/* Last window in linked list		*/

    char    *bufmem;            /* Buffer memory					*/

    int     init,               /* Initialization status        	*/
			suspend,			// suspension status
            x_res,              /* X buffer resolution          	*/
            y_res,              /* Y buffer resolution          	*/
			x_pan,				/* X location in frame buffer		*/
			y_pan,				/* Y location in frame buffer		*/
			x_vres,				/* X video resolution				*/
			y_vres,				/* Y video resolution				*/
            ref_cnt,            /* Refresh counter              	*/
            old_mode,           /* Old mode of character device 	*/
			new_mode,			/* New mode of character device 	*/
            dev_code,           /* Device code being used       	*/
			mem_size,			/* Size of buffer memory			*/
			beep_flag,			/* Flag used for beep on/off		*/
			tab_size,			/* Size used for tab expansion		*/
			equip_fg,			/* Equipment flag               	*/
			equip_nfg;			/* New equipment flag           	*/

	long	control;			/* Default control flags			*/

    int     at_window,			/* Default attributes           	*/
            at_fcursor,			/*  for the field cursor			*/
            at_lcursor,			/*  for the line cursor				*/
            at_icursor,			/*  for the input cursor			*/
            at_border,			/*  for the window border			*/
			at_shadow,			/*  for the shadow border			*/
			at_title,			/*  for the title					*/
			at_subtitle,		/*  for the subtitle				*/
			at_arrows,			/*  for the arrows					*/
			at_prompt,			/*  for the field's prompts			*/
			at_data,			/*  for the field's data			*/
			at_background;		/* Attr for whole screen			*/

    char    bc_top,				/* Top border character         	*/
            bc_bottom,			/* Bottom border char           	*/
            bc_lside,			/* Left side border char        	*/
            bc_rside,			/* Right side border char       	*/
            bc_uplf,			/* Upper left corner char       	*/
            bc_uprt,			/* Upper right corner char      	*/
            bc_dnlf,			/* Lower left corner char       	*/
            bc_dnrt,			/* Lower right corner char      	*/
			bc_shadow,			/* Shadow character					*/
			bc_background;		/* Char for whole screen			*/

	int		inp_control,		/* Input system control flags		*/
			inp_status,			/* Input system status flags		*/
			key_ed_left,		/* Key to move left in input field	*/
			key_ed_right,		/* Key to move right in input field	*/
			key_mv_left,		/* Key to move left one field		*/
			key_mv_right,		/* Key to move left one field		*/
			key_ok_form,		/* Key to enter an entire form		*/
			key_ok_input,		/* Key to enter a single field		*/
			key_no_form,		/* Key to cancel form input			*/
			key_no_input,		/* Key to cancel field input 		*/
			(*keyprep)( void ),	/* Func called before key entry		*/
			(*keywait)( void );	/* Keyboard wait function			*/

    int     key_help;			/* Help key							*/
    void    (*help)( int help_code );
								/* Default help function        	*/
	int		help_code;			/* Global help code					*/
	const char
			*help_file;			/* Global help file name			*/

    int     (*error)( int err_code, int func_num, WINDOW *w ),
								/* Error handler                	*/
			error_code;			/* Error code of last error			*/

	int		*OLD_ungetbuf,		/* Pointer to unget buffer			*/
			OLD_unget,			/* Unget read-pointer				*/
			OLD_unput,			/* Unget write-pointer				*/
			OLD_unsize;			/* Size of unget buffer				*/

	long	mem_used;			/* Amount of memory allocated to	*/
								/* the window system				*/

	int		(*font_func)( const char * );
								/* Function to load font file		*/

	const char
			*font_file;			/* Fontfile for user fonts			*/

    unsigned char
			*plate[ 128 ];		/* Table of template pointers   	*/

	unsigned char
			*palette;			/* Pointer to EGA palette			*/

	void	(*exit_func)(void);	/* Application supplied exit func	*/

	KEYPROC	*keyproc;			/* Ptr to key processing linked list*/

	int		mouse_active,		/* Mouse active flag				*/
			mouse_stack;		/* Amount of stack for mouse thread	*/

	W_MOUSE_PROC
			mouse_func;			/* Default mouse function			*/

};

DLLEXPORT W_GLOBAL
		w_global;


/*
**  Declare types for all window routines (keep 'em honest)
*/

EXPORT_CPP_WINDOW int
		w_start(	int, int, int ),
		w_restart(	void ),
		w_end(     	int ),
		w_init(    	WINDOW * ),
		w_open(    	WINDOW *, int, int, int, int ),
		w_close(   	WINDOW *, int ),
		w_free(		WINDOW * ),
		w_pick(    	WINDOW * ),
		w_find(    	WINDOW *, int, char * ),
		w_del(     	WINDOW *, int, int ),
		w_ins(     	WINDOW *, int, int ),
		w_show(    	WINDOW * ),
		w_hide(    	WINDOW * ),
		w_pos(		WINDOW *, int, int ),
		w_put(     	WINDOW *, int, int, const char * ),
		w_puta(    	WINDOW *, int, int, const char *, int ),
		w_putc(		WINDOW *, char ),
		w_puts(    	WINDOW *, const char * ),
		w_putsa(   	WINDOW *, const char *, int ),
		w_get(     	WINDOW *, int, int, char *, int ),
		w_getfilt( 	WINDOW *, int, int, char *, const char *, int ),
		w_getline( 	WINDOW *, int, int, int ),
		w_getarea(	WINDOW *, int, int, int, int ),
		w_getnow(	void ),
		w_clear(   	WINDOW *, int, int, int, int ),
		w_wipe(		WINDOW * ),
		w_fill(	   	WINDOW *, int, int, int, int, int ),
		w_attr(    	WINDOW *, int, int, int, int, int ),
		w_select(  	WINDOW * ),
		w_unselect(	WINDOW * ),
		w_resize_screen( int xres, int yres );

EXPORT_CPP_WINDOW void
		w_adraw(	WINDOW *, int, int, int, int ),
		w_sdraw(   	WINDOW *, int, int, int, int ),
		w_draw(    	WINDOW * );

EXPORT_CPP_WINDOW int
		w_move(    	WINDOW *, int, int ),
		w_size(    	WINDOW *, int, int ),
		w_palet(	int, int );

EXPORT_CPP_WINDOW void
		w_blink(	int ),
		w_kpalet(	void ),
		w_detach(  	WINDOW *, int ),
		w_vanish(	void );

EXPORT_CPP_WINDOW int
		w_adjust(  	WINDOW * ),
		w_kpan(		void ),
		w_kpos(   	WINDOW *, bool displayStatusLine = true ),
		w_kcolor(  	WINDOW * ),
		w_makevis( 	WINDOW *, int, int, int, int ),
		w_isvis(	WINDOW *, int, int, int, int ),
		w_attach(  	WINDOW *, WINDOW *, int ),
		w_bounce(  	WINDOW * ),
		w_beep(		int ),
		w_shell(   	const char *, int, int ),
		w_getkey(  	void ),
		w_getkey0(	int *pUngot ),
		w_restore(	void ),
		w_ungetc(	int ),
		w_ungets(	const char * ),
		w_unsetup(	int ),
		w_sungetc(	int ),
		w_sungets(	const char * ),
		w_suspend(	void		);

EXPORT_CPP_WINDOW int
		w_stderr(  	int, int, WINDOW * ),
		w_stdkey(	int, int );

EXPORT_CPP_WINDOW void
		w_stdhlp(	int );

EXPORT_CPP_WINDOW char
		w_altkey(	int );

EXPORT_CPP_WINDOW int
		w_link(		WINDOW *, WINDOW * ),
		w_unlink(	WINDOW * ),
		w_animate(	WINDOW *, int, int, int, int, int ),
		w_relocate(	WINDOW *, int, int, int, int ),
		w_box(		WINDOW *, int, int, int, int, int ),
		w_hline(	WINDOW *, int, int, int, int ),
		w_vline(	WINDOW *, int, int, int, int );

EXPORT_CPP_WINDOW int
		w_font(		const char * );

EXPORT_CPP_WINDOW int
		w_printf(	WINDOW *, int, int, char *, const char *, ... ) ARGS_LIKE_PRINTF(5),
		w_printa(	WINDOW *, int, int, int, char *, const char *, ... ) ARGS_LIKE_PRINTF(6),
		w_print(	WINDOW *, char *, const char *, ... ) ARGS_LIKE_PRINTF(3),
		w_prina(	WINDOW *, int, char *, const char *, ... ) ARGS_LIKE_PRINTF(4);

EXPORT_CPP_WINDOW void
		w_off(		WINDOW * ),
		w_on(		WINDOW * ),
		w_refoff(	void ),
		w_RemoveGetKeyThread( int threadID ),
		w_refon(	void );

EXPORT_CPP_WINDOW int
		w_keyproc(	W_KEY_PROC KeyProc, int ),
		w_iskey(	void );

EXPORT_CPP_WINDOW double
		w_calc(		void );

EXPORT_CPP_WINDOW int
		w_mouse(	int );

EXPORT_CPP_WINDOW int
		wf_plate(	int, const char *, int );

EXPORT_CPP_WINDOW int
		wi_getnow(	int ),
		wi_error(   int, int, WINDOW * ),
		wi_alloc(  	WINDOW *, int ),
		wi_rlsml(	char *, long );

EXPORT_CPP_WINDOW void
		wi_redraw( 	int, int, int, int ),
		wi_updte(	int vid_xlo, int vid_ylo ),
		wi_reshow( 	WINDOW *, int, int, int, int ),
		wi_draw(	WINDOW *, int, int, int, int ),
		wi_bord(  	WINDOW *, int, int, int, int ),
		wi_attr( 	int, int, int, int, char ),
		wi_char( 	int, int, int, int, char ),
		wi_str(  	int, int, int, const char * ),
		wi_stra( 	int, int, int, const char *, const char * ),
		wi_beep(	int ),
		wi_unget(	int ),
		wi_setfocus(void );		// set focus to console if possible

EXPORT_CPP_WINDOW char
		*wi_getml(	long );

EXPORT_CPP_WINDOW int
		 wi_getky(	void );

EXPORT_CPP_WINDOW int
		wi_max(		int, int ),
		wi_min(		int, int );

EXPORT_CPP_WINDOW unsigned char
		wi_lines(	unsigned char, unsigned char );

EXPORT_CPP_WINDOW WINDOW
		*wi_find_window(	int x, int y, int *border );

EXPORT_CPP_WINDOW int
		wi_dispatch_init( 
			void (*wi_ctype)( int	ctype ),
			void (*wi_cpos)(	 int	x, int y ),
			void (*wi_setmode)( int dev_code, int mode ),
			int  (*wi_getdev)( void ),
			void (*wi_savemode)( void ),
			void (*wi_oldmode)( void ),
			void (*wi_squeak)(  int x, int	y ),
			void (*wi_palette)( int attr, int	new_color),
			void (*wi_blink)(	 int		bflag ),
			void (*wi_restart)( void ),
			void (*wi_end)( void ),
			void (*wi_suspend)( void ),
			void (*wi_setch)(	int		letter,	int		size,	int		numchars,	char	*data_ptr ),
			void (*wi_beep)(	int		tone ),
			void (*wi_char)( 	int		x_lo,	int		y_lo,	int		x_hi,	int		y_hi,	char	c ),
			void (*wi_str)(	int		x,	int			y,	int			len,	const char	*str ),
			void (*wi_stra)(   int x, int y,	int	len, const char	*str, const char	*atr ),
			void (*wi_updte)(	int		vid_xlo,	int		vid_ylo ),
			void (*wi_attr)(	int		x_lo,	int		y_lo,	int		x_hi,	int		y_hi,	char	attr ),
			const char *(*wi_gettitle)( void ),
			void (*wi_settitle)( char const *Title, char const *IconTitle ),
			void (*wi_InputSuspend)( void ),
			void (*wi_InputResume)( void )
		);


/****************************************************************
**  Routine: W_HELP
**  Params : lev - new help level
**  Returns: None
**  Action : Set the help level with a macro
****************************************************************/

#define	w_help(lev)	(w_global.help_code = (lev))

#endif

