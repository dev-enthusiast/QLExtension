/*******************************************************************
**
** Date:		2013-09-24
** Copyright:	Copyright(C) 2013, Constellation, an Exelon Company.
** Author:		Tom Krzeczek
** 
** DotNetGlue.h : A helper include file.
** This code is based on the original version by Randall Maddox.
**
********************************************************************
**
** $Log: dotnetglue.h,v $
** Revision 1.1  2013/09/24 18:24:03  c038571
** Initial revision.
**
**
********************************************************************/

#ifndef __dotnetglue_h__
#define __dotnetglue_h__

//wrapper for COM memory allocator
void* co_task_mem_alloc(int size);

//wrapper for COM memory de-allocator
void co_task_mem_free(void*);

//defines the format of the custom secdb output tracer. The function is provided by .NET client.
typedef int (__stdcall * custom_secdb_tracer) (const char*msg);

//defines the format of the custom SecDb (error) message output handler - supplied by a .NET client 
typedef void (__stdcall * custom_secdb_msg_handler) (const char*msg);

//tmp here
DT_VALUE* MakeNewDtv(DT_DATA_TYPE const);

//tmp here
DT_VALUE* AllocAndAssign(DT_VALUE&);

//tmp here
EXPORT_C_EXPORT const char* DtStructureToString(DT_VALUE* const);

//tmp here
EXPORT_C_EXPORT DT_VALUE * DtStructureGetByName(DT_VALUE* const, const char * const);


#endif
