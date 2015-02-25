/* $Header:  */
/****************************************************************
**
**	N_MATRIX.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_N_MATRIX_H )
#define IN_N_MATRIX_H

#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif

typedef struct 
{
  Q_MATRIX *VolMatrix;
  Q_VECTOR *Times;
  Q_VECTOR *Spots;
}  Q_LOCAL_VOLS;

EXPORT_C_NUM int N_GrammSchmidt(
		Q_MATRIX *Inp,
		Q_MATRIX *Out
);

EXPORT_C_NUM int	N_InvertLT(
		Q_MATRIX *Inp,
		Q_MATRIX *Out
);

EXPORT_C_NUM int N_Print_Matrix(
		Q_MATRIX *Inp
);

EXPORT_C_NUM int	N_MatTranspose(
	Q_MATRIX *Inp,
	Q_MATRIX *Out
);

EXPORT_C_NUM int	N_MatMult(
		Q_MATRIX *Inp1,
		Q_MATRIX *Inp2,
		Q_MATRIX *Out
);

EXPORT_C_NUM int	N_Initialize_Matrix(
		Q_MATRIX	*mat,
		int			*rows,
		int			*cols
);

EXPORT_C_NUM int	N_Initialize_Vector(
		Q_VECTOR	*vec,
		int			*size
);

EXPORT_C_NUM int N_U_BackSub(
	Q_MATRIX *U,	//Input Upper Triangular Matrix
	Q_VECTOR *y,	//Input Vector
	Q_VECTOR *x		//Output Vector
);

EXPORT_C_NUM int N_L_ForwardElim(
	Q_MATRIX *L,	//Input Lower Triangular Matrix
	Q_VECTOR *b,	//Input Vector
	Q_VECTOR *y		//Output Vector
);

EXPORT_C_NUM int N_CholeskyDecomposition(
	Q_MATRIX *A 	//Input Sym PD matrix / Output Triangular Matrix
);

EXPORT_C_NUM int N_CopyMat2Mat(
	Q_MATRIX *A,	//Input matrix
	Q_MATRIX *B 	//Output matrix
);

EXPORT_C_NUM int N_CopyMat2Vec(
	Q_MATRIX *A,	//Input Matrix
	Q_VECTOR *y 	//Output Vector
);

EXPORT_C_NUM int N_CopyVec2Mat(
	Q_VECTOR *x,	//Input Vector
	Q_MATRIX *A 	//Output Matrix
);

EXPORT_C_NUM int N_CopyVec2Vec(
	Q_VECTOR *x,	//Input Vector
	Q_VECTOR *y 	//Output Vector
);


EXPORT_C_NUM double N_GS (
	Q_MATRIX	*InpMatrix,	// input matrix
	int			i,			// row of output element
	int			j			// column of output element
	);

EXPORT_C_NUM double N_xAx(
	Q_VECTOR	*Vector,	// input vector
	Q_MATRIX	*mat		// input matrix
	);

EXPORT_C_NUM double N_LowestEigenVal	(
	Q_MATRIX     *M        // input matrix;
	);

EXPORT_C_NUM int N_MatrixSolve(
	Q_MATRIX	*Coefficients,	// M
	Q_VECTOR	*Solutions,		// B
	Q_VECTOR	*Answers		// X
	);

EXPORT_C_NUM double N_LocalVol (
	double         Time,     // input time
	double         Time2,    // input time - not used in this function
	double         Spot,     // input spot
	Q_LOCAL_VOLS   *Context  // input structure with vol data
	);

EXPORT_C_NUM int N_MatrixInvert(
	Q_MATRIX   *Coefficients,  // M
	Q_MATRIX   *Answers        // M^(-1)
	);

EXPORT_C_NUM double N_Determinant(
	Q_MATRIX     *M        // input matrix;
	);
			
#endif

