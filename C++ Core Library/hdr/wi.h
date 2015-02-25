/* $Header: /home/cvs/src/window/src/wi.h,v 1.23 2004/10/22 19:59:17 khodod Exp $ */
/****************************************************************
**
**	WI.H		- Window system internal header file
**
**	$Header: /home/cvs/src/window/src/wi.h,v 1.23 2004/10/22 19:59:17 khodod Exp $
****************************************************************/

#ifndef IN_WI_H
#define	IN_WI_H

#include 	<window/base.h>
#include	<wevent.h>



/*
**	Define video display board types (only used locally)
*/

#define	MONO_BOARD	0x0000
#define	CGA_BOARD	0x0001
#define	EGA_BOARD	0x0002
#define	VGA_BOARD	0x0003
#define	VGA_8514	0x0007


/*
**	Prototype functions
*/

void    wi_InputResume(			void ),
		wi_InputSuspend(		void ),
		wi_MouseDispatchEvent(	W_EVENT *Event ),
		wi_MouseHandleInput(	int x, int y, int Buttons ),
		wi_attr(				int x_lo, int y_lo, int x_hi, int y_hi, char attr ),
		wi_beep(				int tone ),
		wi_blink(				int bflag ),
		wi_char(				int x_lo, int y_lo, int x_hi, int y_hi, char c ),
		wi_cpos(				int x, int y ),
		wi_ctype(				int ctype ),
		wi_end(					void ),
		wi_oldmode(				void ),
		wi_palette(				int attr, int new_color ),
		wi_savemode(			void ),
		wi_setmode(				int dev_code, int mode ),
		wi_smode(				int mode ),
		wi_squeak(				int x, int y ),
		wi_start(				void ),
		wi_restart(				void ),
		wi_suspend(				void );


EXPORT_CPP_WINDOW const char *
        wi_gettitle(			void );


EXPORT_CPP_WINDOW void
	 	wi_setch(				int letter, int size, int numchars, char *data_ptr ),
		wi_settitle(			char const* title, char const* icon_title ),
		wi_lock_end(			void ),
		wi_lock_start(			void );

int		wi_getdev(				void ),
		wi_getky(				void ),
		wi_getky0(				int *pungot ),
		wi_gmode(				void ),
		wi_iskey(				void );


/*
**	And one common function as a macro
*/

#define wi_range( _xLo, _yLo, _xHi, _yHi )	\
{	\
	if( _xLo < wi_global.x_lo )     wi_global.x_lo = _xLo;	\
	if( _yLo < wi_global.y_lo )     wi_global.y_lo = _yLo;	\
    if( _xHi > wi_global.x_hi )     wi_global.x_hi = _xHi;	\
    if( _yHi > wi_global.y_hi )     wi_global.y_hi = _yHi;	\
}


/*
**	Video board resolutions
*/

#define	EGA_RES	640, 350
#define	VGA_RES	640, 480


#endif

