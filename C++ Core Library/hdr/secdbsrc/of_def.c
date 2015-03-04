#define GSCVSID "$Header: /home/cvs/src/secdb/src/of_def.c,v 1.32 2004/06/10 19:46:30 khodod Exp $"
/****************************************************************
**
**	OF_DEF.C	- Default security object handler
**
**	$Log: of_def.c,v $
**	Revision 1.32  2004/06/10 19:46:30  khodod
**	First stab at SOX compliance.
**	iBug #12504
**
**	Revision 1.31  2001/03/23 23:26:38  singhki
**	Make ValueTypeMap just an STL map.
**	
**	Revision 1.30  2001/02/25 18:28:12  vengrd
**	Revert build-breaking changes re: FOR_* macros.
**	
**	Revision 1.29  2001/02/23 23:56:09  simpsk
**	Use STL, not FOR* macros.
**	
**	Revision 1.28  2001/02/22 12:32:20  shahia
**	changed ValueTypeMap implementation to use STL map
**	
**	Revision 1.27  2000/06/14 17:37:29  singhki
**	Invalidate SecurityData on a rollback
**	
**	Revision 1.26  1999/09/13 14:31:19  singhki
**	Use static buffers to build child list arrays and copy into real child list when done
**	
**	Revision 1.25  1999/09/01 15:29:26  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.24  1999/09/01 01:55:12  singhki
**	Get rid of SDB_CHILD_REFs, split intermediates and terminals into seperate
**	arrays in SDB_CHILD_LIST. Rationalize NoEval stuff, auto mark args/ints as
**	NoEval if they are never evaluated.
**	Color child links as purple so that we don't have to find the VTI in order
**	to correctly unlink them.
**	Uncolor arg/int which aren't purple so that they won't invalidate the tree.
**	
**	Revision 1.23  1999/03/04 01:29:29  gribbg
**	Remove unused var
**	
**	Revision 1.22  1999/02/09 20:03:01  singhki
**	Added SecDb Base Class which all GOBs inherit from instead of hacks for special VTs. Also careful to not create nodes for special VTs on load message
**	
**	Revision 1.21  1998/08/12 22:46:09  singhki
**	Use a table to initialize special value types instead of the kludge in
**	sdb_val.c. This also speeds up class loading.
**	BugFix: Fix happy faces and segfault by checking if class has been
**	unregistered because it failed to load in ClassInfoFromType.
**	
**	Revision 1.20  1998/01/02 21:32:44  rubenb
**	added GSCVSID
**	
**	Revision 1.19  1997/11/24 19:48:38  lundek
**	Remove 1.16,1.17
**	
**	Revision 1.18  1997/11/18 21:45:58  lundek
**	Misc. value function message tracing
**	
**	Revision 1.17  1997/11/12 20:51:33  lundek
**	Fix sprintf format
**	
**	Revision 1.16  1997/11/03 21:52:29  lundek
**	Limit object stream size by class (default: 32711)
**	
**	Revision 1.15  1997/05/01 18:43:53  schlae
**	Heap functions become GSHeap functions
**	
**	Revision 1.14  1997/04/17 18:25:05  rubenb
**	plug memory leak
**
**	Revision 1.13  1997/03/06 01:44:37  lundek
**	UFO enhancements and clean up
**
**	Revision 1.12  1996/12/23 03:06:08  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**
**	Revision 1.11  1996/12/19 03:41:25  lundek
**	BugFix: Recalc special vts (e.g Security Name) after reload
**
**	Revision 1.10  1995/05/09 19:24:48  GRIBBG
**	Change DiskInfo.Version to ObjectVersion & StreamVersion
**	Change SDB_MSG_UPDATE to ..._INCREMENTAL for incremental updates
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<string.h>
#include	<malloc.h>
#include	<time.h>
#include	<date.h>
#include	<heapinfo.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secerror.h>
#include	<sectrace.h>
#include	<sdb_int.h>



/****************************************************************
**	Routine: ChildItemsMemSize
**	Returns: int
**	Action : calculates size of Items array
****************************************************************/

static int ChildItemsMemSize(
	SDB_CHILD_LIST_ITEM	*Items,
	int					Count
)
{
	int Size = GSHeapMemSize( Items );

	for( int i = 0; i < Count; ++i )
		Size += GSHeapMemSize( Items[ i ].Argv );
	return Size;
}



/****************************************************************
**	Routine: SecDbDefaultObjFunc
**	Returns: TRUE	- Operation was successful
**			 FALSE	- Operation failed
**	Action : Default security object handler
****************************************************************/

#include <slang_validate.h>

int SecDbDefaultObjFunc(
	SDB_OBJECT		*SecPtr,		// Pointer to security to operate on
	int				Msg,			// Type of operation
	SDB_M_DATA		*MsgData		// Data used for operation
)
{

	switch( Msg )
	{


/*
**	Specially handled messages
*/

		case SDB_MSG_START:
			return TRUE;

		case SDB_MSG_LOAD_VERSION:
			return Err( ERR_VERSION_MISMATCH, "@ - Object version: %d  Current version: %d",
					MsgData->Load.Version.Object, MsgData->Load.CurrentVersion );

		case SDB_MSG_DESCRIPTION:
		{
			char	Buffer[ SDB_PATH_NAME_SIZE + SDB_FUNC_NAME_SIZE + SDB_CLASS_ARG_SIZE + 32 ];


			sprintf( Buffer, "%s/%s( \"%s\" )",
					SecPtr->Class->DllPath,
					SecPtr->Class->FuncName,
					SecPtr->Class->Argument );
			MsgData->Description = strdup( Buffer );
			return TRUE;
		}

		case SDB_MSG_MEM_USED:
		{

#define	SET_NUM( Results_, Tag_, Size_ )	Total += (Size_); if( IsStruct ) DtComponentSetNumber( Results_, Tag_, DtDouble, (double) (Size_) )

			const SDB_VALUE_TYPE_INFO
					*Vti;

			SDB_CHILD_PULL_VALUE
					*PullVal;

			int		VTSize			= 0,
					VTMapSize		= 0,
					ChildListSize	= 0,
					IndexListSize	= 0,
					Size			= 0,
					Total 			= MsgData->MemUsed.Size,
					IsStruct;

			SDB_CLASS_INFO
					*ClassInfo 		= MsgData->MemUsed.Class;

			DT_VALUE
					*Results  		= &MsgData->MemUsed.Detail;


			if( Results->DataType == DtStructure )
				IsStruct = TRUE;
			else if( Results->DataType == DtNull )
			{
				IsStruct = TRUE;
				if( !DTM_ALLOC( Results, DtStructure ))
				{
					DTM_INIT( Results );
					return FALSE;
				}
			}
			else
				IsStruct = FALSE;

			Size 		+= GSHeapMemSize( ClassInfo );
			// FIX this isn't really the whole size of the value type map
			VTMapSize 	+= GSHeapMemSize( ClassInfo->ValueTypeMap );

			if( ClassInfo->ValueTypeMap )
			{
				FOREACH_CLASS_VTI_BEGIN(ClassInfo->ValueTypeMap, Vti)
					ChildListSize += GSHeapMemSize( Vti->ChildList.Literals );
					ChildListSize += ChildItemsMemSize( Vti->ChildList.Terminals, Vti->ChildList.TerminalCount );
					ChildListSize += ChildItemsMemSize( Vti->ChildList.Intermediates, Vti->ChildList.IntermediateCount );
					for( PullVal = Vti->ChildList.PullValues; PullVal; PullVal = PullVal->Next )
						ChildListSize += GSHeapMemSize( PullVal );

					// Also count special tilde value types
					if( Vti->Func == SecDbValueFuncGetSecurity && !Vti->Parent )
						VTSize += GSHeapMemSize( Vti );
				FOREACH_CLASS_VTI_END();
			}

			SET_NUM( Results, "ClassInfo", 			Size );
			SET_NUM( Results, "ValueTypeMap", 		VTMapSize );
			SET_NUM( Results, "TildeValueTypes",	VTSize );
			SET_NUM( Results, "ChildList", 			ChildListSize );
			SET_NUM( Results, "IndexList", 			IndexListSize );
			if( IsStruct )
				DtComponentSetNumber( Results, "~Total", DtDouble, (double) Total );

			MsgData->MemUsed.Size = Total;
			return TRUE;
		}


/*
**	Messages that pass the message to each of the special value types
*/

		case SDB_MSG_LOAD:
		case SDB_MSG_NEW:
		case SDB_MSG_RENAME:
			SlangModuleMarkUnsafe( SecPtr );
			return TRUE;
            
		case SDB_MSG_GET_CHILD_LIST:
		case SDB_MSG_END:
		case SDB_MSG_UNLOAD:
		case SDB_MSG_DELETE:
		case SDB_MSG_DIDDLE_SET:
		case SDB_MSG_DIDDLE_CHANGED:
		case SDB_MSG_DIDDLE_REMOVED:
		case SDB_MSG_VALIDATE:
		case SDB_MSG_DUMP_OBJ_DATA:
			return TRUE;

		case SDB_MSG_ROLL_BACK_ADD:
		case SDB_MSG_ROLL_BACK_INCREMENTAL:
		case SDB_MSG_ROLL_BACK_UPDATE:
		case SDB_MSG_ROLL_BACK_RENAME:
			return SecDbInvalidate( SecPtr, SecDbValueSecurityData, 0, NULL );


/*
**	Messages that fail by default
*/

		case SDB_MSG_ADD:
		case SDB_MSG_UPDATE:
		case SDB_MSG_MEM:
		case SDB_MSG_GET_VALUE:
		case SDB_MSG_SET_VALUE:
		case SDB_MSG_INCREMENTAL:
		default:
			return Err( ERR_UNSUPPORTED_OPERATION, "@" );
	}
}
