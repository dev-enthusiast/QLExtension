/****************************************************************
**
**	GSSTREAMMAP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsStreamMap.h,v 1.6 2004/11/11 17:58:50 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSSTREAMMAP_H )
#define IN_GSBASE_GSSTREAMMAP_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsException.h>
CC_BEGIN_NAMESPACE( Gs )


typedef uintptr_t GsStreamMapId;

/****************************************************************
** Class	   : GsStreamMap 
** Description : Abstract base class 
****************************************************************/

class EXPORT_CLASS_GSBASE GsStreamMap
{
private:
	static void loadGsStDatFile();
protected:
	GsStreamMap();
	GsStreamMap( const GsStreamMap & );

	GsStreamMap &operator=( const GsStreamMap &rhs );

	GsStreamMapId 
			m_Id;

	static GsFactory* findFactory( const GsString& );

protected:
	virtual void doStreamStore( GsStreamOut& Stream ) = 0;
	virtual void doStreamRead( GsStreamIn& Stream ) = 0;

public:
	virtual ~GsStreamMap() {}

	GsStreamMapId id() const;
	void setId( GsStreamMapId );

	void StreamStore( GsStreamOut& Stream );
	void StreamRead( GsStreamIn& Stream );
	virtual GsStreamMap* clone( const void* = NULL ) const = 0;
	virtual void* getInObject() const = 0;
	static void add( const GsString& type, GsStreamMap* Clone );
	static GsStreamMap* find( const GsString& );
	static GsStreamMap* find( GsStreamIn& );
};

template < class T > 
class GsStreamMapT : public GsStreamMap
{
protected:
	const T* m_Out;
	T* m_In;

	GsStreamMapT< T > &operator=( const GsStreamMapT< T > &Rhs );

	virtual void doStreamStore( GsStreamOut& Stream );
	virtual void doStreamRead( GsStreamIn& Stream );

public:
	GsStreamMapT();
	GsStreamMapT( const GsStreamMapT< T > & Other, const void* Data  = NULL );

	virtual GsStreamMap* clone( const void* Data = NULL ) const;
	virtual void* getInObject() const;
	static const char* typeName();
};

CC_END_NAMESPACE

#endif 


