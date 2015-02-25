/* $Header: /home/cvs/src/gob/src/gobarg.h,v 1.28 2011/05/17 18:28:59 khodod Exp $ */
#ifndef IN_GOBARG_H
#define IN_GOBARG_H

/****************************************************************
**
**	GOBARG.H	- Gob argument API (used by Gob and Ufo)
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.28 $
**
****************************************************************/

/*
 * Inheritance hierarchy
 *
 * GOB_ARG
 * |
 * +- ARG_PRED_MIXIN
 * |
 * +- ARG_EVAL_MIXIN
 * |  |               
 * |  +- ARG_INT_BASE (*)
 * |  |  |               
 * |  |  +- ARG_LITERAL (C) 
 * |  |  |  |            
 * |  |  |  +- ARG_VALUE (C)
 * |  |  |               
 * |  |  +- ARG_INT (C)
 * |  |                  
 * |  +- ARG_OR_DEFAULT (C)  
 * |  |  |               
 * |  |  +- ARG_OR_NULL (C)
 * |  |                  
 * |  +- ARG_CAST_ARG (*) (C)
 * | 
 * +- ARG_CONSTANT (C)
 * |  |
 * |  +- ARG_CONSTANT_COPY (C)
 * |
 * +- ARG_GOB_ERROR     (C)
 * +- ARG_DIDDLE_SCOPE  (C)
 * +- ARG_SELF_OBJ      (C)
 * +- ARG_SELF_VT       (C)
 * +- ARG_ELLIPSIS      (C)
 * +- ARG_ELLIPSIS_SIZE (C)
 *
 *  (C) - concrete class
 *  (*) - also inherits from ARG_PRED_MIXIN
 */

#include <gob.h>

// turn on some pragmas for sucky compilers
#ifdef WIN32
#pragma warning( disable : 4250 ) // yes, I want dominance, eat it
#endif

EXPORT_C_GOB GOB_ARG* GobErrorSingle();

/*
 * Simple smart pointer wrapper for pointer to GOB_ARGs
 *
 * Does not reference count everyhing, but instead clone()'s GOB_ARGs on copies.
 * Each GOB_ARG_HANDLE owns the GOB_ARG it has a pointer to and destroys the GOB_ARG when it is finished.
 */
class EXPORT_CLASS_GOB GOB_ARG_HANDLE
{
public:
    explicit GOB_ARG_HANDLE( GOB_ARG* ptr );     // non-converting constructor from GOB_ARG* type - does not throw
    GOB_ARG_HANDLE( GOB_ARG_HANDLE const& rhs ); // default copy constructor - may throw if clone() throws

    GOB_ARG_HANDLE();  // default constructor - does not throw
    ~GOB_ARG_HANDLE(); // default destructor - does not throw

    // Tests for equality - NOTE: two GOB_ARG's that are clones will not be equal.
    bool operator==(GOB_ARG_HANDLE const& rhs) const { return m_GobArg == rhs.m_GobArg;} // does not throw
    bool operator==(GOB_ARG        const* rhs) const { return m_GobArg == rhs;} // does not throw

    // Assignment
    GOB_ARG_HANDLE& operator=(GOB_ARG_HANDLE const& rhs); // clone()s the GOB_ARG of rhs - may throw if the clone() throws
	GOB_ARG_HANDLE& operator=(GOB_ARG* ptr); // shallow copy - takes ownership of ptr, does not clone() it. - does not throw

    // standard smart pointer operators
    GOB_ARG*       operator->()       { return m_GobArg;} // does not throw
    GOB_ARG const* operator->() const { return m_GobArg;} // does not throw

    GOB_ARG&       operator*()       { return *m_GobArg;} // does not throw
    GOB_ARG const& operator*() const { return *m_GobArg;} // does not throw

    // access the underlying GOB_ARG* (do not want operator GOB_ARG*() - see std::string class for reason)
    GOB_ARG*       val()       { return m_GobArg;} // does not throw
    GOB_ARG const* val() const { return m_GobArg;} // does not throw

    // releases ownership of the GOB_ARG
    GOB_ARG* release() { GOB_ARG* old_arg = m_GobArg; m_GobArg = 0; return old_arg;} // does not throw

    // reports if the pointer has a value (so if can be used as "if( MyHandle ) ..."
    operator bool() const { return m_GobArg != 0;} // does not throw

    // swap the GOB_ARGs of two handles.
    void swap(GOB_ARG_HANDLE& rhs); // does not throw

private:
	GOB_ARG* m_GobArg;
};

inline bool operator==(GOB_ARG const* lhs, GOB_ARG_HANDLE const& rhs) { return rhs == lhs;}

/*
**	The actual GobArg structure
*/

struct EXPORT_CLASS_GOB GOB_ARG
{
public:
   	// These must be the same as the constants defined for C projects in gob.h
	enum
	{
		PASS_ERRORS 	= 1,
		DYNAMIC			= 2,
		AS_VALUE_TYPE	= 4,
		INTERMEDIATE	= 8
	};

	enum Eval_Enum
	{
	    EVAL            = 0,
		NO_EVAL			= 1,
		NO_EVAL_CONTAGIOUS = 2
	};

    GOB_ARG() : m_Next( 0 ) {}
	// use default copy constructor

    virtual ~GOB_ARG() {}

    virtual GOB_ARG*
  	        clone() const = 0;

	virtual bool
			ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags ) = 0;

	virtual void
			AppendArg( GOB_ARG* Arg )  { GOB_ARG_HANDLE tmp( Arg ); }

	virtual void
			DbRefSet( GOB_ARG* DbRef ) { GOB_ARG_HANDLE tmp( DbRef ); } // pass cloned arg

	virtual GOB_ARG*
			ArgsGet() { return 0;}

	virtual void
  	        NoEvalSet( bool Contagious = false ) {}

	virtual Eval_Enum
	        NoEvalGet() const { return EVAL; }

    virtual void
 	        PredSet( GOB_ARG* Pred, bool) { GOB_ARG_HANDLE tmp( Pred );} // pass cloned arg

	virtual bool
			IsError() const;

	static GOB_ARG*
			ArgListV( va_list Marker );

    GOB_ARG const* NextArg() const { return m_Next.val(); }
    GOB_ARG*       NextArg()       { return m_Next.val(); }

	void	NextSet( GOB_ARG* Next ) { m_Next = Next; }

private:
	GOB_ARG_HANDLE
			m_Next;

	GOB_ARG& operator=(GOB_ARG const&); // unimplemented to disable unwanted copying
};

// NOT exported - used here only for convenience
class ARG_PRED_MIXIN : public virtual GOB_ARG
{
public:
    ARG_PRED_MIXIN()
	  : m_Pred( 0 ),
		m_PredTrue( true ) {}

	// use default copy constructor
	// use default destructor

    void    PredSet( GOB_ARG* Pred, bool PredTrue )
    {
  	    m_Pred = Pred;
		m_PredTrue = PredTrue;
	}

protected:
    GOB_ARG*	   PredGet()        { return m_Pred.val(); }
	GOB_ARG const* PredGet() const	{ return m_Pred.val(); }

    bool    GetPredTrue() const { return m_PredTrue; }

private:
    GOB_ARG_HANDLE
			m_Pred;
    bool	m_PredTrue;
};

// NOT exported - used here only for convenience
struct ARG_EVAL_MIXIN : public virtual GOB_ARG
{
    ARG_EVAL_MIXIN( Eval_Enum val ) : m_NoEval( val ) {}
    // default copy constructor
    // default destructor

    void    NoEvalSet( bool Contagious = false )
    {
	    m_NoEval = Contagious ? NO_EVAL_CONTAGIOUS : NO_EVAL;
	}

    Eval_Enum
	        NoEvalGet() const
    {
	    return m_NoEval;
	}

private:
    Eval_Enum
            m_NoEval;
};

struct EXPORT_CLASS_GOB ARG_INT_BASE : public ARG_EVAL_MIXIN, public ARG_PRED_MIXIN /* public virtual GOB_ARG */
{
	ARG_INT_BASE( Eval_Enum NoEval ) : ARG_EVAL_MIXIN( NoEval ), m_DbRef( 0 ), m_Args( 0 ) {}
	// use default copy constructor
    // use default destructor

	void	AppendArg( GOB_ARG* Arg );

	void	DbRefSet( GOB_ARG* DbRef );

	GOB_ARG* ArgsGet();

	bool	IsError() const;

protected:

	GOB_ARG_HANDLE
			m_DbRef,
			m_Args;

	short	AddWithArgsAndDb( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_LIST_ITEM* Item, int Flags );

	void	ArgsDelete()
	{
		if( m_Args.val() != GobArgError )
			m_Args = 0;
	}
};

struct EXPORT_CLASS_GOB ARG_LITERAL : public ARG_INT_BASE /* public virtual GOB_ARG */
{
	ARG_LITERAL( char const* ValueName, char const* ObjName, GOB_ARG* Args, Eval_Enum NoEval = EVAL )
	  : GOB_ARG(), ARG_INT_BASE( NoEval ),
		m_ValueName( ValueName ? strdup( ValueName ) : 0 ),
		m_ObjName  ( ObjName   ? strdup( ObjName   ) : 0 )
    {
	  m_Args = Args; // from ARG_INT_BASE
	}

    ARG_LITERAL(ARG_LITERAL const& rhs)
	  : GOB_ARG(rhs), ARG_INT_BASE(rhs),
		m_ValueName( rhs.m_ValueName ? strdup( rhs.m_ValueName ) : 0 ),
		m_ObjName  ( rhs.m_ObjName   ? strdup( rhs.m_ObjName   ) : 0 )
    {}

    ~ARG_LITERAL();

    GOB_ARG* clone() const { return new ARG_LITERAL(*this);}

	bool	ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT *Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );

protected:
	char	*m_ValueName,
			*m_ObjName;
};

struct EXPORT_CLASS_GOB ARG_VALUE : public ARG_LITERAL /* public virtual GOB_ARG */
{
	ARG_VALUE( char const* ValueName, char const* ObjectValueName, GOB_ARG* Args, Eval_Enum NoEval = EVAL )
	  : ARG_LITERAL( ValueName, ObjectValueName, Args, NoEval )
	{}

    // use default copy constructor

    GOB_ARG* clone() const { return new ARG_VALUE(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );
};

struct EXPORT_CLASS_GOB ARG_INT : public ARG_INT_BASE /* public virtual GOB_ARG */
{
	ARG_INT( GOB_ARG* ValueRef, GOB_ARG* ObjectRef, GOB_ARG* Args, GOB_ARG* DbRef, Eval_Enum NoEval = EVAL )
	  : ARG_INT_BASE( NoEval ),
		m_ValueRef( ValueRef ),
		m_ObjectRef( ObjectRef )
	{
	  m_Args	= Args;  // from ARG_INT_BASE
	  m_DbRef 	= DbRef; // from ARG_INT_BASE
	}

    // use default copy constructor
	// use default destructor

    GOB_ARG* clone() const { return new ARG_INT(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );

	bool    IsError() const;

protected:
	GOB_ARG_HANDLE
			m_ValueRef,
			m_ObjectRef;
};

struct EXPORT_CLASS_GOB ARG_OR_DEFAULT : public ARG_EVAL_MIXIN /* public virtual GOB_ARG */
{
	ARG_OR_DEFAULT( int ArgNum ) : ARG_EVAL_MIXIN( EVAL ), m_ArgNum( ArgNum ) {}
    // use default copy constructor
    // use default destructor

    GOB_ARG* clone() const { return new ARG_OR_DEFAULT(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );

protected:
    int		m_ArgNum;
};

struct EXPORT_CLASS_GOB ARG_OR_NULL : public ARG_OR_DEFAULT /* public virtual GOB_ARG */
{
	ARG_OR_NULL( int ArgNum ) : ARG_OR_DEFAULT( ArgNum ) {}
	// use default copy constructor
	// use default destructor

	GOB_ARG* clone() const { return new ARG_OR_NULL(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );
};

struct EXPORT_CLASS_GOB ARG_CAST_ARG : public ARG_EVAL_MIXIN, public ARG_PRED_MIXIN /* public virtual GOB_ARG */
{
	ARG_CAST_ARG( GOB_ARG* AnyRef, DT_DATA_TYPE DataType, Eval_Enum NoEval = EVAL )
	  : ARG_EVAL_MIXIN( NoEval ),
		m_AnyRef( AnyRef ),
		m_DataType( DataType )
	{
	}

    // use default copy constructor
	// use default destructor

    GOB_ARG* clone() const { return new ARG_CAST_ARG(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );

	bool    IsError() const;

protected:
	GOB_ARG_HANDLE
			m_AnyRef;

	DT_DATA_TYPE
			m_DataType;
};

struct EXPORT_CLASS_GOB ARG_CONSTANT : public virtual GOB_ARG
{
	ARG_CONSTANT( DT_VALUE *Value )
	{
		m_Constant = *Value;
	}
	ARG_CONSTANT( DT_DATA_TYPE DataType, double Number )
	{
		m_Constant.DataType = DataType;
		m_Constant.Data.Number = Number;
	}
	ARG_CONSTANT( DT_DATA_TYPE DataType, void const* Pointer )
	{
		m_Constant.DataType = DataType;
		m_Constant.Data.Number = 0.0;		// to make Purify happy
		m_Constant.Data.Pointer = Pointer;
	}

	// use default copy constructor ARG_CONSTANT(ARG_CONSTANT const& rhs) : GOB_ARG(rhs) { DTM_ASSIGN( &m_Constant, &rhs.m_Constant ); }
    // use default destructor

    GOB_ARG* clone() const { return new ARG_CONSTANT(*this);}

	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );

protected:
    ARG_CONSTANT()
	{}

	DT_VALUE	m_Constant;
};

struct EXPORT_CLASS_GOB ARG_CONSTANT_COPY : public ARG_CONSTANT /* public virtual GOB_ARG */
{
	ARG_CONSTANT_COPY( DT_VALUE *Value ) : ARG_CONSTANT()
	{
		DTM_ASSIGN( &m_Constant, Value );
	}
	ARG_CONSTANT_COPY( DT_DATA_TYPE DataType, double Number )
	{
		DT_VALUE TmpVal;
		TmpVal.DataType = DataType;
		TmpVal.Data.Number = Number;
		DTM_ASSIGN( &m_Constant, &TmpVal );
	}
	ARG_CONSTANT_COPY( DT_DATA_TYPE DataType, void const* Pointer )
	{
		DT_VALUE TmpVal;
		TmpVal.DataType = DataType;
		TmpVal.Data.Number = 0.0;		// to make Purify happy
		TmpVal.Data.Pointer = Pointer;
		DTM_ASSIGN( &m_Constant, &TmpVal );
	}
	ARG_CONSTANT_COPY(ARG_CONSTANT_COPY const& rhs) : GOB_ARG(rhs), ARG_CONSTANT() { DTM_ASSIGN( &m_Constant, const_cast<DT_VALUE*>(&rhs.m_Constant) ); }
    ~ARG_CONSTANT_COPY();

    GOB_ARG* clone() const { return new ARG_CONSTANT_COPY(*this);}
};

// singleton
struct ARG_GOB_ERROR : public virtual GOB_ARG
{
	GOB_ARG* clone() const { return GobErrorSingle(); }

	bool ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags )
	{
		return false;
	}

	bool IsError() const { return true;}

	friend GOB_ARG* GobErrorSingle(); // could make this a static member function, but gcc complains about having no friends.

private:
	ARG_GOB_ERROR() : GOB_ARG() {}
	ARG_GOB_ERROR(ARG_GOB_ERROR const&); // unimplemented
};

#define SIMPLE_GOB_ARG( _name_ )																													\
struct EXPORT_CLASS_GOB _name_ : public virtual GOB_ARG																								\
{																																					\
    /* use default constructor      */																												\
    /* use default copy constructor */																												\
    /* use default destructor       */																												\
																																					\
    GOB_ARG* clone() const { return new _name_ (*this);}																							\
	bool    ChildRef( SDB_M_GET_CHILD_LIST* ChildList, SDB_CHILD_COMPONENT* Child, SDB_CHILD_ITEM_TYPE OuterType, char const* Name, int Flags );	\
}

SIMPLE_GOB_ARG( ARG_DIDDLE_SCOPE );
SIMPLE_GOB_ARG( ARG_SELF_OBJ );
SIMPLE_GOB_ARG( ARG_SELF_VT );
SIMPLE_GOB_ARG( ARG_ELLIPSIS );
SIMPLE_GOB_ARG( ARG_ELLIPSIS_SIZE );
#undef SIMPLE_GOB_ARG

#endif
