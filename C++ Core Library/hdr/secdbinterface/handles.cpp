#define BUILDING_UBERGLUE

#include <iostream>
#include <sstream>
#include <string>

#include <portable.h>
#include <err.h>
#include <secexpr.h>
#include <secdt.h>

#include <uberglue_internal.h>


// Release resources associated with a value.
ERROR_CODE ReleaseHandle( void *handle )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        free_handle( handle );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


char *get_string( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    return get_data_pointer<char *>( handle, DtString );
}

DT_ARRAY *get_array( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_ARRAY *result = get_data_pointer<DT_ARRAY *>( handle, DtArray );
    logger.omit_traceback();
    return result;
}

DT_VECTOR *get_vector( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_VECTOR *result = get_data_pointer<DT_VECTOR *>( handle, DtVector );
    logger.omit_traceback();
    return result;
}

SDB_DB *get_database( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_DATABASE *dt = get_data_pointer<DT_DATABASE *>( handle, DtDatabase );
    ASSERT_NONZERO( dt->Db );
    logger.omit_traceback();
    return dt->Db;
}

SDB_OBJECT *get_security( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    SDB_OBJECT *result = get_data_pointer<SDB_OBJECT *>( handle, DtSecurity );
    logger.omit_traceback();
    return result;
}

SDB_VALUE_TYPE get_value_type( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    SDB_VALUE_TYPE x = get_data_pointer<SDB_VALUE_TYPE>( handle, DtValueType );
    logger.omit_traceback();
    return x;
}

DT_CURVE *get_curve( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_CURVE *result = get_data_pointer<DT_CURVE *>( handle, DtCurve );
    logger.omit_traceback();
    return result;
}

DT_TCURVE *get_tcurve( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_TCURVE *result = get_data_pointer<DT_TCURVE *>( handle, DtTCurve );
    logger.omit_traceback();
    return result;
}



// Returns a valid void *or throws an exception.
void *new_dt_value_handle( DT_DATA_TYPE data_type )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    SECDB_HANDLE_DATA *handle = new SECDB_HANDLE_DATA();
    handle->debug_string = "DT_VALUE";
    handle->type = SECDB_HANDLE_DATA_TYPE__DT_VALUE;
    handle->delete_on_free = true;
    if( !DTM_ALLOC( &(handle->contents.dt_value), data_type ) )
    {
        delete handle;
        throw UberGlueError( SECDB_CALL_FAILED, "DTM_ALLOC failed" );
    };
    logger.omit_traceback();
    return handle;
}

// Returns a valid void *or throws an exception.
void *new_dt_value_handle( DT_DATA_TYPE data_type, void *pointer )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !pointer )
        throw UberGlueError( BAD_ARGUMENT_ERROR, "pointer is null" );
    SECDB_HANDLE_DATA *result = (SECDB_HANDLE_DATA *)
                                new_dt_value_handle( data_type );
    result->contents.dt_value.Data.Pointer = pointer;
    logger.omit_traceback();
    return result;
}

void *take_and_wrap( DT_VALUE value )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    try
    {
        SECDB_HANDLE_DATA *handle = new SECDB_HANDLE_DATA();
        handle->debug_string = "DT_VALUE";
        handle->type = SECDB_HANDLE_DATA_TYPE__DT_VALUE;
        handle->contents.dt_value = value;
        handle->delete_on_free = true;
        logger.omit_traceback();
        return handle;
    }
    catch(...)
    {
        // If something goes wrong, we still own the value, so we may as
        // well try to free it.
        DTM_FREE( &value );
        throw;
    }
}

void *copy_and_wrap( DT_VALUE value )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_VALUE copy;
    ASSERT_NONZERO( DTM_ASSIGN( &copy, &value ) );
    // We are now responsible for freeing copy up if anything goes wrong.
    try
    {
        SECDB_HANDLE_DATA *handle = new SECDB_HANDLE_DATA();
        handle->debug_string = "DT_VALUE";
        handle->type = SECDB_HANDLE_DATA_TYPE__DT_VALUE;
        handle->contents.dt_value = copy;
        handle->delete_on_free = true;
        logger.omit_traceback();
        return handle;
    }
    catch(...)
    {
        DTM_FREE( &copy );
        throw;
    }
}

void *copy_and_wrap( DT_VALUE *value )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !value ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                          "value is null" );
    void *result = copy_and_wrap( *value );
    logger.omit_traceback();
    return result;
}

void free_handle( void *handle  )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !handle )
        throw UberGlueError( BAD_ARGUMENT_ERROR, "handle is null" );
    SECDB_HANDLE_DATA *handle_ = (SECDB_HANDLE_DATA *) handle;
    if( handle_->type == SECDB_HANDLE_DATA_TYPE__DT_VALUE )
    {
        ASSERT_NONZERO( handle_->delete_on_free );
        const bool succeeded = DTM_FREE( &(handle_->contents.dt_value) );
        delete handle_;
        if( !succeeded )
            throw UberGlueError( SECDB_CALL_FAILED, "DTM_ALLOC failed" );
    }
    else if( handle_->type == SECDB_HANDLE_DATA_TYPE__VARIABLE_SCOPE )
    {
        // I guess this can't actually fail, but we may as well check in
        // case this ever changes. -gpb
        if( handle_->delete_on_free )
        {
            HASH *hash = handle_->contents.variable_scope;
            ASSERT_NONZERO( hash );
            const bool succeeded = SlangVariableHashDestroy( hash );
            delete handle_;
            if( !succeeded )
                throw UberGlueError( SECDB_CALL_FAILED,
                                     "SlangVariableHashDestroy failed" );
        }
    }
    else
    {
        throw UberGlueError( BAD_ARGUMENT_ERROR,
                             "handle does not appear to be valid." );
    }
    logger.omit_traceback();
}

DT_VALUE *get_dt_value( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !handle )
        throw UberGlueError( BAD_ARGUMENT_ERROR, "handle is null" );
    SECDB_HANDLE_DATA *handle_ = (SECDB_HANDLE_DATA *) handle;
    if( handle_->type != SECDB_HANDLE_DATA_TYPE__DT_VALUE )
    {
        const char *msg = "handle does not correspond to a DT_VALUE structure.";
        throw UberGlueError( BAD_ARGUMENT_ERROR, msg );
    }
    logger.omit_traceback();
    return &( handle_->contents.dt_value );
}

DT_VALUE *get_dt_value( void **handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !handle ) throw UberGlueError( BAD_ARGUMENT_ERROR, "handle is null" );
    logger.omit_traceback();
    return get_dt_value( *handle );
}

DT_VALUE *get_dt_value( void *handle, DT_DATA_TYPE expected_data_type )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    DT_VALUE *result = get_dt_value( handle );
    if( result->DataType != expected_data_type )
    {
        std::ostringstream ss;
        ss << "handle does not have datatype " << expected_data_type->Name;
        throw UberGlueError( BAD_ARGUMENT_ERROR, ss.str() );
    }
    logger.omit_traceback();
    return result;
}


void *new_local_variable_scope_handle( const char *name )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !name ) name = "Anonymous local variable scope";
    // See SlangScopePushVariableHash in secdb/src/sdb_eval.c
    SECDB_HANDLE_DATA *result = new SECDB_HANDLE_DATA();
    result->debug_string = "HASH *";
    result->type = SECDB_HANDLE_DATA_TYPE__VARIABLE_SCOPE;
    result->delete_on_free = true;
    result->contents.variable_scope = HashCreate( name, (HASH_FUNC) HashStrHash,
                                                  (HASH_CMP) HashStricmp, 0,
                                                  NULL );
    if( !result->contents.variable_scope )
    {
        delete result;
        throw UberGlueError( SECDB_CALL_FAILED, "HashCreate failed." );
    }
    logger.omit_traceback();
    return result;
}

// According to previous comments by Jason O'Brien, there is no way to
// delete a named variable scope.  We are actually counting on this here;
// the HASH * wrapped inside the void *cannot become invalid. -gpb
void *get_named_variable_scope_handle( const char *name, bool create )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !name )
        throw UberGlueError( BAD_ARGUMENT_ERROR, "name is null" );
    ASSERT_NONZERO( SlangVarScopeHash );
    SLANG_VARIABLE_SCOPE *scope = SlangVariableScopeGet( name, create );
    if( !scope )
    {
        std::ostringstream ss;
        ss << "There is no variable scope named " << name << ".";
        throw UberGlueError( BAD_SCOPE_NAME, ss.str() );
    }
    ASSERT_NONZERO( scope->VariableHash );
    SECDB_HANDLE_DATA *result = new SECDB_HANDLE_DATA();
    result->debug_string = "HASH *";
    result->type = SECDB_HANDLE_DATA_TYPE__VARIABLE_SCOPE;
    result->delete_on_free = false;
    result->contents.variable_scope = scope->VariableHash;
    logger.omit_traceback();
    return result;
}

// Return a valid HASH * or throws an exception.
HASH *get_variable_scope( void *handle )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 2 );
    if( !handle )
        throw UberGlueError( BAD_ARGUMENT_ERROR, "handle is null" );
    SECDB_HANDLE_DATA *handle_ = (SECDB_HANDLE_DATA *) handle;
    if( handle_->type != SECDB_HANDLE_DATA_TYPE__VARIABLE_SCOPE )
    {
        const char *msg = "Handle does not correspond to a variable scope.";
        throw UberGlueError( BAD_ARGUMENT_ERROR, msg );
    }
    logger.omit_traceback();
    return handle_->contents.variable_scope;
}
