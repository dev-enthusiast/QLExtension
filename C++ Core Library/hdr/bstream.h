/****************************************************************
**
**	BSTREAM.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/bstream.h,v 1.23 2004/11/29 19:36:54 khodod Exp $
**
****************************************************************/

#if !defined( IN_BSTREAM_H )
#define IN_BSTREAM_H

#include <ka_base.h>
#include <stdlib.h>
#include <port_int.h>

/*
**	BStream versions
*/

#define BSTREAM_VERSION_OLD		0
#define BSTREAM_VERSION_NEW		1	// Uses hash tables for strings


/*
**	Types
*/

typedef unsigned char			BSTREAM_POS;

typedef struct BStreamStructure	BSTREAM;
typedef struct HashStructure	BSTREAM_HASH;

typedef struct BStreamCompressInitStructure	BSTREAM_COMPRESS_INIT;
typedef struct BStreamCompressStructure		BSTREAM_COMPRESS;


/*
**	BSTREAM_CALLBACK - called when a BSTREAM is about to overflow. Returns
**  TRUE if the stream can be used, FALSE if the overflow flag should be set.
**	Gets passed a pointer to the stream and the number of bytes and a
**	flag which is TRUE for a Get call and FALSE for a Put call.
*/

typedef int			(*BSTREAM_CALLBACK)( BSTREAM *Stream, int Size, int Get );


/*
**	Structure to hold compress/decompress data
*/

struct BStreamCompressStructure
{
	int		Dynamic;	// True if strings are dynamic

	int		IdCounter;	// Incremented as string Ids are used up
	
	BSTREAM_HASH		// Used when outputting strings in streams
			*Hash;		// Key: String		Value: Code (long)

	
	char	**Array;	// Used when inputting strings from streams

	int		ArraySize;
	
};


/*
**	Structure used to hold binary stream
*/

struct BStreamStructure
{
	size_t	DataSize;

	BSTREAM_CALLBACK
			CallBack;

	void	*CallBackHandle;

	int		StreamVersion;	// 0 - Old
							// 1 - New (ok to use Fixed/Dynamic tables)

	BSTREAM_COMPRESS
			*Fixed,			// Strings written this time
			*Dynamic;		// Fixed strings (per class)

	BSTREAM_POS
			*Data,
			*DataStart,
			*DataEnd;

    uintptr_t
			UserData;

	unsigned int
			Overflow : 1,
			FreeData : 1;	// True if BStreamDestroy() should free( Data )

};


/*
**	Structure used to hold FixedHash initialization data
*/

struct BStreamCompressInitStructure
{
	const char
			*Key;

	int		Value;

};


/*
**	Helper macros
*/

#define BStreamInvalidate(bs)   ((bs)->Overflow = 1)
#define	BStreamTell(bs)			((bs)->Data)
#define	BStreamRewind(bs)		BStreamSeek((bs),(bs)->DataStart)
#define	BStreamDataUsed(bs)		((bs)->Data-(bs)->DataStart)
#define	BStreamDataAvail(bs)	((bs)->DataEnd-(bs)->Data)
#define	BStreamFreeData(bs)		if((bs)->DataStart) free((bs)->DataStart); else


/*
**	Prototype functions
*/

EXPORT_CPP_KOOL_ADE void
		BStreamInit(			BSTREAM *BStream, void *Memory, size_t Size ),
		BStreamAutoGrowInit(	BSTREAM *BStream ),
		BStreamDestroy(			BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE int
		BStreamGetByte(			BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE int16_t
		BStreamGetInt16(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE uint16_t
		BStreamGetUInt16(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE int32_t
		BStreamGetInt32(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE uint32_t
		BStreamGetUInt32(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE double
		BStreamGetDouble(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE float
		BStreamGetFloat(		BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE char
		*BStreamGetStringPtr(	BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE void
		BStreamGetMemory(	BSTREAM *BStream, void *Memory, size_t MemorySize ),
		BStreamGetString(	BSTREAM *BStream, char *String, size_t StringSize );

EXPORT_CPP_KOOL_ADE char *
		BStreamGetStringCopy(	BSTREAM *BStream );

EXPORT_CPP_KOOL_ADE void *
		BStreamGetMemoryCopy(	BSTREAM *BStream, size_t MemorySize );

EXPORT_CPP_KOOL_ADE void
		BStreamPutByte(			BSTREAM *BStream, int    Value ),
		BStreamPutDouble(		BSTREAM *BStream, double Value ),
		BStreamPutFloat(		BSTREAM *BStream, float Value ),
		BStreamPutInt16(		BSTREAM *BStream, int16_t  Value ),
		BStreamPutInt32(		BSTREAM *BStream, int32_t  Value ),
		BStreamPutMemory(		BSTREAM *BStream, const void *Memory, size_t MemorySize ),
		BStreamPutString(		BSTREAM *BStream, const char *String ),
		BStreamPutStringLimit(	BSTREAM *BStream, const char *String, size_t StringSize ),
		BStreamPutUInt16(		BSTREAM *BStream, uint16_t Value ),
		BStreamPutUInt32(		BSTREAM *BStream, uint32_t Value );

//void	BStreamPutStringFix(	BSTREAM *BStream, const char *String, size_t StringSize );

EXPORT_CPP_KOOL_ADE void
		BStreamSkip(			BSTREAM *BStream, size_t Size );

EXPORT_CPP_KOOL_ADE int
		BStreamSeek(			BSTREAM *BStream, BSTREAM_POS *NewPos );

EXPORT_CPP_KOOL_ADE int
		BStreamGrowCallBack(	BSTREAM *BStream, int Size, int Get	);


/*
**	BStream compression stuff
*/

EXPORT_CPP_KOOL_ADE int
		 BStreamSetVersion(			BSTREAM *BStream, int Version, BSTREAM_COMPRESS *Fixed );

EXPORT_CPP_KOOL_ADE void
		 BStreamCompressDestroy(	BSTREAM_COMPRESS *Compress );

EXPORT_CPP_KOOL_ADE BSTREAM_COMPRESS
		 *BStreamCompressCreate(	BSTREAM_COMPRESS_INIT *InitData );

#endif

