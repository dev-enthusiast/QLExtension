#define GSCVSID "$Header: /home/cvs/src/dotnetglue/src/SecDbDataTypeAPI.cpp,v 1.25 2014/01/24 19:29:36 c038571 Exp $"
/*******************************************************************
**
** Date:		2013-09-24
** Copyright:	Copyright(C) 2013, Constellation, an Exelon Company.
** Author:		Tom Krzeczek
** 
** SecDbDataTypeAPI.cpp : Defines and exports operations related to SecDb data types.
** This code is based on the original version by Randall Maddox.
**
********************************************************************
**
** $Log: SecDbDataTypeAPI.cpp,v $
** Revision 1.25  2014/01/24 19:29:36  c038571
** AWR: #325268
**
** Revision 1.24  2014/01/17 15:07:30  c038571
** Fixed typo and added a comment to DtArrayInsertAt().
**
** Revision 1.23  2014/01/16 23:31:32  c038571
** Updated in response to the code review.
**
** Revision 1.22  2014/01/16 17:58:04  c038571
** AWR: #324557
**
** Revision 1.21  2013/12/09 19:53:56  c038571
** Bug fix.
**
** Revision 1.20  2013/11/20 20:58:19  khodod
** Add missing GSCVSID.
** AWR: #319760
**
** Revision 1.19  2013/11/19 23:23:55  c038571
** Added DtvToString()
**
** Revision 1.18  2013/11/19 22:53:06  c038571
** Bug fix.
**
** Revision 1.16  2013/11/19 13:30:51  c038571
** Bug fixes, API updates.
**
** Revision 1.15  2013/11/14 13:27:12  c038571
** Cleanup
**
** Revision 1.14  2013/11/11 22:53:59  c038571
** Bug fixes
**
** Revision 1.13  2013/11/08 20:17:27  c038571
** Bug fixes.
**
** Revision 1.12  2013/11/07 23:58:25  c038571
** Date/Time bug fixes
**
** Revision 1.11  2013/11/01 21:16:40  c038571
** InterpolateFill() updates + bug fixes
**
** Revision 1.10  2013/10/31 19:12:11  c038571
** Bug fixes
**
** Revision 1.9  2013/10/30 19:47:35  c038571
** Added DtStructureGetValues()
**
** Revision 1.8  2013/10/29 22:02:02  c038571
** Added DtvEqual(), DtStructureGetKeys() and some bug fixes.
**
** Revision 1.7  2013/10/11 21:41:46  c038571
** Added DeepCopy + commented out dtv pool-related code(temporarily)
**
** Revision 1.6  2013/10/02 22:19:23  c038571
** Updated in response to the code review.
**
** Revision 1.5  2013/10/02 15:43:44  c038571
** Bug fix.
**
** Revision 1.4  2013/09/30 19:06:45  c038571
** Added GetMinDate(), GetMinTime()
**
** Revision 1.3  2013/09/27 22:37:40  c038571
** UTC Time/Date conversions updated in response to the coder review.
**
** Revision 1.2  2013/09/24 23:19:46  c038571
** Added conversions.
**
** Revision 1.1  2013/09/24 18:25:51  c038571
** Initial revision.
**
**
********************************************************************/


#include <portable.h>
#include <datatype.h>
#include <secdb.h>
#include <slangfnc.h>
#include <date.h>
#include "dotnetglue.h"
#include <set>

#define SDB_SUCCESS 1
#define SDB_FAILURE 0
#define MAX_DATE_THAT_WORKS 365609		// Equivalent of HighLimit("Date") in Slang. 

/// Pool to keep pointers to all active DT_VALUE objects which either:
/// 1) were created and initialized by issuing MakeNewDtv
/// 2) were allocated and returned from SecDb as result of SecDb call.
/// NOTE: The pointers kept in the pool are released via ReleaseDtv call.
//static std::set<DT_VALUE*> dtv_pool;

// local helper functions to create/initialize DT_VALUE
//namespace
//{
  /// Initialize an empty DT_VALUE to specified type.
  /// Returns pointer to DT_VALUE on success, or zero on error.
  DT_VALUE * InitDtv(DT_VALUE & dtv, DT_DATA_TYPE const dt)
  {
    memset(&dtv, 0, sizeof(dtv));

    dtv.DataType = dt;

    if(!dtv.DataType->Func(DT_MSG_START, 0, &dtv, 0))
    {
      return SDB_FAILURE;
    }

    if(!dtv.DataType->Func(DT_MSG_ALLOCATE, &dtv, 0, 0))
    {
      return SDB_FAILURE;
    }

    return &dtv;
  }

  /// Initialize an empty DT_VALUE to specified type.
  ///
  /// Returns pointer to DT_VALUE on success, or zero on error.

  DT_VALUE * InitDtv(DT_VALUE & dtv, const char * const secDbTypeName)
  {
    DT_DATA_TYPE const dt = DtFromName(secDbTypeName);

    if(dt) return InitDtv(dtv, dt);

    return SDB_FAILURE;
  }

  /// Allocate and initialize a new DT_VALUE of specified SecDb type name.
  ///
  /// Returns 0 on failure, otherwise a valid pointer to a new DT_VALUE of the
  /// specified type allocated via malloc() and initialized to an empty state.
  DT_VALUE * MakeNewDtv(DT_DATA_TYPE const dt)
  {
    DT_VALUE * const retVal = (DT_VALUE *) malloc(sizeof(*retVal));
	DT_VALUE *dtvalue = InitDtv(*retVal, dt);
	//if(dtvalue)
	//	dtv_pool.insert(dtvalue);
    return dtvalue;
  }

  /// Allocate a new DT_VALUE and assign value of src to it
  ///
  /// On failure returns zero, otherwise pointer to initialized
  DT_VALUE * AllocAndAssign(DT_VALUE & src)
  {
    DT_VALUE * const retPtr = MakeNewDtv(src.DataType);

    if(retPtr) DTM_ASSIGN(retPtr, &src);

    return retPtr;
  }

// Create new DT_VALUE of type secdb_type_id
// Returns valid pointer to DT_VALUE on success, otherwise returns zero.
EXPORT_C_EXPORT DT_VALUE* NewDtv(char* type_name)
{
	if( DT_DATA_TYPE dt = DtFromName(type_name) )
		return MakeNewDtv(dt);
	return 0;
}


// Release DT_VALUE specified by handle
//
// NOTE:  Assumes DT_VALUE was allocated via malloc().  Otherwise results
// are undefined.

EXPORT_C_EXPORT void ReleaseDtv(DT_VALUE * handle)
{
  if(handle)
  {
    DTM_FREE(handle);
    free(handle);
	//dtv_pool.erase(handle);
  }
}

//}  // end unnamed namespace


/**
* Creates a deep copy of a given DT_VALUE.
* @param src handle to a source object
* @param copy pointer to a created duplicate
* @return SDB_SUCCESS when a copy is created successfully, SDB_FAILURE otherwise.
*/
EXPORT_C_EXPORT int DeepCopyDtv(DT_VALUE *src, DT_VALUE **copy)
{
	int ret_code = SDB_FAILURE;
	*copy = 0;
	if(src)
		if( *copy = AllocAndAssign(*src) )
			ret_code = SDB_SUCCESS;
	return ret_code;
}


/// Get pointer to indexed element in DtArray
/// NOTE:  Assumes ndx has already been validated
/// Returns 0 on failure.
EXPORT_C_EXPORT DT_VALUE * DtArrayGetElement(DT_VALUE * const handle,
                                        const int        ndx)
{
  if(handle && (handle->DataType == DtArray))
  {
    return &(((DT_ARRAY *) handle->Data.Pointer)->Element[ndx]);
  }

  return SDB_FAILURE;
}


/**
* Checks if all elements are of the given "pattern type" across the collection.
* @param handle handle to an object of aggregate type
* @param pointer to a pattern object
* @param psize is set to 1 if the collection is homogenous, 0 otherwise.
* @return SDB_FAILURE if failure, SDB_SUCCESS otherwise.
*/
EXPORT_C_EXPORT int AllElementsAreOfSameType(
	const DT_VALUE* handle,
	const DT_VALUE* pattern_handle,
	int* same_type
	)
{
	int ret_val = SDB_FAILURE;
	*same_type = 0;

	if( handle && pattern_handle ){

		if( handle->DataType == DtArray ){
			DT_ARRAY *parray = (DT_ARRAY*)handle->Data.Pointer;
			*same_type = 1;

			for( int idx = 0; idx < parray->Size; ++idx ){
				if( parray->Element[idx].DataType != pattern_handle->DataType ){
					*same_type = 0;
					break;
				}
			}
		}else if( handle->DataType == DtStructure || handle->DataType == DtStructureCase ){

			HASH *phash = (DT_STRUCTURE*)handle->Data.Pointer;
			*same_type = 1;

			if( phash->KeyCount ){			
				for( HASH_ENTRY_PTR pentry = HashFirst(phash);
					!HashEnd(pentry); 
					pentry = HashNext(phash, pentry) ){

					if( ( (DT_VALUE*)HashValue(pentry) )->DataType != pattern_handle->DataType ){
						*same_type = 0;
						break;
					}
				}
			}
		}else
			*same_type = handle->DataType == pattern_handle->DataType;

		ret_val = SDB_SUCCESS;
	}
	return ret_val;
}

/**
* DtArrayGetArrayOf() allocates memory and populates data,
* a managed caller is responsible for cleanup.
* @param handle array handle
* @param pointer to allocated data
* @param psize size of the allocated array.
* @return SDB_FAILURE if failed, SDB_SUCCESS otherwise.
*/
EXPORT_C_EXPORT int DtArrayGetArrayOf(
	DT_VALUE* handle,
	void** data,
	int* psize)
{
	int ret_val = SDB_FAILURE;
	*data = 0;
	*psize = 0;

	if( handle && handle->DataType == DtArray ){

		DT_ARRAY* parray = (DT_ARRAY*)handle->Data.Pointer;
		if( parray->Size ){

			int same_type = 0;
			if( AllElementsAreOfSameType(handle, &(parray->Element[0]), &same_type ) == SDB_SUCCESS &&
				same_type == 1 ){

				ret_val = SDB_SUCCESS;

				//allocate memory and populate the data
				if( parray->Element[0].DataType == DtDouble ||
					parray->Element[0].DataType == DtDate ||
					parray->Element[0].DataType == DtTime ){

					double *tmp = (double*)co_task_mem_alloc(sizeof(double) * parray->Size);
					if( !(*data = tmp) ) return SDB_FAILURE;
						
					for( int i = 0; i < parray->Size; ++i )
						tmp[i] = parray->Element[i].Data.Number;
					*psize = parray->Size;

				}else if(parray->Element[0].DataType == DtFloat ){
					float *tmp = (float*)co_task_mem_alloc(sizeof(float) * parray->Size);
					if( !(*data = tmp) ) return SDB_FAILURE;
					
					for( int i = 0; i < parray->Size; ++i )
						tmp[i] = parray->Element[i].Data.Single;
					*psize = parray->Size;

				}else if(parray->Element[0].DataType == DtString){
					char **ppdata = (char**)co_task_mem_alloc(sizeof(char*) * parray->Size);
					if( !( *data = ppdata ) ) return SDB_FAILURE;

					for(int i = 0; i < parray->Size; ++i){
						char *tmp_str = (char*)parray->Element[i].Data.Pointer;
						ppdata[i] = (char*)co_task_mem_alloc(sizeof(char) * (strlen(tmp_str) + 1));
						
						if(!ppdata[i]){
							for( int j = 0; j < i; ++i){
								co_task_mem_free(ppdata[j]);
							}
							co_task_mem_free(ppdata);
							*data = 0;
							*psize = 0;

							return SDB_FAILURE;
						}

						strcpy( ppdata[i], tmp_str);
					}
					*psize = parray->Size;
					
				}else
					ret_val = SDB_FAILURE;
			}
		}else
			ret_val = SDB_SUCCESS;
	}
	return ret_val;
}


/// Get count of elements in DtArray.
/// Returns -1 on failure.
EXPORT_C_EXPORT int DtArrayGetCount(DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtArray))
  {
    return ((DT_ARRAY *) handle->Data.Pointer)->Size;
  }

  return -1;
}

/**
* Insert copy of single value into DtArray.
* Assumes destPos has already been validated.
*
* @param dest array to insert a value to.
* @param destPost index at which the value is inserted.
* @param value element to be inserted.
* @return SDB_FAILURE on failure, or SDB_SUCCESS on success.
*/
EXPORT_C_EXPORT int DtArrayInsertAt(
	DT_VALUE* const dest,
	const int destPos,
	DT_VALUE* const value)
{
  ErrClear();  // clear any previous error
  int ret_val = SDB_FAILURE;

  if( dest && dest->DataType == DtArray )
  {
	  DT_ARRAY *array = (DT_ARRAY *) dest->Data.Pointer;
	  
	  if( DtArrayInsert( array, destPos, 1 ) == TRUE )
	  {
		  ret_val = DTM_ASSIGN( &array->Element[destPos], value ) == TRUE
			  ? SDB_SUCCESS
			  : SDB_FAILURE;
	  }
  }
  return ret_val;
}

/**
* Insert copy of range of values into DtArray.
* Assumes destPos, srcFirst and srcLast have already been validated,
* and that srcFirst <= srcLast.
* 
* @param dest array to insert data to.
* @param destPos starting index at which the data is inserted.
* @param src input array.
* @param srcFirst starting index in the input array.
* @param srcLast ending index in the input array.
* Returns SDB_FAILURE on failure, or SDB_SUCCESS on success.
*/
EXPORT_C_EXPORT int DtArrayInsertRange(
	DT_VALUE* const dest,
	const int destPos,
	DT_VALUE* const src,
	const int srcFirst,
	const int srcLast)
{
  ErrClear();  // clear any previous error
  
  if( !( dest && src && dest->DataType == DtArray && src->DataType == DtArray) )
	  return SDB_FAILURE;
  
  DT_ARRAY *destArray = (DT_ARRAY*)dest->Data.Pointer, *srcArray = (DT_ARRAY*)src->Data.Pointer;
	
  // check for insert into self!
  if(destArray == srcArray) return SDB_FAILURE;

  int count = (srcLast - srcFirst) + 1;

  if( DtArrayInsert( destArray, destPos, count ) != TRUE ) return SDB_FAILURE;
  
  for( int i = 0; i < count; ++i )
	  if( !DTM_ASSIGN( &destArray->Element[destPos + i], &srcArray->Element[srcFirst + i] ) )
		  return SDB_FAILURE;
  
  return SDB_SUCCESS; 
}

/// Remove and destroy element DtArray[first] through and including element
/// DtArray[last].
/// NOTE:  Assumes first and last have already been validated and that
/// first <= last.
/// Returns SDB_FAILURE on failure, or SDB_SUCCESS on success.
EXPORT_C_EXPORT int DtArrayRemove(DT_VALUE * const handle,
                                    const int        first,
                                    const int        last)
{
  ErrClear();  // clear any previous error

  if(handle && (handle->DataType == DtArray))
  {
    return DtArrayDelete(((DT_ARRAY *) handle->Data.Pointer), first, (last - first) + 1) == 1
		? SDB_SUCCESS
		: SDB_FAILURE;
  }
  return SDB_FAILURE;
}

/// Set value of indexed element in DtArray.
/// NOTE:  Assumes ndx has already been validated.
/// Returns SDB_FAILURE on failure, or SDB_SUCCESS on success.
EXPORT_C_EXPORT int DtArraySet(DT_VALUE * const dest,
                                 const int        ndx,
                                 DT_VALUE * const src)
{
  ErrClear();  // clear any previous error

  if(src && dest && (dest->DataType == DtArray))
  {
    return DTM_ASSIGN(&(((DT_ARRAY *) dest->Data.Pointer)->Element[ndx]), src) == 1
		? SDB_SUCCESS
		: SDB_FAILURE;
  }

  return SDB_FAILURE;
}


// Data structure used to return a knot from a Dt[T]Curve
//
// NOTE:  This structure MUST match the corresponding C# KnotData structure
// used in Class Dt[T]Curve.  Since this structure contains only built-in
// types it is inherently blittable, i.e., may be passed by value back and
// forth across the inter-DLL boundary with no special handling.

namespace Knot
{
  struct KnotData
  {
    double m_date_time;
    double m_value;
  };
}

// Data structure used to return an index pair from a Dt[T]Curve
//
// NOTE:  This structure MUST match the corresponding C# IndexPair structure
// used in Class Dt[T]Curve.  Since this structure contains only built-in
// types it is inherently blittable, i.e., may be passed by value back and
// forth across the inter-DLL boundary with no special handling.

namespace Index
{
  struct Pair
  {
    int m_first;
    int m_last;
  };
}

namespace
{

/**
* Local helper for Dt(T)Curve[Dates,Times]ToIndexes()
*
* Assumes all validation has been done elsewhere.
* first <= Dt(T)Curve[ndxs.m_first ].Date/Time <= Dt(T)Curve[ndxs.m_last].Date/Time <= last
*
* Pair {-1,-1} means that the given range (first, last) does not belong to this Curve.
*/
Index::Pair DtCurveDatesTimesToIndexes(
	const DT_CURVE * const curve,
	const double first,
	const double last)
{
	Index::Pair ndxs = {-1,-1};
	if( !curve->KnotCount ) return ndxs;

	int j = 0, i = curve->KnotCount - 1;	

	if( first < curve->Knots[j].Date && last < curve->Knots[j].Date ||
		first > curve->Knots[i].Date && last > curve->Knots[i].Date ) return ndxs;

	bool updated = true;

	while( updated ){
		updated = false;

		if( curve->Knots[j].Date < first ){
			updated = true;
			if( curve->Knots[ (j + i) / 2].Date < first )
				j = j == (j + i)/2 ? j + 1 : (j + i)/2;
			else
				++j;
		}
		if( curve->Knots[i].Date > last ){
			updated = true;
			if( curve->Knots[ (i + j) / 2].Date > last )
				i = i == (i + j)/2 ? i - 1 : (i + j)/2;
			else
				--i;
		}
	}
	
	if( j <= i ){
		ndxs.m_first = j;
		ndxs.m_last = i;
	}

	return ndxs;
}

// Local helper for Dt[T]CurveRemove()
//
// Assumes all validation has been done elsewhere.

void DtCurveRemoveKnots(DT_CURVE * const curve,
                        const int        first,
                        const int        last)
{
  for( int dest = first, src = last + 1; src < curve->KnotCount; ++dest, ++src)
  {
    curve->Knots[dest] = curve->Knots[src];
  }

  curve->KnotCount -= ((last - first) + 1);
}

}


// Add or replace a knot in a DtCurve
//
// NOTE:  Assumes date is a valid date value.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtCurveAddOrReplace(DT_VALUE * const handle,
                                          double           date,
                                          double           value)
{
  if(handle && (handle->DataType == DtCurve))
  {
    DT_VALUE dtvDate = { 0 };

    if(InitDtv(dtvDate, "Date"))
    {
      dtvDate.Data.Number = date;

      DT_VALUE dtvDouble = { 0 };

      if(InitDtv(dtvDouble, "Double"))
      {
        dtvDouble.Data.Number = value;

        DT_CURVE * const curve = (DT_CURVE *) handle->Data.Pointer;

        if(DtCurveInsertKnotByDate(curve, &dtvDate, &dtvDouble) != 0)
        {
          return SDB_SUCCESS;
        }
        // else fall through and return 0
      }
      // else fall through and return 0
    }
    // else fall through and return 0
  }

  return SDB_FAILURE;
}


/**
* Get range of indexes into DtCurve for which:
*
*       first <= DtCurve[ndx].Date <= last
*
* NOTE:  Assumes dates first and last have already been validated and that
* first <= last.
*
* @return valid index pair if any knots are found in this date range
* 
* NOTE: if either element of the index pair equals -1, 
*       the given range (first, last) does not belong to the curve.
*/
EXPORT_C_EXPORT int DtCurveDatesToIndexes(
	DT_VALUE * const handle,
	const double first,
	const double last,
	Index::Pair** pair)
{
  if( !( handle && handle->DataType == DtCurve ) ) return FALSE;

  *pair = (Index::Pair*)co_task_mem_alloc( sizeof( Index::Pair ) ); //the memory is released on the managed side.
  if(!*pair) return FALSE;

  **pair = DtCurveDatesTimesToIndexes( (DT_CURVE*)handle->Data.Pointer, first, last );
  return TRUE;
}


// Get an existing knot from a DtCurve by index
//
// NOTE:  Returns KnotData with invalid values on failure, otherwise returns
// requested knot data.
//
// NOTE:  Assumes ndx has already been validated.

EXPORT_C_EXPORT Knot::KnotData DtCurveGetByNdx(const DT_VALUE * const handle,
                                                 const int              ndx)
{
  // initialize to invalid values for failure return
   Knot::KnotData retVal = { -1.0, std::numeric_limits<double>::quiet_NaN() };

  if(handle && (handle->DataType == DtCurve))
  {
    DT_CURVE * const curve = ((DT_CURVE *) handle->Data.Pointer);

    retVal.m_date_time = curve->Knots[ndx].Date;
    retVal.m_value     = curve->Knots[ndx].Value;
  }

  return retVal;
}

/**
* Get an existing or interpolated knot from a DtCurve by date.
* @param handle DtCurve handle
* @param date
* @return Returns KnotData with invalid values on failure, otherwise returns
*  requested knot data.
* NOTE: If the date does not belong to this curve, an interpolated knot is returned.
*
* NOTE:  Assumes parameter date has already been validated.
*/
EXPORT_C_EXPORT int DtCurveGetByDate(
	DT_VALUE* const handle,
	double date,
	Knot::KnotData** pknot)
{
	*pknot = 0;
	if( !( handle && handle->DataType == DtCurve ) ) return FALSE;
	
	int ret_val = FALSE;
	DT_VALUE dtv_date, dtv_value;
	DTM_INIT(&dtv_date);
	DTM_INIT(&dtv_value);

	if( InitDtv( dtv_date, "Date") ){

		dtv_date.Data.Number = date;

		if( ret_val = DtSubscriptGetCopy( handle, &dtv_date, &dtv_value) ){
			*pknot = (Knot::KnotData*)co_task_mem_alloc( sizeof(Knot::KnotData) );
			(*pknot)->m_date_time = date;
			(*pknot)->m_value = dtv_value.Data.Number;
		}

		DTM_FREE(&dtv_date);
	}

	return ret_val;
}

/**
* DtCurveGetCurveOf() allocates memory and populates data,
* a managed caller is responsible for cleanup.
* @param handle DtCurve handle
* @param data pointer to allocated data
* @param psize size of the allocated array.
* @return SDB_FAILURE if failed, SDB_SUCCESS otherwise.
*
* NOTE: Data is allocated as an array of doubles [date1/time1, value1, date2/time2,value2, etc].
*/
EXPORT_C_EXPORT int DtCurveGetCurveOf(
	DT_VALUE* handle,
	void** data,
	int* psize)
{
	int ret_val = SDB_FAILURE;
	if( handle && ( handle->DataType == DtCurve || handle->DataType == DtTCurve ) ){
		DT_CURVE *pcurve = (DT_CURVE*)handle->Data.Pointer;
		*psize = 0;
		if( pcurve->KnotCount ){
			double *tmp = (double*)co_task_mem_alloc( sizeof(double) * 2 * pcurve->KnotCount);
			if( !( *data = tmp ) ) return SDB_FAILURE;
				
			int idx = 0;
			for( int i = 0; i < pcurve->KnotCount; ++i ){
				tmp[ idx = 2*i] = pcurve->Knots[i].Date;
				tmp[ idx + 1] = pcurve->Knots[i].Value;
			}
			*psize = 2 * pcurve->KnotCount;
		}
		ret_val = SDB_SUCCESS;
	}
	return ret_val;
}


// Get count of knots in DtCurve.
//
// Returns -1 on failure;

EXPORT_C_EXPORT int DtCurveGetCount(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtCurve))
  {
    return ((DT_CURVE *) handle->Data.Pointer)->KnotCount;
  }

  return -1;
}

// Remove knots from DtCurve[first] through and including DtCurve[last]
//
// NOTE:  Assumes indexes first and last have already been validated and that
// first <= last.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtCurveRemoveRange(DT_VALUE * const handle,
                                    const int        first,
                                    const int        last)
{
  if(handle && (handle->DataType == DtCurve))
  {
    DtCurveRemoveKnots((DT_CURVE *) handle->Data.Pointer, first, last);
    return TRUE;
  }
  return FALSE;
}


// Set an existing knot in a DtCurve by index
//
// NOTE:  Assumes ndx and date have already been validated.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtCurveSetByNdx(DT_VALUE * const handle,
                                      const int        ndx,
                                      const double     value)
{
  if(handle && (handle->DataType == DtCurve))
  {
    DT_CURVE * const curve = (DT_CURVE *) handle->Data.Pointer;

    curve->Knots[ndx].Value = value;

    return SDB_SUCCESS;
  }

  return SDB_FAILURE;
}

// Get double inside DT_VALUE of type DtDate.
//
// NOTE:  Returns 0.0, an invalid date, on failure.

EXPORT_C_EXPORT double DtDateGet(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtDate))
  {
    return handle->Data.Number;
  }

  return 0.0;
}


// Copy double into DT_VALUE of type DtDate.
//
// Returns 1 on success, 0 on failure.

EXPORT_C_EXPORT int DtDateSet(DT_VALUE   * const handle,
                                const double       value)
{
  if(handle && (handle->DataType == DtDate))
  {
    handle->Data.Number = value;
    return 1;
  }

  return 0;
}

// Set value of DtDate from char *.
//
// Returns 1 for success and 0 for failure.

EXPORT_C_EXPORT const int DtDateSetString(DT_VALUE   * const handle,
                                            const char * const src)
{
  ErrClear();   // clear any previous error

  if(handle && (handle->DataType == DtDate) && src && *src)
  {
    long dateVal = 0;

    DateFromString(src, &dateVal);

    if(dateVal != DATE_ERR)
    {
      handle->Data.Number = double(dateVal);

      return 1;
    }
    // else fall through to return 0
  }

  return 0;
}

// Get double inside DT_VALUE of type DtDouble.
//
// NOTE: Returns quiet_NaN() on failure

EXPORT_C_EXPORT double DtDoubleGet(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtDouble))
  {
    return handle->Data.Number;
  }

  return std::numeric_limits<double>::quiet_NaN();
}

// Copy double into DT_VALUE of type DtDouble.
//
// NOTE:  Returns SDB_SUCCESS on success, SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtDoubleSet(DT_VALUE   * const handle,
                                  const double       value)
{
  if(handle && (handle->DataType == DtDouble))
  {
    handle->Data.Number = value;
    return SDB_SUCCESS;
  }

  return SDB_FAILURE;
}

// Get float inside DT_VALUE of type DtFloat.
//
// NOTE: Returns quiet_NaN() on failure

EXPORT_C_EXPORT float DtFloatGet(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtFloat))
  {
    return handle->Data.Single;
  }

  return std::numeric_limits<float>::quiet_NaN();
}

// Copy float into DT_VALUE of type DtFloat.
//
// NOTE:  Returns SDB_SUCCESS on success, SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtFloatSet(DT_VALUE   * const handle,
                                 const float        value)
{
  if(handle && (handle->DataType == DtFloat))
  {
    handle->Data.Single = value;
    return SDB_SUCCESS;
  }

  return SDB_FAILURE;
}

// Get pointer to string inside DT_VALUE of type DtString, which may be
// zero.

EXPORT_C_EXPORT const char * DtStringGet(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtString))
  {
    return (char *) handle->Data.Pointer;
  }

  return 0;
}

// Copy string into DT_VALUE of type DtString and return value of pointer to
// copied string, which will be zero if and only if copy failed or input
// value is null.
EXPORT_C_EXPORT const void * DtStringSet(DT_VALUE   * const handle,
                                           const char * const value)
{
  if(handle && (handle->DataType == DtString))
  {
    if(value)
    {
      const char * const tmp = strdup(value);

      if(tmp)
      {
        DTM_FREE(handle);
        return handle->Data.Pointer = tmp;
      }
      // else fall through to return 0;
    }
    else  // no storage needed for null or empty string
    {
      free((void *) handle->Data.Pointer);
      return handle->Data.Pointer = 0;
    }
  }

  return 0;
}


/**
* Add named element src to DtStructure(Case) dest.
* @param dest object to add the element to
* @param name field name
* @param src object to be added to dest
* 
* @return SDB_SUCCESS for success, or SDB_FAILURE on failure.
*/
EXPORT_C_EXPORT int DtStructureAddOrReplace(
	DT_VALUE* const dest,
	const char* const name,
	DT_VALUE* const src)
{
  ErrClear();  // clear any previous error
  int ret_code = SDB_FAILURE;

  if( name && *name && src && dest && ( dest->DataType == DtStructure || dest->DataType == DtStructureCase ) )
  {
    // make copy of src to add to dest
    DT_VALUE * const srcCopy = AllocAndAssign(*src);

    if(srcCopy)
    {
      // make DT_VALUE of type DtString from name
      DT_VALUE dtvString = { 0 };
      if(InitDtv(dtvString, "String") && DtStringSet(&dtvString, name))
      {
        // add copy to dest
        ret_code = dest->DataType->Func(DT_MSG_COMPONENT_REPLACE, srcCopy, dest, &dtvString) == 1
			? SDB_SUCCESS
			: SDB_FAILURE;
        DTM_FREE(&dtvString);
      }

	  if(ret_code == SDB_FAILURE ){
		  DTM_FREE(srcCopy);
		  free(srcCopy);
	  }
    }
  }
  return ret_code;
}

/** 
* Create new DtStructure(Case) that is the union of two input DtStructures(Cases).
* @param handle_1 DtStructure(Case).
* @param handle_2 DtStructure(Case).
* @return valid pointer on success, or 0 on failure.
*/
EXPORT_C_EXPORT DT_VALUE * DtStructureDoUnion(
	DT_VALUE* const handle_1,
	DT_VALUE* const handle_2)
{
  ErrClear();  // clear any previous error

  if( handle_1 && handle_2 &&
	  ( handle_1->DataType == DtStructure && handle_2->DataType == DtStructure ||
	    handle_1->DataType == DtStructureCase && handle_2->DataType == DtStructureCase ) )
  {
    DT_VALUE * dtStructure = (DT_VALUE *) malloc(sizeof(*dtStructure));

    if( dtStructure && InitDtv( *dtStructure, handle_1->DataType->Name ) )
    {
      if( DtStructureUnion(dtStructure, handle_1) && DtStructureUnion(dtStructure, handle_2) )
      {
        return dtStructure;
      }

      DTM_FREE(dtStructure);
    }

    free(dtStructure);
  }

  return 0;
}

/** 
* Get pointer to named element in DtStructure(Case).
* @param handle DtStructure handle
* @param name field name
* @return valid pointer on success, or 0 on failure.
*/
EXPORT_C_EXPORT DT_VALUE * DtStructureGetByName(
	DT_VALUE* const handle,
	const char* const name)
{
  ErrClear();  // clear any previous error

  if( name && *name && handle &&
	  ( handle->DataType == DtStructure || handle->DataType == DtStructureCase ) )
  {
    // make DT_VALUE for return value from query
    DT_VALUE dtvNull = { 0 };

    if(InitDtv(dtvNull, "Null"))
    {
      // make DT_VALUE of type DtString from name
      DT_VALUE dtvString = { 0 };

      if(InitDtv(dtvString, "String") && DtStringSet(&dtvString, name))
      {
        const int retVal = handle->DataType->Func(DT_MSG_COMPONENT,	&dtvNull,
			handle, &dtvString);

        DTM_FREE(&dtvString);

        return retVal ? (DT_VALUE *) dtvNull.Data.Pointer : 0;
      }
    }
  }

  return 0;
}

/**
* Checks if the key is present in this DtStructure(Case).
* @param handle the structure handler.
* @param key named element
* @param contains_key is set to 1 if the structure contains the key. Otherwise, contains_key is set to 0.
* @return FALSE on failure, TRUE otherwise.
*
*/
EXPORT_C_EXPORT int DtStructureContainsKey(
	DT_VALUE* handle,
	const char* key,
	int* contains_key)
{
	int ret_val = FALSE;
	*contains_key = 0;

	if( key && handle && ( handle->DataType == DtStructure || handle->DataType == DtStructureCase ) ){
		*contains_key = DtComponentGet(handle, key, NULL) ? 1 : 0;
		ret_val = TRUE;
	}
	return ret_val;
}

/**
* Gets a set of keys for a given DT_STRUCTURE. 
* @param handle the structure handler.
* @param pkeys pointer to an array of keys.
* @param psize keeps size of the allocated array.
* @return FALSE on failure, TRUE otherwise.
*
*/
EXPORT_C_EXPORT int DtStructureGetKeys(
	DT_VALUE* handle,
	void** pkeys,
	int* psize)
{
	int ret_val = FALSE;
	*pkeys = 0;
	*psize = 0;

	if( handle && ( handle->DataType == DtStructure || handle->DataType == DtStructureCase ) ){
		DT_VALUE query;
		DTM_INIT(&query);

		query.DataType = DtString;
		query.Data.Pointer = "_keys";
		
		// make DT_VALUE for return value from query
		DT_VALUE dtv_keys;
		DTM_INIT(&dtv_keys);

		if( ret_val = DtSubscriptGetCopy( handle, &query, &dtv_keys) )
			ret_val = DtArrayGetArrayOf( &dtv_keys, pkeys, psize );

		DTM_FREE(&dtv_keys);
	}

	return ret_val;
}

/**
* Gets a set of DT_VALUES currently stored in DT_STRUCTURE 
* (more precisely, a set of pointers to those DT_VALUES).
* @param phandle the structure handler.
* @param pparray pointer to an array of DT_VALUE*
* @param psize size of the allocated array.
* @return FALSE on failure, TRUE otherwise.
*
* NOTE: We need to get original pointers, so unlike with Keys,
*		 the DT_MSG_COMPONENT_VALUE message cannot be used.
* NOTE1: Caller is responsible for releasing allocated array.
*/
EXPORT_C_EXPORT int DtStructureGetValues(
	DT_VALUE* phandle,
	DT_VALUE*** pparray,
	int* psize)
{
	int ret_code = FALSE;
	*pparray = 0;
	*psize = 0;

	if( phandle && ( phandle->DataType == DtStructure || phandle->DataType == DtStructureCase ) ){
		HASH* phash = (DT_STRUCTURE*)phandle->Data.Pointer;
		int count = *psize = phash->KeyCount;

		if( count > 0 ){
			*pparray = (DT_VALUE**)co_task_mem_alloc( sizeof(DT_VALUE*) * count);
			memset(*pparray, 0, count);
			int idx = 0;
			
			for( HASH_ENTRY_PTR pentry = HashFirst(phash);
				!HashEnd(pentry); 
				pentry = HashNext(phash, pentry) )
			{
				*(*pparray + idx++) = (DT_VALUE*)HashValue(pentry);
			}
		}
		ret_code = TRUE;
	}
	return ret_code;
}


/**
* Get count of elements in DtStructure(Case).
* @param handle DtStructure handle.
* @return number of elements on success, -1 otherwise.
*/
EXPORT_C_EXPORT int DtStructureGetCount(DT_VALUE   * const handle)
{
  if( handle && ( handle->DataType == DtStructure || handle->DataType == DtStructureCase ) )
  {
    return ((DT_STRUCTURE *) handle->Data.Pointer)->KeyCount;
  }

  return -1;
}


/** 
* Remove and destroy named element in DtStructure(Case).
* @param handle DtStructure handle.
* @param name field name.
* @return SDB_SUCCESS on success, SDB_FAILURE on failure.
*/
EXPORT_C_EXPORT int DtStructureRemove(
	DT_VALUE* const handle,
	const char* const name)
{
  ErrClear();  // clear any previous error
  int ret_code = SDB_FAILURE;

  if(name && *name && handle && ( handle->DataType == DtStructure || handle->DataType == DtStructureCase ) )
  {
    // make DT_VALUE of type DtString from name
    DT_VALUE dtvString = { 0 };

    if(InitDtv(dtvString, "String") && DtStringSet(&dtvString, name))
    {
      ret_code = handle->DataType->Func(DT_MSG_COMPONENT_DESTROY, 0, handle, &dtvString) == 1
		  ? SDB_SUCCESS
		  : SDB_FAILURE;

      DTM_FREE(&dtvString);
    }
  }

  return ret_code;
}


// Add or replace a knot in a DtTCurve
//
// NOTE:  Assumes time is a valid time value.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtTCurveAddOrReplace(DT_VALUE * const handle,
                                          double            time,
                                          double            value)
{
  if(handle && (handle->DataType == DtTCurve))
  {
    DT_VALUE dtvTime = { 0 };

    if(InitDtv(dtvTime, "Time"))
    {
      dtvTime.Data.Number = time;

      DT_VALUE dtvDouble = { 0 };

      if(InitDtv(dtvDouble, "Double"))
      {
        dtvDouble.Data.Number = value;

        DT_TCURVE * const curve = (DT_TCURVE *) handle->Data.Pointer;

        if(DtCurveInsertKnotByDate(curve, &dtvTime, &dtvDouble) != 0)
        {
          return SDB_SUCCESS;
        }
        // else fall through and return 0
      }
      // else fall through and return 0
    }
    // else fall through and return 0
  }

  return SDB_FAILURE;
}

/**
* Get range of indexes into DtTCurve for which:
*
*       first <= DtTCurve[ndx].Time <= last
*
* NOTE:  Assumes times first and last have already been validated and that
* first <= last.
*
* @return valid index pair if any knots are found in this time range.
* 
* NOTE: Pair {-1,-1} means that no knots are found in this date range.
*/	
EXPORT_C_EXPORT int DtTCurveTimesToIndexes(
	DT_VALUE* const handle,
	const double first,
	const double last,
	Index::Pair** pair)
{
  if( !( handle && handle->DataType == DtTCurve) ) return FALSE;

  *pair = (Index::Pair*)co_task_mem_alloc( sizeof( Index::Pair ) ); //the memory is released on the managed side.
  if(!*pair) return FALSE;

  **pair = DtCurveDatesTimesToIndexes( (DT_CURVE *)handle->Data.Pointer, first, last );
  return TRUE;
}


// Get an existing knot from a DtTCurve by index
//
// NOTE:  Returns KnotData with invalid values on failure, otherwise returns
// requested knot data.
//
// NOTE:  Assumes ndx has already been validated.

EXPORT_C_EXPORT Knot::KnotData DtTCurveGetByNdx(const DT_VALUE * const handle,
                                                  const int              ndx)
{
  // initialize to invalid values for failure return
   Knot::KnotData retVal = { -1.0, std::numeric_limits<double>::quiet_NaN() };

  if(handle && (handle->DataType == DtTCurve))
  {
    DT_TCURVE * const curve = ((DT_TCURVE *) handle->Data.Pointer);

    retVal.m_date_time = curve->Knots[ndx].Date;
    retVal.m_value     = curve->Knots[ndx].Value;
  }

  return retVal;
}

/**
* Get an existing knot from a DtTCurve by time.
* @param handle curve handle.
* @param time UTC time.
* Returns KnotData with invalid values on failure, otherwise returns requested knot data.
*
* NOTE: If the time does not belong to this TCurve, an interpolated knot is returned.
*
* NOTE:  Assumes parameter time has already been validated.
*/
EXPORT_C_EXPORT int DtTCurveGetByTime(
	DT_VALUE * const handle,
	double time,
	Knot::KnotData** pknot)
{
	*pknot = 0;
	if( !( handle && handle->DataType == DtTCurve ) ) return FALSE;

	int ret_val = FALSE;
	DT_VALUE dtv_time, dtv_value;
	DTM_INIT(&dtv_time);
	DTM_INIT(&dtv_value);

	if( InitDtv( dtv_time, "Time") ){

		dtv_time.Data.Number = time;

		if( ret_val = DtSubscriptGetCopy( handle, &dtv_time, &dtv_value) ){
			*pknot = (Knot::KnotData*)co_task_mem_alloc( sizeof(Knot::KnotData) );
			if( !*pknot ){
				DTM_FREE(&dtv_time);
				return FALSE;
			}

			(*pknot)->m_date_time = time;
			(*pknot)->m_value = dtv_value.Data.Number;
		}

		DTM_FREE(&dtv_time);
	}

	return ret_val;
}

// Get count of knots in DtTCurve.
//
// Returns -1 on failure;

EXPORT_C_EXPORT int DtTCurveGetCount(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtTCurve))
  {
    return ((DT_TCURVE *) handle->Data.Pointer)->KnotCount;
  }

  return -1;
}

// Remove knots from DtTCurve[first] through and including DtTCurve[last]
//
// NOTE:  Assumes indexes first and last have already been validated and that
// first <= last.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtTCurveRemoveRange(DT_VALUE * const handle,
                                    const int        first,
                                    const int        last)
{
  if(handle && (handle->DataType == DtTCurve))
  {
    DtCurveRemoveKnots((DT_CURVE *) handle->Data.Pointer, first, last);
    return SDB_SUCCESS;
  }
  return SDB_FAILURE;
}


// Set an existing knot in a DtTCurve by index
//
// NOTE:  Assumes ndx and time have already been validated.
//
// Returns SDB_SUCCESS on success, or SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtTCurveSetByNdx(DT_VALUE * const handle,
                                       const int        ndx,
                                       const double     value)
{
  if(handle && (handle->DataType == DtTCurve))
  {
    DT_TCURVE * const curve = (DT_TCURVE *) handle->Data.Pointer;

    curve->Knots[ndx].Value = value;

    return SDB_SUCCESS;
  }

  return SDB_FAILURE;
}


// Get double inside DT_VALUE of type DtTime.
//
// NOTE:  Returns NaN on failure

EXPORT_C_EXPORT double DtTimeGet(const DT_VALUE * const handle)
{
  if(handle && (handle->DataType == DtTime))
  {
    return handle->Data.Number;
  }

  return std::numeric_limits<double>::quiet_NaN();
}

// Copy double into DT_VALUE of type DtTime.
//
// NOTE:  Returns SDB_SUCCESS on success, SDB_FAILURE on failure.

EXPORT_C_EXPORT int DtTimeSet(DT_VALUE   * const handle,
                                const double       value)
{
  if(handle && (handle->DataType == DtTime))
  {
    handle->Data.Number = value;
    return SDB_SUCCESS;
  }

  return SDB_FAILURE;
}


// Set value of DtTime from char *.
//
// Returns 1 for success and 0 for failure.

EXPORT_C_EXPORT const int DtTimeSetString(DT_VALUE   * const handle,
                                            const char * const src)
{
  if(handle && (handle->DataType == DtTime) && src && *src)
  {
    const time_t timeVal = TimeFromString(src);

    if(timeVal != TIME_ERR)
    {
      // FIXME:  time_t is 64 bits on WinDoze, so the cast to int
      // cuts off the high 32 bits, which works just fine for now.
      handle->Data.Number = double(int(timeVal));

      return 1;
    }
    // else fall through to return 0
  }

  return 0;
}


/**
* Max valid time (date.h).
*/
EXPORT_C_EXPORT double GetMaxTime()
{
	return TIME_MAX;
}

/**
* Min valid time (date.h).
*/
EXPORT_C_EXPORT double GetMinTime()
{
	return TIME_ERR + 1.0;
}

/**
* Max valid date.
*/
EXPORT_C_EXPORT double GetMaxDate()
{
	return MAX_DATE_THAT_WORKS;
}

/** 
* Min valid date.
*/
EXPORT_C_EXPORT double GetMinDate()
{
	return DATE_ERR + 1.0;
}


/**
* Get DT_VALUE data type name for specified handle.
* @param handle object pointer.
* @return type name on success, null pointer otherwise.
*/
EXPORT_C_EXPORT const char * DtvGetTypeName(DT_VALUE * handle)
{
  return handle ? handle->DataType->Name : 0;
}

/**
* Get current error message string.
* @return current error message string, which will be empty iff no error has occurred.
*/
EXPORT_C_EXPORT const char * LastErrString()
{
  return ErrGetString();
}

/**
* Get error message string for error errCode.
* @param errNum error code.
* @return error message string associated with errCode.
*/
EXPORT_C_EXPORT const char * ErrString(int errNum)
{
  return ErrStringFromNum(errNum);
}

/**
* Compares two DT_VALUE objects.
* @param presult is 1 if the objects are equal, 0 otherwise.
* @param pdtv1, pdtv2 objects to compare.
* @return FALSE on failure, TRUE otherwise.
*
*/
EXPORT_C_EXPORT int DtvEqual( 
	DT_VALUE* pdtv1,
	DT_VALUE* pdtv2,
	int* result )
{
	int ret_val = TRUE;
	*result = 1;

	if( pdtv1 == pdtv2 ) return ret_val;
	if( !pdtv2 || !pdtv1){
		*result = 0;
		return ret_val;
	}

	DT_VALUE dtv_result;
	DTM_INIT(&dtv_result);

	if( DT_OP( DT_MSG_EQUAL, &dtv_result, pdtv1, pdtv2 ) == TRUE )
		*result = dtv_result.Data.Number ? 1 : 0;
	else
		*result = 0;//this is how SecDb types interpret the failure.
	
	DTM_FREE(&dtv_result);
	return ret_val;
}

/**
* Returns a given DtValue as a string.
* @param handle pointer to a DT_VALUE.
* @param result DT_VALUE as a string.
* @return FALSE on failure, TRUE otherwise.
*
*/
EXPORT_C_EXPORT int DtvToString(
	DT_VALUE* handle,
	char** result)
{
	int ret_val = FALSE;
	*result = 0;

	if( handle ){
		DT_VALUE dtv_result;
		DTM_INIT(&dtv_result);
		
		if( InitDtv( dtv_result, "String") ){

			if( ret_val = DT_OP( DT_MSG_TO, &dtv_result, handle, 0 ) ){
				int data_size = strlen( (char*)dtv_result.Data.Pointer );
				*result = (char*)co_task_mem_alloc( sizeof(char) * ( data_size + 1 ) ); //this is released by managed caller.

				if( !*result ){
					DTM_FREE(&dtv_result);
					return FALSE;
				}

				::memcpy( *result, (char*)dtv_result.Data.Pointer, data_size );
				*(*result + data_size) = '\0';
			}

			DTM_FREE(&dtv_result);
		}
	}
	return ret_val;
}
