/****************************************************************
**
**	FORM.H	- Form library
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/form/src/form.h,v 1.32 2012/06/13 12:44:08 e19351 Exp $
**
****************************************************************/

#if !defined( IN_FORM_H )
#define IN_FORM_H

#include <form/base.h>
#include <stdio.h>

#ifndef IN_WINDOW_H
#include <window.h>
#endif

#ifndef IN_WKEYS_H
#include <wkeys.h>
#endif

#ifndef IN_DATE_H
#include <date.h>
#endif

#include <time.h>


/*
**	Typedefs
*/

typedef struct form_struct	FORM;
typedef struct field_struct FIELD;


/*
**	String Table types
*/

typedef const char * const * STRTAB;	// The constant version
typedef char **STRTAB_MOD;				// Non-constant version


/*
**	Define callback function types
*/

typedef int		(*form_FUNC_INPUT)(		FORM *form, FIELD *field, int code );
typedef int		(*form_FUNC_CONSTRAINT)(FORM *form, FIELD *field);
typedef void	(*form_FUNC_POS)(		FORM *form, FIELD *field);
typedef void	(*form_FUNC_INIT)(		FORM *form, FIELD *field);
typedef int		(*form_FUNC_REMOVE)( 	FIELD *field);

typedef void	(*form_FUNC_BACK)(		FILE *f,long *n,int nlines,char *buffer,int data_length,void *arg);
typedef void	(*form_FUNC_FWD)( 		FILE *f,long *n,int nlines,char *buffer,int data_length,void *arg);
typedef int		(*form_FUNC_PUT)( 		FILE *f,WINDOW *w,long n,char *buffer, void *arg);
typedef int		(*form_FUNC_DESCRIP)(	char *fname,char *buf);

typedef void	(*form_FUNC_MODIFY)(	FORM *form, FIELD *field );

typedef void	(*form_FUNC_ERRMSG)(	void );
typedef void	(*form_FUNC_STRTAB)( 	STRTAB strtab, WINDOW *popup, FORM *parent );


/************************ FIELD STRUCT ********************************/

struct field_struct
{
	FIELD	*prev,
			*next;

	char	*s,					/* The field description string */
			*data;				/* type-specific data */

	void	*extra;				/* guaranteed unused by form library */

	form_FUNC_INPUT
			input_function;

	form_FUNC_CONSTRAINT
			constraint_function;

	form_FUNC_POS
			pos_function;

	form_FUNC_INIT
			init_function;

	form_FUNC_REMOVE
			rm_function;

	int		x,				   	/* coordinates that field is on */
			y,
			x_offset,			/* offset of input area from beginning of field */
							   	/* (offset < 0 means an uneditable field) */
			width,				/* width of input field */
			type,			   	/* type of field */

#define form_TEXT				0
#define form_INT				1
#define form_FLOAT				2
#define form_YESNO				3
#define	form_YES				1
#define	form_NO					(-1)
#define form_DATE				4
#define form_ROLL				5
#define form_COLUMN				6
#define form_MATCH				7
#define form_TIME				8
#define form_FRAC				9
#define form_STRTAB			   10
#define form_RDATE			   11
#define form_BUTTON			   12
#define form_GMT			   13
#define form_RTIME			   14

			helpnum,
			flags;				/* flags for type-specific use */

signed char	attr,				/* attribute of field's editable portion */
			bgattr;				/* attribute of field's uneditable portion */

} /* FIELD */;

#define form_editable(field)			((field)->x_offset >= 0)
#define form_ed_string(field)			((field)->s + (field)->x_offset)

#define form_has_commas(field)			((field)->flags & 1)
#define form_set_commas(field)			((field)->flags |= 1)
#define form_caps_only(field)			((field)->flags & 1)
#define form_set_caps(field)			((field)->flags |= 1)

#define form_has_maxdec(field)			((field)->flags & 2)
#define form_set_maxdec(field)			((field)->flags |= 2)
#define form_is_down(field)				((field)->flags & 2)
#define form_set_down(field)			((field)->flags |= 2)

#define form_rmstring(field)			((field)->flags & 0x80)
#define form_set_rmstring(field)		((field)->flags |= 0x80)

#define form_modified(field)			((field)->flags & 0x0100)
#define form_set_modified(field)		((field)->flags |= 0x0100)
#define form_set_unmodified(field)		((field)->flags &= ~0x0100)

#define AUTOCLEAR_FLAG 0x0200
#define	form_autoclear(field)			((field)->flags & 0x0200)	// Autoclear feature switch
#define	form_set_autoclear(field)		((field)->flags |= 0x0200)

#define form_autoclear_enabled(field)	((field)->flags & 0x0400)
#define form_autoclear_enable(field)	((field)->flags |= 0x0400)	// Set when entering a field
#define form_autoclear_disable(field)	((field)->flags &= ~0x0400)	// Cleared after first keystroke in field

#define RHSCURSOR_FLAG 0x0800
#define	form_rhscursor(field)			((field)->flags & RHSCURSOR_FLAG)
#define	form_set_rhscursor(field)		((field)->flags |= RHSCURSOR_FLAG)

#define FORM_NO_MSG ((const char*) &FormNoMsg )

/************************ FORM STRUCT ********************************/
struct form_struct
{
	FIELD	*field,				/* doubly linked list of fields */
								/* (first field is always null) */
			*uned,				/* uneditable fields are listed separately */
								/* (first field is always null) */
			*hide,				/* list of hidden fields */
								/* (first field is NOT null) */
			*curfld,			/* field cursor is in */
			*oldcur;			/* last field cursor was in (internal use) */
	WINDOW 	*w;
	int 	*macro;				/* keystroke macro stored with ctrl-q */
	char	*last_string;		/* string of curfld when field was entered */
	void	*extra;				/* guaranteed unused by form library */
	int	xl, yl, xh, yh,			/* low and high box corners */
			curpos,				/* position inside field */
			macro_ptr;			/* index to form->macro's top */

	void	*sem; 				/* for locking */

	unsigned
			opened : 1,
			shown : 1,
			match : 1,				/* am I a match form? */
			insert_mode : 1,
			exit_constraint : 1,	/* TRUE means cannot leave doform() if curfld's constraint returns TRUE */
			in_macro : 1,
			redisplay : 1;

	int     EditingDone;        /* if true, we are finishing the edit */

} /* FORM */;

#define FORM_MACRO_MAX		25

/************************ misc field structs ****************************/
struct roll_ring
{
	struct roll_ring *next, *prev;
	char *s;					/* allowable string */
	char *data;				/* possibly some data associated with string */
};

typedef struct form_ring
{
	struct roll_ring *home;
	struct roll_ring *ring;
	unsigned elements_only : 1;	/* whether only ring strings allowed or not */
	unsigned non_member : 1;		/* set iff a non-member of the ring is now in the field */
	unsigned free_s : 1;				/* set iff the roll ring strings were malloced and may be freed by rmform() */
	void (*modify_func)(struct form_ring *r, FORM *roll_form, FORM *parent_form);
} form_RING;

typedef struct
{
	struct field_struct
			*prev,
			*next;	/* column links */
	int 	type;					/* the type that the column is built of */
	char 	*data;					/* the data of the underlying type */

	form_FUNC_INPUT
			input_function;

	form_FUNC_INIT
			init_function;

} form_COL_DATA;

typedef struct
{
	FIELD *underlying_field;
	int type;					/* underlying type */
	char *data;
	int (*match_function)();
} form_MATCH_DATA;

typedef struct
{
	int  		cur;		// current index into strtab
	STRTAB		strtab;		// NULL terminated string table (>= 1 items)
	form_FUNC_STRTAB
				modify_func_new;	// '_new' because now it takes diff args

#ifdef THIS_STUFF_LOOKS_LIKE_IT_IS_NOT_USED_ANYWHERE_SO_I_AM_REMOVING_IT
	unsigned	free_tab:1,	// free table in form_rmfield	(default FALSE)
				free_str:1;	// free strings in table in form_rmfield (default FALSE)
#endif

} form_STRTAB_DATA;



/************************ function declarations ***************************/
#define	FDECL
	/* in form.c */
EXPORT_CPP_FORM FORM *	form_mkform(int xl, int yl, int xh, int yh);
EXPORT_CPP_FORM void		form_display(FORM *form);
EXPORT_CPP_FORM int		form_doform(FORM *form);
EXPORT_CPP_FORM void		form_position_cursor(FORM *form);
EXPORT_CPP_FORM FIELD *	form_appfld(FORM *form, FIELD *field);
EXPORT_CPP_FORM FIELD *	form_mkfld( char *s, int type, int x, int y, int x_offset, int width, int attr, int helpnum );
EXPORT_CPP_FORM FIELD *	form_mkfld_dup( const char *s, int type, int x, int y, int x_offset, int width, int attr, int helpnum );
EXPORT_CPP_FORM void		form_hide(FORM *form, FIELD *field);
EXPORT_CPP_FORM void		form_unhide(FORM *form, FIELD *field);
EXPORT_CPP_FORM void		form_setblank(FIELD *field);
EXPORT_CPP_FORM int		form_isblank(FIELD *field);
EXPORT_CPP_FORM void		form_rightmost(FORM *form, FIELD *field);
EXPORT_CPP_FORM void		form_leftmost(FORM *form, FIELD *field);
EXPORT_CPP_FORM char *	form_trim(FIELD *field);
EXPORT_CPP_FORM char *	form_untrim(FIELD *field);
EXPORT_CPP_FORM void		form_lock(FORM *form, int on);
EXPORT_CPP_FORM void		form_mouse_func(WINDOW *w, int col, int row, int fs);

	/* in formcol.c */
EXPORT_CPP_FORM FIELD *	form_col_mkfld(FIELD *);
EXPORT_CPP_FORM int		form_col_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM void		form_col_add(FORM *, FIELD *);

	/* in fquest.c */
EXPORT_CPP_FORM char *	form_question( char *, const char *, int, int, int, int);

	/* in fin.c */
EXPORT_CPP_FORM void		form_update(FORM *form, FIELD *field);
EXPORT_CPP_FORM int		form_text_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_int_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_float_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_frac_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_frac_cstr(FORM *form, FIELD *field);
EXPORT_CPP_FORM void		form_kill_commas(FIELD *);
EXPORT_CPP_FORM int		form_comma(char *, int);
EXPORT_CPP_FORM int		form_roll_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM form_RING * form_mkring(char **, int);
EXPORT_CPP_FORM struct roll_ring *	form_roll_help(form_RING *r, FORM *parent_form);
EXPORT_CPP_FORM int		form_date_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_date_cstr(FORM *, FIELD *);
EXPORT_CPP_FORM void		form_date_to_field(DATE,FIELD *);
EXPORT_CPP_FORM DATE		form_field_to_date( FIELD *);
EXPORT_CPP_FORM int		form_rdate_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_rdate_cstr(FORM *, FIELD *);
EXPORT_CPP_FORM void		form_rdate_to_field(RDATE,FIELD *);
EXPORT_CPP_FORM RDATE   	form_field_to_rdate( FIELD *);
EXPORT_CPP_FORM int		form_yesno_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_button_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_button_cstr(FORM *, FIELD *);
EXPORT_CPP_FORM void		form_button_init(FORM *, FIELD *);

	/* in f2roll.c */
EXPORT_CPP_FORM form_RING *	form_f2roll(char *);
EXPORT_CPP_FORM char *	form_left_justify(char *);
EXPORT_CPP_FORM char *	form_right_justify(char *);
EXPORT_CPP_FORM int		form_f2win( FILE *f, WINDOW *w, long *n, form_FUNC_BACK backfunc, form_FUNC_FWD fwdfunc, form_FUNC_PUT putfunc, int data_length, void *arg);
EXPORT_CPP_FORM void		form_fixed_fwd(FILE *f,long *n, int nlines, char *buffer, int data_length, void *arg);
EXPORT_CPP_FORM void		form_fixed_back(FILE *f, long *n, int nlines, char *buffer, int data_length, void *arg);

	/* in rmform.c */
EXPORT_CPP_FORM void		form_rmform(FORM *);
EXPORT_CPP_FORM void		form_rmfield(FIELD *);
EXPORT_CPP_FORM void		form_rmroll(FIELD *);

	/* in match.c */
EXPORT_CPP_FORM FIELD *	form_match_mkfld(FIELD *);
EXPORT_CPP_FORM FORM *	form_match_mkform(FORM *);
EXPORT_CPP_FORM int		form_match_string(char *, char *);
EXPORT_CPP_FORM int		form_match_int(char *, int);
EXPORT_CPP_FORM int		form_match_double(char *, double);

	/* in fdump.c */
EXPORT_CPP_FORM int		form_dump(FORM *, char *);
EXPORT_CPP_FORM int		form_undump(FORM *, char *);

	/* in formtime.c */
EXPORT_CPP_FORM int		form_time_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_time_cstr(FORM *, FIELD *);
EXPORT_CPP_FORM void		form_time_to_field(long, FIELD *);
EXPORT_CPP_FORM DATE		form_time_to_date(long time);
EXPORT_CPP_FORM long		form_ftotime(FIELD *field);

	/* in formgmt.c */
EXPORT_CPP_FORM int		form_gmt_in(FORM *, FIELD *, int);
EXPORT_CPP_FORM int		form_gmt_cstr(FORM *, FIELD *);
EXPORT_CPP_FORM void		form_gmt_to_field(time_t, FIELD *);
EXPORT_CPP_FORM time_t	form_field_to_gmt(FIELD *field);

	/* in formrtim.c */
EXPORT_CPP_FORM int		form_rtime_in(			FORM *Form, FIELD *Field, int Code );
EXPORT_CPP_FORM int		form_rtime_cstr(		FORM *Form, FIELD *Field );
EXPORT_CPP_FORM void		form_rtime_to_field(	RTIME *RTime, FIELD *Field );
EXPORT_CPP_FORM RTIME	   *form_field_to_rtime(	FIELD *Field );

	/* in findfld.c */
EXPORT_CPP_FORM FIELD *	form_findfld(FORM *form, char *s);

	/* in formmem.c */
EXPORT_CPP_FORM char *	form_malloc(unsigned int n);
EXPORT_CPP_FORM void		form_free(char *s);
EXPORT_CPP_FORM char *	form_blank(int n);
EXPORT_CPP_FORM void		form_showmem(void);
EXPORT_CPP_FORM char *	form_strdup( const char *s );

	/* in frolls.c */
EXPORT_CPP_FORM int 		form_set_ring_home(FIELD *field, char *s);
EXPORT_CPP_FORM int		form_add_roll(FIELD *field, char *string, char *data, int sort);
struct roll_ring * form_find_ring(FIELD *field, char *s);

	/* in fquery.c */
EXPORT_CPP_FORM char *	form_query( char *s, int x_offset, int width, int type, void (*modify_func)(FORM *form, FIELD *field), void *arg);

	/* in formnum.c */
EXPORT_CPP_FORM int		form_dtof(double d, FIELD *f);
EXPORT_CPP_FORM int		form_dtof2(double d, FIELD *f, int ndec);
EXPORT_CPP_FORM void		form_ftod(FIELD *field, double *d);
EXPORT_CPP_FORM void		form_itof(int i, FIELD *field);
EXPORT_CPP_FORM int		form_ftoi(FIELD *field);

	/* in fstof.c */
EXPORT_CPP_FORM void		form_stof(const char *s, FIELD *field);
EXPORT_CPP_FORM void		form_ftos(char *s, FIELD *field);
EXPORT_CPP_FORM void		form_ytof(int condition, FIELD *field);

	/* in formfil.c */
EXPORT_CPP_FORM int form_file_in(FORM *f, FIELD *field, int code);
EXPORT_CPP_FORM void form_fname_mod(FORM *form, FIELD *field);

	/* in fmatrix.c */
typedef int (*FORM_MATRIX_SAVE_FUNC)( FORM *form, FIELD *field, int row, int col, void *arg);
typedef int (*FORM_MATRIX_RESTORE_FUNC)( FORM *form, FIELD *field, int row, int col, void *arg);
typedef int (*FORM_MATRIX_XLABEL_FUNC)(FORM *form, FIELD *field, int col, void *arg);
typedef int (*FORM_MATRIX_YLABEL_FUNC)(FORM *form, FIELD *field, int col, void *arg);

EXPORT_CPP_FORM void form_make_matrix(FORM *form, int ydim, int xlabel_width,
	int ylabel_width, int cell_width, int cell_type,
	FORM_MATRIX_YLABEL_FUNC ylabel_func,
	void *arg);
EXPORT_CPP_FORM int form_matrix_doform(FORM *form,
	FORM_MATRIX_SAVE_FUNC save_func,
	FORM_MATRIX_RESTORE_FUNC restore_func,
	FORM_MATRIX_XLABEL_FUNC xlabel_func,
	void *arg);
EXPORT_CPP_FORM int form_matrix_save(FORM *form,
	FORM_MATRIX_SAVE_FUNC save_func,
	void *arg);
EXPORT_CPP_FORM int form_matrix_restore(FORM *form,
	FORM_MATRIX_RESTORE_FUNC restore_func,
	void *arg);
EXPORT_CPP_FORM int form_matrix_xlabel(FORM *form,
	FORM_MATRIX_XLABEL_FUNC xlabel_func,
	void *arg);
EXPORT_CPP_FORM int form_matrix_get_row_col(FORM *form, FIELD *field, int *prow, int *pcol);

	/* in formdup.c */
EXPORT_CPP_FORM FORM * form_duplicate(FORM *form);


// fstrtab.c
EXPORT_CPP_FORM int 	form_strtab_in		( FORM *form, FIELD *field, int code);

EXPORT_CPP_FORM int 	form_strtab_init	( FORM *form, FIELD *field );

EXPORT_CPP_FORM int	form_attach_strtab	( FIELD *field, STRTAB strtab, form_FUNC_STRTAB modify_func );

EXPORT_CPP_FORM void	form_stitof			( int strtbl_index, FIELD *field);

EXPORT_CPP_FORM int	form_ftosti			( FIELD *field);	// returns strtbl_index or -1 if not found

EXPORT_CPP_FORM int form_strtab_help(
	FORM *parent_form,		// current form on screen
	FIELD *strtab_fld		// field who's values to display
);

// formmsg.c
EXPORT_CPP_FORM void form_message( const char *, int, int );
EXPORT_CPP_FORM void form_msg   ( const char *fmtstr, ...) ARGS_LIKE_PRINTF(1);
EXPORT_CPP_FORM void form_wmsg  ( const char *fmtstr, ...) ARGS_LIKE_PRINTF(1);
EXPORT_CPP_FORM void form_errmsg( const char *fmtstr, ...) ARGS_LIKE_PRINTF(1);
EXPORT_CPP_FORM void form_select( WINDOW *w );

DLLEXPORT char FormNoMsg;

DLLEXPORT struct form_msg_params_structure
{
	char 	at_window	   ;	 // window attributes for form_msg and form_wmsg
	char 	at_title	   ;	 // title attributes for form_msg and form_wmsg
	char 	at_subtitle	   ;	 // subtitle attributes for form_wmsg
	const char
			*title		   ;	 // title for form_msg and form_wmsg
	char 	err_at_window  ;	 // window attributes for form_errmsg
	char 	err_at_title   ;	 // title attributes for form_errmsg
	char 	err_at_subtitle;	 // subtitle attributes for form_errmsg
	const char
			*err_title	   ;	 // title for form_errmsg
	const char
			*subtitle	   ;	 // subtitle for form_wmsg and form_errmsg
	int		wait_key	   ;	 // wait key for form_wmsg and form_errmsg, 0 means don't wait
	long	wait_time	   ;	 // always wait this many seconds before changing message
	int		x			   ;	 // xl corner of msg window, -1 for centered
	int		y			   ;	 // yl corner of msg window, -1 for centered
	char	*buffer		   ;	 // buffer used for the vsprintf
	unsigned	center:1   ;	 // center each line
	form_FUNC_ERRMSG
			err_callback   ;	 // callback for form_errmsg
} form_msg_params;

/* Defaults for form_msg_params:
	COLOR( BRIGHT_WHITE, BLUE),			// default at_window
	COLOR( BRIGHT_GREEN, BLUE),			// default at_title
	COLOR( WHITE, BLUE),				// default at_subtitle
	NULL,								// default title
	COLOR( BRIGHT_WHITE, RED),			// default err_at_window
	COLOR( YELLOW, RED),				// default err_at_title
	COLOR( CYAN, RED),					// default err_at_subtitle
	" ERROR ",							// default err_title
	" Press <Esc> to continue ",		// default subtitle
	WK_ESC,								// default wait_key
	2L,									// default wait_time
	-1,									// center msg box horizontally
	-1,									// center msg box vertically
	defaultbuf[1024],					// default buffer
	TRUE,								// center each line by default
	NULL								// No callback for each error
*/



/* formask.c */

EXPORT_CPP_FORM int form_ask_text( 	const char *Prompt, char *Value, int EditWidth,			form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_text_with_flags(	const char *Prompt, char *Value, int EditWidth,	form_FUNC_MODIFY ModifyFunc, void *Arg, int Flags );
EXPORT_CPP_FORM int form_ask_yesno( 	const char *Prompt, int Default,						form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_yesno_defaultESC( 	const char *Prompt, int Default,				form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_strtab(	const char *Prompt, int Default, STRTAB StrTab,			form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_double(	const char *Prompt, double *Value, int EditWidth,		form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_date(	const char *Prompt, DATE *Value,						form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_time(	const char *Prompt, time_t *Value,						form_FUNC_MODIFY ModifyFunc, void *Arg );
EXPORT_CPP_FORM int form_ask_gmt(		const char *Prompt, time_t *Value,						form_FUNC_MODIFY ModifyFunc, void *Arg );

/* in formcal.c */

EXPORT_CPP_FORM DATE form_calendar( DATE dt );
EXPORT_CPP_FORM void form_calendar_set( const char *Ccy );


#endif
