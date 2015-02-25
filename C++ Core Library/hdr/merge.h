/****************************************************************
**
**	MERGE.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/merge.h,v 1.14 2013/10/01 21:36:13 khodod Exp $
**
****************************************************************/

#if !defined( IN_MERGE_H )
#define IN_MERGE_H

//	Structure used to unroll points in the point buffer

typedef struct TsdbMergePointStructure
{
	DATE	Date;

	double	Values[1];

} TSDB_MERGE_POINT;


//  Define macros to read merged points

#define	MERGE_VARIABLES			unsigned char *_Points, *_EndPoint;int _PointSize;int *_Offsets=NULL;


#define EXPAND_ARGUMENT(lst,n,size,mods,nummods) (ExpandArgumentWithModifiers((lst),(n),(size),(mods),(nummods),&_Offsets))
#define CLEAN_UP_EXPANSION()					(MemPoolFree(MemPoolTsdb,_Offsets))
#define NO_ARG_EXPANSION_SHUSH_UNREF_WARNING    SHUSH_UNREF_WARNING(_Offsets);( (void*)( _Offsets + 1 ) );

#define	GET_INITIAL_POINTS(lst)				TsdbGetInitialPoints(lst,&_Points,&_EndPoint,&_PointSize)
#define	GET_INITIAL_SAMPLED_POINTS(lst,frq)	TsdbGetInitialSampledPoints(lst,&_Points,&_EndPoint,&_PointSize,frq)

#define	GET_NEXT_POINT(lst)			((_Points+=_PointSize)>=_EndPoint?TsdbGetMorePoints(lst,&_Points,&_EndPoint):1)
#define	GET_NEXT_SAMPLED_POINT(lst)	((_Points+=_PointSize)>=_EndPoint?TsdbGetMoreSampledPoints(lst,&_Points,&_EndPoint):1)

#define	CUR_POINT_X()			(((TSDB_MERGE_POINT*)_Points)->Date)
#define	CUR_POINT_Y(n)			(_Offsets?((TSDB_MERGE_POINT*)_Points)->Values[_Offsets[(n)]]:((TSDB_MERGE_POINT*)_Points)->Values[(n)])
#define	CUR_POINT_Y_MOD(n,m)	(((TSDB_MERGE_POINT*)_Points)->Values[_Offsets[(n)]+(m)])

// The preferred version, especially if you are using expansions
#define	GET_BASE_SYMBOL(lst,n)	(_Offsets?&((lst)->s[_Offsets[(n)]]):&((lst)->s[(n)]))

// Backward-compatible version which doesn't use merge variables, or consider expanded arguments
#define	GET_SYMBOL(lst,n)		(&(lst)->s[(n)])

#endif
