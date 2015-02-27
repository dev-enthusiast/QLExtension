// This is a graveyard of sorts for code that we don't really need.  I feel
// like we might want to keep the core C bindings API as small as possible,
// but I'm not sure yet. -gpb

// Returns an Array.
void *ListScopeNames()
{
    try
    {
        int32_t return_code, result_type;
        return CHECK_NONZERO(
                   SlangEval( 0, "Scopes()", &return_code, &result_type ) );
    }
    catch (...)
    {
        ErrMore( __FUNCTION__ " failed." );
        return 0;
    }
}

// Returns an Array.
void *ListVariablesInScope( const char * const scope_name )
{
    try
    {
        std::stringstream ss;
        int32_t return_code, result_type;
        ss << "Variables( \"" << scope_name << "\")";
        return CHECK_NONZERO( SlangEval( 0, ss.str().c_str(),
                                         &return_code, &result_type ) );
    }
    catch (...)
    {
        ErrMore( __FUNCTION__ " failed." );
        return 0;
    }
}

// Create a new vector from size and double's.  Copies the data in buffer.
// Data in buffer should be row contiguous (i.e. C format rather than
// Fortran format).
void *NewMatrix( uint64_t rows, uint64_t columns, double *buffer )
{
    try
    {
        DT_VECTOR source;
        source.Size = size;
        CHECK_NONZERO( source.Size == size );
        source.Alloc = size;
        CHECK_NONZERO( source.Alloc == size );
        source.Elem = buffer;

        DT_VALUE wrapper;
        wrapper.DataType = DtVector;
        wrapper.Data.Pointer = &source;

        // Make a copy of the data; nothing prior to this point can leak
        NewDtValueHandle result( &wrapper );
        return result.give_to_caller();
    }
    catch (...)
    {
        ErrMore( __FUNCTION__ " failed." );
        return 0;
    }

}


void *DtDatabase_GetCachedSecurities(
    void *database,
    const char * const security_type )
{
    // Modeled on SlangXCachedSecurities.
    try
    {
        SDB_DB *db;
        if( database )
        {
            CHECK_NONZERO( database->contents.dt_value.DataType == DtDatabase );
            db = (SDB_DB *) database->contents.dt_value.Data.Pointer;
        }
        else
        {
            db = (SDB_DB *) SecDbRootDb;
        }
        CHECK_NONZERO( db );
        CHECK_NONZERO( security_name );

        SDB_OBJECT *SecPtr;
        if( class_name )
            SecPtr = SecDbGetByNameAndClassName( security_name,
                                                 class_name, db, 0 );
        else
            SecPtr = SecDbGetByName( security_name, db, 0 );
        CHECK_NONZERO( SecPtr );

        NewDtValueHandle result(DtSecurity, SecPtr);
        return result.give_to_caller();
    }
    catch (...)
    {
        ErrMore( __FUNCTION__ " failed." );
        return 0;
    }
}

// If database_name is 0, returns the current root database.
ERROR_CODE GetDatabase( const char * const database_name, void **result )
{
    try
    {
        TracebackLogger logger( __FUNCTION__, __FILE__, __LINE__ - 4 );
        DT_VALUE result;
        result.DataType = DtDatabase;
        if( database_name )
        {
            DT_VALUE str;
            str.DataType = DtString;
            str.Data.Pointer = database_name;
            CHECK_NONZERO( DTM_FROM( &result, &str ) );
        }
        else
        {
            CHECK_NONZERO( DTM_ALLOC( &result, DtDatabase ) );
        }
        TakenHandle result_( result );
        *result = result_.give_to_caller();
        return NO_ERROR
    }
    HANDLE_EXCEPTIONS;

}

