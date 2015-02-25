/* $Header: /home/cvs/src/datatype/src/dtoci.h,v 1.28 2014/09/30 15:23:00 c01713 Exp $ */
/****************************************************************
**
**  DTOCI.H    - Header for OCI datatype
**
**  Copyright (c) 2011 - Constellation Energy Group, Baltimore, MD
**  Author:         Shivvasangari Subramani
**  Date:           2011-02-14
**
****************************************************************/

#ifndef IN_DTOCI_H 
#define IN_DTOCI_H

#include <oci.h>
#include <dtdb.h>
#include <hash.h>

#define MAX_STR_SIZE 512

typedef enum { TRANS_COMMIT_AUTO, TRANS_COMMIT_MANUAL } OciTransactionMode;

/* Structure used to contain OCIConnection datatype information. */
typedef struct DtOCIConnectionStructure
{
    OCIEnv        *envhp;						/*	Environment handle        */
    OCIError      *errhp;						/*	Error handle              */
    OCISession    *authp;						/*	User session handle       */
    OCIServer     *srvhp;						/*	Server handle             */
    OCISvcCtx     *svchp;						/*	Service handle            */
    DB_DATE_FMT   dateFormat;					/*  DB_TIME = 0, DB_DATE = 1  */
    int           status,						/*	OCI return values         */
                  useCount;						/*	Reference count           */

    OciTransactionMode transactionMode;         /* TRANS_COMMIT_AUTO or TRANS_COMMIT_MANUAL */

    char          errMsg[ MAX_STR_SIZE ];		/*	OCI error message         */
	char          schemaName[ MAX_STR_SIZE ];	/*	Name of the current schema*/
    char          *connDesc;					/*	Connection string         */
    char          *timeZone;                    /*  Time zone to which values returned from
                                                    OCIRpc(), OCIExec() are converted to.
                                                    Default value is NULL, value is
                                                    converted to GMT. */
    HASH          *procMetaDataCache;			/*	Metadata for stored procs */
    HASH          *namedTypeCache;			    /*	Stores type descriptor of
                                                    named type like ODCINUMBERLIST, ODCIDATELIST
                                                */
} DT_OCI_CONNECTION;

/* Structure used to contain DtOCILobLocator datatype information. */
struct DT_OCI_LOB_LOCATOR
{
    OCILobLocator *Loc;                 /*  Large object locator        */
    int UseCount;						/*	Reference count             */
	ub4 Type;                           /*  Type of LobLocator          */
    ub2 DType;                          /*  Type of data pointed to by Lob locator SQLT_BLOB/SQLT_CLOB */
    
    DT_OCI_LOB_LOCATOR(): Loc( NULL ), UseCount( 0 ), Type( 0 ), DType( 0 )
    {
    }

    DT_OCI_LOB_LOCATOR( OCILobLocator *L, int Uc, ub4 T, ub2 Dt ):
        Loc( L ),
        UseCount( Uc ),
        Type( T ),
        DType( Dt )
    {
    }
    
    ~DT_OCI_LOB_LOCATOR()
    {
        if( this->Loc != NULL )
            OCIDescriptorFree( this->Loc, this->Type );
    }
};

DLLEXPORT DT_DATA_TYPE DtOCIConnection;
DLLEXPORT DT_DATA_TYPE DtOCILobLocator;

/****************************************************************
**	Routine:	DtFuncOCIConnection
**	Returns:	TRUE	- Operation worked.
**				FALSE	- Operation failed.
**	Action :	Handle operations for OCIConnection datatype.
****************************************************************/
DLLEXPORT int DtFuncOCIConnection( 
            int                 Msg, 
            DT_VALUE             *r, 
            DT_VALUE             *a, 
            DT_VALUE             *b 
);

/****************************************************************
**	Routine:	DtFuncOCILobLocator
**	Returns:	TRUE	- Operation worked.
**				FALSE	- Operation failed.
**	Action :	Handle operations for DtOCILobLocator datatype.
****************************************************************/
DLLEXPORT int DtFuncOCILobLocator( 
            int                 Msg, 
            DT_VALUE             *r, 
            DT_VALUE             *a, 
            DT_VALUE             *b 
);

/****************************************************************
**
**	Routine	:	DtOCIConnect
**	Returns	:	Connection structure or NULL.
**	Action	:	Connect to an Oracle server and populate the OCI 
**				Connection structure with connection information. 
**	Args	:	Database	-	Database to connect in to.
**				User		-	User to login as.
**				Password	-	Password for user.
**				dateFormat	-	DB_TIME/DB_DATE (Default: DB_TIME).
**				clientInfo  -   Additional application info
**								provided by the user.
**              transMode   -   TRANS_COMMIT_AUTO, TRANS_COMMIT_MANUAL
**              timeZone    -   Time zone to which DATE values returned
**                              from OCIRpc(), OCIExec() are converted to.
**                              Default value is NULL, value is
**                              converted to GMT.
****************************************************************/
DLLEXPORT DT_OCI_CONNECTION *DtOCIConnect(
            const char           *Database,
            const char           *User, 
            const char           *Password,
            DB_DATE_FMT          dateFormat,
            const char           *clientInfo,
            OciTransactionMode   transMode = TRANS_COMMIT_AUTO,
            const char           *timeZone = NULL
);

/****************************************************************
**
**	Routine	:	DtOCICleanUp
**	Returns	:	Nothing.
**	Action	:	Free OCI handles, and disconnect from an Oracle server.
**	Args	:	OCIConnection - OCI Connection Structure.
**
****************************************************************/
DLLEXPORT void DtOCICleanUp( 
            DT_OCI_CONNECTION    *OCIConnection
);

/****************************************************************
**	Routine:	DT_OCI_ROW_CB
**	Returns:	TRUE	- Operation worked.
**				FALSE	- Operation failed.
**	Action :	Appends row structure to ret array.
****************************************************************/
typedef int DT_OCI_ROW_CB(
            DT_VALUE            *RowValues, 
            int                 ResultNumber,
            void                *CallbackArg
);

/****************************************************************
**	Routine:	DT_OCI_Proc_CB
**	Returns:	TRUE	- Operation worked.
**				FALSE	- Operation failed.
**	Action :	Appends row structure to ret array.
****************************************************************/
typedef void DT_OCI_PROC_CB(
				DT_VALUE    *tempArray,
				DT_VALUE	*RPCResult,
				char        *colName,
				void        *CallbackArg
);

/****************************************************************
**
** Routine: DtOCIExec
** Returns: TRUE/FALSE - Success of DtOCIExec().
** Action : Executes a SQL Statement.
** Args   : OCIConnection	- OCI Connection structure.
**          Statment		- SQL statement to execute.
**          rowCallback		- Function to call for each returned row.
**          callbackArg		- Extra arg to pass to rowCallback.
**          retCount		- Number of rows affected. 
**			callType		- 1 for queries; 0 otherwise.
**
****************************************************************/

DLLEXPORT int DtOCIExec( 
            DT_OCI_CONNECTION	*OCIConnection, 
            const char			*statement, 
            DT_OCI_ROW_CB		*rowCallback,
            void				*callbackArg,
            int					*retCount,
			int					*callType
);

/****************************************************************
**
** Routine: DtOCILobRead
** Returns: TRUE/FALSE.
** Action : Read from Lob locator.
** Args   : OCIConnection	- OCI Connection structure.
**          LobLocator      - Lob locator to operate on.
**          Ret             - Return value DtBinary or DtString.
****************************************************************/
DLLEXPORT int DtOCILobRead( 
            DT_OCI_CONNECTION	*OCIConnection, 
            DT_OCI_LOB_LOCATOR  *LobLocator,
            DT_VALUE			*Ret
);

/****************************************************************
**
** Routine: DtOCILobWrite
** Returns: TRUE/FALSE.
** Action : Write into Lob locator.
** Args   : OCIConnection	- OCI Connection structure.
**          LobLocator      - Lob locator to operate on.
**          Data            - Data to write.
****************************************************************/
DLLEXPORT int DtOCILobWrite( 
            DT_OCI_CONNECTION	*OCIConnection, 
            DT_OCI_LOB_LOCATOR  *LobLocator,
            DT_VALUE			*Data
);                  

/****************************************************************
**
** Routine: DtOCICommit
** Returns: TRUE/FALSE.
** Action : Commit transaction.
** Args   : OCIConnection	- OCI Connection structure.
**
****************************************************************/
DLLEXPORT int DtOCICommit( 
            DT_OCI_CONNECTION	*OCIConnection
);                  

/****************************************************************
**
** Routine: DtOCITransRollback
** Returns: TRUE/FALSE.
** Action : Rollback transaction.
** Args   : OCIConnection	- OCI Connection structure.
**
****************************************************************/
DLLEXPORT int DtOCIRollback( 
            DT_OCI_CONNECTION	*OCIConnection
);                  

/****************************************************************
** Routine: DtOCIRPC
** Returns: Error code TRUE/FALSE
** Action : Make a call to an Oracle stored procedure or function
			by binding arguments either by name or position.
** Args   : OCIConnection	- OCI Connection structure.
**          RPCName			- Procedure to call.
**          rowCallback		- Function to call for each returned row.
**          callbackArg		- Extra arg to pass to rowCallback.
**			callType        - 1 for Stored Functions; 0 otherwise;
**							  (Will be populated when called by SLANG)		
****************************************************************/
DLLEXPORT int DtOCIRPC( 
            DT_OCI_CONNECTION	*OCIConnection, 
            const char          *rpcName,
            DT_VALUE            *rpcArgs,
			DT_VALUE			*RPCResult,
            DT_OCI_ROW_CB       *rowCallback,
			DT_OCI_PROC_CB		*procCallback,
            void                *callbackArg,
			DT_VALUE			*tempArray,
			int					*callType
);

/****************************************************************
** Routine: OCILibInit
** Returns: TRUE for success, throws exceptions otherwise.
** Action:  Initialize the Oracle client environment by loading
**`			the appropriate oci.dll.
** Args: 
**
*****************************************************************/
EXPORT_CPP_EXPORT int OCILibInit( void );

#endif
