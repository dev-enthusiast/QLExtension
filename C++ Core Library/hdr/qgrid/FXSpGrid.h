/****************************************************************
**
**	qgrid/FXSpGrid.h	- Definition of class FXSpGrid
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: FXSpGrid.h,v $
**	Revision 1.1  2000/02/28 16:17:08  irwins
**	- Renamed from fxspgrid.h
**
**	Revision 1.1  1999/12/11 00:36:39  lowthg
**	Added FXSpGrid.cpp and FXSpGrid.h (sparse grid stuff)
**	
**
****************************************************************/

#if !defined( IN_QGRID_FXSPGRID_H )
#define IN_QGRID_FXSPGRID_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	FXSpGrid provides a mapping from an arbitrary dimension sparse
**	grid onto a linear array.
*/

class FXSpGrid
{
	private:
		int		m_hierarchy;
		int		m_size;
	public:
		typedef int iterator;

		FXSpGrid( int hierarchy, int dimension );
		~FXSpGrid();
		inline int		GetHierarchy() const;
		inline int		GetDimension() const;
		inline int		GetSize() const;
		inline iterator	begin() const;
		inline iterator	end() const;
		inline iterator next( int dim, iterator iter ) const;
		inline iterator	prev( int dim, iterator iter ) const;
		inline iterator next_hier( int dim, iterator iter ) const;
		inline iterator prev_hier( int dim, iterator iter ) const;
		inline int		GetHierarchy( iterator iter ) const;
		inline int		GetHierarchy( iterator iter, int dim ) const;
		inline int		GetIndex( iterator iter, int dim ) const;
		inline GsVector CreateSpField() const;
		GsVector 		CreateSpFieldFromMatrix( const GsMatrix& M ) const;
		GsMatrix		CreateFullMatrixFromSpField( const GsVector& F ) const;
		GsMatrix		CreateSpMatrixFromSpField( const GsVector& F ) const;
		void			NodeToHier( GsVector& out, const GsVector& in, int dim ) const;
		void			HierToNode( GsVector& out, const GsVector& in, int dim ) const;
		void			SetLowBarrier( GsVector& field, double value, int dim ) const;
		void			SetHighBarrier( GsVector& field, double value, int dim ) const;
		void			SetLowBarrier( GsVector& field, const GsVector& barrier, int dim ) const;
		void			SetHighBarrier( GsVector& field, const GsVector& barrier, int dim ) const;
		void			SetFinalPayout( GsVector& field, const GsVector& payout ) const;
		void			TransferInitialToFinal( GsVector& out, const GsVector& in ) const;
};

CC_END_NAMESPACE
#endif
