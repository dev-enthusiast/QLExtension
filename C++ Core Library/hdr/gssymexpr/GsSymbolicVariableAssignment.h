/****************************************************************
**
**	GSSYMBOLICVARIABLEASSIGNMENT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicVariableAssignment.h,v 1.13 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICVARIABLEASSIGNMENT_H )
#define IN_GSSYMEXPR_GSSYMBOLICVARIABLEASSIGNMENT_H

#include <gssymexpr/base.h>
#include <gscore/GsRefCount.h>
#include <gssymexpr/GsHashFuncSymbolicVariable.h>
#include <gscore/GsHash.h>
#include <gscore/GsHandle.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsSymbolicVariableAssignmentRep 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableAssignmentRep : public GsRefCountCopyOnWrite
{
public:
	GsSymbolicVariableAssignmentRep();
	GsSymbolicVariableAssignmentRep( const GsSymbolicVariableAssignmentRep & );

	~GsSymbolicVariableAssignmentRep();

	GsSymbolicVariableAssignmentRep &operator=( const GsSymbolicVariableAssignmentRep &rhs );		

	GsSymbolicVariableAssignmentRep * copy() const;

private:
	typedef GsHashMap< GsSymbolicVariableNameAndArgs, double, GsHashFuncSymbolicVariable, CC_NS(std,equal_to)< GsSymbolicVariableNameAndArgs > > HashMapType;

public:
	typedef HashMapType::ConstEntry 	const_entry;
	typedef HashMapType::Entry 			entry;
	typedef HashMapType::ConstIterator 	const_iterator;
	typedef HashMapType::Iterator 		iterator;
		
	const_iterator begin() const { return m_Hash.Begin();	}
	const_iterator end()   const { return m_Hash.End();		}

	iterator begin() { return m_Hash.Begin();	}
	iterator end()   { return m_Hash.End();		}

	bool operator==( const GsSymbolicVariableAssignmentRep &rhs ) const;

	size_t hash() const;

private:

	// Stuff to pass on to the hash map.

	void Insert( const GsSymbolicVariableNameAndArgs &NameAndArgs, double Value )
	{
		m_Hash.Insert( NameAndArgs, Value );
	}

	const_entry Lookup( const GsSymbolicVariableNameAndArgs &NameAndArgs ) const
	{
		return m_Hash.Lookup( NameAndArgs );
	}

	entry Lookup( const GsSymbolicVariableNameAndArgs &NameAndArgs )
	{
		return m_Hash.Lookup( NameAndArgs );
	}

	GsString toString() const;

	HashMapType
			m_Hash;

	friend class GsSymbolicVariableAssignment;
};



/****************************************************************
** Class	   : GsSymbolicVariableAssignment 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableAssignment : public GsHandleCopyOnWrite< GsSymbolicVariableAssignmentRep >
{
public:

	class Iterator;

	GsSymbolicVariableAssignment();
	GsSymbolicVariableAssignment( GsSymbolicVariableAssignmentRep * );
	GsSymbolicVariableAssignment( const GsSymbolicVariableAssignment & );

	~GsSymbolicVariableAssignment();

	GsSymbolicVariableAssignment &operator=( const GsSymbolicVariableAssignment &rhs );		

	bool operator==( const GsSymbolicVariableAssignment &rhs ) const { return ( m_rep == rhs.m_rep ) || ( *m_rep == *(rhs.m_rep) ); }

	typedef GsSymbolicVariableAssignmentRep::const_iterator const_iterator;
	typedef GsSymbolicVariableAssignmentRep::iterator 		iterator;
	typedef GsSymbolicVariableAssignmentRep::const_entry    const_entry;
	typedef GsSymbolicVariableAssignmentRep::entry    		entry;

	const_iterator begin() const { return m_rep->begin();	}
	const_iterator end()   const { return m_rep->end(); 	}

	iterator begin() { singleShareableRep(); return m_rep->begin();	}
	iterator end()   { singleShareableRep(); return m_rep->end(); 	}

	size_t hash() const { return m_rep->hash(); }

	// Stuff to pass on to the rep.

	void Insert( const GsSymbolicVariableNameAndArgs &NameAndArgs, double Value )
	{
		singleShareableRep(); 
		m_rep->Insert( NameAndArgs, Value );
	}

	const_entry Lookup( const GsSymbolicVariableNameAndArgs &NameAndArgs ) const
	{
		return m_rep->Lookup( NameAndArgs );
	}

	entry Lookup( const GsSymbolicVariableNameAndArgs &NameAndArgs )
	{
		singleShareableRep(); 
		return m_rep->Lookup( NameAndArgs );
	}

	size_t size() const;

	// GsDt stuff

	typedef class GsDtSymbolicVariableAssignment GsDtType;
	GsString toString() const;
	static GsString typeName() { return GsString( "GsSymbolicVariableAssignment" ); }

private:

	friend class GsDtSymbolicVariableAssignment;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};



CC_END_NAMESPACE

#endif 
