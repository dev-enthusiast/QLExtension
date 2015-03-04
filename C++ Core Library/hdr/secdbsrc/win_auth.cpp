#define GSCVSID "$Header: /home/cvs/src/secdb/src/win_auth.cpp,v 1.9 2015/02/27 23:12:41 khodod Exp $"
/**************************************************************************
**
**   win_auth.cpp   - Windows-specific Active Directory access code
**
**   Copyright (c) 2013-2004 - Constellation, an Exelon Company - Baltimore
**   
**   $Log: win_auth.cpp,v $
**   Revision 1.9  2015/02/27 23:12:41  khodod
**   Remove the a-key group.
**   AWR: #364118
**
**   Revision 1.8  2015/02/27 22:49:24  khodod
**   Add checks for admin rights, if we are running database administration
**   utilities.
**   AWR: #364118
**
**   Revision 1.7  2013/08/13 05:27:44  khodod
**   Add new group names in preparation for the EXELONDS domain migration.
**   Explicitly check for membership in the basic-access groups so that basic
**   access to the file system is not enough to run the application. Not perfect,
**   but better than before.
**   AWR: #310154
**
**   Revision 1.6  2012/09/17 14:02:49  e19351
**   Use native WinLDAP library on nt instead of packaged copy.
**   This is to make sure we're keeping up with the platform SDK on newer
**   Windows compilers (e.g. VC10).
**   AWR: #177555
**
**   Revision 1.5  2005/10/26 18:01:02  khodod
**   Use a list of IP addresses corresponding to the LDAP server as the
**   first argument to ldap_init to improve reliability.
**   iBug: #30641
**
**   Revision 1.4  2005/08/19 20:12:35  khodod
**   Added SECDB_LDAP_URL, which is now by default set to ldapquery.ceg.corp.net
**   in secdb.dat.
**   iBug: #28290
**
**   Revision 1.3  2005/06/15 14:33:04  khodod
**   Allow to override the bind_dn for the membership search.
**   iBug: #25929
**
**   Revision 1.2  2004/08/16 17:38:07  khodod
**   Changes to link against the dll in the external project.
**   iBug #12504
**   
**   Revision 1.1  2004/08/12 20:52:50  khodod
**   Initial revision.
**   iBug #12504
**   
**
***************************************************************************/

#define BUILDING_SECDB
#include    <portable.h>
#include    <kool_ade.h>
#include    <err.h>
#include    <stdio.h>
#include    <secdb.h>
#include    <socklib.h>
#include    <tcpip.h>

#include    <windows.h>
#include    <winldap.h>
//#include    <winber.h> 

#include    <string>

CC_USING( std::string );

struct case_insensitive_char_traits : public std::char_traits<char> 
{
    static bool eq( char a, char b )
    { 
        return tolower( a ) == tolower( b );
    }

    static bool neq( char a, char b )
    {
        return tolower( a ) != tolower( b );
    }

    static bool lt( char a, char b )
    {
        return tolower( a ) < tolower( b );
    }

    static int compare( const char *s1, const char *s2, size_t n )
    {
        return strnicmp( s1, s2, n ); 
    }

    static const char *find( const char *s, string::size_type n, const char &c )
    {
        while( *s && tolower( *s ) != tolower( c ) ) {
            ++s;
        }
        return tolower( *s ) == tolower( c ) ? s : 0;
    }
};

typedef std::basic_string< char, case_insensitive_char_traits > istring;

/*
 * Static functions.
 */

static bool 
    query1( LDAP *lh, char *bind_dn, char *filter, ULONG search_scope, 
            size_t num_attrs, PCHAR attrs[], istring *attr_values);

static istring 
    extract_id( istring &url );

static bool 
    check_group_membership( istring &groups, const char **groups_to_check );

static bool
   init_user( LDAP *lh, const char *user_name );


struct ldap_handle_guard {
    ldap_handle_guard( LDAP *lh ) : m_lh( lh ) {}
    ~ldap_handle_guard( ) {
        if( m_lh ) {
            ldap_unbind( m_lh );
        }
    }

private:
    LDAP *m_lh;
};

/**************************************************************************
**   Routine:   get_conn_params
**   Returns:   void
**   Action:    Parses the value of SECDB_LDAP_URL. 
***************************************************************************/

string
get_conn_params( int& port)
{
    char *tmp_url = const_cast<char *>( 
        SecDbConfigurationGet( "SECDB_LDAP_URL", NULL, NULL, 0 )
    );
    
    if( !tmp_url ) 
        return "";

    string url = tmp_url;
    if( url.find(" ") != string::npos ) 
    {
        return url; // Let the caller worry about embedded spaces.
    }
    string::size_type pos = url.find(":");
    if( pos != string::npos ) 
    {
        port = atoi( url.substr( pos + 1, string::npos ).c_str() );
        url.erase( pos, string::npos );
    }
    
    SocketInit(); // Needed for gethostbyname to work.
    struct hostent *h_ent = gethostbyname( url.c_str() );
    if (!h_ent) {
        return tmp_url; // Let the caller worry about resolution failures.
    }
    string resolved_list;
    for( char **addr_ptr = h_ent->h_addr_list; *addr_ptr; ++addr_ptr )
    {
        struct in_addr sr_addr;
        sr_addr.s_addr = *(reinterpret_cast<u_long *>( *addr_ptr ) );
        char *ptr = inet_ntoa( sr_addr );
        if( ptr ) {
            resolved_list += ptr;
            resolved_list += " ";
        }
    }
    resolved_list.erase( resolved_list.size() - 1 ); // get rid of trailing space
    return resolved_list;
}

/**************************************************************************
**   Routine:   win_auth
**   Returns:   true/false 
**   Action:    On success sets SecDbId to the legacy id from Active 
**              Directory, sets IsDeveloper to true if the user is 
**              a developer, sets IsAdmin to true if the user is an 
**				admin.
***************************************************************************/

bool
win_auth( const char *user_name, string &SecDbId, bool &IsDeveloper, bool &IsAdmin )
{
    LDAP *lh = 0;
    ULONG ret;
    ULONG opt;

    int port = LDAP_PORT; // Default port
    string url = get_conn_params( port );
    lh = ldap_init( ( url.size() ? const_cast< char * >( url.c_str() ) : NULL ), port );
    if( !lh ) 
        return( Err( ERR_UNKNOWN, 
                     "ldap_init(): %s\n", ldap_err2string( LdapGetLastError() ) ) );

    ldap_handle_guard g( lh );

    opt = LDAP_VERSION3;
    ret = ldap_set_option( lh, LDAP_OPT_PROTOCOL_VERSION, (void *) &opt );
    if( ret != LDAP_SUCCESS )
    {
        return Err( ERR_UNKNOWN, 
                    "ldap_set_option( LDAP_OPT_PROTOCOL_VERSION ): %s\n", 
                    ldap_err2string( ret ) );
    }

    ret = ldap_connect( lh, 0 );
    if( ret != LDAP_SUCCESS ) 
        return Err( ERR_UNKNOWN, "ldap_connect(): %s\n", ldap_err2string( ret ) ) ;   
   
    ret = ldap_bind_s( lh, 0, 0, LDAP_AUTH_NEGOTIATE );
    if( ret != LDAP_SUCCESS ) 
        return Err( ERR_UNKNOWN, "ldap_bind_s(): %s\n", ldap_err2string( ret ) );

    
    char filter[128];
    char *bind_dn = const_cast<char *>( 
            SecDbConfigurationGet( "SECDB_LDAP_BIND_DN", NULL, NULL, 
                                   "dc=CEG,dc=Corp,dc=Net" ) );
    ULONG search_scope = LDAP_SCOPE_SUBTREE;
   
    snprintf( filter, sizeof( filter ) - 1, "(SAMAccountName=%s)", user_name );

    // XXX - this relies on attribute values being aligned with the
    // order of attributes, which, in general, is not the case.
    PCHAR attrs[3] = { "memberOf", "url", 0 };
    istring values[ 2 ];

    if( !query1( lh, bind_dn, filter, search_scope, 2, attrs, values ) ) 
        return ErrMore( "Could not obtain account information; search failed" );

    istring LegacyID = extract_id( values[1] );
    if( LegacyID == "<unknown>" )
        SecDbId = user_name;
    else
        SecDbId = LegacyID.c_str();
    
    const char *basic_access_groups[] = {
        "CPS Active Trading SECDB",
        "CEG Active Trading SECDB",
        "ALL SecDb Users",
        0
    };

    if( !check_group_membership( values[0], basic_access_groups ) )
        return Err( ERR_UNKNOWN, "Access denied: user %s is not in any SecDb groups", user_name );

    const char *dev_access_groups[] = { 
        "CEG Active SECDB DEV",
        "CPS Active SECDB Dev",
        "ALL SecDb Developers",
        "CPS Process",
        0
    };

    IsDeveloper = check_group_membership( values[0], dev_access_groups );

    const char *admin_access_groups[] = {
        "ALL SecDb Admins",
        0
    };

    IsAdmin = check_group_membership( values[0], admin_access_groups );
    return true;
}

/**************************************************************************
**   Routine:   check_group_membership
**   Returns:   true/false 
**   Arguments: groups -- 
**                  newline-separated case-insensitive string of all
**                  groups the account is a member of
**              groups_to_check -- 
**                  list of groups to check for membership in
**   Action:    Check if the user is a member of at least one group in 
**              NULL-terminated list of group names and true on success 
**              and false otherwise.
***************************************************************************/

bool 
check_group_membership( istring &all_groups, const char **groups_to_check )
{
    for( const char **group = groups_to_check; *group; ++group ) {
        if( all_groups.find( *group ) != istring::npos )
            return true;
    }
    return false;
}

/**************************************************************************
**   Routine:   extract_id
**   Returns:   the SecDb legacy id or "<unknown>"
**   Action: 
***************************************************************************/

istring
extract_id( istring &url_blob )
{
    istring SecDbId = "<unknown>";
    istring::size_type pos1, pos2, tmp;
    size_t n;
    
    pos1 = url_blob.find( "legacy" );
    if( pos1 != istring::npos ) {
        tmp = url_blob.find( ":", pos1 );
        if( tmp != istring::npos ) {
            pos1 = tmp + 1;
            pos2 = url_blob.find( "\n", pos1 ); 
            tmp = url_blob.find_first_not_of( " ", pos1 );
            if( tmp != istring::npos ) {
                pos1 = tmp;
                pos2;
                n = pos2 - pos1;
                SecDbId = url_blob.substr( pos1, n );
            }
        }
    }
    return SecDbId;    
}
 
/**************************************************************************
**   Routine:   query1
**   Returns:   true/false
**   Action:    Search Active Directory using filter and search_scope
**              For successful completion, the search must yield exactly 
**              one match. 
***************************************************************************/

struct query_result_guard {
    query_result_guard( LDAPMessage *m ) : m_msg( m ) {}
    ~query_result_guard( ) {
        if( m_msg ) {
            ldap_msgfree( m_msg );
        }
    }

private:
    LDAPMessage *m_msg;
};

bool
query1( LDAP *lh, char *bind_dn, char *filter, ULONG search_scope, size_t num_attrs, 
        PCHAR attrs[], istring *attr_values )
{
    int attrs_and_values = 0; 
    LDAPMessage *search_result = 0;
    ULONG ret;

    ret = ldap_search_s( lh, bind_dn, 
                         search_scope, filter, 
                         attrs, attrs_and_values, &search_result ); 
    query_result_guard g( search_result );

    if( ret != LDAP_SUCCESS ) 
        return Err( ERR_UNKNOWN, "ldap_search_s(): %s\n", ldap_err2string( ret ) );

    ULONG num_entries = ldap_count_entries( lh, search_result );
    if( num_entries != 1 ) 
        return Err( ERR_UNKNOWN, "ldap_count_entries() returned %d matches\nargs were ( %s, %s )", 
                    num_entries, bind_dn, filter );

    char *func_name = 0; 
    
    LDAPMessage *entry = 0; 
    BerElement *ber = 0;
    PCHAR attr = 0;
    PCHAR *values = 0;
    ULONG num_values = 0;
    ULONG count = 0;

    entry = ldap_first_entry( lh, search_result );
    if( !entry ) 
        return Err( ERR_UNKNOWN, "ldap_first_entry(): %s\n", ldap_err2string( LdapGetLastError() ) );
   
    for( count = 0, attr = ldap_first_attribute( lh, entry, &ber );
         count < num_attrs && attr; 
         ++count, attr = ldap_next_attribute( lh, entry, ber ) ) {
        
        values = ldap_get_values( lh, entry, attr );
        if( values ) {
            num_values = ldap_count_values( values );
            if( num_values == 0 ) {
                ldap_value_free( values );
                return Err( ERR_UNKNOWN, "ldap_count_values(): no values for attribute: %s\n", attr ); 
            }

            for( ULONG j = 0; j < num_values; ++j ) {
                attr_values[ count ] += values[ j ];
                attr_values[ count ] += "\n";
            }
            ldap_value_free( values );
        }
        ldap_memfree( attr );
    }

//    if( ber )
//        ber_free( ber, 0 );
 
    return true;
}
