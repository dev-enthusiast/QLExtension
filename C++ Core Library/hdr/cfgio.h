/* $Header: /home/cvs/src/it_utils/src/cfgio.h,v 1.7 1998/10/09 22:14:28 procmon Exp $ */
/****************************************************************
**
**	CFGIO.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
**	Scott Cushman Lives!  (God save us)
**	
****************************************************************/

#if !defined( IN_CFGIO_H )
#define IN_CFGIO_H

typedef short
		(*CfgIoEnumFunc)(char *,char *,char *,long,void *);
		
typedef short
		(*CfgIoSectEnumFunc)(char *,long,void *);

DLLEXPORT short cfg_read	   	(char *,char *,char *,char *,short);
DLLEXPORT short cfg_devread   	(char *,char *,char *,char *,char *,short);
DLLEXPORT short cfg_write		(char *,char *,char *,char *);
DLLEXPORT short cfg_devwrite  	(char *,char *,char *,char *,char *);
DLLEXPORT short cfg_devrename	(char *,char *,char *,char *,char *,char *);
DLLEXPORT short cfg_del	   	(char *,char *,char *);
DLLEXPORT short cfg_devdel		(char *,char *,char *,char *);
DLLEXPORT short cfg_secdel		(char *,char *);
DLLEXPORT short cfg_create		(char *);
DLLEXPORT short cfg_enum	   	(char *,char *, CfgIoEnumFunc,long,void *);
DLLEXPORT short cfg_senum		(char *,    CfgIoSectEnumFunc,long,void *);

/* whether any semicolon starts a remark */
extern short cfg_anysemrem;

/* Error Codes */
// CFG_ERR_OPEN           could not open a file 
// CFG_ERR_TOOBIG         value too big for buffer
// CFG_ERR_CREATE         could not create a file
// CFG_ERR_WRITE          could not write a file 
// CFG_ERR_CLOSE          could not close a file 
// CFG_ERR_CREATEALR      already created 
// CFG_ERR_RENAME         could not rename temp=>real 

#define CFG_ERR_OPEN          (-100)
#define CFG_ERR_TOOBIG        (-101)
#define CFG_ERR_CREATE        (-102)
#define CFG_ERR_WRITE         (-103)
#define CFG_ERR_CLOSE         (-104)
#define CFG_ERR_CREATEALR     (-105)
#define CFG_ERR_RENAME        (-106)


#endif

