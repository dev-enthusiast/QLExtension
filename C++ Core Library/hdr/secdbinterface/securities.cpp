//
//  securities.cpp
//
//  Contains code related to security objects and classes.
//

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


//////////////////////////////////////////////////////////////////////
//
//  Internal utility functions and classes
//

class SecurityClassNameIterator
{
  protected:
    SDB_ENUM_PTR _ptr;
    SDB_CLASS_INFO *_next;
  public:
    SecurityClassNameIterator( )  { _next = SecDbClassEnumFirst( &_ptr ); };
    const char *next()
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_CLASS_INFO *this_result = _next;
        const char *name = 0;
        if( this_result )
        {
            ASSERT_NONZERO( this_result->Name );
            _next = SecDbClassEnumNext( _ptr );
            name = this_result->Name;
        }
        logger.omit_traceback();
        return name;
    };
    ~SecurityClassNameIterator() { SecDbClassEnumClose( _ptr ); };
};

// // Converts a SecDB-Array to an C-arrays of DT_VALUE *'s.
// class ArgumentExtractor
// {
//   protected:
//     std::vector<DT_VALUE *> argument_vector;

//   public:
//     ArgumentExtractor( void *argument_array )
//     {
//         TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//         if( argument_array )
//         {
//             DT_ARRAY *array_p = get_array( argument_array );
//             argument_vector.resize( array_p->Size, 0 );
//             for(int i = 0; i < array_p->Size; i++ )
//             {
//                 argument_vector[i] = &(array_p->Element[i]);
//             }
//         }
//     };

//     int argc() { return numeric_cast<int>( argument_vector.size() ); };
//     DT_VALUE **argv() { return argc() > 0 ? &argument_vector[0] : 0; };
// };

SDB_SEC_TYPE get_security_type( const char *class_name )
{
    SDB_SEC_TYPE security_type = SecDbClassTypeFromName( class_name );
    if( !security_type )
    {
        std::ostringstream msg;
        msg << "Bad security class name: " << class_name;
        throw UberGlueError( BAD_CLASS_NAME_ERROR, msg.str() );
    }
    return security_type;
}


std::string to_upper( const char *s )
{
    std::string result;
    std::transform( s, s + strlen( s ), std::back_inserter( result ),
                    (int (*)(int))std::toupper );
    return result;
}



//////////////////////////////////////////////////////////////////////
//
//  API functions.  See uberglue.h for documentation.
//

ERROR_CODE FindSecurities( void *database, const char *class_name,
                           const char *start, uint32_t exclude_start,
                           const uint32_t limit, void **result )
{
    // Should this use SecDbGetSecurityNamesInRange? -gpb
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_DB *db = database ? get_database( database ) : SecDbRootDb;
        ASSERT_NONZERO( db );
        SDB_SEC_TYPE security_type = 0;
        if( class_name )
            security_type = get_security_type( class_name );
        if( !start )
            start = "";
        if( !limit )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "limit is zero" );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        // Increasing table_size further seems to cause problems; I believe
        // this is due to a cast to int16 somewhere inside
        // SecDbNameLookupMany. -gpb
        const unsigned int table_size = 1000;
        SDB_SEC_NAME_ENTRY *table = new SDB_SEC_NAME_ENTRY[table_size];
        std::unique_ptr<SDB_SEC_NAME_ENTRY[]> table_guard(table);
        OwnedHandle array = new_dt_value_handle( DtArray );
        DT_VALUE temp;
        temp.DataType = DtString;
        int get_type = exclude_start ? SDB_GET_GREATER : SDB_GET_GE;
        std::string security_name = start;
        uint32_t i = 0;

        // Now we build the actual result array.
        while( i < limit )
        {
            unsigned int count = numeric_cast<unsigned int>(
                SecDbNameLookupMany( security_name.c_str(), table, table_size,
                                     security_type, get_type, db ) );
            // If SecDbNameLookupMany returns no results, then we have
            // found all the securities.
            if( !count ) break;
            for( unsigned int j = 0; j < count; i++, j++ )
            {
                if( i >= limit ) break;
                temp.Data.Pointer = table[j];
                if( !DtSubscriptSet( array, numeric_cast<int>( i ), &temp ) )
                {
                    const char *msg = "DtSubscriptSet failed while "
                                      "building result";
                    throw UberGlueError( SECDB_CALL_FAILED, msg );
                }
            }
            security_name = table[count-1];
            // We have already inserted the current value of security_name
            // into array, we need to exclude it from the call to
            // SecDbNameLookupMany.
            get_type = SDB_GET_GREATER;
        }
        *result = array.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE GetSecurity( void *database, const char * const security_name,
                        const char * const class_name, uint64_t flags,
                        void **result )
{
    // Modeled on SlangXGetSecurity.
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_DB *db = database ? get_database( database ) : SecDbRootDb;
        ASSERT_NONZERO( db );
        if( !security_name )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "security_name is null." );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null." );

        SDB_OBJECT *SecPtr;
        if( class_name )
            SecPtr = SecDbGetByNameAndClassName( security_name,
                                                 class_name, db, flags );
        else
            SecPtr = SecDbGetByName( security_name, db, flags );
        if( !SecPtr )
        {
            std::ostringstream ss;
            ss << "Could not load '" << security_name << "'.";
            throw UberGlueError( BAD_SECURITY_NAME_ERROR, ss.str() );
        }

        *result = new_dt_value_handle( DtSecurity, SecPtr );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE ListSecurityClassNames( void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        OwnedHandle array = new_dt_value_handle( DtArray );
        int i = 0;
        SecurityClassNameIterator it;
        DT_VALUE temp;
        temp.DataType = DtString;
        while( const char *class_name = it.next() )
        {
            temp.Data.Pointer = class_name;
            if( !DtSubscriptSet( array, i++, &temp ) )
            {
                const char *msg = "DtSubscriptSet failed while "
                                  "building result";
                throw UberGlueError( SECDB_CALL_FAILED, msg );
            }
        }
        *result = array.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// Look up a SecDB class by name.  Returns a valid SDB_SEC_TYPE or throws
// an exception.
ERROR_CODE NewSecurity( void *database, const char *const class_name,
                        const char *const security_name, void **result)
{
    // Modeled on SlangXNewSecurity in src/slang/src/x_secdb.c
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_DB *db = database ? get_database( database) : SecDbRootDb;
        ASSERT_NONZERO( db );
        if( !class_name )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "class_name is null" );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        SDB_SEC_TYPE SecType = get_security_type( class_name );
        SDB_CLASS_INFO *ClassInfo = SecDbClassInfoFromType( SecType );
        ASSERT_NONZERO( ClassInfo );
        SDB_OBJECT *SecPtr = SecDbNewByClass( security_name, ClassInfo, db );
        if( !SecPtr )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "SecDbNewByClass failed" );
        *result = new_dt_value_handle( DtSecurity, SecPtr );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Security_DiddleValue( void *security,
                                 void *value_type,
                                 void *arguments,
                                 void *value )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_OBJECT *sec = get_security( security );
        SDB_VALUE_TYPE vt = get_value_type( value_type );
        DT_ARRAY *args = get_array( arguments );
        DT_VALUE *val = get_dt_value( value );

        ASSERT_NONZERO( scope_stack );
        if( !SlangScopeSetDiddle( scope_stack, sec, vt, args->Size,
                                  &(args->Element), val ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangScopeSetDiddle failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Security_GetValue( void *security,
                              void *value_type,
                              void *arguments,
                              void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_OBJECT *sec = get_security( security );
        SDB_VALUE_TYPE vt = get_value_type( value_type );
        DT_ARRAY *args = get_array( arguments );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                           "result is null" );

        // According to the documentation, the return value of
        // SecDbSubscribeValue is owned by the sec (I guess it is pointing
        // into the node graph?), so we need to make a copy. -gpb
        DT_VALUE *temp = SecDbGetValueWithArgs( sec, vt, args->Size,
                                                &(args->Element), 0 );
        if( !temp ) throw UberGlueError( SECDB_CALL_FAILED,
                                         "SecDbGetValueWithArgs failed" );
        *result = copy_and_wrap( temp );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Security_InvalidateValue( void *security,
                                     void *value_type,
                                     void *arguments )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_OBJECT *sec = get_security( security );
        SDB_VALUE_TYPE vt = get_value_type( value_type );
        DT_ARRAY *args = get_array( arguments );

        if( !SecDbInvalidate( sec, vt, args->Size, &(args->Element) ) )
            throw UberGlueError( SECDB_CALL_FAILED, "SecDbInvalidate failed" );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Security_ListValueTypeNames( void *security, void **result )
{
    // Modeled on TranslateMem in secdb/src/sdb_trna.c and DT_MSG_FROM of
    // DtFuncValueTypeInfo.
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_OBJECT *SecPtr = get_security( security );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );

        OwnedHandle result_ = new_dt_value_handle( DtArray );
        int i;
        SDB_ENUM_PTR EnumPtr;
        SDB_VALUE_TYPE_INFO *Vti;
        DT_VALUE name;
        name.DataType = DtString;
        for( Vti = SecDbValueTypeEnumFirst( SecPtr, &EnumPtr ), i = 0;
             Vti; Vti = SecDbValueTypeEnumNext( EnumPtr ), i++ )
        {
            ASSERT_NONZERO( Vti->ValueType->Name );
            name.Data.Pointer = Vti->ValueType->Name;
            // DtSubscriptSet copies name;

            if( !DtSubscriptSet( result_, i, &name ) )
            {
                SecDbValueTypeEnumClose( EnumPtr );
                throw UberGlueError( SECDB_CALL_FAILED,
                                         "DtSubscriptSet failed while building result" );
            }
        }
        SecDbValueTypeEnumClose( EnumPtr );
        *result = result_.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE Security_SetValue( void *security,
                              void *value_type,
                              void *arguments,
                              void *value,
                              uint64_t flags )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SDB_OBJECT *sec = get_security( security );
        SDB_VALUE_TYPE vt = get_value_type( value_type );
        DT_ARRAY *args = get_array( arguments );
        DT_VALUE *value_ = get_dt_value( value );
        const SDB_SET_FLAGS flags_ = numeric_cast<SDB_SET_FLAGS>( flags );
        if( !SecDbSetValueWithArgs( sec, vt, args->Size, &(args->Element),
                                    value_ , flags_ ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                     "SecDbSetValueWithArgs failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}



