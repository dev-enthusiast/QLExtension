//uncomment this on order to force member functions of SDB_NODE to not be inlined
//#define SDB_NODE_NO_INLINE
/****************************************************************
**
**	SECNODEI.H	- SDB_NODE definition ( Internal header file )
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secnodei.h,v 1.114 2014/07/16 03:36:41 khodod Exp $
**
****************************************************************/

#ifndef IN_SECNODEI_H
#define IN_SECNODEI_H

#if defined( SDB_NODE_NO_INLINE )
#define SDB_NODE_INLINE
#else
#define SDB_NODE_INLINE inline
#endif

#include <sdbdbset.h>
#include <secdt.h>
#include <secerror.h>
#include <sectrace.h>
#include <skipliko.h>
#include <skiplist.h>
#include <heap.h>
#include <hash.h>

#include <extension/auto_ptr_array.h>

#include <ccstl.h>
#include <map> // for std::map and std::multimap
#include <set> // for std::set
#include <utility> // for std::pair
#include <string> // for std::string

/*
**  Forward declare structures
*/

struct SLANG_SCOPE;
struct SDB_M_DIDDLE;
class SDB_NODE;

struct SDB_DIDDLE;
struct SDB_DIDDLE_HEADER;

/*
**	SDB_NODE constants
*/

int const
		SDB_NODE_MAX_ARGS   = USHRT_MAX;

// 132K max on the number of children for now to help 
// with very large books with a large number of leaves.
// Making this larger seems very scary for performance
// and memory-usage reasons.

unsigned const
		SDB_NODE_CHILDC_MAX = 0x1FFFFU;  

unsigned const
		MIN_PARENTC_FOR_LRU_POINTERS = 3;

/*
**	Values for Node->NodeType
*/

unsigned const
        SDB_NODE_TYPE_UNKNOWN			     = 0,		// NOTE: Must be zero
        SDB_NODE_TYPE_TERMINAL			     = 1,
        SDB_NODE_TYPE_PASSTHRU			     = 2,
                                            // 3,4 retired
        SDB_NODE_TYPE_ARRAY				     = 5,
        SDB_NODE_TYPE_VALUE_ARRAY			 = 6,
        SDB_NODE_TYPE_EACH					 = 7,
        SDB_NODE_TYPE_REF_COUNTED_LITERAL	 = 8,
        SDB_NODE_TYPE_PARENTED_LITERAL 		 = 9;

// For debugging -- most Node routines will treat the following like NULL
SDB_NODE* const SDB_NODE_INVALID_PTR	     = reinterpret_cast<SDB_NODE*>( 0xBAD1BAD0 );

// In addition to above, the CacheCheckSanity routine will use the following to indicate errors
SDB_NODE* const SDB_NODE_CHILD_DISCONNECTED  = reinterpret_cast<SDB_NODE*>( 0xBAD1BAD4 );
SDB_NODE* const SDB_NODE_PARENT_DISCONNECTED = reinterpret_cast<SDB_NODE*>( 0xBAD1BAD8 );

     int* const SDB_NODE_DESTROY_UNDO        = reinterpret_cast<int*>( 0xF00DBAD1 );



/*
** GetValue context
*/

struct SDB_NODE_CONTEXT
{
	SDB_NODE_CONTEXT(): ErrNoted(0) {}
	unsigned
			ErrNoted;
};

struct SDB_VECTOR_CONTEXT
{
	int		InUse,	// True if context is already in use
			Index,
			Size;

	SDB_VECTOR_CONTEXT
			*Next;

};


/*
**	Wrapper around a SDB_NODE * which uses the lower bit for the Purple link flag
*/

class SDB_NODE_PARENT_PTR
{
public:
	SDB_NODE_PARENT_PTR( SDB_NODE *Node )
	{
		m_Ptr = (long) Node;
	}

	operator SDB_NODE *() const
	{
		return this->Node();
	}

	SDB_NODE *Node() const
	{
		return (SDB_NODE *) ( m_Ptr & ~1 );
	}

	SDB_NODE *PtrValue() const
	{
		return (SDB_NODE *) m_Ptr;
	}

	int IsPurple() const
	{
		return m_Ptr & 1;
	}

	SDB_NODE *SetPurple()
	{
		return (SDB_NODE *) ( m_Ptr | 1 );
	}

private:
	long	m_Ptr;
};


/*
**	Wrapper around a SDB_NODE * which uses the lower bit for the Purple link flag
*/

class SDB_NODE_CHILD_PTR
{
public:
	SDB_NODE_CHILD_PTR( SDB_NODE *Node )
	{
		m_Ptr = (long) Node;
	}

	operator SDB_NODE *() const
	{
		return this->Node();
	}

	SDB_NODE* operator->() const
	{
		return this->Node();
	}

	SDB_NODE *Node() const
	{
		return (SDB_NODE *) ( m_Ptr & ~1 );
	}

	SDB_NODE *PtrValue() const
	{
		return (SDB_NODE *) m_Ptr;
	}

	int IsPurple() const
	{
		return m_Ptr & 1;
	}

	SDB_NODE *SetPurple()
	{
		return (SDB_NODE *) ( m_Ptr | 1 );
	}

	SDB_NODE* operator=( SDB_NODE* n )
	{
		m_Ptr = (long) n;
		return n;
	}

private:
	long	m_Ptr;
};


/*
**	Constructs a purple parent link from a SDB_NODE *
*/

inline SDB_NODE *SDB_NODE_PURPLE_PARENT( SDB_NODE *Node )
{
	return (SDB_NODE *) ( (long) Node | 1 );
}


/*
**	Converts a purple parent link to an ordinary node
*/

inline SDB_NODE *SDB_NODE_FROM_PURPLE_PARENT( SDB_NODE *Node )
{
	return (SDB_NODE *) ( (long) Node & ~1 );
}


/*
**	Is this node pointer a purple link?
*/

inline int SDB_NODE_IS_PURPLE_PARENT( SDB_NODE *Node )
{
	return ( (long) Node & 1 );
}


/*
**	Constructs a purple child link from a SDB_NODE *
*/

inline SDB_NODE *SDB_NODE_PURPLE_CHILD( SDB_NODE *Node )
{
	return (SDB_NODE *) ( (long) Node | 1 );
}


/*
**	Converts a purple child link to an ordinary node
*/

inline SDB_NODE *SDB_NODE_FROM_PURPLE_CHILD( SDB_NODE *Node )
{
	return (SDB_NODE *) ( (long) Node & ~1 );
}


/*
**	Is this node pointer a purple link?
*/

inline int SDB_NODE_IS_PURPLE_CHILD( SDB_NODE *Node )
{
	return ( (long) Node & 1 );
}


/*
**	LRUPointers stored in parent list when Node is on LRU Cache
*/

struct NODE_LRU_POINTERS
{
	int		ParentIndex;

	SDB_NODE
			*Prev,
			*Next;
};

/*
**	An SDB_NODE
*/

class EXPORT_CLASS_SECDB SDB_NODE
{
public:
    class SaveAutoGetNode;
	class PushAutoGetNode;

	struct node_less
	{
		bool operator()( SDB_NODE const* const p, SDB_NODE const* const q ) const
		{
			long t;
            
#define CMP(ptr1, ptr2)                                             \
               {                                                    \
                   unsigned long val1 = (unsigned long) ptr1;       \
                   unsigned long val2 = (unsigned long) ptr2;       \
                                                                    \
                   if( val1 > val2 ) return false;                  \
                   else if( val1 < val2) return true;               \
               }                                                    

            CMP( p->ObjectGet(), q->ObjectGet() );
            CMP( p->ValueTypeGet(), q->ValueTypeGet() );
			
            t = p->ArgcGet() - q->ArgcGet();
			if( t < 0 )
				return true;
			if( t > 0 )
				return false;

			for( int arg = 0; arg < p->ArgcGet(); ++arg )
                CMP( p->ArgGet( arg ), q->ArgGet( arg ) );
			
#undef CMP
			return false;
		}
	};

	struct node_compare
	{
		static int compare( void const* Key1, void const* Key2 ) { return SDB_NODE::Compare( Key1, Key2 ); }
	};

	typedef CC_STL_SET_WITH_COMP( SDB_NODE*, node_less ) 	            NodeSet;
	typedef CC_STL_MAP_WITH_COMP( SDB_NODE*, NodeSet, node_less ) 		AutoChildrenMap;

	typedef CC_NS(std,pair)<SDB_NODE*,NodeSet*>					SideEffectDiddleInfo;
	typedef CC_STL_MAP( SDB_DIDDLE_ID, SideEffectDiddleInfo )	SideEffectDiddleMap;

	class NODE_STATS
	{
	public:
		NODE_STATS() : m_MemUsed( 0. ), m_TotalMemUsed( 0. ), m_CpuTime( 0. ), m_TotalCpuTime( 0. ), m_Count( 0 ) {};

        double   MemUsedGet()                        const { return m_MemUsed;                               }
        double   TotalMemUsedGet()                   const { return m_TotalMemUsed;                          }
        double   AverageMemUsedGet()                 const { return m_Count ? m_TotalMemUsed / m_Count : 0.; }
        double   CpuTimeGet()                        const { return m_CpuTime;                               }
        double   TotalCpuTimeGet()                   const { return m_TotalCpuTime;                          }
        double   AverageCpuTimeGet()                 const { return m_Count ? m_TotalCpuTime / m_Count : 0.; }
        unsigned StatsCountGet()                     const { return m_Count;                                 }

		void     Add( double mem, double cpu_time )
		{
			++m_Count;
			m_MemUsed += mem;
			m_CpuTime += cpu_time;
		}

		void	 Reset()
		{
			m_CpuTime = m_TotalCpuTime = m_MemUsed = m_TotalMemUsed = 0.;
			m_Count = 0;
		}

	private:
		double	m_MemUsed,
				m_TotalMemUsed,
				m_CpuTime,
				m_TotalCpuTime;

		unsigned
				m_Count;
	};

	struct children_iterator
	{
		children_iterator( SDB_NODE** pos ) : m_pos( pos ) {}

		SDB_NODE* operator*()  const { return SDB_NODE_FROM_PURPLE_CHILD( *m_pos ); }

		bool is_purple() const { return SDB_NODE_IS_PURPLE_CHILD( *m_pos ); }

		bool at_object( SDB_NODE* n ) const { return m_pos == n->m_children; }
		bool at_value_type( SDB_NODE* n ) const { return m_pos == n->m_children + 1; }
		bool at_vti( SDB_NODE* n ) const { return m_pos == n->m_children + 2; }
		bool at_arg( SDB_NODE* n ) const { int const pos = m_pos - n->m_children - 3; return pos >= 0 && pos < n->ArgcGet(); }

		void operator++() { ++m_pos; }

		bool operator==( children_iterator const& it ) const { return m_pos == it.m_pos; }
		bool operator!=( children_iterator const& it ) const { return m_pos != it.m_pos; }

	private:
		SDB_NODE** m_pos;
	};

	typedef CC_STL_MAP_WITH_COMP( SDB_NODE const*, NODE_STATS, node_less ) stats_map_t;
	typedef std::multiset< SDB_NODE*, node_less > nodes_map;

	typedef nodes_map::iterator nodes_iterator;
	typedef CC_NS(std,pair)<nodes_map::iterator, nodes_map::iterator> nodes_iterator_range;

	/// When node is invalid, the Value.DataType is set to 0.
	/// But, when a 'ReleaseValue*' node releases its Value,
	/// it sets Value.DataType to SDB_NODE::ReservedValidValue() to denote
	/// that it is "valid" from its children perspective (!)
	DT_VALUE
			m_Value;		// Value of the node

	// Nodev is laid out as: Args, Children, Parents, [NumParents, NumDeletedParents]
	// [NumParent, NumDeletedParents] are conditional on the Parentc >= MinParentIndexForHash

	SDB_NODE
			**m_children;				// object, vt, vti, args and children

	SDB_DBSET
			*m_DbSet;		// Dbs which this node represents

    unsigned 
			m_Childc;		// Number of children

	unsigned short
			m_Argc;			// Number of arguments

	unsigned
			m_CacheFlags              :3,	// Flags describing the value
			m_ParentIndex             :5,	// Index into NodeParentInfo
			m_PassErrors              :1,	// Children need not be present and valid to calculate the value
			m_NodeType                :4,	// Terminal or what sort of non-terminal
			m_Valid                   :1,	// Value is valid
			m_ChildrenValid           :1,	// Child links (but not necessarily their values) are valid
			m_ValidLink               :1,	// Invalidate parents even if already invalid (when parent has PassErrors)
			m_GetValueLock            :1,	// NodeGetValue in progress -- used to detect dependency cycles
			m_SetOrDiddleLock         :1,   // SetValue in progess -- used to detect cycles
			m_Diddle                  :1,   // Value is a diddle
			m_DuplicateChildren       :1,	// True if node has duplicate children
			m_OnLRUCache              :1,	// Node is in object LRU cache
			m_DestroyFlags            :2,	// Marked by destroy when destroying a node which is invalidating
			m_ReleaseValueFlag        :2,	// Is set by a parent if VT has SDB_RELEASE_VALUE... flags.
			m_ChildDbChanged          :1,   // Child Db changed during BuildChildren or GetValue
											// redo the operation
			m_SplitChildrenLock       :1,	// SplitChildren in progress
			m_AlwaysRecompute		  :1,	// Always recompute this node (treat as an object method)
			m_ReleaseDescendentValues :1,	// When this node is valid, it will release values of some descendents.
			m_CycleIsNotFatal         :1,   // Make children's cycle error non-fatal so it can be PassErrors() and continue calling the value func of the vt, for ROFO ( GS2 enabled securities)
            m_Subscribed              :1,   // If node is subscribed, ie if invalidation generates a callback
		    m_AsyncResult             :1,	// Waiting for async result, do not restart computation
			m_UnusedBits              :1;	// bits availiable for use

	SDB_NODE
			**m_parents;				// parent list

    enum { MinParentIndexForHash = 7 }; // ugh - should be: static int const MinParentIndexForHash = 7;

    enum { NODE_MAX_PARENTC = 31 };     // ugh - should be: static int const NODE_MAX_PARENTC = 31;

	enum {
        DESTROYED      = 1,
        DESTROY_AFFECT = 2,
        DESTROY_FAKE   = 3
	};

	enum EnumSupported
	{
		DO_NOT_CHECK_SUPPORTED,
		CHECK_SUPPORTED
	};

	enum EnumShare
	{
		DO_NOT_SHARE,
		SHARE
	};

	enum EnumCreate
	{
		DO_NOT_CREATE,
		CREATE
	};

	typedef int get_value_result;

	enum
	{
		RES_ASYNC_ERROR = 0,
		RES_ASYNC_OK = 1,
		RES_SYNC_ERROR = 2,
		RES_SYNC_OK = 3
	};

	enum ReleaseValueFlags { 
			ReleaseValueNoneFlag 				= 0, 
			ReleaseValueNeedParentHashFlag 		= 1,
			ReleaseValueWhenParentsValidFlag	= ReleaseValueNeedParentHashFlag, 
			ReleaseValueFlag 					= 2 + 0, 
			ReleaseValueByAncestorFlag 			= 2 + ReleaseValueNeedParentHashFlag
	};

	////////////////////////////////////////////////////////////////////////////////
	//
	// Get/Set Accessors for all the flags
	//
	////////////////////////////////////////////////////////////////////////////////

    unsigned CacheFlagsGet()                     const { return m_CacheFlags;                                                           }
    unsigned ParentIndexGet()                    const { return m_ParentIndex;                                                          }
    unsigned NodeTypeGet()                       const { return m_NodeType;                                                             }
    bool     PassErrorsGet()                     const { return m_PassErrors;                                                           }
    bool     CycleIsNotFatalGet()                const { return m_CycleIsNotFatal;                                                      }
    bool     ValidGet()                          const { return m_Valid;                                                                }
    bool     ChildrenValidGet()                  const { return m_ChildrenValid;                                                        }
    bool     ValidLinkGet()                      const { return m_ValidLink;                                                            }
    bool     GetValueLockGet()                   const { return m_GetValueLock;                                                         }
    bool     SetOrDiddleLockGet()                const { return m_SetOrDiddleLock;                                                      }
    bool     DiddleGet()                         const { return m_Diddle;                                                               }
    bool     DuplicateChildrenGet()              const { return m_DuplicateChildren;                                                    }
    bool     OnLRUCacheGet()                     const { return m_OnLRUCache;                                                           }
    unsigned DestroyFlagsGet()                   const { return m_DestroyFlags;                                                         }
    bool     DestroyedGet()                      const { return m_DestroyFlags == DESTROYED;                                            }
    bool     ReleaseValueGet()                   const { return m_ReleaseValueFlag == ReleaseValueFlag;                                 }
    bool     ReleaseValueWhenParentsValidGet()   const { return m_ReleaseValueFlag == ReleaseValueWhenParentsValidFlag;                 }
    bool     ReleaseValueByAncestorGet()         const { return m_ReleaseValueFlag == ReleaseValueByAncestorFlag;                       }
    bool     ReleaseDescendentValuesGet()        const { return m_ReleaseDescendentValues;                                              }
    bool     ChildDbChangedGet()                 const { return m_ChildDbChanged;                                                       }
    bool     SplitChildrenLockGet()              const { return m_SplitChildrenLock;                                                    }
    bool     SubscribedGet()                     const { return m_Subscribed;                                                           }
												  	   
    bool     ReleaseValueNeedParentHashGet()     const { return ( m_ReleaseValueFlag & ReleaseValueNeedParentHashFlag ? true : false ); }
    bool     ReleaseValueAnyFlagGet()            const { return m_ReleaseValueFlag != ReleaseValueNoneFlag;                             }
	bool     IsPassThroughGet()                  const { return m_Value.DataType == DtPassThrough;                                      }

	NODE_STATS* StatsGet()                       const { return s_stats_enabled ? &s_stats_map[ this ] : 0; }

	NodeSet* AutoChildrenGet();

	DT_VALUE* AccessValue()
	{
		DT_VALUE*
				SubValue = &m_Value;

		while( SubValue->DataType == DtPassThrough )
			SubValue = (DT_VALUE *) SubValue->Data.Pointer;

		return s_VectorContext ? VectorContextAccessValue( SubValue ) : SubValue;
	}

	// Node pointer accessors

	// FIX FIX literals do this to decide whether they are literals, they shouldnt
    SDB_NODE*  ObjectGet() const                       { return m_children ? SDB_NODE_FROM_PURPLE_CHILD( m_children[0] ) : 0; }
    SDB_NODE*& ObjectGetRef() const                    { return m_children[0];                               }
    SDB_NODE*  ValueTypeGet() const                    { return m_children ? SDB_NODE_FROM_PURPLE_CHILD( m_children[1] ) : 0; }
    SDB_NODE*& ValueTypeGetRef() const                 { return m_children[1];                               }
    SDB_NODE*  VtiNodeGet() const                      { return m_children ? SDB_NODE_FROM_PURPLE_CHILD( m_children[2] ) : 0; }
    SDB_NODE*& VtiNodeGetRef() const                   { return m_children[2];                               }
    SDB_DBSET* DbSetGet() const                        { return m_DbSet;                                     }
													   
    int     ArgcGet() const                               { return m_Argc;                                }
    int     ChildcGet() const                             { return m_Childc;                              }

	// Set functions

    void    DbSetSet( SDB_DBSET *DbSet )                  { m_DbSet                        = DbSet;       }

    void    CacheFlagsSet  ( unsigned CacheFlags  )       { m_CacheFlags                   = CacheFlags;  }
    void    ParentIndexSet ( unsigned ParentIndex )       { m_ParentIndex                  = ParentIndex; }
    void    NodeTypeSet    ( unsigned NodeType    )       { m_NodeType                     = NodeType;    }
    void    DestroyFlagsSet( unsigned Value       )       { m_DestroyFlags                 = Value;       }
    void    PassErrorsSet       ( bool Value )            { m_PassErrors                   = Value;       }
    void    CycleIsNotFatalSet  ( bool Value )            { m_CycleIsNotFatal              = Value;       }
    void    ValidSet            ( bool Value )            { m_Valid                        = Value;       }
    void    ChildrenValidSet    ( bool Value )            { m_ChildrenValid                = Value;       }
    void    ValidLinkSet        ( bool Value )            { m_ValidLink                    = Value;       }
    void    GetValueLockSet     ( bool Value )            { m_GetValueLock                 = Value;       }
    void    SetOrDiddleLockSet  ( bool Value )            { m_SetOrDiddleLock              = Value;       }
    void    DiddleSet           ( bool Value )            { m_Diddle                       = Value;       }
    void    DuplicateChildrenSet( bool Value )            { m_DuplicateChildren            = Value;       }
    void    OnLRUCacheSet       ( bool Value )            { m_OnLRUCache                   = Value;       }
	void	AlwaysRecomputeSet	( bool Value )			  { m_AlwaysRecompute			   = Value;		  }
	void    ReleaseValueFlagSet( ReleaseValueFlags Flag ) { m_ReleaseValueFlag             = Flag;        }
	void    ReleaseDescendentValuesSet( bool Value )	  { m_ReleaseDescendentValues	   = Value;       }
    void    ChildDbChangedSet         ( bool Value )      { m_ChildDbChanged               = Value;       }
    void    SplitChildrenLockSet      ( bool Value )      { m_SplitChildrenLock            = Value;       }
	void    SubscribedSet             ( bool Value )      { m_Subscribed                   = Value;       }

	////////////////////////////////////////////////////////////////////////////////
	//
	// Various Helpers
	//
	////////////////////////////////////////////////////////////////////////////////

	bool	IsErrorNonFatal( int ErrNum ) const
	{
	    return ErrNum < SDB_NODE_FIRST_FATAL_ERROR ||
		       ErrNum > SDB_NODE_LAST_FATAL_ERROR  ||
		       (ErrNum == SDB_ERR_CYCLE_DETECTED && CycleIsNotFatalGet() );
	}

	bool	IsErrorFatal( int ErrNum ) const
	{
	    return !IsErrorNonFatal( ErrNum );
	}

	static bool is_res_error( get_value_result res ) { return ( res & RES_ASYNC_OK ) == 0; }

	static bool is_res_async( get_value_result res ) { return ( res | RES_ASYNC_OK ) != RES_SYNC_OK; }

	////////////////////////////////////////////////////////////////////////////////
	//
	// Children/Parent/Arg Pointer accessors
	//
	////////////////////////////////////////////////////////////////////////////////

	// Node child array size
	int		child_list_size() const
	{
		return m_Argc + m_Childc + 3;
	}

	// Get number of children
	int		ChildrenNum( SDB_VALUE_TYPE_INFO*& Vti )
    {
		if( m_NodeType != SDB_NODE_TYPE_TERMINAL )
			return m_Childc;

		if( !Vti )
			Vti = VtiGet();
		return Vti ? Vti->ChildList.TerminalCount : 0;
	}

	// Node Argument accessors
	SDB_NODE**         ArgGetAddr( int i ) const { return m_children + 3 + i; }
	SDB_NODE_CHILD_PTR ArgGetPurple( int i )     { return *ArgGetAddr( i ); }

	SDB_NODE* ArgGet( int i ) const              { return SDB_NODE_FROM_PURPLE_CHILD( *ArgGetAddr( i )); }

	// Node Children accessors
	SDB_NODE**         ChildGetAddr( int i )     { return m_children + 3 + m_Argc + i; }
	SDB_NODE_CHILD_PTR ChildGetPurple( int i )   { return *ChildGetAddr( i ); }

	SDB_NODE* ChildGet( int i ) { return SDB_NODE_FROM_PURPLE_CHILD( *ChildGetAddr( i )); }

	bool	IsPurpleChild( SDB_VALUE_TYPE_INFO const* Vti, int Num ) const
	{
		return Vti && ( Num >= Vti->ChildList.TerminalCount	|| Num < Vti->ChildList.ArgChildCount );
	}

	// Node Parent accessors
	int		ParentsMax() const
	{
		return m_OnLRUCache ? 0 : s_Parentc[ m_ParentIndex ];
	}
	int		ParentsNumCalc() const
	{
		if( ParentIndexGet() >= MinParentIndexForHash )
			return ParentsNum();
		int Num = 0;
		for( int i = 0; i < ParentsMax(); ++i )
			if( *ParentGetAddr( i ))
				++Num;
		return Num;
	}

	int&    ParentsNum()       { return *((int*      )( m_parents + ParentsMax() )); }
    int     ParentsNum() const { return *((int const*)( m_parents + ParentsMax() )); }

	int&    ParentsNumDeleted()       { return *((int*      )( m_parents + ParentsMax() + 1 )); }
    int     ParentsNumDeleted() const { return *((int const*)( m_parents + ParentsMax() + 1 )); }

	int		parent_list_size() const
	{
		int ParentIndex = m_OnLRUCache ? LruPointers()->ParentIndex : m_ParentIndex;
		return s_Parentc[ ParentIndex ] + 2 * ( ParentIndex >= MinParentIndexForHash );
	}

	// True if Parents are maintained as a refcount
	// False if they are maintained as a hash
	bool	RefCountedParents() const { return m_NodeType == SDB_NODE_TYPE_REF_COUNTED_LITERAL; }

	// Reference counted nodes use the following members
	// you had better not use them unless the node is REF_COUNTED_LITERAL!
	void	LiteralRefsIncrement() { ++*(int *)&m_parents; }

	void	LiteralRefsDecrement()
	{
		if( --*(int *)&m_parents <= 0 )
			FreeLiteral( this );
	}

	int		LiteralRefsGet() { return *(int *)&m_parents; }

	SDB_NODE**
	        ParentGetAddr( int i )
	{
		return m_parents + i;
	}

    SDB_NODE const* const*
	        ParentGetAddr( int i ) const
    {
	    return m_parents + i;
	}

	SDB_NODE_PARENT_PTR
			ParentGet( int i )
	{
		return *ParentGetAddr( i );
	}

	void	ParentSet( int i, SDB_NODE *Parent )
	{
		*ParentGetAddr( i ) = Parent;
	}

	SDB_NODE_INLINE int ParentLink( SDB_NODE* Parent );
	SDB_NODE_INLINE int ParentUnlink( SDB_NODE* Parent );
	SDB_NODE_INLINE int ParentLookup( SDB_NODE* Parent );
	SDB_NODE_INLINE int ParentHashStats( double& MeanChainLength, int& MaxChainLength );

	children_iterator all_children_begin() const { return children_iterator( this->m_children ); }
	children_iterator all_children_end() const { return ChildrenValidGet() ? ( this->m_children + child_list_size() ) : this->m_children + 3; }

	// Vti Node accessors
	void	VtiNodeSet( SDB_NODE* Vti );

    inline SDB_VALUE_TYPE_INFO*
	        VtiGet();

	////////////////////////////////////////////////////////////////////////////////
	//
	// Node Attributes
	//
	////////////////////////////////////////////////////////////////////////////////

	// Decide whether node is a terminal
    static bool
	        IsNode( SDB_NODE const* Node ) { return Node != NULL && Node != SDB_NODE_INVALID_PTR; }

	bool    IsTerminalQuick() const { return NodeTypeGet() == SDB_NODE_TYPE_TERMINAL; }
	bool    IsTerminal()      const { return IsNode( this ) && IsTerminalQuick(); }

	bool    IsLiteralQuick() const { return m_children == 0; }
	bool    IsLiteral()      const { return IsNode( this ) && IsLiteralQuick(); }

	bool    ComputeIsTerminal() const
	{
		SDB_NODE* const object = ObjectGet();
		SDB_NODE* const vt = ValueTypeGet();
		return object->IsLiteralQuick() && object->m_Value.DataType == DtSecurity &&
				vt->IsLiteralQuick() && vt->m_Value.DataType == DtValueType;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	// Diddle Scope stuff
	//
	////////////////////////////////////////////////////////////////////////////////

	SDB_DB* ExpectedChildDb()
	{
		DT_VALUE* const ObjValue = ObjectGet()->ValidGet() ? ObjectGet()->AccessValue() : NULL;
		return ( ObjValue && ObjValue->DataType == DtDiddleScope )
				? static_cast<DT_DIDDLE_SCOPE const*>( ObjValue->Data.Pointer )->Db
				: m_DbSet->Db();
	}

	SDB_DB* ExpectedChildDb( SDB_DB* Db )
	{
		DT_VALUE* const ObjValue = ObjectGet()->ValidGet() ? ObjectGet()->AccessValue() : 0;
		return ( ObjValue && ObjValue->DataType == DtDiddleScope )
				? static_cast<DT_DIDDLE_SCOPE const*>( ObjValue->Data.Pointer )->Db
				: Db;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	// Operations on nodes
	//
	////////////////////////////////////////////////////////////////////////////////

	// Initialize
	static bool							// called to initialize various stuff
			Initialize();

	static bool							// called after all tables have been loaded
			InitializeMore();

	static void
			Cleanup();					// destroy all statics

	static const NodeSet&
			getChangedNodes()			// Get the set of Changed nodes
	{
		return *s_changedNodes;
	}

	static void
			clearChangedNodes()			// clear the set of changed nodes
	{
		delete s_changedNodes;
		s_changedNodes = new NodeSet;
	}

	// Graph operations
	bool	Invalidate();

	bool	InvalidateSafe( int const Mask = SDB_CACHE_DIDDLE | SDB_CACHE_SET )
	{
		return !( CacheFlagsGet() & Mask ) ? Invalidate() : true;
	}

	bool	InvalidateChildren();

	bool	GetValue( SDB_NODE_CONTEXT *Ctx, SDB_DB *Db );

	get_value_result GetValueInternal( SDB_NODE_CONTEXT* Ctx, SDB_DB* Db );

    SDB_DIDDLE_ID ChangeDiddle   ( DT_VALUE *Value, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );
    SDB_DIDDLE_ID SetDiddle      ( DT_VALUE* Value, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );
    SDB_DIDDLE_ID WhiteoutDiddle (                                                              );
    bool          RemoveDiddle   ( SDB_DIDDLE_ID DiddleID                                       );

	bool    BuildChildren( SDB_NODE_CONTEXT* Ctx );

	static bool
			ArgumentUsage( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE SrcValueType, SDB_VALUE_TYPE_INFO *Vti, DT_VALUE *RetArray );


	SDB_NODE* FindAgain( EnumCreate CreateNode, SDB_DB *Db, EnumShare Share );
	SDB_NODE* FindCastTo( DT_DATA_TYPE CastToDt, SDB_DB *Db, EnumShare Share );
	SDB_NODE* FindSubscriptOf( DT_DATA_TYPE RetDt, DT_VALUE *Index, SDB_DB *Db, EnumShare Share );

	static SDB_NODE* FindLiteral( DT_VALUE *Value, SDB_DB *Db );
	static SDB_NODE* Find( SDB_NODE *Object, SDB_NODE *ValueType, int Argc, SDB_NODE **Argv, EnumCreate CreateNode, SDB_DB *Db, EnumShare Share );
	static SDB_NODE* FindTerminal( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, SDB_NODE **Argv, SDB_DB *Db, EnumShare Share, EnumSupported CheckSupported = CHECK_SUPPORTED );
	static SDB_NODE* FindTerminalAndArgs( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv, EnumSupported CheckSupported, SDB_DB *Db, EnumShare Share );
	static SDB_NODE* FindTerminalInt( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, SDB_NODE **Argv, EnumCreate CreateNode, SDB_DB *Db, EnumShare Share );
	static SDB_NODE* FindDefaultVtiNode( SDB_DB *Db, SDB_NODE *Object, SDB_NODE *ValueType, EnumCreate CreateNode, SDB_NODE *ClassObjNode = 0 );

	SDB_NODE
			*SplitChildren( SDB_DB *Db, EnumCreate CreateNode );

	bool	RemoveDbFromParents( SDB_DB *Db );

	static void
			Destroy( SDB_NODE *Node, int *pToken );

	static bool
			HaveLiteral( DT_VALUE *Value )
	{
		return HashLookup( s_LiteralHash, Value ) != NULL;
	}

	static SDB_NODE
			*LinkLiteral( DT_DATA_TYPE DataType, const void *Pointer );	// This makes a reference to the literal node

	void	LRUCacheByObjectDelete();

	void	LRUCacheByObjectInsert();

	SDB_DIDDLE_HEADER
			*DiddleHeaderGet();

	SDB_DIDDLE
			*DiddleFind( SDB_DIDDLE_ID DiddleId );

	void	AddAsSideEffect( SDB_DIDDLE_ID DiddleId, SDB_NODE* root )
	{
		SideEffectDiddleInfo& info = s_side_effect_diddles[ DiddleId ];
		if( !info.first )
		{
			info.first = root;
			info.second = new NodeSet;
		}
		info.second->insert( this );
	}

	static SideEffectDiddleMap const& SideEffectDiddles()
	{
		return s_side_effect_diddles;
	}

	static NodeSet const* SideEffects( SDB_DIDDLE_ID DiddleId )
	{
		SideEffectDiddleMap::iterator side_effect_it = s_side_effect_diddles.find( DiddleId );
		return side_effect_it == s_side_effect_diddles.end() ? 0 : side_effect_it->second.second;
	}

	// Semi-private accessors which you shouldn't really be using

	static HASH* DiddleHash()  { return s_DiddleHash;  }
	static HASH* LiteralHash() { return s_LiteralHash; }

	static nodes_map&          nodes()         { return s_nodes; }
	static HEAP*               NodeHeap()      { return s_NodeHeap;      }
	static SDB_VECTOR_CONTEXT* VectorContext() { return s_VectorContext; }

	bool	DeleteAutoChildList(); // always returns true (for use in while loops)

	////////////////////////////////////////////////////////////////////////////////
	//
	// Queries, Debugging and other Statistics
	//
	////////////////////////////////////////////////////////////////////////////////

	CC_NS(std,string) GetText( int AppendValue );
	CC_NS(std,string) to_string() { return GetText( FALSE ); }

	int		Error( const char *Tag );

	static bool Statistics( DT_VALUE* Result );
	static bool DiddleList( DT_VALUE* Result, SDB_OBJECT* SecPtr = 0, bool IncludeArgs = false );

	// Tracing
    inline void	TraceFunc( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg );
    inline void TraceFunc( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg, void* Arg );

    inline int  TraceFuncTest( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg );
    inline int  TraceFuncTest( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg, void* Arg );

	static bool IsInGet() { return s_GetDepth != 0; }

protected:

	////////////////////////////////////////////////////////////////////////////////
	//
	// Node pointer array manipulators
	//
	////////////////////////////////////////////////////////////////////////////////

	static int
			s_Parentc[ NODE_MAX_PARENTC + 1 ];

	// Reallocate node link array
	SDB_NODE_INLINE int
			AdjustChildc( unsigned NumChildren );

	// FIX- Should throw exception if out of memory
	static SDB_NODE
			**ListAlloc( int Size )
	{
		return (SDB_NODE **) malloc( sizeof( SDB_NODE * ) * Size );
	}
	static void
			ListFree( SDB_NODE **List )
	{
		free( List );
	}
	static SDB_NODE
			**ListRealloc( SDB_NODE **List, int Size )
	{
		return (SDB_NODE **) realloc( List, sizeof( SDB_NODE * ) * Size );
	}

	// Node parent manipulators
	int		ParentsNeedToRebuild()
	{
		return ( ParentsNum() + ParentsNumDeleted() ) > ( ParentsMax() * 3 ) / 4;
	}

	SDB_NODE_INLINE void
			ParentsRebuild( int InPlace ); // rebuild parents hash

	////////////////////////////////////////////////////////////////////////////////
	//
	// Literal nodes
	//
	////////////////////////////////////////////////////////////////////////////////

	static HASH
			*s_LiteralHash;

	static HASH_BUCKET
			LiteralHashFunc( HASH_KEY Key );

	static int
			LiteralHashCmp( HASH_KEY KeyA, HASH_KEY KeyB );

	////////////////////////////////////////////////////////////////////////////////
	//
	//	Parent Hash helpers
	//
	////////////////////////////////////////////////////////////////////////////////

	int		NextBucket( int Bucket )
	{
		return ( Bucket + 1 ) & ( ParentsMax() - 1 );
	}

	int		PrevBucket( int Bucket )
	{
		return ( Bucket + ParentsMax() - 1 ) & ( ParentsMax() - 1 );
	}

	unsigned long NodeHash( SDB_NODE *Node )
	{
		return (unsigned long) ( (unsigned long) Node * 1664525L + 1013904223L ) & (unsigned long) ( ParentsMax() - 1 );
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	//	SkipList helpers
	//
	////////////////////////////////////////////////////////////////////////////////

	static nodes_map
			s_nodes;

public:
	static int
			Compare( const void *, const void * );
protected:

	////////////////////////////////////////////////////////////////////////////////
	//
	// Node diddle datastructures
	//
	////////////////////////////////////////////////////////////////////////////////

	static HASH
			*s_DiddleHash;

	static HASH
			*s_PhantomDiddleHash;

	static SideEffectDiddleMap
			s_side_effect_diddles;

	static int
			DiddleIdCompare( const void *Key1, const void *Key2 );

	bool	DiddleStackMoveTo( SDB_NODE *Dest );

	bool	PlaceNotMyDiddleFrom( SDB_NODE *Src );

	SDB_DIDDLE_ID
			DiddleSet( SDB_OBJECT *SecPtr, DT_VALUE *Value, SDB_M_DIDDLE *DiddleMsgData, SDB_DIDDLE_ID &PrevDiddleID );

    bool    DiddleChange( SDB_OBJECT *SecPtr, DT_VALUE *Value, SDB_M_DIDDLE *DiddleMsgData, int OkIfNotFound, SDB_DIDDLE_ID &PrevDiddleID );

	bool	PlaceDiddle( SDB_DIDDLE *Diddle, SDB_OBJECT *SecPtr, DT_VALUE *Value, int DiddleMsg, SDB_M_DIDDLE *DiddleMsgData );

	bool	DiddleRemove( SDB_OBJECT *SecPtr, SDB_M_DIDDLE *DiddleMsgData );

	void	DiddleToNode( SDB_DIDDLE *Diddle );

	static SDB_NODE*
			IsPhantomDiddle( SDB_DIDDLE_ID DiddleID ) { return (SDB_NODE*) HashLookup( s_PhantomDiddleHash, (HASH_KEY) DiddleID ); }

	static void
			MarkPhantomDiddle( SDB_DIDDLE_ID DiddleID, SDB_NODE* root ) { HashInsert( s_PhantomDiddleHash, (HASH_KEY) DiddleID, (HASH_VALUE) root ); }

	static void
			UnMarkPhantomDiddle( SDB_DIDDLE_ID DiddleID ) { HashDelete( s_PhantomDiddleHash, (HASH_KEY) DiddleID ); }

	////////////////////////////////////////////////////////////////////////////////
	//
	// Stuff to deal with RELEASE_VALUE nodes
	//
	////////////////////////////////////////////////////////////////////////////////


	static HASH	
			*s_InvParentsHash;	// to store children with ReleaseValue* flags

	static bool
			s_ReleaseValueWhenParentsValidDefault;


    static HASH
			*InvParentsHash() { return s_InvParentsHash; }

    static const char
			*InvParentsHashName() { return "InvParentsHash"; }

	static void
			InvParentsHashInc( SDB_NODE *Child )
    {
		HASH_ENTRY_PTR	ep = HashLookupPtr( s_InvParentsHash, (HASH_KEY) Child );
		if( !ep )
			HashInsert( s_InvParentsHash, (HASH_KEY) Child, (HASH_VALUE) 1 );
		else
			ep->Value = (HASH_VALUE) (((long) ep->Value) + 1);

		if( SecDbTraceFunc )
			SecDbTraceFunc( SDB_TRACE_NUM_INV_PARENTS_INCREMENT, Child, Child, (long) HashLookup( s_InvParentsHash, (HASH_KEY) Child ));
	}

    // returnts a # of parents left or -1 if the child is not in the hash
	static int
			InvParentsHashDec( SDB_NODE *Child )	
    {
		HASH_ENTRY_PTR ep = HashLookupPtr( s_InvParentsHash, (HASH_KEY) Child );
		if( !ep )
			return -1;
				
		int ParentsLeft = ((long) ep->Value) - 1;
		if( !ParentsLeft )
			HashDeleteEntry( s_InvParentsHash, ep );
		else
			ep->Value = (HASH_VALUE) ParentsLeft;

		if( SecDbTraceFunc )
			SecDbTraceFunc( SDB_TRACE_NUM_INV_PARENTS_DECREMENT, Child, Child, (long) HashLookup( s_InvParentsHash, (HASH_KEY) Child ));
		
		return ParentsLeft;
	}

	bool	IsReservedValidValue() const
    {
		return m_Value.DataType == ReservedValidValue();
	}

	static DT_DATA_TYPE
			ReservedValidValue()
    {
		return (DT_DATA_TYPE) 0xdeadbeef;
	}

	static int
			InvParentsHashInit();
	static void
			InvParentsHashDestroy();
	static void
			InvParentsHashSet( SDB_NODE *Child, int Num );

	// Re-calculate a number of invalid/locked parents
	int		CalcInvOrLockedParentsNum()
    {
		int NumInvalidParents = 0;

		//--------------- Calculate the # of invalid parents
		SDB_NODE
				**ParentMax = ParentGetAddr( ParentsMax() );
		
		for( SDB_NODE **ParentPtr = ParentGetAddr( 0 ); ParentPtr < ParentMax; ++ParentPtr )
		{
			if( IsNode( *ParentPtr ))
			{
				SDB_NODE
						*Parent = SDB_NODE_PARENT_PTR( *ParentPtr ).Node();
				
				if( !Parent->ValidGet() || Parent->GetValueLockGet() )
					NumInvalidParents++;
			}
		}
		return NumInvalidParents;
	}

	bool	ReleaseChildValue( SDB_NODE *Child );

	bool	ReleaseDescendentValues();
	bool	ReleaseOneDescendent( SDB_NODE *Child );

	////////////////////////////////////////////////////////////////////////////////
	//
	// Auto discovered children
	//
	////////////////////////////////////////////////////////////////////////////////

	static SDB_NODE
			*s_CurrentAutoChildListNode;

	static bool
			s_CollectAutoChildren;

	static AutoChildrenMap
			s_AutoChildrenMap;

	static int
			s_GetDepth;

	// Found an auto child, add it
	void    AddAutoChild( SDB_NODE *Child );

	// this this child as an auto child into the given set for the Parent
	void	InsertAutoChild( NodeSet &Children, SDB_NODE *Parent );

	// Find all auto children starting at this child and insert them into the set
	SDB_NODE
			*FindAutoChildren( SDB_NODE *Child, NodeSet &Children );

	bool	CollectAutoChildren()
	{
		return s_CollectAutoChildren;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	//	LRU Cache Pointer manipulators
	//
	////////////////////////////////////////////////////////////////////////////////

	static SDB_OBJECT
			s_NonTerminalObject;		// Object on which we store unparented non terminals

	NODE_LRU_POINTERS*
			LruPointers()
	{
		return (NODE_LRU_POINTERS *) ParentGetAddr( 0 );
	}

    NODE_LRU_POINTERS const*
	        LruPointers() const
    {
	    return (NODE_LRU_POINTERS const*) ParentGetAddr( 0 );
	}

	// true if the node is a candidate for the LRU Cache
	bool	CandidateForLRUCache()
	{
		return !( GetValueLockGet() || SetOrDiddleLockGet() || DestroyedGet() || DiddleGet()
				  || ( CacheFlagsGet() & ( SDB_CACHE_SET | SDB_CACHE_DIDDLE )) );
	}

	void	PutOnLRUCache()
	{
		if( m_ParentIndex < MIN_PARENTC_FOR_LRU_POINTERS )
		{
			m_ParentIndex = MIN_PARENTC_FOR_LRU_POINTERS;
			m_parents = ListRealloc( m_parents, parent_list_size() );
		}
		m_OnLRUCache = TRUE;
		NODE_LRU_POINTERS *Ptr = LruPointers();
		Ptr->ParentIndex = m_ParentIndex;
		m_ParentIndex = 0;
	}

	void	TakeOffLRUCache()
	{
		NODE_LRU_POINTERS *Ptr = LruPointers();
		m_ParentIndex = Ptr->ParentIndex;
		m_OnLRUCache = FALSE;
		memset( ParentGetAddr( 0 ), 0, sizeof( NODE_LRU_POINTERS ));
	}

	void	InsertHead( SDB_OBJECT * );

	SDB_NODE
			*RemoveNode( SDB_OBJECT * );

	////////////////////////////////////////////////////////////////////////////////
	//
	// Free/Alloc helpers
	//
	////////////////////////////////////////////////////////////////////////////////

	static HEAP
			*s_NodeHeap;

	void FreeValue()
	{
		m_Valid = FALSE;
		if( !( m_CacheFlags & SDB_CACHE_NO_FREE ) && !GetValueLockGet() )
			DTM_FREE( &m_Value );
		DTM_INIT( &m_Value );
	}

	static void
			Free( SDB_NODE *Node )
	{
		if( Node->m_OnLRUCache )
			Node->LRUCacheByObjectDelete();

		ListFree( Node->m_children );
		ListFree( Node->m_parents );
		Node->m_children = (SDB_NODE **) SDB_NODE_INVALID_PTR;
		GSHeapFree( s_NodeHeap, Node );

		if( s_stats_enabled )
			s_stats_map.erase( Node );
	}

	static void
			FreeLiteral( SDB_NODE *Node );

	static SDB_NODE
			*Alloc()
	{
		SDB_NODE *Node;

		do
		{
			Node = (SDB_NODE * ) GSHeapCalloc( s_NodeHeap );
		}
		while( ( (long) Node & ~0xF ) == ( (long) SDB_NODE_INVALID_PTR & ~0xF ));

		return Node;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	// Node Vectorization stuff
	//
	////////////////////////////////////////////////////////////////////////////////

	static SDB_VECTOR_CONTEXT
			*s_VectorContext;

	static DT_VALUE
			*VectorContextAccessValue( DT_VALUE *SubValue );

	static SDB_NODE
			*s_ValueArrayEvalVT;

	void	VectorSizeMismatchMessage();

	static bool
			VectorIndexPush();

	static void
			VectorIndexPop();

	////////////////////////////////////////////////////////////////////////////////
	//
	// Internal graph operations
	//
	////////////////////////////////////////////////////////////////////////////////

	// save the getvalue call
	get_value_result GetValueInternalQuick( SDB_NODE_CONTEXT* Ctx, SDB_DB* Db )
	{
		return GetValueLockGet() ?
				( Err( SDB_ERR_CYCLE_DETECTED, "@ - fatal error while getting child values" ), RES_SYNC_ERROR )
				: ( ValidGet() ? RES_SYNC_OK : GetValueInternal( Ctx, Db ));
	}

    bool    BuildChildrenOnce( SDB_NODE_CONTEXT* Ctx );

	static NodeSet*
			s_changedNodes;

	// Invalidation
	int		InvalidateChildrenInt();

	SDB_NODE_INLINE void
			MakeFirstChildNonNull();

	// Mark invalid links as ValidLinks for invalidation
	void    MarkValidLinks();

	// Finding nodes

	inline void LiteralExtend( SDB_DB *Db );

	static bool
			RedirectObjectValueFunc( SDB_NODE *VtiNode, SDB_NODE *Object, SDB_NODE *ValueType, SDB_DB *Db );

	static SDB_NODE*
			Create( SDB_DB *Db, SDB_NODE *Object, SDB_NODE *ValueType, int Argc, SDB_NODE **Argv, SDB_NODE *BestNode, nodes_iterator position );

	// Building the graph
	SDB_NODE*
	        FindChild( SDB_VALUE_TYPE_INFO *Vti, SDB_CHILD_ITEM_TYPE TargetResultType, int TargetResultElement ); // can throw an exception
	SDB_NODE*
	        FindPredicatedChild( SDB_CHILD_LIST_ITEM*, SDB_VALUE_TYPE_INFO* ); // can throw an exception
	SDB_NODE*
			GetItem( SDB_CHILD_COMPONENT *Desc, SDB_VALUE_TYPE_INFO *Vti, int NoEval );

	bool   	CheckArgDataTypes( SDB_VALUE_TYPE_INFO *Vti );

	DT_VALUE*
			AccessTargetValue();

	static void
			DestroyCommit( void *Collect );	// FIX- arghh! can't forward declare a vector

	// calling value functions
	static bool
	        SubVfChildValues( struct NODE_SUBVF_ARGS * );
	static bool
	        SubVfChildData( struct NODE_SUBVF_ARGS * );
	static bool
			SubVfExpensive( struct NODE_SUBVF_ARGS*, bool& );

    bool    collect_child_values( SDB_NODE** const first_child_ddr , SDB_NODE** const last_child_addr,
                                  DT_VALUE** const SubValues       , CC_NS(extension,auto_ptr_array)<DT_VALUE_ARRAY*> &VecValues,
                                  DT_VALUE*  const ellipsis        , DT_VALUE*  const vec_result );

	// debugging/queries
	CC_NS(std,string)
			GetTextI( int AppendValue );

	////////////////////////////////////////////////////////////////////////////////
	//
	// node statistics
	//
	////////////////////////////////////////////////////////////////////////////////

	static bool			s_stats_enabled; // if stats are enabled
	static stats_map_t	s_stats_map;	// maps nodes to stats

	////////////////////////////////////////////////////////////////////////////////
	//
	// Friends - so many friends - may someday we can have useful namespaces...
	//
	////////////////////////////////////////////////////////////////////////////////

	friend class SDB_NODE_ITER;
	friend class SDB_NODE_ITER_RE_USE_UPDATE;
	friend class SideEffectDiddleIterator;
	friend class SaveAutoGetNode;
	friend class PushAutoGetNode;
	friend struct in_get_value;
	friend struct push_vec_index;

	friend long
			NodeMemoryUsage( DT_VALUE *Results );

	friend int
			SecDbSetValueWithArgs( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv, DT_VALUE *Value, SDB_SET_FLAGS Flags );

	friend int
			DiddleScopeFromArray( DT_ARRAY *DiddleState, SLANG_SCOPE *Scope );

	friend int SecDbValFuncValueTypeArgs( SDB_OBJECT*, int, SDB_M_DATA*, SDB_VALUE_TYPE_INFO* );

} /* SDB_NODE */;

inline bool equal( SDB_NODE const* const p, SDB_NODE const* const q )
{
	if( p->ObjectGet() != q->ObjectGet()
		|| p->ValueTypeGet() != q->ValueTypeGet()
		|| p->ArgcGet() != q->ArgcGet() )
		return false;
	for( int arg = 0; arg < p->ArgcGet(); ++arg )
		if( p->ArgGet( arg ) != q->ArgGet( arg ) )
			return false;
	return true;
}

void SDB_NODE::TraceFunc( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg )
{
    if( TraceFunc )
	    TraceFunc( Msg, this, Ctx );
}

void SDB_NODE::TraceFunc( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg, void* Arg )
{
    if( TraceFunc )
	    TraceFunc( Msg, this, Ctx, Arg );
}

int SDB_NODE::TraceFuncTest( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg )
{
    return TraceFunc ? TraceFunc( Msg, this, Ctx ) : TRUE;
}

int SDB_NODE::TraceFuncTest( SDB_NODE_CONTEXT* Ctx, SDB_TRACE_FUNC TraceFunc, SDB_TRACE_MSG Msg, void* Arg )
{
    return TraceFunc ? TraceFunc( Msg, this, Ctx, Arg ) : TRUE;
}

template< SDB_TRACE_MSG _begin_msg, SDB_TRACE_MSG _end_msg >
struct sdb_node_trace_scope_ctx
{
	sdb_node_trace_scope_ctx( SDB_NODE* Node, SDB_NODE_CONTEXT* Ctx ) : m_Node( Node ), m_Ctx( Ctx )
	{
		m_Node->TraceFunc( m_Ctx, SecDbTraceFunc, _begin_msg );
	}
	~sdb_node_trace_scope_ctx()
	{
		m_Node->TraceFunc( m_Ctx, SecDbTraceFunc, _end_msg );
	}
private:
	SDB_NODE*         const m_Node;
	SDB_NODE_CONTEXT* const m_Ctx;
};

template< SDB_TRACE_MSG _begin_msg, SDB_TRACE_MSG _end_msg >
struct sdb_node_trace_scope
{
	sdb_node_trace_scope( SDB_NODE* Node ) : m_Node( Node )
	{
		if( SecDbTraceFunc ) SecDbTraceFunc( _begin_msg, m_Node );
	}
	~sdb_node_trace_scope()
	{
		if( SecDbTraceFunc ) SecDbTraceFunc( _end_msg, m_Node );
	}
private:
	SDB_NODE* const m_Node;
};

class SDB_NODE::SaveAutoGetNode
{
public:
    SaveAutoGetNode() throw() : m_Saved( s_CurrentAutoChildListNode ), m_SavedCollect( s_CollectAutoChildren ) {}
    ~SaveAutoGetNode() throw() { s_CurrentAutoChildListNode = m_Saved; s_CollectAutoChildren = m_SavedCollect; }
    SDB_NODE* SavedGet() throw()    { return m_Saved;}
	bool      CollectAutoChildren() { return m_SavedCollect; }

private:
    SDB_NODE* m_Saved;
	bool      m_SavedCollect;
};

class SDB_NODE::PushAutoGetNode : private SDB_NODE::SaveAutoGetNode
{
public:
    PushAutoGetNode( SDB_NODE* NewVal ) throw() { s_CurrentAutoChildListNode = NewVal; s_CollectAutoChildren = false; }
    /* using */ SaveAutoGetNode::SavedGet; // ugh. uncomment using when the compilers get current
	/* using */ SaveAutoGetNode::CollectAutoChildren; // ugh. uncomment using when 
};

SDB_VALUE_TYPE_INFO*
        SDB_NODE::VtiGet()
{
	SDB_NODE* const vti = VtiNodeGet();
    if( vti )
	{
		SDB_NODE_CONTEXT
		        Ctx;

		PushAutoGetNode
		        Save( 0 );

		if( vti->ValidGet() || ( m_DbSet->Db() && vti->GetValue( &Ctx, m_DbSet->Db() )))
		{
		    DT_VALUE*
			        VtiValue = vti->AccessValue();

			if( VtiValue->DataType == DtValueTypeInfo && VtiValue->Data.Pointer )
			    return ((DT_VALUE_TYPE_INFO *) VtiValue->Data.Pointer)->Vti;
			else
				Err( ERR_NOT_CONSISTENT, "@: NODE_GET_VTI- Node Value is not valid" );
		}
		return 0;
	}
	Err( ERR_NOT_CONSISTENT, "@: NODE_GET_VTI - Node is not a terminal, cannot get vti" );
	return 0;
}


/*
**	Node helper functions
*/

#ifndef SDB_NODE_NO_INLINE
#include "sdbn_inl.h"
#endif


#endif 
