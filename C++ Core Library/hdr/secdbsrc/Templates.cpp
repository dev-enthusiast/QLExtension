#define GSCVSID "$Header: /home/cvs/src/secdb/src/Templates.cpp,v 1.12 2012/06/11 19:58:44 e19351 Exp $"
/****************************************************************
**
**	Templates.cpp	- Templates for secdb
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: Templates.cpp,v $
**	Revision 1.12  2012/06/11 19:58:44  e19351
**	Work around VC6's broken template support.
**	AWR: #177555
**
**	Revision 1.11  2011/05/24 13:52:34  khodod
**	Another try to make this work on Windows.
**	AWR: #177463
**
**	Revision 1.9  2001/11/27 23:23:31  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.6  2001/08/22 20:22:37  singhki
**	initial revision of compiled graphs.
**	modified GsNodeValues to be virtual so that we can define SecDbCompiledVectorValue.
**	Created SDB_DYNAMIC_CHILDLIST which invokes the VF during BuildChildren in order
**	to get the dependencies from the compiled graph nodes.
**	
**	Revision 1.5  2001/08/20 22:24:15  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.4  2001/05/11 14:48:33  johnsoc
**	Correct registration for GsDtSecurity
**	
**	Revision 1.3  2001/03/19 15:12:16  johnsoc
**	Removed GsTimeSeries.h
**	
**	Revision 1.2  2001/03/19 14:18:33  johnsoc
**	Tidied up unnecessary includes
**	
**	Revision 1.1  2001/01/26 19:07:50  johnsoc
**	Relocated LtSecDbObj class
**	
**	
**	
****************************************************************/

#define BUILDING_SECDB

#include	<portable.h>
#include    <secdb/base.h>
#include	<secdb/LtSecDbObj.h>
#include	<secdb/SecDbCompiledGraph.h>
#include    <gscore/AdVector.h>
#include    <gscore/GsTimeSeries.hi>
#include    <gsdt/GsDtGeneric.h>
#include	<gsdt/GsDtGeneric.hi>

#include    <string>

CC_BEGIN_NAMESPACE( Gs )

// HACK: VC6 does not like the correct order. Remove when we upgrade.
#if CC_COMPILER == CC_COMPILER_MSVC6

template class EXPORT_CLASS_SECDB GsDtGeneric< LtSecDbObj >;
// Whether this works at all is dubious.. It's already instantiated above.
// The linker seems to patch it up in the end, but it's definitely invalid.
template<>
std::string GsDtGeneric< LtSecDbObj >::myTypeName()
{
	return std::string( "GsDtSecurity" );
}

#else

template<>
std::string GsDtGeneric< LtSecDbObj >::myTypeName()
{
	return std::string( "GsDtSecurity" );
}
template class EXPORT_CLASS_SECDB GsDtGeneric< LtSecDbObj >;

#endif

template class EXPORT_CLASS_SECDB GsDtGeneric< SecDbCompiledGraph >;

INSTANTIATE_TIMESERIES_TEMPLATES( SECDB, LtSecDbObj )

CC_END_NAMESPACE