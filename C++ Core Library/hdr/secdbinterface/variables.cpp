#define BUILDING_UBERGLUE

#include <iostream>
#include <sstream>
#include <string>

// #include <boost/numeric_cast.hpp>

#include <portable.h>
#include <secexpr.h>
#include <err.h>

#include <uberglue_internal.h>


ERROR_CODE GetNamedVariableScope( const char *name, int32_t create,
                                  void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                           "result is null" );
        *result = get_named_variable_scope_handle( name, create );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE CreateLocalVariableScope( const char *name, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                           "result is null" );
        *result = new_local_variable_scope_handle( name );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE PushVariableScope( void *variable_scope )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        HASH *hash = get_variable_scope( variable_scope );
        if( !SlangScopePushVariableHash( scope_stack, hash->Title,
                                         0, 0, hash ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangScopePushVariableHash failed" );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE PopVariableScope()
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( SlangFunctionDepth <= 0 )
        {
            const char *msg = "There are no variable scopes "
                              "on the stack.";
            throw UberGlueError( EMPTY_STACK_ERROR, msg );
        }

        // I expect all of the following to be true.  If they are not,
        // something bad has happened and I'd like to know about. -gpb
        ASSERT_NONZERO( scope_stack );
        ASSERT_NONZERO( scope_stack->FunctionStack );
        ASSERT_ZERO( scope_stack->FunctionStack->Locals );
        ASSERT_NONZERO( scope_stack->FunctionStack->VariableHash );
        ASSERT_NONZERO( scope_stack->FunctionStack->SlangFunc );
        ASSERT_ZERO( scope_stack->FunctionStack->SlangFunc->Locals );

        // We don't a destroy a local variable scope the moment it is
        // pushed off the stack.  Instead, it is the callers responsibility
        // to destroy the hash by calling ReleaseHandle when the variable
        // scope is no longer needed scope.  In particular, the caller can
        // push a single variable scope onto the stack multiple times.
        scope_stack->FunctionStack->VariableHash = 0;

        if( !SlangScopePopFunction( scope_stack, 0 ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangScopePopFunction failed" );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Returns an array of strings.
ERROR_CODE VariableScope_ListVariables( void *variable_scope,
                                        void **result)
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        HASH *hash = get_variable_scope( variable_scope );
        OwnedHandle result_ = new_dt_value_handle( DtArray );

        // Modeled on SlangXVariables in slang/src/x_misc.cpp -gpb
        int Count = 0;
        SDB_ENUM_PTR EnumPtr;
        SLANG_VARIABLE *Variable;
        for( Variable = SlangEnumFirstVar( &EnumPtr, hash );
             Variable; Variable = SlangEnumNextVar( EnumPtr ) )
        {
            if( !( Variable->Flags & SLANG_VARIABLE_DESTROYED ) )
                // It appears that DtSubscriptSetPointer copies of the
                // string. -gpb
                if( !DtSubscriptSetPointer( result_, Count++,
                                            DtString, Variable->Name ) )
                {
                    SlangEnumVarClose( EnumPtr );
                    throw UberGlueError( SECDB_CALL_FAILED,
                                         "DtSubscriptSetPointer failed." );
                }
        }
        SlangEnumVarClose( EnumPtr );
        *result = result_.give_to_caller();
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Internal helper function.  Returns a valid SLANG_VARIABLE * or throws an
// exception.  If scope_name is null, the variable is looked up in the
// topmost slang variable scope on the stack.
static SLANG_VARIABLE *GetSlangVariable( void *variable_scope,
                                         const char *const variable_name )
{
    TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
    HASH *hash = get_variable_scope( variable_scope );
    if( !variable_name ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                              "variable_name is null" );
    SLANG_VARIABLE *result = SlangVariableGetScoped( hash, variable_name );
    if( !result )
    {
        std::ostringstream ss;
        ss << "Variable " << variable_name << " does not exist in ";
        if( hash->Title )
            ss << "scope '" << hash->Title << "'.";
        else
            ss << "the given scope.";
        throw UberGlueError( BAD_VARIABLE_NAME, ss.str() );
    }
    logger.omit_traceback();
    return result;
}

ERROR_CODE SlangVariable_GetValue( void *variable_scope,
                                   const char *const variable_name,
                                   void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SLANG_VARIABLE *variable = GetSlangVariable( variable_scope,
                                                     variable_name );
        if( !result )
            throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null" );
        *result = copy_and_wrap( *(variable->Value) );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE SlangVariable_SetValue( void *variable_scope,
                                   const char *const variable_name,
                                   void *value )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        HASH *hash = get_variable_scope( variable_scope );
        if( !variable_name ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                                  "variable_name is null" );
        DT_VALUE *val = get_dt_value( value );

        // SlangVariableSetScoped (really SlangVariableSetValue) with no
        // flags appears to DTM_ASSIGN to a new DT_VALUE. -gpb
        if( !SlangVariableSetScoped( hash, variable_name, val, 0 ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangVariableSetScoped failed." );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

// Delete a variable in a named Slang variable scope.
ERROR_CODE SlangVariable_Destroy( void *variable_scope,
                                  const char *const variable_name )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SLANG_VARIABLE *variable = GetSlangVariable( variable_scope,
                                                     variable_name );
        // See SlangVariableDestroyByName in sdb_eval.c -gpb
        if( ( variable->Flags & SLANG_VARIABLE_PROTECT ) )
            throw UberGlueError( VARIABLE_IS_PROTECTED,
                                 "Variable is protected" );
        if( !SlangVariableDestroy( variable ) )
            throw UberGlueError( SECDB_CALL_FAILED,
                                 "SlangVariableDestroy failed" );
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


ERROR_CODE SlangVariable_GetReferenceCount( void *variable_scope,
                                            const char *const variable_name,
                                            uint64_t *result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        SLANG_VARIABLE *variable = GetSlangVariable( variable_scope,
                                                     variable_name );
        if( !result ) throw UberGlueError( BAD_ARGUMENT_ERROR,
                                           "result is null" );
        *result = variable->ReferenceCount;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}

ERROR_CODE SlangVariable_GetFlags( void *variable_scope,
                                   const char * const variable_name,
                                   int64_t *flags )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        if( !flags ) throw UberGlueError( BAD_ARGUMENT_ERROR, "flags is null" );
        SLANG_VARIABLE *variable = GetSlangVariable( variable_scope,
                                                     variable_name );
        *flags = variable->Flags;
        logger.omit_traceback();
        return 0;
    }
    catch(...) { return handle_exceptions(); }
}


// ERROR_CODE GetStackDepth( int32_t *result )
// {
//     try
//     {
//         TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//         if( !result )
//             throw UberGlueError( BAD_ARGUMENT_ERROR, "result is null." );
//         *result = numeric_cast<int32_t>( SlangFunctionDepth );
//         logger.omit_traceback();
//         return 0;
//     }
//     catch(...) { return handle_exceptions(); }
// }

// ERROR_CODE GetStackInfo( int32_t depth, const char * *const function_name,
//                          const char * *const hash_name )
// {
//     try
//     {
//         TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//         if( !function_name ) throw UberGlueError( BAD_ARGUMENT_ERROR,
//                                                   "function_name is null." );
//         if( !hash_name ) throw UberGlueError( BAD_ARGUMENT_ERROR,
//                                               "hash_name is null." );
//         int scopes_to_pop = SlangFunctionDepth - depth;
//         SLANG_SCOPE_FUNCTION *function_scope = scope_stack->FunctionStack;
//         while( scopes_to_pop-- > 0 ) {
//             function_scope = function_scope->Next;
//         }
//         *function_name = function_scope->FunctionName;
//         *hash_name = function_scope->VariableHash->Title;
//         logger.omit_traceback();
//         return 0;
//     }
//     catch(...) { return handle_exceptions(); }
// }

// ERROR_CODE EnsureScopeExists( const char *const scope_name )
// {
//     try
//     {
//         TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//         SLANG_VARIABLE_SCOPE *scope = SlangVariableScopeGet( scope_name, 1 );
//         CHECK_NONZERO( scope );
//         logger.omit_traceback();
//         return 0;
//     }
//     catch(...) { return handle_exceptions(); }
// }

// // Delete all variables in a named Slang variable scope.
// ERROR_CODE ClearScope( const char *const scope_name )
// {
//     // According to Jason O'Brien: There is no API function to properly
//     // destroy a variable scope.  The following will destroy the variables,
//     // but it doesn't clean up the entry in the Slang variable scopes hash,
//     // which isn't exported, so it'll just get resurrected as a crash-happy
//     // zombie scope next SlangVariableScopeGet call so we'll just clean up
//     // the variables and call it a day.
//     //
//     // Do we want to add a SecDb function to delete a named slang variable
//     // scope?
//     try
//     {
//         TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//         SLANG_VARIABLE_SCOPE *scope = SlangVariableScopeGet( scope_name, 1 );
//         CHECK_NONZERO( scope );
//         const char *title = scope->VariableHash->Title;
//         CHECK_NONZERO( SlangVariableHashDestroy( scope->VariableHash ) );
//         scope->VariableHash = HashCreate( title, (HASH_FUNC) HashStrHash,
//                                           (HASH_CMP) HashStricmp, 100, NULL );
//         CHECK_NONZERO( scope->VariableHash );
//         logger.omit_traceback();
//         return 0;
//     }
//     catch(...) { return handle_exceptions(); }
// }


// // Internal helper function.  Returns a valid HASH* or throws an exception.
// // If scope is null, the hash is retrieved from the top level of the
// // Slang scope function stack.
// static HASH *GetSlangVariableHash( void *scope_name )
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     HASH *hash;
//     if( scope_name )
//     {
//         SLANG_VARIABLE_SCOPE *scope = SlangVariableScopeGet( scope_name, 0 );
//         if( !scope )
//         {
//             std::ostringstream ss;
//             ss << "There is no variable scope named " << scope_name << ".";
//             throw UberGlueError( BAD_SCOPE_NAME, ss.str() );
//         }
//         hash = scope->VariableHash;
//     }
//     else
//     {
//         // The argument scope_name is null, so we look in the top level
//         // variable hash on the stack.
//         SLANG_SCOPE_FUNCTION *Func = scope_stack->FunctionStack;
//         ASSERT_NONZERO( Func );
//         hash = Func->VariableHash;
//     }
//     ASSERT_NONZERO( hash );
//     logger.omit_traceback();
//     return hash;
// }

// // Internal helper function.  Returns a valid HASH* or throws an exception.
// // If scope is null, the hash is retrieved from the top level of the
// // Slang scope function stack.
// static HASH *GetSlangVariableHash( const char *const scope_name )
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     HASH *hash;
//     if( scope_name )
//     {
//         SLANG_VARIABLE_SCOPE *scope = SlangVariableScopeGet( scope_name, 0 );
//         if( !scope )
//         {
//             std::ostringstream ss;
//             ss << "There is no variable scope named " << scope_name << ".";
//             throw UberGlueError( BAD_SCOPE_NAME, ss.str() );
//         }
//         hash = scope->VariableHash;
//     }
//     else
//     {
//         // The argument scope_name is null, so we look in the top level
//         // variable hash on the stack.
//         SLANG_SCOPE_FUNCTION *Func = scope_stack->FunctionStack;
//         ASSERT_NONZERO( Func );
//         hash = Func->VariableHash;
//     }
//     ASSERT_NONZERO( hash );
//     logger.omit_traceback();
//     return hash;
// }

// This function is very similiar to SlangScopeCreate; the only difference is that we push variables_hash onto the stack.

// NewSlangEvalScope::NewSlangEvalScope( HASH *variables_hash )
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     CHECK_NONZERO( variables_hash );
//     _ptr = (SLANG_SCOPE *) CHECK_NONZERO( calloc( 1, sizeof( SLANG_SCOPE ) ) );
//     try
//     {
//         CHECK_NONZERO(
//             SlangScopePushVariableHash( _ptr, "(Script Interface)",
//                                         NULL, NULL, variables_hash ) );
//         CHECK_NONZERO(
//             _ptr->Diddles = (SLANG_SCOPE_DIDDLES *)
//                             calloc( 1, sizeof( SLANG_SCOPE_DIDDLES ) ) );
//         _ptr->Diddles->DiddleDepth = _ptr->DiddleDepth = 0;
//     }
//     catch(...)
//     {
//         free( _ptr );
//         throw;
//     }
// }

// NewSlangEvalScope::~NewSlangEvalScope()
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     SlangScopeDestroy( CHECK_NONZERO( _ptr ) );
// }

// NewSlangEvalScope::operator SLANG_SCOPE*()
// {
//     TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
//     return CHECK_NONZERO( _ptr );
// }


