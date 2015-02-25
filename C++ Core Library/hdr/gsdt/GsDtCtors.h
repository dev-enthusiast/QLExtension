/****************************************************************
**
**	GSDTCTORS.H	- Macro definitions for Type Constructors
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtCtors.h,v 1.4 2001/11/27 22:43:57 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTCTORS_H )
#define IN_GSDT_GSDTCTORS_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsType.h>
#include <ccstream.h>

CC_BEGIN_NAMESPACE( Gs )

// This macro assumes that the TypeObj GsDt implements a public member function
// which returns a reference to its m_data

#define	GSDT_CTOR( RetObj, TypeObj, GsDtParam )								\
																					\
	GsType	&Type = TypeObj::staticType();											\
																					\
	TypeObj	*Obj = static_cast< TypeObj* > ( Type.ConvertFrom( GsDtParam ) );		\
																					\
	if(!Obj)																		\
	{																				\
  		CC_OSTRSTREAM Stream;														\
  		Stream << "Cannot convert from " << GsDtParam.typeName()					\
			   << " to " << TypeObj::staticType().name()							\
               << "\n"   <<  GsErrGetString();										\
  		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );					\
	}																				\
																					\
	/* else - we don't get here if !Obj */											\
																					\
	RetObj  RetVal = Obj->data();													\
	delete Obj;																		\
																					\
	return RetVal;																


#define	GSDTPTR_CTOR( RetObj, TypeObj, GsDtParam )							\
																					\
	GsType 	&Type = TypeObj::staticType();											\
																					\
	TypeObj *Obj = static_cast< TypeObj* > ( Type.ConvertFrom( GsDtParam ) );		\
																					\
	if(!Obj)																		\
	{																				\
  		CC_OSTRSTREAM Stream;														\
  		Stream << "Cannot convert from " << GsDtParam.typeName()					\
			   << " to " << TypeObj::staticType().name()							\
               << "\n"   << GsErrGetString();										\
  		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );					\
	}																				\
																					\
	/* else	- we don't get here if !Obj */											\
																					\
	RetObj  RetVal = Obj;															\
																					\
	return RetVal;								


CC_END_NAMESPACE

#endif 
