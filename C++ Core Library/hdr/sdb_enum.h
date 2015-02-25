/****************************************************************
**
**	SDB_ENUM.H	- SecDb Enum definitions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_enum.h,v 1.12 2001/09/27 20:55:20 singhki Exp $
**
****************************************************************/

#if !defined( IN_SECDB_SDB_ENUM_H )
#define IN_SECDB_SDB_ENUM_H

#include <sdb_base.h>
#include <secnodei.h>

struct EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER( SDB_CHILD_ENUM_FILTER *NextFilter )
	  : m_NextFilter( NextFilter )
	{}

	virtual ~SDB_CHILD_ENUM_FILTER();

	// This is designed to be called for Terminals only
	virtual bool
			Match( SDB_NODE *Node ) = 0;

	virtual bool
			Recurse( SDB_NODE *Node, bool Matched );

protected:
	SDB_CHILD_ENUM_FILTER
			*m_NextFilter;
};

class EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER_FLAGS : public SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER_FLAGS( unsigned Flags, SDB_CHILD_ENUM_FILTER *NextFilter )
	: SDB_CHILD_ENUM_FILTER( NextFilter ),
	  m_Flags( Flags ),
	  m_MatchExternalAndExternalVt( stricmp( SecDbConfigurationGet( "SECDB_FOR_CHILDREN_USE_EXTERNAL_VT", 0, 0, "True"), "True" ) == 0 )
	{
	}

    // use default destructor

	bool	Match( SDB_NODE *Node );
	bool	Recurse( SDB_NODE *Node, bool Matched );

protected:
	SDB_VALUE_TYPE
			ExternalVtFromVt( SDB_VALUE_TYPE Vt );

	bool	MatchExternalVt( SDB_NODE *Node )
	{
		// This must be called only for terminals else it will break

		ERR_OFF();						// we don't really care if this
		SDB_VALUE_TYPE					// next operation fails
				ExternalVt = ExternalVtFromVt( (SDB_VALUE_TYPE) Node->ValueTypeGet()->AccessValue()->Data.Pointer );
		ERR_ON();
	
		if( !ExternalVt )
			return false;

		DT_VALUE const* const Value = SecDbGetValue( (SDB_OBJECT*) Node->ObjectGet()->AccessValue()->Data.Pointer, ExternalVt);

		if( !Value || Value->DataType != DtDouble )
			return false;

		return Value->Data.Number != 0.;
	}

	unsigned
			m_Flags;

	bool	m_MatchExternalAndExternalVt;
};

class EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER_SEC_TYPE : public SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER_SEC_TYPE( SDB_SEC_TYPE SecType, SDB_CHILD_ENUM_FILTER *NextFilter )
	: SDB_CHILD_ENUM_FILTER( NextFilter ),
	  m_SecType( SecType )
	{}
    // use default destructor

	bool    Match( SDB_NODE *Node );
	// use default Recurse

protected:
	SDB_SEC_TYPE
			m_SecType;
};

class EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER_VALUE_TYPE : public SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER_VALUE_TYPE( SDB_VALUE_TYPE ValueType, SDB_CHILD_ENUM_FILTER *NextFilter )
	: SDB_CHILD_ENUM_FILTER( NextFilter ),
	  m_ValueType( ValueType )
	{}
    // use default destructor

	bool    Match( SDB_NODE *Node );
    // use default Match

protected:
	SDB_VALUE_TYPE
			m_ValueType;
};

class EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER_SELF : public SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER_SELF( SDB_OBJECT *SecPtr, SDB_CHILD_ENUM_FILTER *NextFilter )
	: SDB_CHILD_ENUM_FILTER( NextFilter ),
	  m_SecPtr( SecPtr )
	{}
    // use default destructor

	bool    Match( SDB_NODE *Node );
    bool    Recurse( SDB_NODE *Node, bool Matched );

protected:
	SDB_OBJECT
			*m_SecPtr;
};

class EXPORT_CLASS_SECDB SDB_CHILD_ENUM_FILTER_NONE : public SDB_CHILD_ENUM_FILTER
{
public:
	SDB_CHILD_ENUM_FILTER_NONE( SDB_CHILD_ENUM_FILTER *NextFilter )
	: SDB_CHILD_ENUM_FILTER( NextFilter )
	{}
    // use default destructor

	bool    Match( SDB_NODE *Node );
    bool    Recurse( SDB_NODE *Node, bool Matched );

protected:
	SDB_OBJECT
			*m_SecPtr;
};

#endif 
