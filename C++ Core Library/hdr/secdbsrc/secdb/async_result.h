/****************************************************************
**
**	ASYNC_RESULT.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/async_result.h,v 1.17 2013/08/13 05:28:47 khodod Exp $
**
****************************************************************/

#ifndef IN_SECDB_ASYNC_RESULT_H
#define IN_SECDB_ASYNC_RESULT_H

#include <secdb.h>
#include <secnodei.h>
#include <sdb_base.h>
#include <datatype.h>

#include <memory>  // for std::auto_ptr
#include <vector>  // for std::vector
#include <string>  // for std::string
#include <stack>   // for std::stack
#include <utility> // for std::pair
#include <ccstl.h>

#include <gsthread/resource_queue.h>

struct EXPORT_CLASS_SECDB Sdb_Async_Result
{
	Sdb_Async_Result( long _id ) : m_id( _id ), m_node( 0 ) {}
	virtual ~Sdb_Async_Result() {}

	// the node must have been assigned else this will fail
	virtual bool apply_result() = 0;

	virtual bool is_failure() = 0;

	long      m_id;
	SDB_NODE* m_node;					// this is assigned later when the result is identified
};

struct EXPORT_CLASS_SECDB Sdb_Async_Result_Completed : public Sdb_Async_Result
{
	Sdb_Async_Result_Completed( long _id ) : Sdb_Async_Result( _id )
	{}

	virtual bool is_failure() { return false; }
	virtual bool apply_result();

	CC_STL_VECTOR(unsigned char) m_stream;
};

struct EXPORT_CLASS_SECDB Sdb_Async_Failed_Request : public Sdb_Async_Result
{
	Sdb_Async_Failed_Request( long _id ) : Sdb_Async_Result( _id )
	{}

	virtual bool is_failure() { return true; }
	virtual bool apply_result();

	CC_NS(std,string) error;
};

struct EXPORT_CLASS_SECDB Sdb_Async_Results
{
	// type definitions
	struct Scheduler
	{
		Scheduler() {}
		virtual ~Scheduler() {}

		virtual bool schedule( SDB_SEC_TYPE sec_type, char const* vt, DT_VALUE* child_values, long request_id ) = 0;
		
		virtual void cancel_job( long request_id ) = 0;
	};

    typedef CC_STL_STACK( Sdb_Async_Result* ) Result_Stack;

	struct Result_List
	{
		Result_List() {}

		Result_Stack       m_completed;
		CC_STL_SET( long ) m_outstanding;
	};

	typedef CC_NS(Gs, GsMutex )                              Mutex;      
	typedef CC_NS(Gs, GsMutexLock )                          Mutex_Lock;
    typedef CC_NS(Gs, GsResourceQueue) < Sdb_Async_Result* > Result_Queue;
    typedef CC_NS(std,pair)< SDB_NODE*, SDB_NODE* >          Node_Pair;
    typedef CC_STL_MAP( long      , Node_Pair )              Outstanding_Requests_Map;
    typedef CC_STL_MAP( SDB_NODE* , Result_List )            Result_List_By_Node;
	typedef CC_STL_VECTOR( long )							 long_vec_t;

	// safe accessor for outstanding requests since this may be posted/queried from
	// multiple threads. scheduler/abort are the important ones
	struct Outstanding_Requests_t
	{
		Outstanding_Requests_t() {}
		~Outstanding_Requests_t() {}

		// attempt to remove request 'id'. return true if found, false if not
		bool remove( long id, Node_Pair& req_info )
		{
			Mutex_Lock ml( m_mut );
			Outstanding_Requests_Map::iterator it = m_requests.find( id );
			if( it != m_requests.end() )
			{
				req_info = it->second;
				m_requests.erase( it );
				return true;
			}
			return false;
		}

		void insert( long id, Node_Pair const& req_info )
		{
			Mutex_Lock ml( m_mut ); m_requests[ id ] = req_info;
		}

		void request_ids( long_vec_t& ids )
		{
			Mutex_Lock ml( m_mut );
			ids.reserve( m_requests.size() );
			for( Outstanding_Requests_Map::const_iterator it = m_requests.begin(); it != m_requests.end(); ++it )
				ids.push_back( it->first );
		}

	private:
		Mutex m_mut;
		Outstanding_Requests_Map m_requests;
	};

	// Destructor pops back to the previous synchronizer node
	//   Note that any collected results which you have not waited for will be reaped
	//   Any outstanding results will be cancelled
	// It's no copy and no construct from the outside for obvious reasons
	struct Synchronize_Guard_t
	{
		~Synchronize_Guard_t()
		{
			m_results->pop_synchronizer();
			m_results->m_synchronizer = m_old_node;
			m_results->m_current_result_list = m_old_list;
		}

		friend struct Sdb_Async_Results;

	private:
		Synchronize_Guard_t( Sdb_Async_Results* results, SDB_NODE* old_node, Result_List* old_list )
		: m_results( results ), m_old_node( old_node ), m_old_list( old_list ) {}
		Synchronize_Guard_t( Synchronize_Guard_t& ) {} // no copy

        Sdb_Async_Results* m_results;
        SDB_NODE*          m_old_node;
        Result_List*       m_old_list;
	};

	typedef CC_NS(std,auto_ptr)<Synchronize_Guard_t> Synchronize_Guard;

	friend struct Synchronize_Guard_t;

	// operations for the dispatcher

    Scheduler *register_handler( Scheduler* handler ) { Scheduler* old = m_handler; m_handler = handler; return old; }

    void add_result( Sdb_Async_Result* res )          { m_unprocessed_results.add_resource( res ); }

	// graph operations

	bool async_servers_available() { return m_handler != 0; }

	// push a synchronizer block, the destructor of the Synchronize_Guard will pop it
	//   this is used to control which results you wait for. Only requests which are scheduled
	//   within a synchronize block are waited for
	Synchronize_Guard push_synchronizer( SDB_NODE* node );

	// note that only requests which you scheduled during the current synchronizer block will be returned
	// the others will be queued

	// number of outstanding async results
	size_t outstanding_async_results()
	{
		return m_current_result_list ? m_current_result_list->m_outstanding.size() : 0;
	}

	// retrieve an outstanding results, NULL only if no outstanding, will block
	Sdb_Async_Result* retrieve_result()
	{
		return m_retrieve_result( true );
	}

	// try to retrieve a result, NULL if no outstanding or would block
	Sdb_Async_Result* try_retrieve_result()
	{
		return m_retrieve_result( false );
	}

	// attempt to schedule a vt
	// the current synchronizer block is used to control who will wait for it
	bool schedule( SDB_NODE* node, SDB_SEC_TYPE sec_type, char const* vt, DT_VALUE* child_values );

	void outstanding_request_ids( long_vec_t& request_ids )
	{
		m_outstanding_requests.request_ids( request_ids );
	}

	Sdb_Async_Results();

	// there should be no outstanding results by the time we get here as the Synchronize_Guard has cancelled them
	~Sdb_Async_Results();

private:
    Scheduler*               m_handler;

	// the current synchronizing node which will wait for all the async results in this subgraph
    SDB_NODE*                m_synchronizer;

	// Result lists by synchronizing node
    Result_List_By_Node      m_result_lists;
	Result_List*             m_current_result_list;	// result list for m_synchronizer

	// Resource Queue of unprocessed results which have been returned by calc servers
    Result_Queue             m_unprocessed_results;

	// Outstanding requests by request id
	// maps to a pair of <Node,Synchronizing Node>
    Outstanding_Requests_t m_outstanding_requests;

	////////////////////////////////////////////////////////////////////////////////
	// Private operations

	void pop_synchronizer();

	// assign the result to the correct Result_List based on the synchronizer node which its ID gives us
	void assign_result( Sdb_Async_Result* res );

	Sdb_Async_Result* m_retrieve_result( bool block )
	{
		if( m_current_result_list == 0 )
			return 0;

		Result_List& results = *m_current_result_list;
		while( results.m_completed.empty() && results.m_outstanding.size() > 0 )
		{
			Sdb_Async_Result* res;
			if( block )
				res = m_unprocessed_results.take_resource();
			else if( !m_unprocessed_results.trytake_resource( res ) )
				return 0;				// no available and non block, so return 0

			assign_result( res );
		}
		if( results.m_completed.empty() )
			return 0;

		Sdb_Async_Result* res = results.m_completed.top();
		results.m_completed.pop();
		return res;
	}

	// Cancel a job and unmark the node as waiting for an async result
	// note that this will not remove the job from the Result_List for the synchronizing node
	// but this is invoked when the Result_List is going away. thats why its private :-)
	bool cancel_job( long id );
};

EXPORT_C_SECDB Sdb_Async_Results* sdb_async_results;

#endif 
