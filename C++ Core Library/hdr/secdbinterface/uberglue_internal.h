#ifndef UBERGLUE_H_8C31C56D_DB52_4FE1_BF45_8F69985595B5
#define UBERGLUE_H_8C31C56D_DB52_4FE1_BF45_8F69985595B5

#include <portable.h>
#include <cstdint>
#include <secdb.h>
#include <secexpr.h>
#include <dtcore.h>
#include <sstream>
#include <stdexcept>
#include <uberglue.h>
#include <limits>
#include <type_traits>

// This is top level "SLANG_SCOPE".  The terminology is a bit confusing
// here.  This is not a Slang variable scope; instead, it is essentially a
// stack of variable scope/local variables for functions and diddle scopes.
extern SLANG_SCOPE *scope_stack;

// We've got all kinds of ints to deal with.  Lets try and be somewhat
// careful about the type casts. -gpb
#include <boost/numeric/conversion/cast.hpp>
using boost::numeric_cast;
template <typename S, typename T>
void safe_set(S &dest, T src)
{
   dest = numeric_cast<S>( src );
}

////////////////////////////////////////////////////////////////////////////////
//
//  Error handling
//

ERROR_CODE handle_exceptions();

#define CHECK_ZERO( val ) check_zero( val, UNKNOWN_CPP_ERROR, #val )
#define CHECK_NONZERO( val ) check_nonzero( val, UNKNOWN_CPP_ERROR, #val )

// I'd rather return an error than crash when assertions fail. -gpb
#define ASSERT_ZERO( val ) ErrClear(); \
                           check_zero( val, ASSERTION_ERROR, #val )
#define ASSERT_NONZERO( val ) ErrClear(); \
                              check_nonzero( val, ASSERTION_ERROR, #val )


class UberGlueError : public std::runtime_error
{
  public:
    const ERROR_CODE error_code;
    UberGlueError( ERROR_CODE error_code, std::string msg )
        : std::runtime_error( msg ), error_code(error_code) { };
};

// This class exists to provide traceback info.  We keep track of the
// current function and file and push that information to the SecDb error
// message log as we unwind the stack during exceptions to produce
// a primative traceback in the SecDb last error log.
class TracebackLogger
{
  private:
    const char *function_name;
    const char *source_file;
    const unsigned int line;
    bool log_traceback;

  public:
    TracebackLogger( const char *function_name, const char *source_file,
                     const unsigned int line )
        : function_name( function_name ), source_file( source_file ),
          line( line ), log_traceback( true )
    {
        ErrClear();
    };

    void omit_traceback() { log_traceback = false; };

    ~TracebackLogger()
    {
        if( !log_traceback ) return;
        std::ostringstream ss;
        ss << "In function " << function_name << ", file " << source_file
           << ", line " << line;
        ErrMore( ss.str().c_str() );
    };
};

template <typename T>
T check_zero( T result, int code, const char *expression )
{
    if( !result ) return result;
    std::stringstream ss;
    ss << expression << " is nonzero";
    throw UberGlueError( code, ss.str() );
};

template <typename T>
T check_nonzero( T result, int code, const char *expression )
{
    if( result ) return result;
    std::stringstream ss;
    ss << expression << " is zero";
    throw UberGlueError( code, ss.str() );
};


////////////////////////////////////////////////////////////////////////////////
//
//    Main "handle" data structure.
//
//    This is the only non-trivial data structure exposed by the uberglue
//    bindings.  Any data type that we neeed to export should be added to
//    SECDB_HANDLE_DATA_CONTENTS so that it may be managed in a uniform way
//    by calling code.
//


enum SECDB_HANDLE_DATA_TYPE { SECDB_HANDLE_DATA_TYPE__DT_VALUE,
                              SECDB_HANDLE_DATA_TYPE__VARIABLE_SCOPE };

union SECDB_HANDLE_DATA_CONTENTS
{
    DT_VALUE dt_value;
    HASH *variable_scope;
};

struct SECDB_HANDLE_DATA
{
    const char *debug_string;
    SECDB_HANDLE_DATA_TYPE type;
    bool delete_on_free;
    SECDB_HANDLE_DATA_CONTENTS contents;
};

void free_handle( void *handle );

// Function for working with a DT_VALUE wrapped inside a SECDB_HANDLE_DATA.
void *new_dt_value_handle( DT_DATA_TYPE data_type );
void *new_dt_value_handle( DT_DATA_TYPE data_type, void *pointer );
void *take_and_wrap( DT_VALUE value );
void *copy_and_wrap( DT_VALUE value );
void *copy_and_wrap( DT_VALUE *value );
DT_VALUE *get_dt_value( void *handle );
DT_VALUE *get_dt_value( void **handle );
DT_VALUE *get_dt_value( void *handle, DT_DATA_TYPE expected_data_type );

// Function for working with a HASH * wrapped inside a SECDB_HANDLE_DATA.
void *new_local_variable_scope_handle( const char *name );
void *get_named_variable_scope_handle( const char *name, bool create );
HASH *get_variable_scope( void *handle );

template <typename T>
T get_data_pointer( void *handle, DT_DATA_TYPE expected_data_type )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
    DT_VALUE *object = get_dt_value( handle, expected_data_type );
    if( !object->Data.Pointer )
        throw UberGlueError( BAD_ARGUMENT_ERROR,
                             "wrapped DT_VALUE has a null data pointer" );
    return T( object->Data.Pointer );
};

char *get_string( void *handle );
DT_ARRAY *get_array( void *handle );
DT_VECTOR *get_vector( void *handle );
SDB_DB *get_database( void *handle );
SDB_OBJECT *get_security( void *handle );
SDB_VALUE_TYPE get_value_type( void *handle );
DT_CURVE *get_curve( void *handle );
DT_CURVE *get_tcurve( void *handle );

// RAII rapper for a void *that we may need to free on error.
class OwnedHandle
{
  protected:
    void *_handle;

  public:
    // I can certainly imagine this destructor being called from a derived
    // class.  -gpb
    OwnedHandle( void *handle ) : _handle( handle ) { };
    ~OwnedHandle() { if( _handle ) free_handle( _handle ); };

    // The following are convenience casts automatically handle validation.
    operator DT_VALUE *() { return get_dt_value( _handle ); };
    DT_VALUE *operator ->() { return get_dt_value( _handle ); };

    operator DT_ARRAY *() { return get_array( _handle ); };
    operator DT_VECTOR *() { return get_vector( _handle ); };
    operator SDB_OBJECT *() { return get_security( _handle ); };
    operator SDB_VALUE_TYPE() { return get_value_type( _handle ); };

    // Give up ownership of the handle.
    void *give_to_caller() // noexcept
    {
        void *p = _handle;
        _handle = 0;
        return p;
    };

};



////////////////////////////////////////////////////////////////////////////////
//
//  RAII wrappers.
//
//  Variaous wrappers to facilitate exception-safe resource (mainly memory)
//  allocation.
//


// RAII wrapper for a SLANG_SCOPE.
struct NewSlangEvalScope
{
  protected:
    SLANG_SCOPE *_ptr;

  public:
    NewSlangEvalScope( HASH *variable_hash );
    ~NewSlangEvalScope();
    operator SLANG_SCOPE*();
};



#endif
