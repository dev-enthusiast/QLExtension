/** DO NOT DELETE THIS LINE! Janified_by_janify Wed Jan 12 11:41:07 2000 **/
#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_

#include <fimathrecipes/fimathrecipesExport.h>

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static double dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

static double minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
        (minarg1) : (minarg2))

static long lmaxarg1,lmaxarg2;
#define LMAX(a,b) (lmaxarg1=(a),lmaxarg2=(b),(lmaxarg1) > (lmaxarg2) ?\
        (lmaxarg1) : (lmaxarg2))

static long lminarg1,lminarg2;
#define LMIN(a,b) (lminarg1=(a),lminarg2=(b),(lminarg1) < (lminarg2) ?\
        (lminarg1) : (lminarg2))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

/* Keep compiler happy */
static int nrutil_dummy_init()
{
	sqrarg   = 0.0;
	dsqrarg  = 0.0;
	dmaxarg1 = 0.0;
	dmaxarg2 = 0.0;
	dminarg1 = 0.0;
	dminarg2 = 0.0;
	maxarg1  = 0.0;
	maxarg2  = 0.0;
	minarg1  = 0.0;
	minarg2  = 0.0;
	lmaxarg1 = 0;
	lmaxarg2 = 0;
	lminarg1 = 0;
	lminarg2 = 0;
	imaxarg1 = 0;
	imaxarg2 = 0;
	iminarg1 = 0;
	iminarg2 = 0;
	return 0;
}
static int used_nrutil_dummy_init = nrutil_dummy_init();

#if 1 /* ANSI */

DLLEXPORT void (*nrErrorHandler)(char*);
  
EXPORT_CPP_FIMATHRECIPES void nrerror(char error_text[]);
#ifdef FIRFRAMEWORK
#ifndef  __STL
EXPORT_CPP_FIMATHRECIPES double *vector(long nl, long nh);
#endif /* __STL */
#else /* not FIRFRAMEWORK */
EXPORT_CPP_FIMATHRECIPES double *vector(long nl, long nh);
#endif /* not FIRFRAMEWORK */
EXPORT_CPP_FIMATHRECIPES int *ivector(long nl, long nh);
EXPORT_CPP_FIMATHRECIPES unsigned char *cvector(long nl, long nh);
EXPORT_CPP_FIMATHRECIPES unsigned long *lvector(long nl, long nh);
EXPORT_CPP_FIMATHRECIPES double *dvector(long nl, long nh);
EXPORT_CPP_FIMATHRECIPES double **matrix(long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES double **dmatrix(long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES int **imatrix(long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES double **submatrix(double **a, long oldrl, long oldrh, long oldcl, long oldch,
	long newrl, long newcl);
EXPORT_CPP_FIMATHRECIPES double **convert_matrix(double *a, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES double ***f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
EXPORT_CPP_FIMATHRECIPES void free_vector(double *v, long nl, long nh);
EXPORT_CPP_FIMATHRECIPES void free_ivector(int *v, long nl, long nh);
EXPORT_CPP_FIMATHRECIPES void free_cvector(unsigned char *v, long nl, long nh);
EXPORT_CPP_FIMATHRECIPES void free_lvector(unsigned long *v, long nl, long nh);
EXPORT_CPP_FIMATHRECIPES void free_dvector(double *v, long nl, long nh);
EXPORT_CPP_FIMATHRECIPES void free_matrix(double **m, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES void free_submatrix(double **b, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES void free_convert_matrix(double **b, long nrl, long nrh, long ncl, long nch);
EXPORT_CPP_FIMATHRECIPES void free_f3tensor(double ***t, long nrl, long nrh, long ncl, long nch,
	long ndl, long ndh);

#else /* ANSI */
/* traditional - K&R */

extern void (*nrErrorHandler)();

void nrerror();
double *vector();
double **matrix();
double **submatrix();
double **convert_matrix();
double ***f3tensor();
double *dvector();
double **dmatrix();
int *ivector();
int **imatrix();
unsigned char *cvector();
unsigned long *lvector();
void free_vector();
void free_dvector();
void free_ivector();
void free_cvector();
void free_lvector();
void free_matrix();
void free_submatrix();
void free_convert_matrix();
void free_dmatrix();
void free_imatrix();
void free_f3tensor();

#endif /* ANSI */

#endif /* _NR_UTILS_H_ */

