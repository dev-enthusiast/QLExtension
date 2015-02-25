/* $Header: /home/cvs/src/files/src/files.h,v 1.2 2001/04/12 10:29:10 goodfj Exp $ */
/****************************************************************
**
**	files.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_FILES_H )
#define IN_FILES_H

#include <stdio.h>

typedef struct lj_struct {
	int page_size;					/* 0 defaults to letter */
#define PRINTER_EXEC			1
#define PRINTER_LETTER		2
#define PRINTER_LEGAL		3
#define PRINTER_A4			26
	int text_lines_per_page;	/* 0 means no bottom margin */
	int top_margin;
	int left_margin;
	int lines_per_inch;			/* valid values: 1,2,3,4,6,8,12,16,24,48 */
	float pitch;
	float point_size;
	int symbol_set;
#define PRINTER_ROMAN8		1
#define PRINTER_IBM			2
	unsigned portrait :1;
} LASERJET;

#define file_to_start(F)	fseek(F,NULL,SEEK_SET)
#define file_to_end(F)		fseek(F,NULL,SEEK_END)

DLLEXPORT char *print_output;

/* forward declarations */

/* in copyfile.c */
DLLEXPORT int file_copy(char *in, char *out, int append);
DLLEXPORT int file_backup(char *s, char *ext);

/* in printfil.c */
DLLEXPORT int file_print(char *file_name, char *header, LASERJET *lj);
DLLEXPORT int laserjet_lines_per_page(LASERJET *lj);
DLLEXPORT void laserjet_setup(FILE *outf, LASERJET *lj);
DLLEXPORT LASERJET *laserjet_suitable(char *file_name);
DLLEXPORT LASERJET *laserjet_portrait(void);
DLLEXPORT LASERJET *laserjet_landscape(void);
DLLEXPORT void laserjet_bold_on(FILE *f);
DLLEXPORT void laserjet_bold_off(FILE *f);
DLLEXPORT void laserjet_underline_on(FILE *f);
DLLEXPORT void laserjet_underline_off(FILE *f);
DLLEXPORT void laserjet_italic_on(FILE *f);
DLLEXPORT void laserjet_italic_off(FILE *f);
DLLEXPORT void laserjet_reset(FILE *f);
DLLEXPORT void laserjet_helvbold(FILE *f);
DLLEXPORT void laserjet_times_roman(FILE *f);
DLLEXPORT void laserjet_times_light(FILE *f);
DLLEXPORT void laserjet_courier(FILE *f);
DLLEXPORT void laserjet_xcol(FILE *f,int col);
DLLEXPORT void laserjet_yrow(FILE *f,int row);
DLLEXPORT void laserjet_xdec(FILE *f,int dec);
DLLEXPORT void laserjet_ydec(FILE *f,int dec);
DLLEXPORT void laserjet_xpat_id(FILE *f,int id);
DLLEXPORT void laserjet_xpat_size(FILE *f,int size);
DLLEXPORT void laserjet_ypat_size(FILE *f,int size);
DLLEXPORT void laserjet_pattern(FILE *f,int pat);
DLLEXPORT void laserjet_lines_per_in(FILE *f,int n);
DLLEXPORT void laserjet_blob(FILE *f);
DLLEXPORT void laserjet_raster_res(FILE *f,int res);
DLLEXPORT void laserjet_raster_from_cursor(FILE *f);
DLLEXPORT void laserjet_raster_uncoded(FILE *f);
DLLEXPORT void laserjet_raster_pixrows(FILE *f,int p);
DLLEXPORT void laserjet_raster_send(FILE *f,int bytes);
DLLEXPORT void laserjet_raster_end(FILE *f);
DLLEXPORT void laserjet_gslogo(FILE *f,int x,int y,int res);
DLLEXPORT void laserjet_box(FILE *f, int xl, int yl, int xh, int yh, int thick);

/* in existfil.c */
DLLEXPORT int file_exists(char *name);

/* in filedir.c */
DLLEXPORT int file_is_directory(char *s);
DLLEXPORT FILE *file_mkdir_open(char *name, char *mode, void (*dir_show_func)(char *path, int error));
DLLEXPORT void file_same_path_name(char *path_file, char *name, char *outbuf);
DLLEXPORT char *file_prog_name(void);

/* in filenlin.c */
DLLEXPORT int file_nlines(char *name);

/* in fileenv.c */
DLLEXPORT int file_env_name(char *env_var, char *file_name, char *outbuf);

/* in country.c */
DLLEXPORT int file_country_code(void);

/* in justify.c */
DLLEXPORT int file_justified_output(FILE *f, char *s, int left_margin, int right_margin, int *curpos);

#endif

