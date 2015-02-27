#define BUILDING_UBERGLUE

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <iterator>
#include <algorithm>
#include <cctype>

#include <portable.h>
#include <err.h>

#include <uberglue_internal.h>

// This variable holds the global functin/local variable stacks and the
// global diddle stack.
SLANG_SCOPE *scope_stack = 0;

ERROR_CODE handle_exceptions()
{
    try
    {
        std::rethrow_exception( std::current_exception() );
    }
    catch( const UberGlueError &exception )
    {
        ErrMore( exception.what() );
        return exception.error_code;
    }
    catch( const std::exception &exception )
    {
        ErrMore( exception.what() );
        return UNKNOWN_CPP_ERROR;
    }
    catch(...)
    {
        ErrMore( "Unknown C++ Exception" );
        return UNKNOWN_CPP_ERROR;
    }
    return UNKNOWN_CPP_ERROR;
}


// Handle typical STDOUT-type stuff.
static void message( const char *msg )
{
	std::cout << "SecDB: " << msg << "\n";
}

// Handle typical STDERR-type stuff.
static void error_message( const char *msg )
{
	std::cerr << "SecDB Error: " << msg << "\n";
}

// Initialize SecDb.
ERROR_CODE Connect( const char *application, const char *username,
                    const char *database, const char *sourceDatabase,
                    int32_t dead_pool_size )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( dead_pool_size == -1 )
            dead_pool_size = SDB_DEFAULT_DEADPOOL_SIZE;
        if( !SecDbStartup( database, numeric_cast<int>( dead_pool_size ),
                           message, error_message, application, username ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "Failed to connect to database." );
        if( sourceDatabase && !SecDbSourceDbSet( sourceDatabase ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "Failed to set source database." );
        scope_stack = SlangScopeCreate();
        if( !scope_stack )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangScopeCreate failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE LastError( const char **result )
{
    try
    {
        // You should not set up a traceback in this fuction, as you have
        // to call this function to get the traceback for the previous
        // failure. -gpb
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );
        *result = ErrGetString();
        if( !( *result ) ) throw UberGlueError( NO_LAST_ERROR,
                                                    "there is no last error" );
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Get last registered error code.
ERROR_CODE LastErrorCode( int64_t *result )
{
    try
    {
        // You should not set up a traceback in this fuction, as you have
        // to call this function to get the traceback for the previous
        // failure. -gpb
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                           "result is null" );
        *result = numeric_cast<int64_t>( ErrNum );
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// RAII wrapper for a SLANG_NODE.
class SlangNodeScopeGuard
{
  protected:
    SLANG_NODE *_ptr;
  public:
    SlangNodeScopeGuard( SLANG_NODE *slangNode )  : _ptr(slangNode) { };
    ~SlangNodeScopeGuard() { ASSERT_NONZERO( _ptr ); SlangFree( _ptr ); };
};

// RAII wrapper for a SLANG_RET.
class SlangRetScopeGuard
{
  protected:
    SLANG_RET *_ptr;
  public:
    SlangRetScopeGuard( SLANG_RET *ptr ) : _ptr( ptr ) { };
    ~SlangRetScopeGuard() { ASSERT_NONZERO( _ptr ); SLANG_RET_FREE( _ptr ); };
};

ERROR_CODE SlangEval( const char *slang_expression, int32_t *return_code,
                      int32_t *result_type, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !slang_expression )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "slang_expression is null" );
        if( !return_code )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "return_code is null" );
        if( !result_type )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result_type is null" );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );
        if( SlangFunctionDepth <= 0 )
        {
            const char *msg = "There are no variable scopes "
                              "on the stack.";
            throw UberGlueError( EMPTY_STACK_ERROR, msg );
        }

        SLANG_NODE *node = SlangParse( slang_expression, "Scripting Interface",
                                       SLANG_MODULE_STRING );
        if( !node )
            throw UberGlueError( SECDB_CALL_FAILED, "SlangParse failed." );
        SlangNodeScopeGuard node_guard = node;

        SLANG_RET ret;
        ASSERT_NONZERO( scope_stack );
        *return_code = numeric_cast<int32_t>(
            SlangEvaluate( node, SLANG_EVAL_RVALUE, &ret, scope_stack ) );
        // Looking at SlangEvalExpr in dotnetglue/src/SecDbAPI_Impl.cpp, it
        // appears that we should always free Ret.  -gpb
        SlangRetScopeGuard ret_guard = &ret;

        // FIXME: We should probably do some validation on result_type. -gpb
        *result_type = numeric_cast<int32_t>( ret.Type );
        if( *return_code != 0 )
            throw UberGlueError( SLANG_EVAL_FAILED,
                                 "SlangEvaluate call failed." );
        // We need to copy the value out of ret, because ret must be free'd.
        *result = copy_and_wrap( ret.Data.Value );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE NewString( const char *text, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !text )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "text is null" );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        DT_VALUE temp;
        temp.DataType = DtString;
        temp.Data.Pointer = text;

        *result = copy_and_wrap( temp );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Create a new vector from size and double's.  Copies the data in buffer.
ERROR_CODE NewVector( uint64_t size, double *buffer, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !buffer )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "buffer is null" );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        DT_VECTOR source;
        // static_assert(std::is_same<decltype(source.Size), int>::value,
        //               "DT_VECTOR has changed.");
        source.Size = numeric_cast<decltype(source.Size)>( size );
        // static_assert(std::is_same<decltype(source.Alloc), int>::value,
        //               "DT_VECTOR has changed.");
        source.Alloc = numeric_cast<decltype(source.Alloc)>( size );
        source.Elem = buffer;

        DT_VALUE wrapper;
        wrapper.DataType = DtVector;
        wrapper.Data.Pointer = &source;

        // Make a copy of the data; nothing prior to this point can leak;
        *result = copy_and_wrap( wrapper );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Returns a pointer into the data contained by vector..
ERROR_CODE Vector_GetDataView( void *vector, uint64_t *size, double **const data )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VECTOR *vec = get_vector( vector );
        if( !size ) throw UberGlueError( BAD_ARGUMENT_ERROR, "size is null" );
        if( !data ) throw UberGlueError( BAD_ARGUMENT_ERROR, "data is null" );
        *size = numeric_cast<uint64_t>( vec->Size );
        *data = vec->Elem;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}



// std::vector<DT_VALUE *> extract_args( void *args_array )
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     std::vector<DT_VALUE *> result;
//     if( args_array )
//     {
//         DT_ARRAY *array_p = get_data_pointer<DT_ARRAY *>( args_array, DtArray );
//         result.resize( array_p->Size, 0 );
//         for(int i = 0; i < array_p->Size; i++ )
//         {
//             result[i] = &(array_p->Element[i]);
//         }
//     }
//     return result;
// }

// Insert value into array at index.  If index < Size( array ), the element
// is inserted between the elements currently at index and index + 1.  If
// index >= Size( array ), this is reduces to setting index equal to value
// which implicitly sets any elements from Size( array ) through index - 1
// to Null.  After this operation the array contains a deep copy of value.
ERROR_CODE  Array_InsertElement( void *array, uint64_t index,
                                 void *value )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_ARRAY *const array_ = get_array( array );
        const int index_ = numeric_cast<int>( index );

        // I'm going to be more strict about index than DtArrayInsert. -gpb
        if( index > numeric_cast<uint64_t>( array_->Size ) )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "invalid index" );

        ASSERT_NONZERO( DtArrayInsert( array_, index_ , 1 ) );
        ASSERT_NONZERO( DtSubscriptSet( get_dt_value( array ), index_,
                                        get_dt_value( value ) ) );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Array_DestroyElements( void *array, const uint64_t index,
                                  const uint64_t count )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_ARRAY *array_ = get_array( array );
        const uint64_t size = numeric_cast<uint64_t>( array_->Size );

        // I'm going to be more strict about index and count than
        // DtArrayDelete; this can only help. -gpb
        if( index >= size )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "invalid index" );
        // The following can't overflow. -gpb
        if( count > size - index )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "invalid count" );
        ASSERT_NONZERO( DtArrayDelete( array_, index, count ) );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// extern "C" { DT_CURVE *DtCurveAlloc( int ); }

ERROR_CODE NewCurve( uint64_t length, int64_t *dates, double *values,
                     void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_CURVE *curve = DtCurveAlloc( numeric_cast<int>( length ) );
        if( !curve )
            throw UberGlueError( SECDB_CALL_FAILED, "DtCurveAlloc failed" );
        OwnedHandle result_ = new_dt_value_handle( DtCurve, curve );
        for( uint64_t i=0; i < length; i++ )
        {
            curve->Knots[i].Date = dates[i];
            curve->Knots[i].Value = values[i];
        }
        *result = result_.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE Curve_GetDates( void *curve, uint64_t length, int64_t *dates )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_CURVE *curve_ = get_curve( curve );
        if( length != numeric_cast<uint64_t>( curve_->KnotCount ) )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "wrong length" );
        if( !dates )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "dates is null" );

        for( uint64_t i=0; i < length; i++ )
        {
            dates[i] = curve_->Knots[i].Date;
        }
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE Curve_GetValues( void *curve, uint64_t length, double *values )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_CURVE *curve_ = get_curve( curve );
        if( length != numeric_cast<uint64_t>( curve_->KnotCount ) )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "wrong length" );
        if( !values )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "values is null" );

        for( uint64_t i=0; i < length; i++ )
        {
            values[i] = curve_->Knots[i].Value;
        }
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE NewTCurve( uint64_t length, int64_t *timestamps, double *values,
                      void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        int size = numeric_cast<int>( length );
        DT_TCURVE *tcurve = DtCurveAlloc( size );
        if( !tcurve )
            throw UberGlueError( SECDB_CALL_FAILED, "DtCurveAlloc failed" );
        OwnedHandle result_ = new_dt_value_handle( DtTCurve, tcurve );
        for( uint64_t i=0; i < length; i++ )
        {
            tcurve->Knots[i].Date = timestamps[i];
            tcurve->Knots[i].Value = values[i];
        }
        *result = result_.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE TCurve_GetTimestamps( void *tcurve, uint64_t length,
                                 int64_t *timestamps )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_TCURVE *tcurve_ = get_tcurve( tcurve );
        if( length != numeric_cast<uint64_t>( tcurve_->KnotCount ) )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "wrong length" );
        if( !timestamps )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "timestamps is null" );

        for( uint64_t i = 0; i < length; i++ )
            timestamps[i] = tcurve_->Knots[i].Date;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE TCurve_GetValues( void *tcurve, uint64_t length, double *values )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_TCURVE *tcurve_ = get_tcurve( tcurve );
        if( length != numeric_cast<uint64_t>( tcurve_->KnotCount ) )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "wrong length" );
        if( !values )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "values is null" );

        for( uint64_t i = 0; i < length; i++ )
            values[i] = tcurve_->Knots[i].Value;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

