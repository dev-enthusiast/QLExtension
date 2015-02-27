#ifdef BUILDING_UBERGLUE
#	define UBERGLUE_API SECDB_DLL_EXPORT ERROR_CODE
#else
#	define UBERGLUE_API SECDB_DLL_IMPORT ERROR_CODE
#endif

#define NO_ERROR 0
#define UNKNOWN_CPP_ERROR 1
#define ASSERTION_ERROR 2
#define BAD_ARGUMENT_ERROR 3
#define BAD_DATABASE_NAME_ERROR 4
#define BAD_SECURITY_NAME_ERROR 5
#define MEMORY_ERROR 6
#define SECDB_CALL_FAILED 7
#define SLANG_EVAL_FAILED 8
#define BAD_SUBSCRIPT 9
#define BAD_SCOPE_NAME 10
#define BAD_VARIABLE_NAME 11
#define OVERFLOW_ERROR 12
#define NO_LAST_ERROR 13
#define VARIABLE_IS_PROTECTED 14
#define EMPTY_STACK_ERROR 15
#define LIST_IS_TOO_LARGE_ERROR 16
#define BAD_CLASS_NAME_ERROR 17

EXPORT_C_SECDB const int32_t SDB_SEC_NAME_SIZE_;

//////////////////////////////////////////////////////////////////////
//
//    Comments on resource management
//
// Resource management: every void * (really void ** since it is a return value) returned by a
// function below is an opaque handle to a SecDB resource that is now owned by the calling
// application, and it is the responsibility of that calling application to call ReleaseHandle on
// that handle when the application is done using the resource.  In particular, all wrapped
// DT_VALUEs that are returned are copies of underyling SecDB data (as opposed to views into the
// another compound DT_VALUE or the node graph) that may be deleted by the calling application
// whenever it wants.  ReleaseHandle is the only function that takes back ownership of a handle.
// Other function are required to copy the data (or increment a reference) referenced by a
// SECDB_HANDLE argument if they use underlying SecB API calls that take possession of a resource.


extern "C"
{

typedef uint32_t ERROR_CODE;

//////////////////////////////////////////////////////////////////////
//
//    Top level API functions
//

/// Connect to SecDb database.
/// @param application reported application name
/// @param username reported user name
/// @param database root security database
/// @param database security database from which to retrieve Slang scripts
/// @param deadPoolSize size of deadpool cache, 0 to disable, -1 for default
/// @return whether initialization succeeded
UBERGLUE_API Connect( const char *application, const char *username,
                      const char *database, const char *sourceDatabase,
                      int32_t deadPoolSize );

/// Get last registered error string.
/// @param result (out) returns a string pointer valid until next SecDb API call
/// @return zero on success or an error code.
UBERGLUE_API LastError( const char **result );

/// Get last registered error code.
/// @parameter result (out) the current error code.
/// @return zero on success or an error code.
UBERGLUE_API LastErrorCode( int64_t *result );

/// Execute a Slang expression and return the result.  The result will be a
/// DT_VALUE allocated in SecDB memory, but owned by the calling
/// application, and only accessible through an opaque handle.
/// @param slang_expression a string containing slang code
/// @param return_code (out) information about result
/// @param result_type (out) information about result
/// @param result (out) the resulting value
/// @return zero on success or an error code.
//
// Comments: I'm not sure we really need to pass back return_code and
// result_type once we understand how to handle all the possibilities, but
// until then I'd like to see what is going on. -gpb
//
UBERGLUE_API SlangEval( const char *slang_expression, int32_t *return_code,
                        int32_t *result_type, void **result );

/// Release resources associated with a handle.  This should be called for
/// every void *received from this API.  After calling this
/// function, handle is invalid and should not be passed to any further
/// uberglue API calls.
/// @param the handle to be released.
/// @return zero on success or an error code.
UBERGLUE_API ReleaseHandle( void *handle );

/// List the names of all security classes.
/// @param result (out) the resulting array of strings.
/// @return zero on success or an error
UBERGLUE_API ListSecurityClassNames( void **result );



//////////////////////////////////////////////////////////////////////
//
//
//    Variable and scope related functions.
//
//
// We deal with two kinds of variable scopes: "named scopes" and "local
// scopes".  Named scopes correspond to the strings returned by the Slang
// function "Scopes()" and can be accessed in Slang code by prepending a
// namespace to a varaible (e.g. My Scope::My Variable).  Local scopes are
// like the variables that are local to a Slang function, and the scope
// that they define is essentially anonymous.  Both kinds of variables
// scopes can be pushed and popped off the variable scope stack.  The
// innermost level of this stack determines the variables that are visible
// to Slang code executed used by SlangEval.  The calling applications can
// use the Slang function "CallStack()" to see the current state of this
// stack.  Variables in a local scopes can only be reached by Slang code
// excuting in a different scope through the call stack.  In particular,
// the Slang function "Scope" can used to access any variables in the
// current Slang call stack.

/// Look up or a creates a named variable scope.
/// @param name of the scope to get or create.
/// @param create (bool) If create is false, this functions fails when
///        no variable scope with the given name exists.  If create is true
///        (i.e., not 0), then the scope is silently created.
/// @param result (out) the resulting scope object.
/// @return zero on success or an error
UBERGLUE_API GetNamedVariableScope( const char *name, int32_t create,
                                    void **result );

/// @param name the name for the scope.  This variable is mainly
///        informational and appears as the "function name" in the Slang
///        call stack.  In particular, all local scopes are distict, even
///        those with the same name.
/// @param result (out) the resulting scope object.
/// @return zero on success or an error
UBERGLUE_API CreateLocalVariableScope( const char *name, void **result );

/// Push a new variable scope onto the Slang evaluation stack.
/// @param variable_scope A scope handle from a previous call to
///        GetNamedVariableScope or CreateLocalVariableScope.
/// @return zero on success or an error code.
UBERGLUE_API PushVariableScope( void *variable_scope );

/// Pop a new variable scope onto the Slang evaluation stack.  Throws an
/// error if we are at the outmost scope on the stack.
/// @param variable_scope A scope handle from a previous call to
///        GetNamedVariableScope or CreateLocalVariableScope.
/// @return zero on success or an error code.
UBERGLUE_API PopVariableScope();

/// List all the names of all variables defined in the given scope.
/// @param variable_scope A scope handle from a previous call to
///        GetNamedVariableScope or CreateLocalVariableScope.
/// @param result (out) An array of strings.
/// @return zero on success or an error code.
UBERGLUE_API VariableScope_ListVariables( void *variable_scope,
                                          void **result);

/// Retrieve the value associated with the given variable name in the given
/// scope.  The result is passed by value and changes to result do no
/// impact the value associated with the Slang variable.  This function is
/// slightly more general than the corresponding Slang function "Scope"
/// because it can used to access variables in local scopes that are not
/// currently on the call stack.
/// @param variable_scope a handle from GetNamedVariableScope or
///        CreateLocalVariableScope.
/// @param variable_name the name of variable whose value it returned
/// @param result (out) the DT_VALUE associated with variable_name in
///        scope_name
/// @return zero on success or an error code.
UBERGLUE_API SlangVariable_GetValue( void *variable_scope,
                                     const char *const variable_name,
                                     void **result );

/// Change the value associated with the given variable name in the
/// given scope.  The value is essentially passed by value, and the
/// Slang variable contains a deep copy of value after this call
/// completes.
/// @param variable_scope a handle from GetNamedVariableScope or
///        CreateLocalVariableScope.
/// @param variable_name the name of variable whose value it returned
/// @param value new DT_VALUE to associate with variable_name in
///        scope_name
/// @return zero on success or an error code.
UBERGLUE_API SlangVariable_SetValue( void *variable_scope,
                                     const char *const variable_name,
                                     void *value );

/// Delete a variable from a scope.
/// @param variable_scope a handle from GetNamedVariableScope or
///        CreateLocalVariableScope.
/// @param variable_name the name of variable whose value it returned
/// @return zero on success or an error code.
UBERGLUE_API SlangVariable_Destroy( void *variable_scope,
                                    const char *const variable_name );

/// @return zero on success or an error code.
UBERGLUE_API SlangVariable_GetReferenceCount( const char *const scope_name,
                                              const char *const variable_name,
                                              uint64_t *result );

/// @return zero on success or an error code.
UBERGLUE_API SlangVariable_GetFlags( const char *const scope_name,
                                     const char *const variable_name,
                                     int64_t *flags );



//////////////////////////////////////////////////////////////////////
//
//
//    DT_VALUE accessor functions
//
//
//  Functions for accessing wrapped DT_VALUE object/struct/pointer things.
//  At the moment, the API assumes that the calling functions has some
//  understanding of the underlying implementation.  For example, once can
//  call DtValue_GetPointer on a double, and the result will be garbage.  I
//  suppose we could add safer accessor functions at some point, like in
//  dot net glue, but I'm not sure it really buys us much.

/// Allocate a new DtValue structure.
/// @return zero on success or an error code.
UBERGLUE_API DtValue_Allocate( const char *data_type_name,
                               void **result );

/// Compare two wrapped DT_VALUE's.  After the call, result contains 1
/// if the results are equal, and zero otherwise.
/// @return zero on success or an error code.
UBERGLUE_API DtValue_AreEqual( void *x, void *y,
                               uint32_t *result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_GetTypeName( void *handle,
                                  const char **const result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_GetNumber( void *handle, double *result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_SetNumber( void *handle, double number );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_GetPointer( void *handle, const void **pointer );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_SetPointer( void *handle, void *pointer );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_Convert( void *handle,
                              const char *const new_type_name,
                              void **result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_GetSize( void *object, int64_t *size );

/// Returns an Array contains all the valid subscripts for object.
UBERGLUE_API DtValue_ListSubscripts( void *object,
                                     void **result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_GetSubscriptValue( void *object,
                                        void *subscript,
                                        void **result );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_SetSubscriptValue( void *object,
                                        void *subscript,
                                        void *value );

/// @return zero on success or an error code.
UBERGLUE_API DtValue_DestroySubscript( void *object,
                                       void *subscript );



//////////////////////////////////////////////////////////////////////
//
//
//    Data type specific creation and accessor functions
//
//
// Specialized functions for a few key DT_VALUE types.  The accessor
// functions below are "safe" to the extent that they fail with meaningful
// error messages when given a handle that wraps the wrong type of
// underlying data object.

/// @return zero on success or an error code.
UBERGLUE_API NewString( const char *text, void **result );

/// @return zero on success or an error code.
UBERGLUE_API Array_InsertElement( void *array, const uint64_t index,
                                  void *value );

/// @return zero on success or an error code.
UBERGLUE_API Array_DestroyElements( void *array, const uint64_t index,
                                    const uint64_t count );

/// Create a new Vector.  Copies data from buffer into a new SecDB
/// allocated buffer.
/// @return zero on success or an error code.
UBERGLUE_API NewVector( uint64_t size, double *buffer, void **result );

/// Get a pointer into the data assocaited with a Vector.  This pointer
/// is valid until Handle_Free is called on vector.
/// @return zero on success or an error code.
UBERGLUE_API Vector_GetDataView( void *vector, uint64_t *size,
                                 double **data );

/// Get a reference to an existing security object in a database.
/// @param database A handle to a database object or 0/NULL.  0 denotes the
///        current root database.
/// @param security_name The name of the security to look up.
/// @param class_name May be zero.  If given, throws an error if the
///        resulting security does not have the correct type.
/// @param flags Flags that are passed to underlying
///        SecDbGetByNameAndClassName or SecDbGetByName call.
/// @param result (out) The resulting  security object.
/// @return zero on success or an error code.
// We are currently exposing flags.  Do we want to abstract this? -gpb
UBERGLUE_API GetSecurity( void *database, const char *const security_name,
                          const char *const class_name, uint64_t flags,
                          void **result );


/// Create a new security object.
/// @param database A handle to a database object or 0/NULL.  0 denotes the
///        current root database.
/// @param security_name The name of the new security.  This argument can
///        be null, and in this case, the name will be automatically created.
/// @param class_name May be zero.  If given, throws an error if the
///        resulting security does not have the correct type.
/// @param flags Flags that are passed to underlying
///        SecDbGetByNameAndClassName or SecDbGetByName call.
/// @param result (out) The resulting  security object.
/// @return zero on success or an error code.
UBERGLUE_API NewSecurity( void *database, const char * const class_name,
                          const char *const security_name, void **result );

/// Search for the names of securities that satisfy certain conditions.  If
/// the conditions are not very strict, the result will be huge (how many
/// securities are in prod? -gpb), and the result must be returned in
/// "chunks" of size limit.  In particular, one can use this function to
/// implement a buffered iterator that iterates over security names in
/// case-insensitive, lexical order.
///
/// @param database
///   A handle to a database object or 0/NULL.  0 denotes the current root
///   database.
/// @param class_name
///   May be zero.  If given, the result is restricted to
///   objects of this class.
/// @param start
///   Can be null.  If provided, only security names that follow
///   start in case-insensitive, lexical order will be returned.
/// @param exclude_start
///   A bool condition.  Controls the strictness of the comparison between
///   cadidate names and the argument start.  In particular, if start is
///   non-null and exclude_start is non-zero, then start will not be the
///   first entry in the resulting array.
/// @param limit
///   The maximum number of names to return.  Multiples of 1000 are the
///   best choices.
/// @param result (out)
///   The resulting array of strings that contains security names.  These
///   name are sorted and contiguous in the case-insensitive, lexical
///   order.
/// @return
///   Contains zero on success or an error code.
///
UBERGLUE_API FindSecurities( void *database, const char *class_name,
                             const char *start, uint32_t exclude_start,
                             uint32_t limit, void **result );

/// @return zero on success or an error code.
UBERGLUE_API Security_GetValue( void *security, void *value_type,
                                void *arguments, void **result );

/// @param flags Flags that are passed to underlying SecDbSetValueWithArgs call.
/// @return zero on success or an error code.
UBERGLUE_API Security_SetValue( void *security, void *value_type,
                                void *arguments, void *value,
                                uint64_t flags );

/// @return zero on success or an error code.
UBERGLUE_API Security_DiddleValue( void *security, void *value_type,
                                   void *arguments, void *value );

/// @return zero on success or an error code.
UBERGLUE_API Security_InvalidateValue( void *security, void *value_type,
                                       void *arguments );


/// Returns all value type names assocaited with a given security.
/// @param security a handle associated with a SecDb security.
/// @param result (out) an array of strings.
/// @return zero on success or an error code.
UBERGLUE_API Security_ListValueTypeNames( void *security,
                                          void **result );

/// @param result (out) a new Curve.
/// @return zero on success or an error code.
UBERGLUE_API NewCurve( uint64_t length, int64_t *dates, double *values,
                       void **result );

/// @return zero on success or an error code.
UBERGLUE_API Curve_GetDates( void *curve, uint64_t length, int64_t *dates );


/// @return zero on success or an error code.
UBERGLUE_API Curve_GetValues( void *curve, uint64_t length, double *values );

/// @param result (out) a new TCurve.
/// @return zero on success or an error code.
UBERGLUE_API NewTCurve( uint64_t length, int64_t *timestamps, double *values,
                        void **result );

/// @return zero on success or an error code.
UBERGLUE_API TCurve_GetTimestamps( void *tcurve, uint64_t length,
                                   int64_t *timestamps );
/// @return zero on success or an error code.
UBERGLUE_API TCurve_GetValues( void *tcurve, uint64_t length, double *values );

}

