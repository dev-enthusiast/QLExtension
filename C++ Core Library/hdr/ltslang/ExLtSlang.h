/****************************************************************
**
**	EXLTSLANG.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltslang/src/ltslang/ExLtSlang.h,v 1.3 2001/01/05 17:44:41 vengrd Exp $
**
****************************************************************/

#if !defined( IN_LTSLANG_EXLTSLANG_H )
#define IN_LTSLANG_EXLTSLANG_H

#include <ltslang/base.h>


CC_BEGIN_NAMESPACE( Gs )

// A series of macros used to declare and define the dummy addin
// functions that will be used to implement slang addins.


// In order to increase the number of slang addins allowed, increase
// the five occurences of the number below, which is the log of the
// number of addins.  For example, if it is set to 8, then 256 addins
// are allowed.  Be sure to change it both in the
// EXLTSLANG_LG_OF_NUM_ADDINS and in the bodies of
// EXLTSLANG_DECLARE_DUMMIES and EXLTSLANG_DEFINE_DUMMIES.
//
// The overall maximum is determined by the macros in
// 
#define EXLTSLANG_LG_OF_NUM_ADDINS	12

#define EXLTSLANG_DECLARE_DUMMIES	\
EXLTSLANG_DECLARE_DUMMY12( 0 )		\
EXLTSLANG_DECLARE_DUMMY12( 1 )

#define EXLTSLANG_DEFINE_DUMMIES	\
EXLTSLANG_DEFINE_DUMMY12( 0, 0 )	\
EXLTSLANG_DEFINE_DUMMY12( 1, 0 )

#define EXLTSLANG_NUM_ADDINS	( 1 << EXLTSLANG_LG_OF_NUM_ADDINS )

#define LTSLANG_MAX_ARGS	16

CC_END_NAMESPACE

#endif 
