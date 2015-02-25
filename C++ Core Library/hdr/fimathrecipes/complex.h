/** DO NOT DELETE THIS LINE! Janified_by_janify Wed Jan 12 11:41:07 2000 **/

#ifndef _NR_COMPLEX_H_
#define _NR_COMPLEX_H_

#include <fimathrecipes/fimathrecipesExport.h>

#ifndef _FCOMPLEX_DECLARE_T_
typedef struct FCOMPLEX {double r,i;} fcomplex;
#define _FCOMPLEX_DECLARE_T_
#endif /* _FCOMPLEX_DECLARE_T_ */

#if 1 /* ANSI */

fcomplex Cadd(fcomplex a, fcomplex b);
fcomplex Csub(fcomplex a, fcomplex b);
fcomplex Cmul(fcomplex a, fcomplex b);
fcomplex Complex(double re, double im);
fcomplex Conjg(fcomplex z);
fcomplex Cdiv(fcomplex a, fcomplex b);
double Cabs(fcomplex z);
fcomplex Csqrt(fcomplex z);
fcomplex RCmul(double x, fcomplex a);

#else /* ANSI */
/* traditional - K&R */

fcomplex Cadd();
fcomplex Csub();
fcomplex Cmul();
fcomplex Complex();
fcomplex Conjg();
fcomplex Cdiv();
double Cabs();
fcomplex Csqrt();
fcomplex RCmul();

#endif /* ANSI */

#endif /* _NR_COMPLEX_H_ */
