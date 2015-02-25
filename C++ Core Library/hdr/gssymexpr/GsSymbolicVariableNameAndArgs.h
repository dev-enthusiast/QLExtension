/****************************************************************
**
**	GSSYMBOLICVARIABLENAMEANDARGS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicVariableNameAndArgs.h,v 1.13 2001/11/27 22:47:48 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICVARIABLENAMEANDARGS_H )
#define IN_GSSYMEXPR_GSSYMBOLICVARIABLENAMEANDARGS_H

#include <gssymexpr/base.h>
#include <gscore/GsSymbol.h>
#include <gscore/gsdt_fwd.h>
#include <gsdt/GsDtArray.h>

CC_BEGIN_NAMESPACE( Gs )


class GsSymbolicExpressionCopyOperatorReplaceWildCards;

/****************************************************************
** Class	   : GsSymbolicVariableNameAndArgs 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableNameAndArgs
{
public:
	GsSymbolicVariableNameAndArgs();
	GsSymbolicVariableNameAndArgs( GsSymbol Name, const GsDtArray &Args );
	GsSymbolicVariableNameAndArgs( const GsSymbolicVariableNameAndArgs & );

	~GsSymbolicVariableNameAndArgs();

	GsSymbolicVariableNameAndArgs &operator=( const GsSymbolicVariableNameAndArgs &rhs );

	bool operator==( const GsSymbolicVariableNameAndArgs &rhs ) const;

	GsSymbol	 Name() const;
	GsSymbol	&Name();

	const GsDtArray	&Args() const;
		  GsDtArray	&Args();

	bool ReplacementMatches( const GsDt &Target, size_t Position ) const;
	void ReplaceInPlace( const GsDt &Target, const GsDtArray &Replacement, size_t Position );

	bool MatchesTemplate( const GsSymbolicVariableNameAndArgs &Target, GsDtArray &TemplateReplacements ) const;
	
	bool ContainsWildCards() const;
	void ReplaceWildCards( const GsSymbolicExpressionCopyOperatorReplaceWildCards *Oper );

	bool operator< ( const GsSymbolicVariableNameAndArgs &rhs ) const;

	bool operator!=( const GsSymbolicVariableNameAndArgs &rhs ) const { return !( *this ==   rhs ); }

	bool operator>=( const GsSymbolicVariableNameAndArgs &rhs ) const { return !( *this <    rhs ); }
	bool operator> ( const GsSymbolicVariableNameAndArgs &rhs ) const { return  (  rhs  <  *this ); }
	bool operator<=( const GsSymbolicVariableNameAndArgs &rhs ) const { return !(  rhs  <  *this ); }

	size_t hash() const;

	// GsDt stuff

	typedef GsDtGeneric< GsSymbolicVariableNameAndArgs > GsDtType;
	GsString toString() const;
	static GsString typeName() { return GsString( "GsSymbolicVariableNameAndArgs" ); }

private:
	GsSymbol	
			m_Name;

	GsDtArray
			m_Args;

};


typedef GsSymbolicVariableNameAndArgs::GsDtType GsDtSymbolicVariableNameAndArgs;

EXPORT_CPP_GSSYMEXPR CC_OSTREAM &operator<<( CC_OSTREAM& s, const GsSymbolicVariableNameAndArgs &NameAndArgs );


CC_END_NAMESPACE

#endif 


