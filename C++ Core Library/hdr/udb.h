/* $Header: /home/cvs/src/userdb/src/udb.h,v 1.15 2000/07/26 23:35:14 keatlj Exp $ */
/****************************************************************
**
**	UDB.H		- User Database API
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.15 $
**
**	$Id: udb.h,v 1.15 2000/07/26 23:35:14 keatlj Exp $
** 	$Source: /home/cvs/src/userdb/src/udb.h,v $
**
****************************************************************/

#ifndef	IN_UDB_H
#define	IN_UDB_H

#include	<portable.h>
#include	<datatype.h>

#include	<sys/types.h>

/*
**	Define constants
*/

#define	UDB_VERSION_MAJOR			0
#define	UDB_VERSION_MINOR			0

#define UDB_PRIV_NO					0
#define UDB_PRIV_YES				1
#define UDB_PRIV_ONLY				2

#define UDB_STATUS_NIL				-2
#define UDB_STATUS_DISABLED			-1
#define UDB_STATUS_PENDING			0
#define UDB_STATUS_ACTIVE			1

#define FUdbStatusLegal(status)		( ( status>=-1 ) && ( status<=1 ) )


/*
**	Define types
*/

typedef struct	Udb_ContextStructure			UDB_CONTEXT;
typedef struct  Udb_ContextInfoStructure		UDB_CONTEXTINFO;
typedef struct	Udb_UserInfoStructure			UDB_USERINFO;
typedef struct	Udb_CompanyInfoStructure		UDB_COMPANYINFO;
typedef struct	Udb_GroupInfoStructure			UDB_GROUPINFO;
typedef struct	Udb_EntitlementInfoStructure	UDB_ENTITLEMENTINFO;
typedef struct	Udb_UrlInfoStructure			UDB_URLINFO;
typedef struct	Udb_AppInfoStructure			UDB_APPINFO;

/*
**	Structure used for returning user information
*/

struct Udb_ContextInfoStructure
{
	char	*szServerName;
	char	*szUserID;
	char	*szApplication;

};	/* UDB_CONTEXTINFO */

struct Udb_UserInfoStructure
{
	char	*szUserID;
	int	iStatus;
	char	*szLast;
	char	*szFirst;
	char	*szMiddle;
	time_t	timUpd;

};	/* UDB_USERINFO */


/*
**	Structure used for returning company information
*/

struct Udb_CompanyInfoStructure
{
	char	*szCompany;
	char	*szName;
	int		iEntity;
	time_t	timUpd;

};	/* UDB_COMPANYINFO */

#define UDB_ENTITY_NULL (-2)
#define UDB_ENTITY_DEFAULT (-1)


/*
**	Structure used for returning group information
*/

struct Udb_GroupInfoStructure
{
	char	*szGroup;
	char	*szDescription;
	time_t	timUpd;

};	/* UDB_GROUPINFO */


/*
**	Structure used for returning entitlement information
*/

struct Udb_EntitlementInfoStructure
{
	char	*szEntitlement;
	char	*szDescription;
	time_t	timUpd;

};	/* UDB_ENTITLEMENTINFO */


/*
**	Structure used for returning url information
*/

struct Udb_UrlInfoStructure
{
	char	*szUrl;
	char	*szEntitlement;
	char	*szDescription;
	time_t	timUpd;

};	/* UDB_URLINFO */


/*
**	Structure used for returning app information
*/

struct Udb_AppInfoStructure
{
	char	*szApp;
	char	*szPassword;
	char	*szDescription;
	char	*szContact;
	char	*szEmail;
	char	*szPhone;
	time_t	timUpd;

};	/* UDB_APPINFO */

/*
**	Functions for operating on udb connections
*/


DLLEXPORT void		Udb_Destroy(				UDB_CONTEXT *Context );
DLLEXPORT void 		Udb_MemoryFree(	   			UDB_CONTEXT *Context );
DLLEXPORT int		Udb_ActAs(	 				UDB_CONTEXT *Context, const char *szUserID );
DLLEXPORT UDB_CONTEXTINFO
					*Udb_ContextInfo(			UDB_CONTEXT *Context );


DLLEXPORT int		Udb_Statistics(				UDB_CONTEXT *Context, int Connection, DT_VALUE *pdtv);
DLLEXPORT int		Udb_Ping(				    UDB_CONTEXT *Context );

/*
**	Cache management
*/

DLLEXPORT int Udb_EntitlementCacheOn( UDB_CONTEXT *Context, const char *szUser);
DLLEXPORT int Udb_EntitlementCacheOff( UDB_CONTEXT *Context );

/*
**	Functions to manage users
*/

DLLEXPORT UDB_USERINFO
		*Udb_UserGet(							UDB_CONTEXT	*Context, const char *szUserID );
DLLEXPORT int 		Udb_UserSet(				UDB_CONTEXT *Context, const char *szUserID, UDB_USERINFO *pUserInfo );
DLLEXPORT int 		Udb_UserDelete(				UDB_CONTEXT	*Context, const char *szUserID );
DLLEXPORT int 		Udb_UserAdd(   				UDB_CONTEXT	*Context, const char *szCompany, const char *szUserID, int iStatus, const char *szLast, const char *szFirst, const char *szMiddle );
DLLEXPORT int 		Udb_UserList(  				UDB_CONTEXT	*Context, const char *szRoot, int cLevels, DT_VALUE *pdtv );

		
/*
**	Functions to manage companies
*/

DLLEXPORT UDB_COMPANYINFO
		*Udb_CompanyGet(						UDB_CONTEXT	*Context, const char *szCompany );
DLLEXPORT int 		Udb_CompanySet(				UDB_CONTEXT	*Context, const char *szCompany, UDB_COMPANYINFO *pCompanyInfo );
DLLEXPORT int 		Udb_CompanyDelete(			UDB_CONTEXT	*Context, const char *szCompany );
DLLEXPORT int 		Udb_CompanyAdd(				UDB_CONTEXT	*Context, const char *szCompany, const char *szName, const int EntityID );
DLLEXPORT int 		Udb_CompanyList(	    	UDB_CONTEXT	*Context, const char *szRoot, int cLevels, DT_VALUE *pdtv );
DLLEXPORT int		Udb_CompanyRefCount(		UDB_CONTEXT *Context, const char *szCompany );

/*
**	Functions to manage groups
*/

DLLEXPORT UDB_GROUPINFO
		*Udb_GroupGet(							UDB_CONTEXT	*Context, const char *szGroup );
DLLEXPORT int		Udb_GroupSet(				UDB_CONTEXT *Context, const char *szGroup, UDB_GROUPINFO *pinfo );
DLLEXPORT int 		Udb_GroupDelete(			UDB_CONTEXT	*Context, const char *szGroup );
DLLEXPORT int  		Udb_GroupAdd(				UDB_CONTEXT	*Context, const char *szGroup, const char *szDescription );
DLLEXPORT int  		Udb_GroupList(				UDB_CONTEXT	*Context, const char *szRoot, int cLevels, DT_VALUE *pdtv );
DLLEXPORT int		Udb_GroupRefCount(		UDB_CONTEXT *Context, const char *szGroup );


/*
**	Functions to manage entitlements
*/

DLLEXPORT UDB_ENTITLEMENTINFO
					*Udb_EntitlementGet(		UDB_CONTEXT	*Context, const char *szEntitlement );
DLLEXPORT int		Udb_EntitlementSet(			UDB_CONTEXT	*Context, const char *szEntitlement, UDB_ENTITLEMENTINFO *pinfo );
DLLEXPORT int  		Udb_EntitlementDelete(		UDB_CONTEXT	*Context, const char *szEntitlement );
DLLEXPORT int  		Udb_EntitlementAdd(			UDB_CONTEXT	*Context, const char *szEntitlement, const char *szDescription );
DLLEXPORT int  		Udb_EntitlementList(		UDB_CONTEXT	*Context, const char *szRoot, int cLevels, DT_VALUE *pdtv );
DLLEXPORT int  		Udb_EntitlementUrlList(		UDB_CONTEXT	*Context, const char *szEntitlement, DT_VALUE *pdtv );
DLLEXPORT int		Udb_EntitlementRefCount(		UDB_CONTEXT *Context, const char *szEntitlement );


/*
**	Functions to manage urls
*/

DLLEXPORT UDB_URLINFO
					*Udb_UrlGet(	   			UDB_CONTEXT	*Context, const char *szUrl );
DLLEXPORT int		Udb_UrlSet(					UDB_CONTEXT	*Context, const char *szUrl, UDB_URLINFO *pinfo );
DLLEXPORT int 		Udb_UrlDelete(				UDB_CONTEXT	*Context, const char *szUrl );
DLLEXPORT int 		Udb_UrlAdd(					UDB_CONTEXT	*Context, const char *szUrl, const char *szEntitlement, const char *szDescription );
DLLEXPORT int 		Udb_UrlList(				UDB_CONTEXT	*Context, const char *szRoot, int cLevels, DT_VALUE *pdtv );
					
/*
**	Functions to manage applications
*/

DLLEXPORT UDB_APPINFO *Udb_AppGet(
				  UDB_CONTEXT		*Context,
				  const char		*szApp
				  )
     ;

DLLEXPORT int Udb_AppSet(
			 UDB_CONTEXT 	*Context,
			 const char	*szApp,
			 UDB_APPINFO	*pinfo
			 )
     ;

DLLEXPORT int Udb_AppDelete(
			    UDB_CONTEXT		*Context,
			    const char		*szApp
			    )
     ;

DLLEXPORT int Udb_AppAdd(
			 UDB_CONTEXT		*Context,
			 const char		*szApp,
			 const char		*szPassword,
			 const char		*szDescription,
			 const char		*szContact,
			 const char		*szEmail,
			 const char		*szPhone
			 )
     ;

DLLEXPORT int Udb_AppList(
			  UDB_CONTEXT *pudb,
			  const char *szRoot,
			  int cLevels,
			  DT_VALUE *pdtv
			  )
     ;


/*
**	Functions to operate on entitlements
*/

DLLEXPORT int		Udb_UserUrlCheck(			UDB_CONTEXT *Context, const char *szUserID, const char *szUrl );
DLLEXPORT int		Udb_UserUrlCheckAny(			UDB_CONTEXT *Context, const char *szUserID, DT_VALUE *Urls );
DLLEXPORT int		Udb_UserUrlCheckMultiple(			UDB_CONTEXT *Context, const char *szUserID, DT_VALUE *Urls, DT_VALUE *Result );
DLLEXPORT int		Udb_UserUrlList(		UDB_CONTEXT *Context, const char *szUserID, const char *szRoot, int cLevels, DT_VALUE *pdtv );
DLLEXPORT int 		Udb_UserEntitlementCheck(	UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT int 		Udb_UserEntitlementGet(		UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT int 		Udb_UserEntitlementSet(		UDB_CONTEXT *Context, const char *szUserID, const char *szProperty, int iEntitlement );
DLLEXPORT int 		Udb_UserEntitlementDelete(	UDB_CONTEXT	*Context, const char *szUserID, const char *szProperty );
DLLEXPORT int 		Udb_UserEntitlementList(	UDB_CONTEXT	*Context, const char *szUserID, DT_VALUE *pdtv );
		

/*
**	Functions to manage user membership in groups
*/

DLLEXPORT int		Udb_UserGroupGet(	 		UDB_CONTEXT *Context, const char *szUserID, const char *szGroup );
DLLEXPORT int		Udb_UserGroupCheck(	 		UDB_CONTEXT *Context, const char *szUserID,	const char *szGroup );
DLLEXPORT int		Udb_UserGroupSet(			UDB_CONTEXT *Context, const char *szUserID,	const char *szGroup, int iEntitlement );
DLLEXPORT int 		Udb_UserGroupDelete(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szGroup );
DLLEXPORT int		Udb_UserGroupList(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *pdtv );
									

/*
**	Functions to manage group entitlements
*/

DLLEXPORT int		Udb_GroupEntitlementGet(	UDB_CONTEXT *Context, const char *szGroupID, const char *szProperty );
DLLEXPORT int		Udb_GroupEntitlementCheck(	UDB_CONTEXT *Context, const char *szGroupID, const char *szProperty );
DLLEXPORT int 		Udb_GroupEntitlementSet(	UDB_CONTEXT *Context, const char *szGroupID, const char *szProperty, int iEntitlement );
DLLEXPORT int		Udb_GroupEntitlementDelete(	UDB_CONTEXT	*Context, const char *szGroupID, const char *szProperty );
DLLEXPORT int		Udb_GroupEntitlementList(	UDB_CONTEXT	*Context, const char *szGroupID, DT_VALUE *pdtv );


/*
**	Functions to manage groups within groups (ineherited membership)
*/

DLLEXPORT int		Udb_GroupGroupGet(			UDB_CONTEXT *Context, const char *szGroupID, const char *szGroup );
DLLEXPORT int		Udb_GroupGroupCheck(		UDB_CONTEXT *Context, const char *szGroupID, const char *szGroup );
DLLEXPORT int		Udb_GroupGroupSet(			UDB_CONTEXT *Context, const char *szGroupID, const char *szGroup, int iEntitlement );
DLLEXPORT int		Udb_GroupGroupDelete(		UDB_CONTEXT *Context, const char *szGroupID, const char *szPGroup );
DLLEXPORT int		Udb_GroupGroupList(			UDB_CONTEXT *Context, const char *szGroupID, DT_VALUE *pdtv );

/*
**	Functions to operate on application entitlements
*/

DLLEXPORT int		Udb_AppEntitlementGet(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty );
DLLEXPORT int		Udb_AppEntitlementCheck(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty );
DLLEXPORT int 		Udb_AppEntitlementSet(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty, int iEntitlement );
DLLEXPORT int		Udb_AppEntitlementDelete(	UDB_CONTEXT	*Context, const char *szApp, const char *szProperty );
DLLEXPORT int		Udb_AppEntitlementList(	UDB_CONTEXT	*Context, const char *szApp, DT_VALUE *pdtv );

/*
**	Functions to operate on application groups
*/

DLLEXPORT int		Udb_AppGroupGet(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty );
DLLEXPORT int		Udb_AppGroupCheck(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty );
DLLEXPORT int 		Udb_AppGroupSet(	UDB_CONTEXT *Context, const char *szApp, const char *szProperty, int iGroup );
DLLEXPORT int		Udb_AppGroupDelete(	UDB_CONTEXT	*Context, const char *szApp, const char *szProperty );
DLLEXPORT int		Udb_AppGroupList(	UDB_CONTEXT	*Context, const char *szApp, DT_VALUE *pdtv );


/*
**	Functions to operate on the user registry
*/

DLLEXPORT int 		Udb_UserRegistryDelete(			UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT char 		*Udb_UserRegistryGetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_UserRegistrySetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const char *Value );
DLLEXPORT double	Udb_UserRegistryGetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_UserRegistrySetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const double Value );
DLLEXPORT int		Udb_UserRegistryGet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int		Udb_UserRegistryGetMultiple(			UDB_CONTEXT	*Context, const char *szUserID, DT_VALUE *Keys, DT_VALUE *Value );
DLLEXPORT int		Udb_UserRegistryMultiGet(		UDB_CONTEXT *Context, DT_VALUE *Owners, const char *szKey, DT_VALUE *Value );
DLLEXPORT int 		Udb_UserRegistrySet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int 		Udb_UserRegistryList(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Value );

DLLEXPORT char 		*Udb_UserRegistryGetStringFallback(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT double	Udb_UserRegistryGetNumberFallback(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int		Udb_UserRegistryGetFallback(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int		Udb_UserRegistryGetMultipleFallback(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Keys, DT_VALUE *Value );
DLLEXPORT int		Udb_UserRegistryMultiGetFallback(		UDB_CONTEXT *Context, DT_VALUE *Owners, const char *szKey, DT_VALUE *Value );

/*
**	Functions to operate on the company registry
*/

DLLEXPORT int 		Udb_CompanyRegistryDelete(			UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT char 		*Udb_CompanyRegistryGetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_CompanyRegistrySetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const char *Value );
DLLEXPORT double	Udb_CompanyRegistryGetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_CompanyRegistrySetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const double Value );
DLLEXPORT int		Udb_CompanyRegistryGet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int		Udb_CompanyRegistryGetMultiple(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Keys, DT_VALUE *Value );
DLLEXPORT int		Udb_CompanyRegistryMultiGet(		UDB_CONTEXT *Context, DT_VALUE *Owners, const char *szKey, DT_VALUE *Value );
DLLEXPORT int 		Udb_CompanyRegistrySet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int 		Udb_CompanyRegistryList(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Value );

/*
**	Functions to operate on the URL registry
*/

DLLEXPORT int 		Udb_UrlRegistryDelete(			UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT char 		*Udb_UrlRegistryGetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_UrlRegistrySetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const char *Value );
DLLEXPORT double	Udb_UrlRegistryGetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_UrlRegistrySetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const double Value );
DLLEXPORT int		Udb_UrlRegistryGet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int		Udb_UrlRegistryGetMultiple(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Keys, DT_VALUE *Value );
DLLEXPORT int		Udb_UrlRegistryMultiGet(		UDB_CONTEXT *Context, DT_VALUE *Owners, const char *szKey, DT_VALUE *Value );
DLLEXPORT int 		Udb_UrlRegistrySet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int 		Udb_UrlRegistryList(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Value );

/*
**	Functions to operate on the app registry
*/

DLLEXPORT int 		Udb_AppRegistryDelete(			UDB_CONTEXT *Context, const char *szUserID, const char *szProperty );
DLLEXPORT char 		*Udb_AppRegistryGetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_AppRegistrySetString(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const char *Value );
DLLEXPORT double	Udb_AppRegistryGetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty );
DLLEXPORT int 		Udb_AppRegistrySetNumber(		UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, const double Value );
DLLEXPORT int		Udb_AppRegistryGet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int		Udb_AppRegistryGetMultiple(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Keys, DT_VALUE *Value );
DLLEXPORT int		Udb_AppRegistryMultiGet(		UDB_CONTEXT *Context, DT_VALUE *Owners, const char *szKey, DT_VALUE *Value );
DLLEXPORT int 		Udb_AppRegistrySet(			UDB_CONTEXT	*Context, const char *szUserID,	const char *szProperty, DT_VALUE *Value );
DLLEXPORT int 		Udb_AppRegistryList(			UDB_CONTEXT	*Context, const char *szUserID,	DT_VALUE *Value );

/* Multiple lookups */
DLLEXPORT int Udb_UserEntitlementCheckAny( UDB_CONTEXT *pudb, const char *szUser, DT_VALUE *pdtvEntitlements );
DLLEXPORT int Udb_EntitlementCacheFetch( UDB_CONTEXT *pudb, const char *szUser, DT_VALUE *pdtvEntitlement );
DLLEXPORT int Udb_UserEntitlementCheckMultiple( UDB_CONTEXT *pudb, const char *szUser, DT_VALUE *pdtvEntitlements, DT_VALUE *pdtvReturn );

/* Child lookups */
DLLEXPORT int Udb_UserEntitlementCheckChildren( UDB_CONTEXT *pudb, const char *szUser, const char *szEntitlement );
DLLEXPORT int Udb_UserUrlCheckChildren( UDB_CONTEXT *pudb, const char *szUser, const char *szUrl );


	 
DLLEXPORT UDB_CONTEXT *Udb_Connect(
			const char	*szServer,
			const char	*szAppId,
			const char	*szPassword,
			const char	*szAppDesc,
			const char	*szUser,
			unsigned int	iFlags
			);

DLLEXPORT int Udb_ConnectionList(
		       UDB_CONTEXT	*Context,
		       DT_VALUE		*pdtvReturn
		       );
DLLEXPORT int Udb_ServerInfo(
		       UDB_CONTEXT	*Context,
		       DT_VALUE		*pdtvReturn
	);

DLLEXPORT int Udb_InitializeSafely(
	void
	);

#endif /* IN_UDB_H */
