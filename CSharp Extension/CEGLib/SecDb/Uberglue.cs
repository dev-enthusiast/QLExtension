using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using System.Data;
using System.Data.SqlClient;

namespace CEGLib.SecDb
{
    sealed public class Uberglue
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////       DLLImport functions          ///////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        #region Top level API functions
        /// Connect to SecDb database. (*)
        /// @param application reported application name
        /// @param username reported user name
        /// @param database root security database
        /// @param database security database from which to retrieve Slang scripts
        /// @param deadPoolSize size of deadpool cache, 0 to disable, -1 for default
        /// @return whether initialization succeeded
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Connect(string application, string username,
                string database, string sourceDatabase, Int32 deadPoolSize);

        /// Get last registered error string. (*)
        /// @param result (out) returns a string pointer valid until next SecDb API call
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 LastError( [Out]StringHandle result );

        /// Get last registered error code.
        /// @parameter result (out) the current error code.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 LastErrorCode(IntPtr result);

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
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangEval(string slang_expression, IntPtr return_code,
                        IntPtr result_type, Handle result );

        /// Release resources associated with a handle.  This should be called for
        /// every void *received from this API.  After calling this
        /// function, handle is invalid and should not be passed to any further
        /// uberglue API calls. (*)
        /// @param the handle to be released.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 ReleaseHandle(IntPtr handle);

        /// List the names of all security classes. (*)
        /// @param result (out) the resulting array of strings.
        /// @return zero on success or an error
        public static extern UInt32 ListSecurityClassNames(Handle result);
        #endregion

        #region Variable and scope related functions.
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

        /// Look up or a creates a named variable scope. (*)
        /// @param name of the scope to get or create.
        /// @param create (bool) If create is false, this functions fails when
        ///        no variable scope with the given name exists.  If create is true
        ///        (i.e., not 0), then the scope is silently created.
        /// @param result (out) the resulting scope object.
        /// @return zero on success or an error
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 GetNamedVariableScope(string name, Int32 create,
                                    Handle result );

        /// @param name the name for the scope.  This variable is mainly (*)
        ///        informational and appears as the "function name" in the Slang
        ///        call stack.  In particular, all local scopes are distict, even
        ///        those with the same name.
        /// @param result (out) the resulting scope object.
        /// @return zero on success or an error
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 CreateLocalVariableScope(string name, Handle result);

        /// Push a new variable scope onto the Slang evaluation stack. (*)
        /// @param variable_scope A scope handle from a previous call to
        ///        GetNamedVariableScope or CreateLocalVariableScope.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 PushVariableScope(IntPtr variable_scope);

        /// Pop a new variable scope onto the Slang evaluation stack.  Throws an
        /// error if we are at the outmost scope on the stack.
        /// @param variable_scope A scope handle from a previous call to
        ///        GetNamedVariableScope or CreateLocalVariableScope.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 PopVariableScope();

        /// List all the names of all variables defined in the given scope. (*)
        /// @param variable_scope A scope handle from a previous call to
        ///        GetNamedVariableScope or CreateLocalVariableScope.
        /// @param result (out) An array of strings.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 VariableScope_ListVariables( IntPtr variable_scope, Handle result);

        /// Retrieve the value associated with the given variable name in the given (*)
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
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangVariable_GetValue(IntPtr variable_scope,
                                             string variable_name,
                                             Handle result );

        /// Change the value associated with the given variable name in the (*)
        /// given scope.  The value is essentially passed by value, and the
        /// Slang variable contains a deep copy of value after this call
        /// completes.
        /// @param variable_scope a handle from GetNamedVariableScope or
        ///        CreateLocalVariableScope.
        /// @param variable_name the name of variable whose value it returned
        /// @param value new DT_VALUE to associate with variable_name in
        ///        scope_name
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangVariable_SetValue(IntPtr variable_scope,
                                             string variable_name,
                                             IntPtr value );

        /// Delete a variable from a scope. (*)
        /// @param variable_scope a handle from GetNamedVariableScope or
        ///        CreateLocalVariableScope.
        /// @param variable_name the name of variable whose value it returned
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangVariable_Destroy( IntPtr variable_scope,
                                            string variable_name );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangVariable_GetReferenceCount( StringBuilder scope_name,
                                                      StringBuilder variable_name,
                                                      IntPtr result );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 SlangVariable_GetFlags( StringBuilder scope_name,
                                             StringBuilder variable_name,
                                             IntPtr flags );
        #endregion

        #region DtValue
        //////////////////////////////////////////////////////////////////////
        //
        //  DT_VALUE accessor functions
        //    
        //////////////////////////////////////////////////////////////////////
        /// Allocate a new DtValue structure. (*)
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_Allocate( string data_type_name, Handle result );

        /// Compare two wrapped DT_VALUE's.  After the call, result contains 1
        /// if the results are equal, and zero otherwise.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_AreEqual( IntPtr x, IntPtr y,
                                       IntPtr result );

        /// Get type name (*)
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_GetTypeName( IntPtr handle, [Out]StringHandle result );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_GetNumber( Handle handle, IntPtr result );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_SetNumber( IntPtr handle, double number );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_GetPointer( IntPtr handle, IntPtr pointer );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_SetPointer( IntPtr handle, IntPtr pointer );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_Convert( IntPtr handle, string new_type_name, Handle result );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_GetSize( IntPtr obj,IntPtr size );

        /// Returns an Array contains all the valid subscripts for object. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_ListSubscripts( IntPtr obj, IntPtr result );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_GetSubscriptValue( IntPtr obj, IntPtr subscript, Handle result );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_SetSubscriptValue( IntPtr obj, IntPtr subscript, IntPtr value );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 DtValue_DestroySubscript( IntPtr obj, IntPtr subscript );
        #endregion  

        #region Data type specific creation and accessor functions
        // Specialized functions for a few key DT_VALUE types.  The accessor
        // functions below are "safe" to the extent that they fail with meaningful
        // error messages when given a handle that wraps the wrong type of
        // underlying data object.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 NewString( string text, Handle result );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Array_InsertElement( IntPtr array, UInt64 index, IntPtr value );

        /// @return zero on success or an error code. (*)
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Array_DestroyElements( IntPtr array, UInt64 index, UInt64 count );

        /// Create a new Vector.  Copies data from buffer into a new SecDB (*)
        /// allocated buffer.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 NewVector( UInt64 size, IntPtr buffer, IntPtr result );

        /// Get a pointer into the data assocaited with a Vector.  This pointer
        /// is valid until Handle_Free is called on vector.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Vector_GetDataView( IntPtr vector, IntPtr size,
                                         IntPtr[] data );

        /// Get a reference to an existing security object in a database. (*)
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
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 GetSecurity( IntPtr database, StringBuilder security_name,
                                  StringBuilder class_name, UInt64 flags,
                                  IntPtr[] result );


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
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 NewSecurity( IntPtr database, StringBuilder class_name,
                                  StringBuilder security_name, IntPtr[] result );

        /// Search for the names of securities that satisfy certain conditions.  If (*)
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
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 FindSecurities( IntPtr database, string class_name,
                                     string start, UInt32 exclude_start,
                                     UInt32 limit, IntPtr result);

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Security_GetValue(IntPtr security, IntPtr value_type,
                                        IntPtr rguments, IntPtr[] result);

        /// @param flags Flags that are passed to underlying SecDbSetValueWithArgs call.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Security_SetValue(IntPtr security, IntPtr value_type,
                                        IntPtr arguments, IntPtr value,
                                        UInt64 flags );

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Security_DiddleValue(IntPtr security, IntPtr value_type,
                                           IntPtr arguments, IntPtr value);

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Security_InvalidateValue(IntPtr security, IntPtr value_type,
                                               IntPtr arguments);


        /// Returns all value type names assocaited with a given security. (*)
        /// @param security a handle associated with a SecDb security.
        /// @param result (out) an array of strings.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Security_ListValueTypeNames(IntPtr security,
                                                  IntPtr[] result);

        /// @param result (out) a new Curve.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 NewCurve(UInt64 length, IntPtr dates, IntPtr values,
                               IntPtr[] result);

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Curve_GetDates(IntPtr curve, UInt64 length, IntPtr dates);


        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 Curve_GetValues(IntPtr curve, UInt64 length, IntPtr values);

        /// @param result (out) a new TCurve.
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 NewTCurve(UInt64 length, IntPtr imestamps, IntPtr values,
                                IntPtr[] result);

        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 TCurve_GetTimestamps( IntPtr tcurve, UInt64 length,
                                           IntPtr timestamps);
        /// @return zero on success or an error code.
        [DllImport("uberglue.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern UInt32 TCurve_GetValues(IntPtr tcurve, UInt64 length, IntPtr values);
 
        #endregion
    }
}
