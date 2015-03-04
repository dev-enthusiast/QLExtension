/****************************************************************
**
**	LTSECDBOBJ.H	- LtSecDbObj class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/LtSecDbObj.h,v 1.7 2001/11/27 23:25:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDB_LTSECDBOBJ_H )
#define IN_SECDB_LTSECDBOBJ_H

#include <secdb.h>
#include <gscore/GsDate.h>
#include <gscore/gsdt_fwd.h>
#include <secdb/LtDtArgs.h>
#include <gsdt/Enums.h>

CC_BEGIN_NAMESPACE( Gs )


class EXPORT_CLASS_SECDB LtSecDbObj
{

public:
	typedef GsDtGeneric<LtSecDbObj> GsDtType;

	inline static const char* typeName() { return "LtSecDbObj"; }
	GsString toString() const;

	LtSecDbObj();
	LtSecDbObj( SDB_OBJECT* SecDbObject ); // We are stealing this pointer
	LtSecDbObj( const LtSecDbObj& Rhs );
	virtual ~LtSecDbObj();

	LtSecDbObj& operator=( const LtSecDbObj& Rhs );

	virtual LtSecDbObj* copy() const { return new LtSecDbObj( *this ); }

    static SDB_DB* SecDbRootDb();

	static LtSecDbObj getByName( const GsString& SecurityName, SDB_DB* Db = SecDbRootDb(),
								  unsigned Flags = 0 );

	static SDB_OBJECT* secDbGetByName(const GsString& SecurityName, SDB_DB* Db = SecDbRootDb(),
									  unsigned Flags = 0 );

	static LtSecDbObj securityNew( const GsString& SecurityType, const GsString& SecurityName = "",
									SDB_DB* Db = SecDbRootDb() );

	inline SDB_OBJECT* ptr() 		const { return m_SecDbObject; }
	inline operator SDB_OBJECT*()  	const { return m_SecDbObject; }
	inline SDB_OBJECT* operator->() const { return m_SecDbObject; }

	inline bool hasSecurity() const { return m_SecDbObject ? true : false; }

	static SDB_VALUE_TYPE valueTypeFromTypeName( const GsString& ValueName, DT_DATA_TYPE DataType = NULL );

	inline DT_VALUE* getValue( const GsString& ValueName, DT_DATA_TYPE DataType = NULL, LT_DT_ARGS_DECLARE ) const
        { return getValue( m_SecDbObject, ValueName, DataType, LT_DT_ARGS ); }
	inline DT_VALUE* getValue( SDB_VALUE_TYPE ValueType, LT_DT_ARGS_DECLARE ) const
        { return getValue( m_SecDbObject, ValueType, LT_DT_ARGS ); }
	static DT_VALUE* getValue( SDB_OBJECT* SecDbObject, const GsString& ValueName, DT_DATA_TYPE DataType = NULL, LT_DT_ARGS_DECLARE );
	static DT_VALUE* getValue( SDB_OBJECT* SecDbObject, SDB_VALUE_TYPE ValueType, LT_DT_ARGS_DECLARE );

	inline void setValue( const GsString& ValueName, DT_VALUE* Value, LT_DT_ARGS_DECLARE )
		{ setValue( m_SecDbObject, ValueName, Value, LT_DT_ARGS ); }	
	inline void setValue( SDB_VALUE_TYPE ValueType, DT_VALUE* Value, LT_DT_ARGS_DECLARE )
	    { setValue( m_SecDbObject, ValueType, Value, LT_DT_ARGS ); }
	static void setValue( SDB_OBJECT* SecDbObject, const GsString& ValueName, DT_VALUE*, LT_DT_ARGS_DECLARE );
	static void setValue( SDB_OBJECT* SecDbObject, SDB_VALUE_TYPE ValueType, DT_VALUE*, LT_DT_ARGS_DECLARE );

	// FIX FIX FIX - setDiddle and removeDiddle are not really const... FIX FIX FIX

	inline SDB_DIDDLE_ID setDiddle( const GsString& ValueName, DT_VALUE* Value, LT_DT_ARGS_DECLARE ) 
		{ return setDiddle( m_SecDbObject, ValueName, Value, LT_DT_ARGS ); }
	inline SDB_DIDDLE_ID setDiddle( SDB_VALUE_TYPE ValueType, DT_VALUE* Value, LT_DT_ARGS_DECLARE ) 
	    { return setDiddle( m_SecDbObject, ValueType, Value, LT_DT_ARGS ); }
	static SDB_DIDDLE_ID setDiddle( SDB_OBJECT* SecDbObject, const GsString& ValueName, DT_VALUE*, LT_DT_ARGS_DECLARE );
	static SDB_DIDDLE_ID setDiddle( SDB_OBJECT* SecDbObject, SDB_VALUE_TYPE ValueType, DT_VALUE*, LT_DT_ARGS_DECLARE );

	inline void removeDiddle( const GsString& ValueName, DT_VALUE* Value, SDB_DIDDLE_ID DiddleId, LT_DT_ARGS_DECLARE ) 
		{ removeDiddle( m_SecDbObject, ValueName, Value, DiddleId, LT_DT_ARGS ); }	
	inline void removeDiddle( SDB_VALUE_TYPE ValueType, DT_VALUE* Value, SDB_DIDDLE_ID DiddleId, LT_DT_ARGS_DECLARE ) 
	    { removeDiddle( m_SecDbObject, ValueType, DiddleId, LT_DT_ARGS ); }
	static void removeDiddle( SDB_OBJECT* SecDbObject, const GsString& ValueName, DT_VALUE*, SDB_DIDDLE_ID DiddleId, LT_DT_ARGS_DECLARE );
	static void removeDiddle( SDB_OBJECT* SecDbObject, SDB_VALUE_TYPE ValueType, SDB_DIDDLE_ID DiddleId, LT_DT_ARGS_DECLARE );

	inline double getDouble( const GsString& ValueName, LT_DT_ARGS_DECLARE ) const
        { return getDouble( m_SecDbObject, ValueName, LT_DT_ARGS ); }
	inline GsString getString( const GsString& ValueName, LT_DT_ARGS_DECLARE  ) const
        { return getString( m_SecDbObject, ValueName, LT_DT_ARGS ); }
	inline GsDate getDate( const GsString& ValueName, LT_DT_ARGS_DECLARE  ) const
        { return getDate( m_SecDbObject, ValueName, LT_DT_ARGS ); }
	inline const GsDt* getGsDt( const GsString& ValueName, LT_DT_ARGS_DECLARE ) const
        { return getGsDt( m_SecDbObject, ValueName, LT_DT_ARGS ); }
	inline GsDtArray* getArray( const GsString& ValueName, LT_DT_ARGS_DECLARE ) const
        { return getArray( m_SecDbObject, ValueName, LT_DT_ARGS ); }
	inline GsDtDictionary* getDictionary( const GsString& ValueName, LT_DT_ARGS_DECLARE ) const
        { return getDictionary( m_SecDbObject, ValueName, LT_DT_ARGS ); }

	// Get any type that happens to be there and try to convert to a string.
	inline GsString getAsString( const GsString& ValueName, LT_DT_ARGS_DECLARE  ) const
        { return getAsString( m_SecDbObject, ValueName, LT_DT_ARGS ); }

	static double 			getDouble( 		SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE );
	static GsString 		getString( 		SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE );
	static GsDate 			getDate( 		SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE );
	static const GsDt* 		getGsDt( 		SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE );
	static GsDtArray*		getArray( 		SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE ); 
	static GsDtDictionary*	getDictionary( 	SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE ); 
	static GsString 		getAsString( 	SDB_OBJECT* SecDbObject, const GsString& ValueName, LT_DT_ARGS_DECLARE );	

private:

	SDB_OBJECT*
			m_SecDbObject;
};

CC_END_NAMESPACE

#endif 

