/* $Header: /home/cvs/src/window/src/werrors.h,v 1.2 1998/10/09 22:28:08 procmon Exp $ */
/****************************************************************
**
**	WERRORS.H	- Window system error handler header file
**
**	$Revision: 1.2 $
**
****************************************************************/

#ifndef	IN_WERRORS_H
#define	IN_WERRORS_H


/*
**  Define window error codes
*/

#define WE_MEMORY   1				/* Memory allocation error			*/
#define WE_BOUNDS   2				/* Location out of bounds error		*/
#define WE_RELEASE  3				/* Error freeing memory				*/
#define WE_NOSHOW   4				/* Window not being viewed error	*/
#define WE_NOSTART  5				/* Window system not started error	*/
#define WE_NOOPEN   6				/* Access of an unopen window error	*/
#define WE_REOPEN   7				/* Open attempted on an open window	*/
#define WE_OFFSCRN  8				/* Move/size window off screen		*/
#define	WE_BADUNGET	9				/* Overflow in the unget buffer		*/


/*
**	Define function error numbers
*/

#define	WE_START	1
#define	WE_END		2
#define	WE_INIT		3
#define	WE_OPEN		4
#define	WE_CLOSE	5
#define	WE_FREE		6
#define	WE_PICK		7
#define	WE_FIND		8
#define	WE_DEL		9
#define	WE_INS		10
#define	WE_SHOW		11
#define	WE_HIDE		12
#define	WE_PUT		13
#define	WE_PUTA		14
#define	WE_PUTS		15
#define	WE_PUTSA	16
#define	WE_GET		17
#define	WE_GETFILT	18
#define	WE_GETLINE	19
#define	WE_GETAREA	20
#define	WE_CLEAR	21
#define	WE_FILL		22
#define	WE_ATTR		23
#define	WE_SELECT	24
#define	WE_UNSELECT	25
#define	WE_ADRAW	26
#define	WE_SDRAW	27
#define	WE_DRAW		28
#define	WE_MOVE		29
#define	WE_SIZE		30
#define	WE_PALET	31
#define	WE_ADJUST	32
#define	WE_KPOS		33
#define	WE_KCOLOR	34
#define	WE_KPALET	35
#define	WE_MAKEVIS	36
#define	WE_ATTACH	37
#define	WE_DETACH	38
#define	WE_BOUNCE	39
#define	WE_BEEP		40
#define	WE_SHELL	41
#define	WE_GETKEY	42
#define	WE_RESTORE	43
#define	WE_VANISH	44
#define	WE_CUNGET	45
#define	WE_SUNGET	46
#define	WE_UNSETUP	47

#define	WE_BOX		48
#define	WE_HLINE	49
#define	WE_VLINE	50

#define	WE_VMENU	51
#define	WE_HMENU	52

#define	WE_FGET		53
#define	WE_FPUT		54
#define	WE_FPROMPT	55
#define	WE_FPLATE	56
#define	WE_FINIT	57
#define	WE_FOUTSTR	58

#define	WE_ANIMATE	59
#define	WE_RELOCATE	60

#define	WE_ISVIS	61

#define	WE_FONT		62

#define	WE_POS		63
#define	WE_PRINTF	64

#define	WE_LINK		65
#define	WE_UNLINK	66

#endif

