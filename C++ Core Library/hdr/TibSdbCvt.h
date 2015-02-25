/** @file TibSdbCvt.h
  *
  * Tibco SecDb Datatype conversions
  *
  * @author Kirill Lapshin
  *
  * $Header: /home/cvs/src/tibadapter/src/TibSdbCvt.h,v 1.4 2011/10/03 21:51:06 khodod Exp $
  *
  */

#ifndef TIBSDBCVT_H_INCLUDED
#define TIBSDBCVT_H_INCLUDED

#ifdef _MSC_VER
	#ifdef TIBSDBCVT_EXPORTS
		#define TIBSDBCVT_DLLEXPORT __declspec(dllexport)
	#else
		#define TIBSDBCVT_DLLEXPORT __declspec(dllimport)
	#endif
#else
	#define TIBSDBCVT_DLLEXPORT
#endif

struct DT_VALUE;
class MNode;
class MData;
class MBaseClassDescription;
class MClassRegistry;

#include <memory>
#include <exception>
#include <string>

#include <MString.h>

namespace tib
{
	class ConversionEx : public std::exception {};

	/** Converts Tibco MNode to SecDb DT_VALUE.
	@param msg - [in] MNode to convert
	@param dtVal - [out] Output DT_VALUE
	@exception throws ConversionEx in case of failure
	*/
	TIBSDBCVT_DLLEXPORT void DtValueFromMNode( const MNode& msg, DT_VALUE * dtVal); 

	/** Converts Tibco MData to SecDb DT_VALUE.
	@param inst - [in] MData to convert
	@param dtVal - [out] Output DT_VALUE
	@exception throws ConversionEx in case of failure
	*/
	TIBSDBCVT_DLLEXPORT void DtValueFromMData( const MData& inst, DT_VALUE * dtVal); 

	/** Converts SecDb DT_VALUE to Tibco MData given Tibco metadata.
	Metadata is required because first we do not have one to one relationship between Tibco
	datatypes and secdb datapyes, and second to create MInstance or MSequence we have to know the exact
	class name. For example all numeric tibco datatypes (I1, I2, I4, UI1, UI2, UI4, R4, R8 and even boolean)
	represented as DtDouble in SecDb, so during conversion back we are using metdata to figure out
	where convert DtDouble to.
	@param dtVal - [in] DT_VALUE to convert
	@param pClassRegistry - [in] Tibco Class registry, it is needed to create advanced data structures, such as MInstance or MSequence
	@param desc  - [in] Class description (metadata)
	@return auto_ptr<MData>
	@exception throws ConversionEx in case of failure
	*/
	TIBSDBCVT_DLLEXPORT std::auto_ptr<MData> DtValueToMData( const DT_VALUE * dtVal, MClassRegistry * pClassRegistry, const MBaseClassDescription * desc); 

	/** Truncate a Tibco MString to the maximum length specified replacing newline with the '\\n' sequence.
	@param ms  - [in] MString
    @param len - [in] size_t (default 1024)
	@return std::string
	*/
    TIBSDBCVT_DLLEXPORT std::string TruncateMString( const MString& ms, size_t len = 1024 );
}

#endif

