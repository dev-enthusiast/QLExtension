/****************************************************************
**
**	gsdt/dttypes.h - All the types for the GsDt class heirarchy
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/dttypes.h,v 1.2 2001/06/28 19:07:56 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSDT_DTTYPES_H )
#define IN_GSDT_DTTYPES_H

CC_BEGIN_NAMESPACE( Gs )


/*
**	Forward declare all classes
*/

class GsDt;						// Base class for all GsDt classes
class GsType;					// Base TypeInfo class for each GsDtc class
class FcDataTypeInstanceInfo;	// Instance information class
//class GsDtuFormatInfo;			// Information for FcValue::Format() function
//class GsDtuUserMessage;			// Registered user messages
class GsStreamOut;				// Binary output stream
class GsStreamIn;				// Binary input stream
class GsDtuFuncArgs;			// Function arguments for Value.Func( Arg1, Arg2 )
class GsDtuMathFunction;		// For MathOperateOnAll--sin,cos,exp,...
class GsDtuConvFunc;			// Registered conversion function (one for each Type <- Type)

// We wish this could be GsDt::BinaryFunc, but we need to forward declare it for GsType
class GsDtuBinaryFunc;			// Registered binary operator function (one for each Type x Type)


/*
**	Random types
*/

typedef long	FcHashCode;			// Hash code
typedef int		GsTypeId;			// Datatype Id


/*
**	Operator enums
*/

enum GSDT_BINOP
{
	GSDTBO_ADD				= 0x0010,	// r = a + b
	GSDTBO_SUBTRACT			= 0x0011,	// r = a - b
	GSDTBO_MULTIPLY			= 0x0012,	// r = a * b
	GSDTBO_DIVIDE			= 0x0013,	// r = a / b
	GSDTBO_AND				= 0x0014,	// r = a & b
	GSDTBO_SHIFT			= 0x0015,	// r = a shifted by b
	GSDTBO_UNION			= 0x0016,	// r = a merged with b
	GSDTBO_AND_NC			= 0x0017,	// r = a & b (w/no copy--meaningless w/o assign)

	GSDTBO_EQUAL			= 0x0100,	// r = ( a == b )
	GSDTBO_NOT_EQUAL		= 0x0101,	// r = ( a != b )
	GSDTBO_GREATER_THAN		= 0x0102,	// r = ( a >  b )
	GSDTBO_LESS_THAN		= 0x0103,	// r = ( a <  b )
	GSDTBO_GREATER_OR_EQUAL = 0x0104,	// r = ( a >= b )
	GSDTBO_LESS_OR_EQUAL 	= 0x0105,	// r = ( a <= b )

	GSDTBO_LAST				= 0xFFFF
};

// FIX-Should this be in this file?
enum GSDT_UNARYOP
{
	GSDTUO_UNARY_MINUS		= 0x0081,	// r = -a
	GSDTUO_INCREMENT		= 0x0082,	// ++a
	GSDTUO_DECREMENT		= 0x0083,	// --a
	GSDTUO_INVERT			= 0x0084,	// a = 1/a

	GSDTUO_LAST				= 0xFFFF
};


CC_END_NAMESPACE

#endif	/* IN_GSDT_DTTYPES_H */
