#define GSCVSID "$Header: /home/cvs/src/secdb/src/compile_graph.cpp,v 1.24 2004/01/30 23:38:17 khodod Exp $"
/****************************************************************
**
**	compile_grap.cpp	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: compile_graph.cpp,v $
**	Revision 1.24  2004/01/30 23:38:17  khodod
**	Fixed the comparators for large pointer values.
**
**	Revision 1.23  2001/11/27 23:23:31  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.13  2001/10/10 00:53:42  singhki
**	small bugfix for crash when eval_list is empty
**	
**	Revision 1.12  2001/10/04 15:18:39  singhki
**	get_level needs to be exportable to be instantiated in a template
**	
**	Revision 1.11  2001/10/03 15:29:45  singhki
**	get rid of references to piece of crap roguewave, my hash maps are faster and they actually compile everywhere.
**	
**	Revision 1.10  2001/10/02 22:29:01  singhki
**	allow args with different levels. necessary when there are multiple args with different paths down to them.
**	
**	Revision 1.9  2001/10/02 22:23:46  singhki
**	args is a vector in order to preserve order
**	
**	Revision 1.8  2001/10/02 18:44:31  singhki
**	rw and other fixes for NT, need to define own min/max grrr, fix should remove it when rw issues are sorted
**	
**	Revision 1.7  2001/10/02 15:52:53  singhki
**	SW4.2 fixes
**	
**	Revision 1.6  2001/10/02 15:34:34  singhki
**	rewrite compile graph algorithms, order of magnitude faster.
**	
**	Revision 1.5  2001/09/19 19:11:41  singhki
**	do not allow args if purple during compile. also add trace
**	
**	Revision 1.4  2001/08/28 22:24:57  singhki
**	turn off no_more_info debug trick because it doesnt work on NT/sol where dev is unoptimized
**	
**	Revision 1.3  2001/08/22 21:32:43  singhki
**	fixes for SW4
**	
**	Revision 1.2  2001/08/22 21:17:59  singhki
**	namespace and other fixes to work on NT
**	
**	Revision 1.1  2001/08/22 20:22:38  singhki
**	initial revision of compiled graphs.
**	modified GsNodeValues to be virtual so that we can define SecDbCompiledVectorValue.
**	Created SDB_DYNAMIC_CHILDLIST which invokes the VF during BuildChildren in order
**	to get the dependencies from the compiled graph nodes.
**	
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_ALLOCA
#include <portable.h>
#include <secdb/compile_graph.h>
#include <secnodei.h>
#include <map>
#include <extension/auto_ptr_array.h>
#include <secdb.h>
#include <sdb_enum.h>
#include <algorithm>
#include <extension/functional>
#include <functional>
#include <memory>
#include <sstream>
#include <extension/collision_hash>
#include <extension/GsRefCountedPtr.h>
#include <extension/hash_fun>

CC_USING( extension::auto_ptr_array );
CC_USING( std::auto_ptr );
CC_USING( std::sort );
CC_USING( std::for_each );
CC_USING( std::ostringstream );
CC_USING( std::cerr );
CC_USING( std::cout );
CC_USING( std::endl );

#define for if(0); else for

CC_BEGIN_NAMESPACE( Gs )

template<class T>
T my_min( T a, T b )
{
	return a < b ? a : b;
}

struct node_info;

CC_END_NAMESPACE

CC_BEGIN_NAMESPACE( extension )

template<> struct GsHash< CC_NS(Gs,node_info)* >
{
	size_t operator()( CC_NS(Gs,node_info)* n ) const { return size_t( n ); }
};

CC_END_NAMESPACE

CC_BEGIN_NAMESPACE( Gs )

struct time_block
{
	time_block( char const* name ) : m_name( name ), m_time( Now() )
	{
	}
	~time_block()
	{
		m_time = Now() - m_time;
		cout << m_name << " " << m_time << " secs." << endl;
	}

	string m_name;
	double m_time;
};

struct node_visited_info
{
	node_visited_info() : visited( true ), info( 0 )
	{}

	bool visited;
	node_info* info;
};

typedef COLLISION_HASH_MAP_H( SDB_NODE*, node_visited_info, CC_NS(extension,GsHash)< SDB_NODE* > ) node_visited_map_t;
typedef CC_STL_VECTOR( node_info* ) node_info_vec_t;
typedef COLLISION_HASH_SET( node_info* ) node_info_set_t;
typedef COLLISION_HASH_MAP( SDB_NODE*, int ) node_int_map_t;

struct compile_helper
{
	compile_helper( sdb_node_vec_t& compile_deps, SdbNodeSet const& args )
	: m_nodes_map( node_visited_map_t::value_type( (SDB_NODE*)0, node_visited_info() ),
				   node_visited_map_t::value_type( SDB_NODE_INVALID_PTR, node_visited_info() )
				 ),
	  m_added_compile_deps( node_int_map_t::value_type( (SDB_NODE*)0, 0 ),
							node_int_map_t::value_type( SDB_NODE_INVALID_PTR, 0 )
						  ),
	  m_compile_deps( compile_deps ),
	  m_args( args )
	{}
	~compile_helper();

	node_visited_map_t m_nodes_map;
	node_info_vec_t m_all_node_info;

	node_int_map_t m_added_compile_deps;

	sdb_node_vec_t& m_compile_deps;

	SdbNodeSet const& m_args;

	void add_dep( SDB_NODE* node, bool purple );
	void add_deps( SDB_NODE* node );

	node_info* assign_levels( SDB_NODE* node, int curr_level );
};


struct node_info
{
	node_info( SDB_NODE* node ) : m_node( node ), m_level( 0 ), m_index( -1 ), m_num_visited_parents( 0 ), m_visited( false ), m_added( false ),
								  m_shared( false ), m_minimized_level( false ), m_is_arg( false ),
								  m_children( node->ChildrenValidGet() ? node->ChildcGet()+3 : 4, (node_info*)0, (node_info*) 1 ),
								  m_parents( node->ParentsMax(), (node_info*)0, (node_info*) 1 )
	{
	}

	void set_level( int curr )
	{
		if( curr < m_level )			// only push level down, not up
		{
			m_level = curr;
			// push level down to children if we have any
			// note that this can be n*2 in the worst case but we expect that to be unlikely
			for( node_info_set_t::iterator it = m_children.begin(); it != m_children.end(); ++it )
				(*it)->set_level( curr - 1 );
		}
	}

	void add_parent( node_info* parent_info, compile_helper* helper )
	{
		if( m_parents.insert( parent_info ).second ) // if we did not already have parent
		{
			if( m_node->NodeTypeGet() != SDB_NODE_TYPE_TERMINAL ) // if not a terminal then push the parent down to the children
			{
				for( node_info_set_t::iterator it = m_children.begin(); it != m_children.end(); ++it )
					(*it)->add_parent( parent_info, helper );
			}
		}
	}

	void add_child( node_info* child_info, compile_helper* helper )
	{
		if( m_children.insert( child_info ).second ) // if we did not already have child
		{
			// if not terminal then if we have any parents something bad has happened
			// I guess we could just push the current set of parents down to the child, but it doesn't seem like this
			// could/should ever happen
			if( m_node->NodeTypeGet() != SDB_NODE_TYPE_TERMINAL )
			{
				if( m_parents.size() )
				{
					Err( ERR_NOT_CONSISTENT, "node_info::add_child @ adding child %s after non terminal %s has parents!",
						 child_info->m_node->to_string().c_str(),
						 m_node->to_string().c_str()
					   );
					throw false;
				}
			}
		}
	}

	SDB_NODE* m_node;
	int m_level;
	int m_index;
	unsigned m_num_visited_parents;
	bool m_visited;
	bool m_added;
	bool m_shared;
	bool m_minimized_level;
	bool m_is_arg;

	node_info_set_t m_children;
	node_info_set_t m_parents;
};

void compile_helper::add_dep( SDB_NODE* node, bool purple )
{
}

compile_helper::~compile_helper()
{
	for( node_info_vec_t::iterator it = m_all_node_info.begin(); it != m_all_node_info.end(); ++it )
		delete *it;
}

int sdb_get_node_level( node_info* info )
{
	if( info->m_minimized_level )
		return info->m_level;
	info->m_minimized_level = true;		// protect against cycles. FIX shouldn't silently fail, we want to detect them too

	DEBUG_SDB_COMPILE() {
		cerr << "minimizing level (current " << info->m_level << " ) for: " << info->m_node->to_string() << endl;
	}

	int level = info->m_level;
	for( node_info_set_t::iterator it = info->m_parents.begin(); it != info->m_parents.end(); ++it )
	{
		int const parent_level = sdb_get_node_level( *it );
		DEBUG_SDB_COMPILE() {
			cerr << "\tparent level: " << parent_level << endl;
		}
		level = my_min( level, parent_level - 1 );
	}
	return info->m_level = level;
}

string level_str( node_info const& info )
{
	ostringstream res;
	res << "(level " << info.m_level << ")";
	return res.str();
}

struct comp_nodes_by_level
{
	bool operator()( node_info* const a, node_info* const b ) const
	{
		return sdb_get_node_level( a ) < sdb_get_node_level( b );
	}
};

void compile_helper::add_deps( SDB_NODE* node )
{
	if( node->IsLiteralQuick() )
		return;

	try
	{
		for( SDB_NODE::children_iterator it = node->all_children_begin(); it != node->all_children_end(); ++it )
		{
			SDB_NODE* const child = *it;
			if( it.at_arg( node ) || !child )
				continue;

			node_visited_map_t::iterator visited_it = m_nodes_map.find( child );

			// if node is visited or not shared then we don't want it (not visited => shared)
			if( visited_it != m_nodes_map.end() && visited_it->second.info && !visited_it->second.info->m_shared )
				continue;

			// literals never invalidate, no need to have deps
			if( child->IsLiteralQuick() && child->RefCountedParents() )
				continue;

			CC_NS(std,pair)<node_int_map_t::iterator,bool> inserted = m_added_compile_deps.insert( node_int_map_t::value_type( child, 0 ) );
			if( inserted.second == false ) // already added
				continue;

			int const index = m_compile_deps.size();
			inserted.first->second = index;	// save away the index
			m_compile_deps.push_back( it.is_purple() ? SDB_NODE_PURPLE_CHILD( child ) : child );
		}
	}
	catch( bool& )
	{
		ErrMore( "Could not add dependencies of %s", node->to_string().c_str() );
		throw;
	}
}

// note that if there is a cycle, then the node being visited is assumed not shared
// i think this is right

node_info *compile_helper::assign_levels( SDB_NODE* n, int curr_level )
{
	if( !n->IsLiteralQuick() && n->NodeTypeGet() != SDB_NODE_TYPE_TERMINAL && n->NodeTypeGet() != SDB_NODE_TYPE_EACH && n->NodeTypeGet() != SDB_NODE_TYPE_ARRAY && n->NodeTypeGet() != SDB_NODE_TYPE_PASSTHRU )
	{
		Err( ERR_UNSUPPORTED_OPERATION, "@- cannot compile graph because %s is an unsupported node type", n->GetText( false ).c_str() );
		throw false;
	}

	CC_NS(std,pair)<node_visited_map_t::iterator,bool> inserted = m_nodes_map.insert( node_visited_map_t::value_type( n, node_visited_info() ) );

	if( !inserted.second )				// if found return the info we've computed
	{
		node_info* const info = inserted.first->second.info;
		if( info )
			info->set_level( curr_level );
		return info;
	}

	bool shared = true;					// assume shared

	node_info_vec_t child_infos;		// info for all children which are split, so we can add them if we turn out not to be shared

	bool is_arg = false;
	if( m_args.find( n ) != m_args.end() ) // args are never shared, never have any children as they are the leaves
	{
		shared = false;
		is_arg = true;
	}
	else
	{
		if( n->IsLiteralQuick() )		// literals are always shared
		{
			inserted.first->second.visited = true;	// no recursion, iterator is still valid, mark visited
			return 0;
		}

		for( SDB_NODE::children_iterator child_it = n->all_children_begin(); child_it != n->all_children_end(); ++child_it )
		{
			SDB_NODE* const child = *child_it;
			if( !child || child_it.at_arg( n ) )
				continue;
			node_info* child_info = assign_levels( child, curr_level - 1 );
			if( child_info && !child_info->m_shared )
			{
				shared = false;
				child_infos.push_back( child_info );
				if( child_it.is_purple() && !child->IsLiteralQuick() )
				{
					Err( SDB_ERR_COMPILE_FAILED, "Cannot compile graph because child %s of %s which depends upon an arg is purple",
						 child->to_string().c_str(),
						 n->to_string().c_str()
					   );
					throw false;
				}
			}
		}
	}

	if( !shared )
	{
		DEBUG_SDB_COMPILE() {
			cerr << "Initially assigning " << curr_level << " to " << n->to_string() << endl;
		}

		node_info* info = new node_info( n );

		info->m_shared = false;
		info->m_is_arg = is_arg;
		info->set_level( curr_level );
		for( node_info_vec_t::iterator child_it = child_infos.begin(); child_it != child_infos.end(); ++child_it )
		{
			(*child_it)->add_parent( info, this );
			info->add_child( *child_it, this );
		}
		// reinsert into map and return
		m_all_node_info.push_back( info );
		return m_nodes_map[ n ].info = info;
	}
	return 0;							// not shared
}

void sdb_node_eval::evaluate( SecDbCompiledGraph* compiled_graph, DT_VALUE* res )
{
	if( m_pass_thru )
	{
		DTM_ASSIGN( res, m_children[0].get_value( compiled_graph ) );
	}
	else if( !m_vti )					// array node
	{
		DEBUG_SDB_COMPILE() {
			cerr << "evaluating vector node" << endl;
		}
		res->DataType = DtGsNodeValues;
		SecDbCompiledVectorValue* vec_val = new SecDbCompiledVectorValue( this, compiled_graph );
		res->Data.Pointer = vec_val;
	}
	else if( m_vti->ValueFlags & SDB_ALIAS )
	{
		DTM_ASSIGN( res, m_children[ m_children.size() - 1].get_value( compiled_graph ) );
	}
	else
	{
		SDB_M_DATA msg_data;

		msg_data.GetValue.ChildData = 0;
		msg_data.GetValue.ChildValues = 0;
		msg_data.GetValue.Db = SecDbRootDb;
		msg_data.GetValue.Value = res;
		msg_data.GetValue.Childc = m_children.size();
		msg_data.GetValue.Argc = m_argc;
		msg_data.GetValue.Node = 0;

		SDB_TRACE_FUNC const trace_func = SecDbTraceFunc;

		if( m_vti->ChildList.ChildDataSize > 0 )
		{
			// need to pull values into struct
			char* child_data = (char*) alloca( m_vti->ChildList.ChildDataSize );
			msg_data.GetValue.ChildData = child_data;

			DEBUG_SDB_COMPILE() {
				cerr << "evaluating " << m_vti->ValueType->Name << endl;
			}
			for( SDB_CHILD_PULL_VALUE* pull_val = m_vti->ChildList.PullValues; pull_val; pull_val = pull_val->Next )
			{
				DEBUG_SDB_COMPILE() {
					cerr << "\tpulling child " << pull_val->ChildNumber << ": ";
					SecDbChildDescription( cerr, &m_vti->ChildList.Terminals[ pull_val->ChildNumber ], &m_vti->ChildList );
					cerr << endl << "\t";
				}
				DT_VALUE* child_val = m_children[ pull_val->ChildNumber ].get_value( compiled_graph );
				DEBUG_SDB_COMPILE() {
					cerr << "\t= " << to_string( child_val ) << endl;
				}

				if( trace_func )
				{
					ostringstream child_str;
					SecDbChildDescription( child_str, &m_vti->ChildList.Terminals[ pull_val->ChildNumber ], &m_vti->ChildList );
					trace_func( SDB_TRACE_COMPILED_GET_CHILD, child_str.str().c_str(), child_val );
				}

				if( child_val )
				{
					if( child_val->DataType->Size == sizeof( double ) )
						*(double*)( child_data + pull_val->Offset ) = child_val->Data.Number;
					else
						*(void const **)( child_data + pull_val->Offset ) = child_val->Data.Pointer;
				}
				else
				{
					if( (*pull_val->DataType)->Flags & DT_FLAG_NUMERIC )
						*(double*)( child_data + pull_val->Offset ) = HUGE_VAL;
					else
						*(void **)( child_data + pull_val->Offset ) = 0;
				}
			}
		}
		else
		{
			DEBUG_SDB_COMPILE() {
				cerr << "evaluating " << m_vti->ValueType->Name << endl;
			}
			DT_VALUE** child_values = (DT_VALUE**) alloca( m_children.size() * sizeof( DT_VALUE* ) );

			DT_VALUE** curr_child = child_values;
			for( sdb_node_value_ref_vec_t::iterator it = m_children.begin(); it != m_children.end(); ++it, ++curr_child )
			{
				DEBUG_SDB_COMPILE() {
					int elem = it - m_children.begin();
					cerr << "pulling elem " << elem << endl << "\t";
					SecDbChildDescription( cerr, &m_vti->ChildList.Terminals[ elem ], &m_vti->ChildList );
					cerr << endl << "\t";
				}
				*curr_child = it->get_value( compiled_graph );
				DEBUG_SDB_COMPILE() {
					cerr << "\t= " << to_string( *curr_child ) << endl;
				}

				if( trace_func )
				{
					ostringstream child_str;
					child_str << "child " << it - m_children.begin();
					trace_func( SDB_TRACE_COMPILED_GET_CHILD, child_str.str().c_str(), *curr_child );
				}
			}

			msg_data.GetValue.ChildValues = child_values;
		}
		res->DataType = m_vti->ValueType->DataType;
		if( !m_vti->Func( 0, SDB_MSG_GET_VALUE, &msg_data, m_vti ) )
		{
			DTM_INIT( res );
			throw false;
		}
		if( trace_func )
			trace_func( SDB_TRACE_COMPILED_CALC, m_vti, res );
	}
	DEBUG_SDB_COMPILE() {
		cerr << "\tresult = " << to_string( res ) << endl;
	}
	return;
}

DT_VALUE SecDbCompiledGraph::evaluate( DT_VALUE* arg_values )
{
	if( m_eval_list.size() == 0 )
	{
		Err( ERR_INVALID_ARGUMENTS, "Trying to evaluate an empty compiled graph" );
		dt_value_var null;
		return null.release();
	}

	m_curr_args = arg_values;

	auto_ptr< dt_value_holder_vec_t > stack( new dt_value_holder_vec_t );
	stack->resize( m_max_slots );
	m_curr_stack = stack.get();
	DEBUG_SDB_COMPILE() {
		cerr << "using a stack of size " << m_max_slots << endl;
	}

	try
	{
		for( unsigned i = 0; i < m_eval_list.size(); ++i )
		{
			dt_value_var res;

			DEBUG_SDB_COMPILE() {
				cerr << "Evaluating elem " << i << endl;
			}
			m_eval_list[ i ].evaluate( this, res.get() );

			DEBUG_SDB_COMPILE() {
				cerr << "putting into " << m_eval_list[i].m_result_elem << endl;
			}
			(*m_curr_stack)[ m_eval_list[i].m_result_elem ] = res.release();
		}
		return (*m_curr_stack)[ m_eval_list.back().m_result_elem ].release();
	}
	catch( bool& )
	{
	}

	// error, return null
	dt_value_var null;
	return null.release();
}

bool SecDbCompiledGraph::compile(
	SDB_NODE*			  f,
	sdb_node_vec_t const& args_vec,
	sdb_node_vec_t&		  compile_deps
)
{
	if( !f->IsTerminal() )
		return Err( ERR_INVALID_ARGUMENTS, "%s is not a terminal, cannot compile", f->GetText( false ).c_str() );

	SdbNodeSet args( 0, SDB_NODE_INVALID_PTR );
	{
		for( sdb_node_vec_t::const_iterator arg_it = args_vec.begin(); arg_it != args_vec.end(); ++arg_it )
			args.insert( *arg_it );
	}

	compile_helper compiler( compile_deps, args );

	m_num_args = args.size();

	////////////////////////////////////////////////////////////////////////////////
	// build the graph first
	////////////////////////////////////////////////////////////////////////////////

	auto_ptr_array<DT_VALUE*> f_args( new DT_VALUE*[ f->ArgcGet() ] );
	for( int i = 0; i < f->ArgcGet(); ++i )
		f_args[i] = f->ArgGet( i )->AccessValue();
	DT_VALUE** argv = f_args.get();

	{
		//time_block __timer( "enum_children" );

		SDB_ENUM_PTR child_enum = 0;
		SDB_CHILD_ENUM_FILTER* filter = new SDB_CHILD_ENUM_FILTER_FLAGS( SDB_ENUM_TREE, 0 );
		filter = new SDB_CHILD_ENUM_FILTER_NONE( filter );
		SecDbChildEnumFirst(
			&child_enum, 
			(SDB_OBJECT*) f->ObjectGet()->AccessValue()->Data.Pointer,
			(SDB_VALUE_TYPE) f->ValueTypeGet()->AccessValue()->Data.Pointer,
			f->ArgcGet(),
			argv,
			filter,
			false
		);
		SecDbChildEnumClose( child_enum );
		delete filter;
	}

	////////////////////////////////////////////////////////////////////////////////
	// traverse graph and
	// 1. assign levels to all nodes
	// 2. determine shared deps
	////////////////////////////////////////////////////////////////////////////////

	{
		//time_block __timer( "assign_levels" );

		compiler.assign_levels( f, 0 );	// 0 is highest level
	}

	////////////////////////////////////////////////////////////////////////////////
	// for each unshared node add to execution list, and sort
	////////////////////////////////////////////////////////////////////////////////

	node_info_vec_t nodes_by_level;
	{
		//time_block __timer( "sorted_nodes_by_level_generate" );

		for( node_info_vec_t::const_iterator it = compiler.m_all_node_info.begin(); it != compiler.m_all_node_info.end(); ++it )
		{
			node_info* const info = *it;
			if( !info->m_shared && !info->m_is_arg )
			{
				nodes_by_level.push_back( info );
				
				if( args.find( info->m_node ) == args.end() )
					compiler.add_deps( info->m_node );
			}
		}

		sort( nodes_by_level.begin(), nodes_by_level.end(), comp_nodes_by_level() );
	}

	////////////////////////////////////////////////////////////////////////////////
	// assign indices to args in order they appear in args_vec
	////////////////////////////////////////////////////////////////////////////////

	{
		for( sdb_node_vec_t::const_iterator arg_it = args_vec.begin(); arg_it != args_vec.end(); ++arg_it )
		{
			node_info* const info = compiler.m_nodes_map[ *arg_it ].info;
			if( info && info->m_is_arg )
				info->m_index = arg_it - args_vec.begin();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// assign slots to nodes which are sorted by level
	////////////////////////////////////////////////////////////////////////////////

	int max_slots = 0;
	{
		//time_block __timer( "assign_slots" );

		CC_STL_SET(int) free_slots;
		for( node_info_vec_t::const_iterator it = nodes_by_level.begin(); it != nodes_by_level.end(); ++it )
		{
			node_info* const info = (*it);

			int slot = 0;
			if( free_slots.size() )
			{
				slot = *free_slots.begin();
				free_slots.erase( slot );
			}
			else
			{
				++max_slots;
				slot = max_slots - 1;
			}

			DEBUG_SDB_COMPILE() {
				cerr << "assigning slot " << slot << " to " << info->m_node->to_string() << level_str( *info ) << ": have " << info->m_parents.size() << " parents to visit" << endl;
			}
			info->m_index = slot;
			info->m_visited = true;

			for( node_info_set_t::const_iterator child_it = info->m_children.begin(); child_it != info->m_children.end(); ++child_it )
			{
				node_info* const child_info = *child_it;

				if( child_info->m_is_arg ) // ignore args as they do not live in the stack
					continue;

				++child_info->m_num_visited_parents;
				DEBUG_SDB_COMPILE() {
					cerr << "\tvisited " << child_info->m_node->to_string() << level_str( *child_info ) << endl;
				}
				if( child_info->m_num_visited_parents == child_info->m_parents.size() )
				{							// child is no longer reachable, remove slot
					if( !child_info->m_shared ) // only have a free slot if we were not shared
					{
						DEBUG_SDB_COMPILE() {
							cerr << "\tFreeing slot " << child_info->m_index << " for " << child_info->m_node->to_string() << " as it is no longer reachable" << endl;
						}
						free_slots.insert( child_info->m_index );
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// generate execution list
	////////////////////////////////////////////////////////////////////////////////

	{
		//time_block __timer( "generate_execution_list" );

		m_max_slots = max_slots;
		CC_STL_MAP_WITH_COMP( SDB_NODE*, int, SDB_NODE::node_less ) literal_to_const;
		for( node_info_vec_t::const_iterator it = nodes_by_level.begin(); it != nodes_by_level.end(); ++it )
		{
			node_info* const info = *it;

			m_eval_list.push_back( sdb_node_eval() );
			sdb_node_eval& last = m_eval_list.back();

			last.m_result_elem = info->m_index;
			last.m_argc = info->m_node->ArgcGet();
			last.m_pass_thru = info->m_node->NodeTypeGet() == SDB_NODE_TYPE_PASSTHRU;
			int childc = info->m_node->ChildrenValidGet() ? info->m_node->ChildrenNum( last.m_vti ) : 0;

			DEBUG_SDB_COMPILE() {
				cerr << "evaluate " << info->m_node->to_string() << " into stack elem " << info->m_index << endl;
			}
			last.m_children.resize( childc );
			for( int i = 0; i < childc; ++i )
			{
				SDB_NODE* child = info->m_node->ChildGet( i );
				sdb_node_value_ref& child_ref = last.m_children[ i ];
				if( child )
				{
					DEBUG_SDB_COMPILE() {
						cerr << "\t" << child->to_string() << " maps to ";
					}
					if( child->IsLiteralQuick() )
					{
						int& id = literal_to_const[ child ];
						if( id == 0 )
						{
							id = literal_to_const.size();

							m_constants.push_back( dt_value_holder() );
							DTM_ASSIGN( m_constants.back().get(), child->AccessValue() );
						}
						child_ref.m_type = sdb_node_value_ref::CONSTANT;
						child_ref.m_elem = id - 1;
						DEBUG_SDB_COMPILE() {
							cerr << " constant " << child_ref.m_elem << endl;
						}
					}
					else
					{
						node_info* child_info = compiler.m_nodes_map[ child ].info;
						if( child_info && !child_info->m_shared )
						{
							child_ref.m_elem = child_info->m_index;
							child_ref.m_type = child_info->m_is_arg ? sdb_node_value_ref::ARG : sdb_node_value_ref::EVAL_NODE;
							DEBUG_SDB_COMPILE() {
								cerr << " compiled_node " << child_ref.m_elem << endl;
							}
						}
						else
						{
							child_ref.m_elem = compiler.m_added_compile_deps[ child ];
							child_ref.m_type = sdb_node_value_ref::NODE;
						}
					}
				}
			}
		}
	}
	//cout << "node_info hash stats " << compiler.m_nodes_map.statistics() << endl;
	//cout << "num nodes: " << compiler.m_nodes_map.size() << "; max_size: " << compiler.m_nodes_map.max_size() << endl;

	m_curr_shared_values = compile_deps;
	return true;
}

CC_NS(std,string) SecDbCompiledGraph::toString() const
{
	return "A compiled graph";
}

CC_END_NAMESPACE

bool SecDbCompileGraph(
	SDB_NODE*						f,
	sdb_node_vec_t const&           args_vec,
	CC_STL_VECTOR( SDB_NODE*)&		compile_deps,
	CC_NS(Gs,SecDbCompiledGraph)&	compiled_graph
)
{
	try
	{
		return compiled_graph.compile( f, args_vec, compile_deps );
	}
	catch( bool& )
	{
		//cerr << "some error was caught" << endl;
		return false;
	}
}

