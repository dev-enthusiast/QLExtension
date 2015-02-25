#if !defined( IN_MARQUARD_H )
#define IN_MARQUARD_H


#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif

EXPORT_C_NUM int
		N_Marquardt(					double x[], double y[], int NPT, double a[], int ia[], int NA, double *chisq,
										N_GENERIC_FUNC_PARAM1_VECTOR funcs, void *Context, double MaxChi2, int MaxIter );

EXPORT_C_NUM int
		N_Marquardt2(					double x[], double y[], int NPT, double a[], int ia[], int NA, double *chisq, Q_MATRIX *cov,
										N_GENERIC_FUNC_PARAM1_VECTOR funcs, void *Context, double MaxChi2, int MaxIter );
			
#endif


