/****************************************************************
**
**	GSDTTIME.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtTime.h,v 1.8 2001/11/27 22:44:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTTIME_H )
#define IN_GSDT_GSDTTIME_H

#include	<gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtTime;


class EXPORT_CLASS_GSDT GsDtTime : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtTime )
public:

	typedef double imp_type;
		
	GsDtTime();
	GsDtTime( const GsDtTime &Obj );
	GsDtTime &operator=( const GsDtTime &Obj );

	virtual ~GsDtTime();

	virtual GsDt*		highLimit() const;
	virtual GsDt*		lowLimit() const;
	virtual GsDt*		errorSentinel() const;

	virtual GsDt*		CopyDeep() const;
	virtual GsDt*		CopyShallow() const;

	virtual size_t		GetSize() const;
	virtual size_t		GetCapacity() const;
	virtual size_t		GetMemUsage() const;
	virtual void		GetInfo( FcDataTypeInstanceInfo &Info ) const;

	virtual void		StreamStore( GsStreamOut &Stream ) const;
	virtual void		StreamRead(  GsStreamIn  &Stream );

	virtual FcHashCode	HashComplete() const;
	virtual FcHashCode  HashQuick() const;

	virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );
	virtual int			CompareSame( const GsDt &Other ) const;

	virtual GsBool		Truth() const;
	virtual GsBool		IsValid() const;

	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsString	toString() const;

private:
	imp_type m_data;
public:
	GsDtTime( imp_type* time, GsCopyAction action );
	GsDtTime( const imp_type& time );
	const imp_type& data() const { return m_data; }
	      imp_type& data()       { return m_data; }
	void	dataSet( const imp_type& time ) { m_data = time; }

};

CC_END_NAMESPACE

#endif
