/****************************************************************
**
**	n_tensor.h	- operators on Q_TENSORs 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_N_TENSOR_H )
#define IN_N_TENSOR_H

#include <n_base.h>
#include <qenums.h>

/*
**	The structure used as Args for LinearRescale
*/

typedef struct
{
	int first;		// first index on new grid within bounds of old
	int last;		// last index on new grid within bounds of old 
	double dx;		// dx on new grid in terms of old grid dx
	double x;		// x value of first in old grid coords
} N_RESCALING_STRUCTURE;

/*
**	Structure allowing easy stepping through rows of a tensor
*/

typedef struct
{
	int dim; 		  // the dimension of the tensor
	int* size;		  // size of the tensor in each dimension
	int* step;		  // step in each dimension
	int* nrows;		  // number of rows parallel to each dimension
	int** rowoffsets; // rowoffsets[ i ][ j ] is the j'th row
					  // perpendicular to dimension i ( 0 <= j < nrows[ i ] )
} N_TENSOR_ROWS;

EXPORT_C_NUM int N_CreateTensor(
	Q_TENSOR*	V,		// the tensor with dim and size members initialised
	int*		size	// returns the size of the tensor
);

EXPORT_C_NUM int N_CreateTensorRows(
	const Q_TENSOR* Tensor,		// tensor structure (data is not used)
	N_TENSOR_ROWS* TensorRows	// TensorRows to be created
);

EXPORT_C_NUM void N_FreeTensorRows(
	N_TENSOR_ROWS* TensorRows
);

EXPORT_C_NUM int N_CopyTensor(
	const Q_TENSOR	*Vin,	// tensor to copy from
	Q_TENSOR		*Vout	// tensor to copy to
);

EXPORT_C_NUM int N_1DTensorOp(
	const Q_TENSOR 	*Vin,		// tensor to operate on
	const Q_OP		*Operator,	// the 1D vector operator
	int				dim,		// dimension along which to operate
	Q_TENSOR 		*Vout		// result (can be equal to Vin)
);

EXPORT_C_NUM int N_ReduceTensor(
	const Q_TENSOR 	*Vin,		// tensor to operate on
	const Q_OP		*Operator,	// the 1D vector operator
	int				dim,		// dimension along which to operate
	Q_TENSOR 		*Vout		// result (can be equal to Vin)
);

EXPORT_C_NUM int N_1DRescaleOp(
	Q_OP			*Operator,	// operator to be created
	double			OldMin,		// old value of mingrid
	double			OldMax,		// old value of maxgrid
	double			NewMin,		// new value of mingrid
	double			NewMax,		// new value of maxgrid
	int				nStepsIn,	// size of input vectors
	int				nStepsOut	// size of output vectors
);

EXPORT_C_NUM int N_1DTensorRescale(
	const Q_TENSOR 	*Vin,		// tensor to operate on
	int				dim,		// dimension along which to operate
	double			OldMin,		// old value of mingrid
	double			OldMax,		// old value of maxgrid
	double			NewMin,		// new value of mingrid
	double			NewMax,		// new value of maxgrid
	Q_TENSOR 		*Vout		// result (can be equal to Vin)
);

EXPORT_C_NUM int N_SliceTensor(
	const Q_TENSOR* InVals,		// tensor to slice
	int				dim,		// dimension along which to slice
	double			Min,		// min value in interpolating dimension
	double			Max,		// max value in interpolating dimension 
	double			x,			// value at which to slice
	Q_TENSOR*		OutVals		// returned values
);

EXPORT_C_NUM int N_TensorLineOp(
	const Q_TENSOR 	 *Vin,		// tensor to operate on
	const Q_OPERATOR Operator,	// the 1D vector operator
	Q_TENSOR		 *args,		// args (tensor of void*'s, not doubles)
	int				 dim,		// dimension along which to operate
	Q_TENSOR 		 *Vout		// result (can be equal to Vin)
);

#endif

