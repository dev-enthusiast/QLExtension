//#define DEBUG_SDB_COMPILE_ON
/****************************************************************
**
**	SECDBCOMPILEDGRAPH.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/SecDbCompiledGraph.h,v 1.18 2001/11/27 23:25:02 procmon Exp $
**
****************************************************************/

#ifndef IN_SECDB_SECDBCOMPILEDGRAPH_H
#define IN_SECDB_SECDBCOMPILEDGRAPH_H

#include <secdb/base.h>
#include <secnodei.h>
#include <map>
#include <set>
#include <ccstl.h>
#include <vector>
#include <gsdt/GsDtGeneric.h>
#include <secdb/GsNodeValues.h>
#include <iostream>
#include <extension/collision_hash>

CC_BEGIN_NAMESPACE( extension )

template<> struct GsHash< SDB_NODE* >
{
	size_t operator()( SDB_NODE* n ) const { return size_t( n ); }
};

CC_END_NAMESPACE

typedef COLLISION_HASH_SET( SDB_NODE* ) SdbNodeSet;

typedef CC_STL_VECTOR(SDB_NODE*) sdb_node_vec_t;

#ifdef DEBUG_SDB_COMPILE_ON
#define DEBUG_SDB_COMPILE()
#else
#define DEBUG_SDB_COMPILE() if(0)
#endif

CC_BEGIN_NAMESPACE( Gs )

inline string to_string( DT_VALUE* val )
{
	if( !val )
		return "<null>";
	dt_value_var str_val;
	str_val->DataType = DtString;
	if( !DTM_TO( str_val.get(), val ) )
	{
		DTM_INIT( str_val.get() );		// who knows what junk is in str_val..
		string res = "<unknown ";
		res += val->DataType->Name;
		res += ">";
		return res;
	}
	return static_cast<char const*>( str_val->Data.Pointer );
}

struct SecDbCompiledGraph;

struct dt_value_holder
{
	dt_value_holder()
	{
		DTM_INIT( &m_value );
	}
	dt_value_holder( dt_value_holder const& src )
	{
		DTM_ASSIGN( &m_value, const_cast<DT_VALUE*>( &src.m_value ));
	}
	~dt_value_holder()
	{
		DTM_FREE( &m_value );
	}

	DT_VALUE* operator->() { return &m_value; }
	DT_VALUE* get() { return &m_value; }

	DT_VALUE release() { DT_VALUE tmp = m_value; DTM_INIT( &m_value ); return tmp; }

	void operator=( DT_VALUE* new_val )	// gives ownership of the new_val
	{
		DTM_FREE( &m_value );
		m_value = *new_val;
	}
	void operator=( DT_VALUE new_val )	// gives ownership of the new_val
	{
		DTM_FREE( &m_value );
		m_value = new_val;
	}

private:
	DT_VALUE m_value;
};

struct sdb_node_value_ref
{
	sdb_node_value_ref() : m_type( NULL_CHILD ), m_elem( -1 )
	{}
	sdb_node_value_ref( int type, int elem ) : m_type( type ), m_elem( elem )
	{}

	enum
	{
		NODE,
		CONSTANT,
		EVAL_NODE,
		NULL_CHILD,
		ARG
	};

	int m_type;
	int m_elem;

	DT_VALUE* get_value( SecDbCompiledGraph* );
};

struct sdb_node_eval
{
	typedef CC_STL_VECTOR(sdb_node_value_ref) sdb_node_value_ref_vec_t;

	sdb_node_eval() : m_vti(0), m_result_elem( -1 ), m_argc( 0 ), m_pass_thru( false )
	{}

	void evaluate( SecDbCompiledGraph* compiled_graph, DT_VALUE* res );

	SDB_VALUE_TYPE_INFO* m_vti;

	sdb_node_value_ref_vec_t m_children;

	int m_result_elem;
	int m_argc;
	bool m_pass_thru;
};

struct EXPORT_CLASS_SECDB SecDbCompiledGraph
{
	typedef GsDtGeneric<SecDbCompiledGraph> GsDtType;
	typedef CC_STL_VECTOR(dt_value_holder) dt_value_holder_vec_t;
	typedef CC_STL_VECTOR(sdb_node_eval) sdb_node_eval_vec_t;

	inline static const char* typeName() { return "SecDbCompiledGraph"; };
	CC_NS(std,string) toString() const;

	SecDbCompiledGraph()
	{}
	~SecDbCompiledGraph()
	{
	}

	DT_VALUE evaluate( DT_VALUE* arg_values );

	bool compile( SDB_NODE* f, sdb_node_vec_t const& args_vec, sdb_node_vec_t& compile_deps );

protected:
	dt_value_holder_vec_t m_constants;
	sdb_node_eval_vec_t m_eval_list;

	sdb_node_vec_t m_curr_shared_values;
	CC_STL_VECTOR(dt_value_holder) *m_curr_stack;

	DT_VALUE* m_curr_args;				// curr arg list during evaluation

	int m_max_slots;
	int m_num_args;

	friend struct sdb_node_value_ref;
};

class SecDbCompiledVectorValue : public GsNodeValues
{
private:
	sdb_node_eval* m_node;
	SecDbCompiledGraph* m_compiled_graph;

public:
	SecDbCompiledVectorValue( sdb_node_eval* node, SecDbCompiledGraph* compiled_graph )
	: GsNodeValues( node->m_children.size() ), m_node( node ), m_compiled_graph( compiled_graph )
	{
	}
	~SecDbCompiledVectorValue()
	{}

	DT_VALUE* lookup( int index ) const
	{
		DT_VALUE* res = m_node->m_children[ index ].get_value( m_compiled_graph );
		if( !res )
		{
			DTM_INIT( &s_NullValue );
			res = &s_NullValue;
		}
		return res;
	}

	DT_VALUE* lookup_no_null( int index ) const
	{
		return m_node->m_children[ index ].get_value( m_compiled_graph );
	}
};

inline DT_VALUE* sdb_node_value_ref::get_value( SecDbCompiledGraph* comp )
{
	switch( m_type )
	{
		case NODE:
		{
			SDB_NODE* node = SDB_NODE_FROM_PURPLE_CHILD( comp->m_curr_shared_values[ m_elem ] );
			DEBUG_SDB_COMPILE() {
				CC_NS(std,cerr) << "get_val: " << (node ? node->to_string() : string( "<null_node>" )) << CC_NS(std,endl);
			}
			SDB_NODE_CONTEXT Ctx;
			return node && ( node->ValidGet() || node->GetValue( &Ctx, SecDbRootDb ) ) ? node->AccessValue() : 0;
		}

		case CONSTANT:
			DEBUG_SDB_COMPILE() {
				CC_NS(std,cerr) << "get_val: const " << m_elem << CC_NS(std,endl);
			}
			return comp->m_constants[ m_elem ].get();

		case EVAL_NODE:
			DEBUG_SDB_COMPILE() {
				CC_NS(std,cerr) << "get_val: stack_elem " << m_elem << CC_NS(std,endl);
			}
			return (*comp->m_curr_stack)[ m_elem ].get();

		case ARG:
			DEBUG_SDB_COMPILE() {
				CC_NS(std,cerr) << "get_val: arg " << m_elem << CC_NS(std,endl);
			}
			return comp->m_curr_args + m_elem;

		default:
			break;
	}
	return 0;
}

CC_END_NAMESPACE

#endif 
