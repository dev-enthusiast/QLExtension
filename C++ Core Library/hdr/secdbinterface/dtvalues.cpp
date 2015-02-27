#define BUILDING_UBERGLUE

#include <iostream>
#include <sstream>
#include <string>

#include <portable.h>
#include <err.h>
#include <secexpr.h>

#include <uberglue_internal.h>


// Allocate and initialize a new DT_VALUE structure.
ERROR_CODE DtValue_Allocate( const char *data_type_name, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );
        const DT_DATA_TYPE data_type = DtFromName( data_type_name );
        if( !data_type ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                                  "invalid data type name" );
        *result = new_dt_value_handle( data_type );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Returns a wrapped DtDouble on success and a null pointer on error.
ERROR_CODE DtValue_AreEqual( void *x, void *y, uint32_t *result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *x_ = get_dt_value( x );
        DT_VALUE *y_ = get_dt_value( y );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );
        DT_VALUE are_equal;
        ASSERT_NONZERO( DT_OP( DT_MSG_EQUAL, &are_equal, x_, y_ ) );
        ASSERT_NONZERO( are_equal.DataType == DtDouble );
        if( are_equal.Data.Number ) *result = 1;
        else *result = 0;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Retrieve the type name from a DT_VALUE structure.
ERROR_CODE DtValue_GetTypeName( void *handle, const char **const result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *value = get_dt_value( handle );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );
        *result = value->DataType->Name;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Retrieve the number from a DT_VALUE structure.
ERROR_CODE DtValue_GetNumber( void *handle, double *result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );
        *result = object->Data.Number;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Retrieve the number from a DT_VALUE structure.
ERROR_CODE DtValue_SetNumber( void *handle, double number )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        object->Data.Number = number;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Retrieve the pointer from a DT_VALUE structure.
// "const void **const"; yah, thats right. -gpb
ERROR_CODE DtValue_GetPointer( void *handle, const void **const result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );
        *result = object->Data.Pointer;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Retrieve the pointer from a DT_VALUE structure.
ERROR_CODE DtValue_SetPointer( void *handle, void *pointer )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        object->Data.Pointer = pointer;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE DtValue_Convert( void *handle, const char *const new_type_name,
                            void **result)
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        if( !new_type_name ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                                      "new_type_name is null" );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );

        // It appears that DTM_TO just overwrites the data in result.Data. -gpb
        DT_VALUE temp;
        temp.DataType = DtFromName( new_type_name );
        if( !temp.DataType ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                                      "Bad value type name" );
        if( !DTM_TO( &temp, object ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                     "type conversion failed" );
        *result = take_and_wrap( temp );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE DtValue_GetSize( void *handle, int64_t *result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object = get_dt_value( handle );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                               "result is null" );
        *result = numeric_cast<int64_t>( DtSize( object ) );
        if( result < 0 )
            throw UberGlueError( SECDB_CALL_FAILED, "DtSize failed" );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Returns an array containing all subscripts for object.
ERROR_CODE DtValue_ListSubscripts( void *object, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        int i = 0;
        DT_VALUE *object_ = get_dt_value( object );
        OwnedHandle result_ = new_dt_value_handle( DtArray );
        DT_VALUE Enum;
        DTM_FOR( &Enum, object_ )
        {
            if( !DtSubscriptSet( result_, i++, &Enum ) )
            {
                // Not sure if the following DTM_FOR_BREAK is necessary. -gpb
                DTM_FOR_BREAK( &Enum, object_ );
                throw UberGlueError( SECDB_CALL_FAILED,
                                         "DtSubscriptSet failed while building array" );
            }
        }
        *result = result_.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE DtValue_GetSubscriptValue( void *object,
                                      void *subscript,
                                      void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object_ = get_dt_value( object );
        DT_VALUE *subscript_ = get_dt_value( subscript );
        DT_VALUE temp;
        // According to comments in the source code, "DtSubscriptGetCopy
        // will work more often than DtSubscriptGet". -gpb
        if( !DtSubscriptGetCopy( object_, subscript_, &temp ) )
            throw UberGlueError( BAD_SUBSCRIPT,
                                     "Object does not have this subscript." );
        *result = take_and_wrap( temp );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE DtValue_SetSubscriptValue( void *object,
                                      void *subscript,
                                      void *value )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object_ = get_dt_value( object );
        DT_VALUE *subscript_ = get_dt_value( subscript );
        DT_VALUE *value_ = get_dt_value( value );
        // DtSubscriptReplace assumes ownership on success and DTM_FREE's
        // it on error, so we need to make a deep copy to leave the value
        // owned by the caller undisturbed.
        DT_VALUE copy;
        ASSERT_NONZERO( DTM_ASSIGN( &copy, value_ ) );
        if( !DtSubscriptReplace( object_, subscript_, &copy ) )
            throw UberGlueError( SECDB_CALL_FAILED, "DtSubscriptReplace failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE DtValue_DestroySubscript( void *object,
                                     void *subscript )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE *object_ = get_dt_value( object );
        DT_VALUE *subscript_ = get_dt_value( subscript );
        if( !DT_OP( DT_MSG_SUBSCRIPT_DESTROY, 0, object_, subscript_ ) )
            throw UberGlueError( BAD_SUBSCRIPT,
                                     "DT_MSG_SUBSCRIPT_DESTROY failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

