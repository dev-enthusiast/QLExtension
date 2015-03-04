#define GSCVSID "$Header: /home/cvs/src/secdb/src/async_result.cpp,v 1.15 2013/10/07 19:33:17 khodod Exp $"
/****************************************************************
**
**	async_result.cpp	- Async result handler
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: async_result.cpp,v $
**	Revision 1.15  2013/10/07 19:33:17  khodod
**	Address obvious cases of format-string vulnerabilities.
**	#315536
**
**	Revision 1.14  2004/11/29 23:48:11  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <secdb/async_result.h>

#include <algorithm>
CC_USING( std::for_each );

#include <functional>
CC_USING( std::bind1st );
CC_USING( std::mem_fun );

#include <memory>
CC_USING( std::auto_ptr );

Sdb_Async_Results* sdb_async_results = 0;

bool Sdb_Async_Result_Completed::apply_result()
{
	if( !m_node )
		return Err( ERR_UNKNOWN, "Sdb_Async_Result_Completed::apply_result(): attempted to apply a result to an unidentified node." );

	m_node->m_AsyncResult = false;

	BSTREAM stream;
	BStreamInit( &stream, &m_stream[0], m_stream.size() );

	int type = BStreamGetInt32( &stream );
	DT_DATA_TYPE dt = DtFromID( type );
	if( !dt )
		return Err( ERR_UNKNOWN, "Sdb_Async_Result_Completed::apply_result(): Unknown datatype id %d in stream from server", type );

	if( !DtValueFromStream( &stream, dt, &m_node->m_Value ) )
		return m_node->Error( "Sdb_Async_Result_Completed::apply_result() cannot unstream result" );

	m_node->ValidSet( true );

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_APPLY_ASYNC_RESULT, m_node );

	return true;
}

bool Sdb_Async_Failed_Request::apply_result()
{
	if( !m_node )
		return Err( ERR_UNKNOWN, "Sdb_Async_Failed_Request::apply_result(): attempted to apply an error to an unidentified node." );

	m_node->m_AsyncResult = false;

	m_node->Error( "Sdb_Async_Failed_Request::apply_result()" );
	ErrMore( "%s", error.c_str() );

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_ERROR_NODE, m_node );

	return false;
}

Sdb_Async_Results::Sdb_Async_Results()
: m_handler( 0 ), m_synchronizer( 0 ), m_current_result_list( 0 )
{
}

Sdb_Async_Results::~Sdb_Async_Results()
{
}

// given a result returned by a server assign it to the correct Result_List by synchronizing node
void Sdb_Async_Results::assign_result( Sdb_Async_Result* res )
{
	Node_Pair req_info;
	if( !m_outstanding_requests.remove( res->m_id, req_info ) )
		delete res;						// unknown result, throw it away
	else
	{
		// Found result in outstanding request map
		// this gives us the node it refers to as well as the synchronizing node
		res->m_node = req_info.first;
		SDB_NODE* const synchronizer = req_info.second;

		// cancelled jobs can still return results later. They shouldn't be in the outstanding requests
		// but say they are, and we can't find the synchronizing node, then we just throw away the result
		// we have no idea if the target node even exists.

		Result_List_By_Node::iterator it1 = m_result_lists.find( synchronizer );
		if( it1 == m_result_lists.end() )
			delete res;				// unknown result, throw away
		else
		{
			Result_List& list = (*it1).second;
			// mark the result as completed and squirrel it away in the completed list
			list.m_outstanding.erase( res->m_id );
			list.m_completed.push( res );
		}
	}
}

// Push a new synchronizer node
Sdb_Async_Results::Synchronize_Guard Sdb_Async_Results::push_synchronizer( SDB_NODE* node )
{
	Synchronize_Guard_t *guard = new Synchronize_Guard_t( this, m_synchronizer, m_current_result_list );

	m_synchronizer = node;
	m_current_result_list = 0;
	return auto_ptr<Synchronize_Guard_t>( guard );
}

// Pop a synchronizer node, cancel all outstanding requests
// empty completed result queue leaving nodes invalid
void Sdb_Async_Results::pop_synchronizer()
{
	if( m_current_result_list == 0 )	// no async requests were fired
		return;

	Result_List& list = *m_current_result_list;

	// cancel all the outstanding jobs
	// note that this includes jobs which have completed but are waiting on the results resource queue
	// they will be flushed when someone tries to get a legitimate result
	for_each( list.m_outstanding.begin(), list.m_outstanding.end(),
			  bind1st( mem_fun( &Sdb_Async_Results::cancel_job ), this ) );

	// for all completed results which were assigned to us, we go through and apply them
	while( !list.m_completed.empty() )
	{
		auto_ptr<Sdb_Async_Result> res( list.m_completed.top() );
		list.m_completed.pop();
		// note we must apply all completed, uncancelled results
		// else the nodes will be left marked async which would suck
		res->apply_result();
	}

	m_result_lists.erase( m_synchronizer );
}

bool Sdb_Async_Results::schedule( SDB_NODE* node, SDB_SEC_TYPE sec_type, char const* vt, DT_VALUE* child_values )
{
	static long next_id = 1;

	if( !m_current_result_list )
		m_current_result_list = &m_result_lists[ m_synchronizer ];

	long id = next_id++;
	if( !m_handler->schedule( sec_type, vt, child_values, id ) )
		return false;

	// if we scheduled it successfully then insert the id into the outstanding set for current result list
	m_current_result_list->m_outstanding.insert( id );
	// and also add it to the pool of all outstanding requests so that we can match it to the synchronizer
	m_outstanding_requests.insert( id, Node_Pair( node, m_synchronizer ) );

	// mark the node as having an async result
	// we guarantee to unmark it when this synchronizer block is popped or a result/error is returned
	// note that you don't want to repeat a getvalue after an async error because you will end up rescheduling it
	node->m_AsyncResult = true;

	return true;
}

bool Sdb_Async_Results::cancel_job( long id )
{
	Node_Pair req_info;
	if( m_outstanding_requests.remove( id, req_info ) )
	{
		SDB_NODE* target = req_info.first;
		target->m_AsyncResult = false; // unset async result flag since we are cancelling the job
	}
	if( m_handler )					// if no handler, can't cancel
		m_handler->cancel_job( id );
	return true;					// return true for compiler compat where mem_fun must return non void
}
