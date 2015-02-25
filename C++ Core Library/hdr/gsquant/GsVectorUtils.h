/****************************************************************
**
**	GSVECTORUTILS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsVectorUtils.h,v 1.6 2004/10/25 21:25:02 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSVECTORUTILS_H )
#define IN_GSQUANT_GSVECTORUTILS_H

#include <gsquant/base.h>
#include <gscore/GsVector.h>
#include <gscore/GsCFunc.h>

CC_BEGIN_NAMESPACE( Gs )

class GsVectorFill
    : public GsCFunc< GsVector, GsVector >
{
public:
    GsVectorFill( double fillValue, unsigned leftFill, unsigned rightFill ) 
    : m_fillValue( fillValue ),
      m_leftFill( leftFill ),
      m_rightFill( rightFill ) {}
    GsVectorFill( const GsVectorFill &f )
    : m_fillValue( f.m_fillValue ),
      m_leftFill( f.m_leftFill ),
      m_rightFill( f.m_rightFill ) {}
    ~GsVectorFill() {}
    GsVector operator()( const GsVector &vec ) const
	{
         GsVector resultVec( vec.size() + m_leftFill + m_rightFill );

		 unsigned i, j;

         for( i=0; i<m_leftFill; i++ )
            resultVec[i] = m_fillValue;
         for( j=0; j<vec.size(); i++, j++)
            resultVec[i] = vec[j];
         for( ; i < resultVec.size(); i++)
            resultVec[i] = m_fillValue;
         return resultVec;
    }
private:
    double m_fillValue;
    unsigned m_leftFill, m_rightFill;
};

class GsVectorSliceBegin
	: public GsCFunc< GsVector, GsVector >
{
public:
    GsVectorSliceBegin( int sliceSize )
	: m_sliceSize( sliceSize ) {}
    GsVectorSliceBegin( const GsVectorSliceBegin &v )
	: m_sliceSize( v.m_sliceSize ) {}
    ~GsVectorSliceBegin() {}
    
    GsVector operator()( const GsVector &v ) const
    {
        return GsVector( m_sliceSize, v.data() );
    }
private:
    int m_sliceSize;
};

class GsVectorSliceEnd
   : public GsCFunc< GsVector, GsVector >
{
public:
    GsVectorSliceEnd( int sliceSize )
	: m_sliceSize( sliceSize ) {}
    GsVectorSliceEnd( const GsVectorSliceEnd &v )
	: m_sliceSize( v.m_sliceSize ) {}
    ~GsVectorSliceEnd() {}
  
    GsVector operator()( const GsVector &v ) const
    {
        return GsVector( m_sliceSize, v.data()+v.size()-m_sliceSize );
    }
private:
    int m_sliceSize;
};

class GsVectorAppend
	: public GsCFunc< GsVector,GsVector >
{
public:
    GsVectorAppend( const GsVector &baseParams )
	: m_baseParams( baseParams ) {}
    GsVectorAppend( const GsVectorAppend &v )
    : m_baseParams( v.m_baseParams ) {}
    ~GsVectorAppend() {}
    GsVector operator()( const GsVector &v ) const
    {
       GsVector retVec( m_baseParams.size()+v.size() );
       memcpy( retVec.begin(), m_baseParams.data(), 
             m_baseParams.size() * sizeof( double ) );
       memcpy( retVec.begin()+m_baseParams.size(), v.data(), 
              v.size() * sizeof( double ) );
       return retVec;
    }
private:
    GsVector m_baseParams;
};

CC_END_NAMESPACE

#endif 





