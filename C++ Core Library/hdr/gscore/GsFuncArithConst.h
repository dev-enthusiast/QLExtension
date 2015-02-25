/****************************************************************
**
**	GSFUNCARITHCONST.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncArithConst.h,v 1.2 2000/04/07 17:09:15 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCARITHCONST_H )
#define IN_GSFUNC_GSFUNCARITHCONST_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsFuncTimesConst 
** Description : 
****************************************************************/

template< class DomainAndRange, class CoeffT >
class EXPORT_CLASS_GSFUNC GsFuncTimesConst : public GsFunc< DomainAndRange, DomainAndRange >
{
public:
	GsFuncTimesConst( CoeffT Coefficient );
	GsFuncTimesConst( const GsFuncTimesConst & );

	virtual ~GsFuncTimesConst();

	GsFuncTimesConst &operator=( const GsFuncTimesConst &rhs );

	virtual DomainAndRange operator()( DomainAndRange Arg ) const;

private:
		
	CoeffT	m_Coefficient;
};


/****************************************************************
** Class	   : GsFuncPlusConst 
** Description : 
****************************************************************/

template< class DomainAndRange, class CoeffT >
class EXPORT_CLASS_GSFUNC GsFuncPlusConst : public GsFunc< DomainAndRange, DomainAndRange >
{
public:
	GsFuncPlusConst( CoeffT Coefficient );
	GsFuncPlusConst( const GsFuncPlusConst & );

	virtual ~GsFuncPlusConst();

	GsFuncPlusConst &operator=( const GsFuncPlusConst &rhs );

	virtual DomainAndRange operator()( DomainAndRange Arg ) const;

private:
		
	CoeffT	m_Coefficient;
};



/****************************************************************
** Class	   : GsFuncMinusConst 
** Description : 
****************************************************************/

template< class DomainAndRange, class CoeffT >
class EXPORT_CLASS_GSFUNC GsFuncMinusConst : public GsFunc< DomainAndRange, DomainAndRange >
{
public:
	GsFuncMinusConst( CoeffT Coefficient );
	GsFuncMinusConst( const GsFuncMinusConst & );

	virtual ~GsFuncMinusConst();

	GsFuncMinusConst &operator=( const GsFuncMinusConst &rhs );

	virtual DomainAndRange operator()( DomainAndRange Arg ) const;

private:
		
	CoeffT	m_Coefficient;
};



/****************************************************************
** Class	   : GsFuncDivConst 
** Description : 
****************************************************************/

template< class DomainAndRange, class CoeffT >
class EXPORT_CLASS_GSFUNC GsFuncDivConst : public GsFunc< DomainAndRange, DomainAndRange >
{
public:
	GsFuncDivConst( CoeffT Coefficient );
	GsFuncDivConst( const GsFuncDivConst & );

	virtual ~GsFuncDivConst();

	GsFuncDivConst &operator=( const GsFuncDivConst &rhs );

	virtual DomainAndRange operator()( DomainAndRange Arg ) const;

private:
		
	CoeffT	m_Coefficient;
};



CC_END_NAMESPACE

#endif 
