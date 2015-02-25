/** DO NOT DELETE THIS LINE! Janified_by_janify Wed Jan 12 11:41:07 2000 **/

#ifndef _NR_H_
#define _NR_H_

#include <fimathrecipes/fimathrecipesExport.h>

#ifndef _WAVEFILT_DECLARE_T_ 
typedef struct { int ncof,ioff,joff; double *cc,*cr;} wavefilt;
#define _WAVEFILT_DECLARE_T_
#endif /* _WAVEFILT_DECLARE_T_ */

#ifndef _FCOMPLEX_DECLARE_T_
typedef struct FCOMPLEX {double r,i;} fcomplex;
#define _FCOMPLEX_DECLARE_T_
#endif /* _FCOMPLEX_DECLARE_T_ */

#ifndef _ARITHCODE_DECLARE_T_
typedef struct {
	unsigned long *ilob,*iupb,*ncumfq,jdif,nc,minint,nch,ncum,nrad;
} arithcode;
#define _ARITHCODE_DECLARE_T_
#endif /* _ARITHCODE_DECLARE_T_ */

#ifndef _HUFFCODE_DECLARE_T_
typedef struct {
	unsigned long *icod,*ncod,*left,*right,nch,nodemax;
} huffcode;
#define _HUFFCODE_DECLARE_T_
#endif /* _HUFFCODE_DECLARE_T_ */

#include <stdio.h>

EXPORT_CPP_FIMATHRECIPES void addint(double **uf, double **uc, double **res, int nf);
EXPORT_CPP_FIMATHRECIPES void airy(double x, double *ai, double *bi, double *aip, double *bip);
EXPORT_CPP_FIMATHRECIPES void amebsa(double **p, double y[], int ndim, double pb[],	double *yb,
	double ftol, double (*funk)(double []), int *iter, double temptr);
EXPORT_CPP_FIMATHRECIPES void amoeba(double **p, double y[], int ndim, double ftol,
	double (*funk)(double []), int *iter);
EXPORT_CPP_FIMATHRECIPES double amotry(double **p, double y[], double psum[], int ndim,
	double (*funk)(double []), int ihi, double fac);
EXPORT_CPP_FIMATHRECIPES double amotsa(double **p, double y[], double psum[], int ndim, double pb[],
	double *yb, double (*funk)(double []), int ihi, double *yhi, double fac);
EXPORT_CPP_FIMATHRECIPES void anneal(double x[], double y[], int iorder[], int ncity);
EXPORT_CPP_FIMATHRECIPES double anorm2(double **a, int n);
EXPORT_CPP_FIMATHRECIPES void arcmak(unsigned long nfreq[], unsigned long nchh, unsigned long nradd,
	arithcode *acode);
EXPORT_CPP_FIMATHRECIPES void arcode(unsigned long *ich, unsigned char **codep, unsigned long *lcode,
	unsigned long *lcd, int isign, arithcode *acode);
EXPORT_CPP_FIMATHRECIPES void arcsum(unsigned long iin[], unsigned long iout[], unsigned long ja,
	int nwk, unsigned long nrad, unsigned long nc);
EXPORT_CPP_FIMATHRECIPES void asolve(unsigned long n, double b[], double x[], int itrnsp);
EXPORT_CPP_FIMATHRECIPES void atimes(unsigned long n, double x[], double r[], int itrnsp);
EXPORT_CPP_FIMATHRECIPES void avevar(double data[], unsigned long n, double *ave, double *var);
EXPORT_CPP_FIMATHRECIPES void balanc(double **a, int n);
EXPORT_CPP_FIMATHRECIPES void banbks(double **a, unsigned long n, int m1, int m2, double **al,
	unsigned long indx[], double b[]);
EXPORT_CPP_FIMATHRECIPES void bandec(double **a, unsigned long n, int m1, int m2, double **al,
	unsigned long indx[], double *d);
EXPORT_CPP_FIMATHRECIPES void banmul(double **a, unsigned long n, int m1, int m2, double x[], double b[]);
EXPORT_CPP_FIMATHRECIPES void bcucof(double y[], double y1[], double y2[], double y12[], double d1,
	double d2, double **c);
EXPORT_CPP_FIMATHRECIPES void bcuint(double y[], double y1[], double y2[], double y12[],
	double x1l, double x1u, double x2l, double x2u, double x1,
	double x2, double *ansy, double *ansy1, double *ansy2);
EXPORT_CPP_FIMATHRECIPES void beschb(double x, double *gam1, double *gam2, double *gampl,
	double *gammi);
EXPORT_CPP_FIMATHRECIPES double bessi(int n, double x);
EXPORT_CPP_FIMATHRECIPES double bessi0(double x);
EXPORT_CPP_FIMATHRECIPES double bessi1(double x);
EXPORT_CPP_FIMATHRECIPES void bessik(double x, double xnu, double *ri, double *rk, double *rip,
	double *rkp);
EXPORT_CPP_FIMATHRECIPES double bessj(int n, double x);
EXPORT_CPP_FIMATHRECIPES double bessj0(double x);
EXPORT_CPP_FIMATHRECIPES double bessj1(double x);
EXPORT_CPP_FIMATHRECIPES void bessjy(double x, double xnu, double *rj, double *ry, double *rjp,
	double *ryp);
EXPORT_CPP_FIMATHRECIPES double bessk(int n, double x);
EXPORT_CPP_FIMATHRECIPES double bessk0(double x);
EXPORT_CPP_FIMATHRECIPES double bessk1(double x);
EXPORT_CPP_FIMATHRECIPES double bessy(int n, double x);
EXPORT_CPP_FIMATHRECIPES double bessy0(double x);
EXPORT_CPP_FIMATHRECIPES double bessy1(double x);
/* double beta(double z, double w); */
EXPORT_CPP_FIMATHRECIPES double betacf(double a, double b, double x);
EXPORT_CPP_FIMATHRECIPES double betai(double a, double b, double x);
EXPORT_CPP_FIMATHRECIPES double bico(int n, int k);
EXPORT_CPP_FIMATHRECIPES void bksub(int ne, int nb, int jf, int k1, int k2, double ***c);
EXPORT_CPP_FIMATHRECIPES double bnldev(double pp, int n, long *idum);
EXPORT_CPP_FIMATHRECIPES double brent(double ax, double bx, double cx,
	double (*f)(double), double tol, double *xmin);
EXPORT_CPP_FIMATHRECIPES void broydn(double x[], int n, int *check,
	void (*vecfunc)(int, double [], double []));
EXPORT_CPP_FIMATHRECIPES void bsstep(double y[], double dydx[], int nv, double *xx, double htry,
	double eps, double yscal[], double *hdid, double *hnext,
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void caldat(long julian, int *mm, int *id, int *iyyy);
EXPORT_CPP_FIMATHRECIPES void chder(double a, double b, double c[], double cder[], int n);
EXPORT_CPP_FIMATHRECIPES double chebev(double a, double b, double c[], int m, double x);
EXPORT_CPP_FIMATHRECIPES void chebft(double a, double b, double c[], int n, double (*func)(double));
EXPORT_CPP_FIMATHRECIPES void chebpc(double c[], double d[], int n);
EXPORT_CPP_FIMATHRECIPES void chint(double a, double b, double c[], double cint[], int n);
EXPORT_CPP_FIMATHRECIPES double chixy(double bang);
EXPORT_CPP_FIMATHRECIPES void choldc(double **a, int n, double p[]);
EXPORT_CPP_FIMATHRECIPES void cholsl(double **a, int n, double p[], double b[], double x[]);
EXPORT_CPP_FIMATHRECIPES void chsone(double bins[], double ebins[], int nbins, int knstrn,
	double *df, double *chsq, double *prob);
EXPORT_CPP_FIMATHRECIPES void chstwo(double bins1[], double bins2[], int nbins, int knstrn,
	double *df, double *chsq, double *prob);
EXPORT_CPP_FIMATHRECIPES void cisi(double x, double *ci, double *si); // See new function SineCosineIntegral in num/n_special.c
EXPORT_CPP_FIMATHRECIPES void cntab1(int **nn, int ni, int nj, double *chisq,
	double *df, double *prob, double *cramrv, double *ccc);
EXPORT_CPP_FIMATHRECIPES void cntab2(int **nn, int ni, int nj, double *h, double *hx, double *hy,
	double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy);
EXPORT_CPP_FIMATHRECIPES void convlv(double data[], unsigned long n, double respns[], unsigned long m,
	int isign, double ans[]);
EXPORT_CPP_FIMATHRECIPES void sq_copy(double **aout, double **ain, int n);
EXPORT_CPP_FIMATHRECIPES void correl(double data1[], double data2[], unsigned long n, double ans[]);
EXPORT_CPP_FIMATHRECIPES void cosft(double y[], int n, int isign);
EXPORT_CPP_FIMATHRECIPES void cosft1(double y[], int n);
EXPORT_CPP_FIMATHRECIPES void cosft2(double y[], int n, int isign);
EXPORT_CPP_FIMATHRECIPES void covsrt(double **covar, int ma, int ia[], int mfit);
EXPORT_CPP_FIMATHRECIPES void crank(unsigned long n, double w[], double *s);
EXPORT_CPP_FIMATHRECIPES void cyclic(double a[], double b[], double c[], double alpha, double beta,
	double r[], double x[], unsigned long n);
EXPORT_CPP_FIMATHRECIPES void daub4(double a[], unsigned long n, int isign);
EXPORT_CPP_FIMATHRECIPES double dawson(double x);
EXPORT_CPP_FIMATHRECIPES double dbrent(double ax, double bx, double cx,
	double (*f)(double), double (*df)(double), double tol, double *xmin);
EXPORT_CPP_FIMATHRECIPES void ddpoly(double c[], int nc, double x, double pd[], int nd);
EXPORT_CPP_FIMATHRECIPES int decchk(char string[], int n, char *ch);
EXPORT_CPP_FIMATHRECIPES void derivs(double x, double y[], double dydx[]);
EXPORT_CPP_FIMATHRECIPES double df1dim(double x);
EXPORT_CPP_FIMATHRECIPES void dfour1(double data[], unsigned long nn, int isign);
EXPORT_CPP_FIMATHRECIPES void dfpmin(double p[], int n, double gtol, int *iter, double *fret,
	double (*func)(double []), void (*dfunc)(double [], double []));
EXPORT_CPP_FIMATHRECIPES double dfridr(double (*func)(double), double x, double h, double *err);
EXPORT_CPP_FIMATHRECIPES void dftcor(double w, double delta, double a, double b, double endpts[],
	double *corre, double *corim, double *corfac);
EXPORT_CPP_FIMATHRECIPES void dftint(double (*func)(double), double a, double b, double w,
	double *cosint, double *sinint);
EXPORT_CPP_FIMATHRECIPES void difeq(int k, int k1, int k2, int jsf, int is1, int isf,
	int indexv[], int ne, double **s, double **y);
EXPORT_CPP_FIMATHRECIPES void dlinmin(double p[], double xi[], int n, double *fret,
	double (*func)(double []), void (*dfunc)(double [], double[]));
EXPORT_CPP_FIMATHRECIPES double dpythag(double a, double b);
EXPORT_CPP_FIMATHRECIPES void drealft(double data[], unsigned long n, int isign);
EXPORT_CPP_FIMATHRECIPES void dsprsax(double sa[], unsigned long ija[], double x[], double b[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES void dsprstx(double sa[], unsigned long ija[], double x[], double b[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES void dsvbksb(double **u, double w[], double **v, int m, int n, double b[],
	double x[]);
EXPORT_CPP_FIMATHRECIPES void dsvdcmp(double **a, int m, int n, double w[], double **v);
EXPORT_CPP_FIMATHRECIPES void eclass(int nf[], int n, int lista[], int listb[], int m);
EXPORT_CPP_FIMATHRECIPES void eclazz(int nf[], int n, int (*equiv)(int, int));
EXPORT_CPP_FIMATHRECIPES double ei(double x);
EXPORT_CPP_FIMATHRECIPES void eigsrt(double d[], double **v, int n);
EXPORT_CPP_FIMATHRECIPES double elle(double phi, double ak);
EXPORT_CPP_FIMATHRECIPES double ellf(double phi, double ak);
EXPORT_CPP_FIMATHRECIPES double ellpi(double phi, double en, double ak);
EXPORT_CPP_FIMATHRECIPES void elmhes(double **a, int n);
EXPORT_CPP_FIMATHRECIPES double erfcc(double x);
/* double erff(double x); */
EXPORT_CPP_FIMATHRECIPES double erffc(double x);
EXPORT_CPP_FIMATHRECIPES void eulsum(double *sum, double term, int jterm, double wksp[]);
EXPORT_CPP_FIMATHRECIPES double evlmem(double fdt, double d[], int m, double xms);
EXPORT_CPP_FIMATHRECIPES double expdev(long *idum);
EXPORT_CPP_FIMATHRECIPES double expint(int n, double x);
EXPORT_CPP_FIMATHRECIPES double f1(double x);
EXPORT_CPP_FIMATHRECIPES double f1dim(double x);
EXPORT_CPP_FIMATHRECIPES double f2(double y);
EXPORT_CPP_FIMATHRECIPES double f3(double z);
EXPORT_CPP_FIMATHRECIPES double factln(int n);
EXPORT_CPP_FIMATHRECIPES double factrl(int n);
EXPORT_CPP_FIMATHRECIPES void fasper(double x[], double y[], unsigned long n, double ofac, double hifac,
	double wk1[], double wk2[], unsigned long nwk, unsigned long *nout,
	unsigned long *jmax, double *prob);
EXPORT_CPP_FIMATHRECIPES void fdjac(int n, double x[], double fvec[], double **df,
	void (*vecfunc)(int, double [], double []));
EXPORT_CPP_FIMATHRECIPES void fgauss(double x, double a[], double *y, double dyda[], int na);
EXPORT_CPP_FIMATHRECIPES void fill0(double **u, int n);
EXPORT_CPP_FIMATHRECIPES void fit(double x[], double y[], int ndata, double sig[], int mwt,
	double *a, double *b, double *siga, double *sigb, double *chi2, double *q);
EXPORT_CPP_FIMATHRECIPES void fitexy(double x[], double y[], int ndat, double sigx[], double sigy[],
	double *a, double *b, double *siga, double *sigb, double *chi2, double *q);
EXPORT_CPP_FIMATHRECIPES void fixrts(double d[], int m);
EXPORT_CPP_FIMATHRECIPES void fleg(double x, double pl[], int nl);
EXPORT_CPP_FIMATHRECIPES void flmoon(int n, int nph, long *jd, double *frac);
EXPORT_CPP_FIMATHRECIPES double fmin(double x[]);
EXPORT_CPP_FIMATHRECIPES void four1(double data[], unsigned long nn, int isign);
EXPORT_CPP_FIMATHRECIPES void fourew(FILE *file[5], int *na, int *nb, int *nc, int *nd);
EXPORT_CPP_FIMATHRECIPES void fourfs(FILE *file[5], unsigned long nn[], int ndim, int isign);
EXPORT_CPP_FIMATHRECIPES void fourn(double data[], unsigned long nn[], int ndim, int isign);
EXPORT_CPP_FIMATHRECIPES void fpoly(double x, double p[], int np);
EXPORT_CPP_FIMATHRECIPES void fred2(int n, double a, double b, double t[], double f[], double w[],
	double (*g)(double), double (*ak)(double, double));
EXPORT_CPP_FIMATHRECIPES double fredin(double x, int n, double a, double b, double t[], double f[], double w[],
	double (*g)(double), double (*ak)(double, double));
EXPORT_CPP_FIMATHRECIPES void frenel(double x, double *s, double *c);
EXPORT_CPP_FIMATHRECIPES void frprmn(double p[], int n, double ftol, int *iter, double *fret,
	double (*func)(double []), void (*dfunc)(double [], double []));
EXPORT_CPP_FIMATHRECIPES void ftest(double data1[], unsigned long n1, double data2[], unsigned long n2,
	double *f, double *prob);
EXPORT_CPP_FIMATHRECIPES double gamdev(int ia, long *idum);
EXPORT_CPP_FIMATHRECIPES double gammln(double xx);
EXPORT_CPP_FIMATHRECIPES double gammp(double a, double x);
EXPORT_CPP_FIMATHRECIPES double gammq(double a, double x);
EXPORT_CPP_FIMATHRECIPES double gasdev(long *idum);
EXPORT_CPP_FIMATHRECIPES void gaucof(int n, double a[], double b[], double amu0, double x[], double w[]);
EXPORT_CPP_FIMATHRECIPES void gauher(double x[], double w[], int n);
EXPORT_CPP_FIMATHRECIPES void gaujac(double x[], double w[], int n, double alf, double bet);
EXPORT_CPP_FIMATHRECIPES void gaulag(double x[], double w[], int n, double alf);
EXPORT_CPP_FIMATHRECIPES void gauleg(double x1, double x2, double x[], double w[], int n);
EXPORT_CPP_FIMATHRECIPES void gaussj(double **a, int n, double **b, int m);
EXPORT_CPP_FIMATHRECIPES void gcf(double *gammcf, double a, double x, double *gln);
EXPORT_CPP_FIMATHRECIPES double golden(double ax, double bx, double cx, double (*f)(double), double tol,
	double *xmin);
EXPORT_CPP_FIMATHRECIPES void gser(double *gamser, double a, double x, double *gln);
EXPORT_CPP_FIMATHRECIPES void hpsel(unsigned long m, unsigned long n, double arr[], double heap[]);
EXPORT_CPP_FIMATHRECIPES void hpsort(unsigned long n, double ra[]);
EXPORT_CPP_FIMATHRECIPES void hqr(double **a, int n, double wr[], double wi[]);
EXPORT_CPP_FIMATHRECIPES void hufapp(unsigned long index[], unsigned long nprob[], unsigned long n,
	unsigned long i);
EXPORT_CPP_FIMATHRECIPES void hufdec(unsigned long *ich, unsigned char *code, unsigned long lcode,
	unsigned long *nb, huffcode *hcode);
EXPORT_CPP_FIMATHRECIPES void hufenc(unsigned long ich, unsigned char **codep, unsigned long *lcode,
	unsigned long *nb, huffcode *hcode);
EXPORT_CPP_FIMATHRECIPES void hufmak(unsigned long nfreq[], unsigned long nchin, unsigned long *ilong,
	unsigned long *nlong, huffcode *hcode);
EXPORT_CPP_FIMATHRECIPES void hunt(double xx[], unsigned long n, double x, unsigned long *jlo);
EXPORT_CPP_FIMATHRECIPES void hypdrv(double s, double yy[], double dyyds[]);
EXPORT_CPP_FIMATHRECIPES fcomplex hypgeo(fcomplex a, fcomplex b, fcomplex c, fcomplex z);
EXPORT_CPP_FIMATHRECIPES void hypser(fcomplex a, fcomplex b, fcomplex c, fcomplex z,
	fcomplex *series, fcomplex *deriv);
EXPORT_CPP_FIMATHRECIPES unsigned short icrc(unsigned short crc, unsigned char *bufptr,
	unsigned long len, short jinit, int jrev);
EXPORT_CPP_FIMATHRECIPES unsigned short icrc1(unsigned short crc, unsigned char onech);
EXPORT_CPP_FIMATHRECIPES unsigned long igray(unsigned long n, int is);
EXPORT_CPP_FIMATHRECIPES void iindexx(unsigned long n, long arr[], unsigned long indx[]);
EXPORT_CPP_FIMATHRECIPES void indexx(unsigned long n, double arr[], unsigned long indx[]);
EXPORT_CPP_FIMATHRECIPES void interp(double **uf, double **uc, int nf);
EXPORT_CPP_FIMATHRECIPES int irbit1(unsigned long *iseed);
EXPORT_CPP_FIMATHRECIPES int irbit2(unsigned long *iseed);
EXPORT_CPP_FIMATHRECIPES void jacobi(double **a, int n, double d[], double **v, int *nrot);
EXPORT_CPP_FIMATHRECIPES void jacobn(double x, double y[], double dfdx[], double **dfdy, int n);
EXPORT_CPP_FIMATHRECIPES long julday(int mm, int id, int iyyy);
EXPORT_CPP_FIMATHRECIPES void kendl1(double data1[], double data2[], unsigned long n, double *tau, double *z,
	double *prob);
EXPORT_CPP_FIMATHRECIPES void kendl2(double **tab, int i, int j, double *tau, double *z, double *prob);
EXPORT_CPP_FIMATHRECIPES void kermom(double w[], double y, int m);
EXPORT_CPP_FIMATHRECIPES void ks2d1s(double x1[], double y1[], unsigned long n1,
	void (*quadvl)(double, double, double *, double *, double *, double *),
	double *d1, double *prob);
EXPORT_CPP_FIMATHRECIPES void ks2d2s(double x1[], double y1[], unsigned long n1, double x2[], double y2[],
	unsigned long n2, double *d, double *prob);
EXPORT_CPP_FIMATHRECIPES void ksone(double data[], unsigned long n, double (*func)(double), double *d,
	double *prob);
EXPORT_CPP_FIMATHRECIPES void kstwo(double data1[], unsigned long n1, double data2[], unsigned long n2,
	double *d, double *prob);
EXPORT_CPP_FIMATHRECIPES void laguer(fcomplex a[], int m, fcomplex *x, int *its);
EXPORT_CPP_FIMATHRECIPES void lfit(double x[], double y[], double sig[], int ndat, double a[], int ia[],
	int ma, double **covar, double *chisq, void (*funcs)(double, double [], int));
EXPORT_CPP_FIMATHRECIPES void linbcg(unsigned long n, double b[], double x[], int itol, double tol,
	 int itmax, int *iter, double *err);
EXPORT_CPP_FIMATHRECIPES void linmin(double p[], double xi[], int n, double *fret,
	double (*func)(double []));
EXPORT_CPP_FIMATHRECIPES void lnsrch(int n, double xold[], double fold, double g[], double p[], double x[],
	 double *f, double stpmax, int *check, double (*func)(double []));
EXPORT_CPP_FIMATHRECIPES void load(double x1, double v[], double y[]);
EXPORT_CPP_FIMATHRECIPES void load1(double x1, double v1[], double y[]);
EXPORT_CPP_FIMATHRECIPES void load2(double x2, double v2[], double y[]);
EXPORT_CPP_FIMATHRECIPES void locate(double xx[], unsigned long n, double x, unsigned long *j);
EXPORT_CPP_FIMATHRECIPES void lop(double **out, double **u, int n);
EXPORT_CPP_FIMATHRECIPES void lubksb(double **a, int n, int *indx, double b[]);
EXPORT_CPP_FIMATHRECIPES void ludcmp(double **a, int n, int *indx, double *d);
EXPORT_CPP_FIMATHRECIPES void machar(int *ibeta, int *it, int *irnd, int *ngrd,
	int *machep, int *negep, int *iexp, int *minexp, int *maxexp,
	double *eps, double *epsneg, double *xmin, double *xmax);
EXPORT_CPP_FIMATHRECIPES void matadd(double **a, double **b, double **c, int n);
EXPORT_CPP_FIMATHRECIPES void matsub(double **a, double **b, double **c, int n);
EXPORT_CPP_FIMATHRECIPES void medfit(double x[], double y[], int ndata, double *a, double *b, double *abdev);
EXPORT_CPP_FIMATHRECIPES void memcof(double data[], int n, int m, double *xms, double d[]);
EXPORT_CPP_FIMATHRECIPES int metrop(double de, double t);
EXPORT_CPP_FIMATHRECIPES void mgfas(double **u, int n, int maxcyc);
EXPORT_CPP_FIMATHRECIPES void mglin(double **u, int n, int ncycle);
EXPORT_CPP_FIMATHRECIPES double midexp(double (*funk)(double), double aa, double bb, int n);
EXPORT_CPP_FIMATHRECIPES double midinf(double (*funk)(double), double aa, double bb, int n);
EXPORT_CPP_FIMATHRECIPES double midpnt(double (*func)(double), double a, double b, int n);
EXPORT_CPP_FIMATHRECIPES double midsql(double (*funk)(double), double aa, double bb, int n);
EXPORT_CPP_FIMATHRECIPES double midsqu(double (*funk)(double), double aa, double bb, int n);
EXPORT_CPP_FIMATHRECIPES void miser(double (*func)(double []), double regn[], int ndim, unsigned long npts,
	double dith, double *ave, double *var);
EXPORT_CPP_FIMATHRECIPES void mmid(double y[], double dydx[], int nvar, double xs, double htot,
	int nstep, double yout[], void (*derivs)(double, double[], double[]));
EXPORT_CPP_FIMATHRECIPES void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb,
	double *fc, double (*func)(double));
EXPORT_CPP_FIMATHRECIPES void mnewt(int ntrial, double x[], int n, double tolx, double tolf);
EXPORT_CPP_FIMATHRECIPES void moment(double data[], int n, double *ave, double *adev, double *sdev,
	double *var, double *skew, double *curt);
EXPORT_CPP_FIMATHRECIPES void mp2dfr(unsigned char a[], unsigned char s[], int n, int *m);
EXPORT_CPP_FIMATHRECIPES void mpadd(unsigned char w[], unsigned char u[], unsigned char v[], int n);
EXPORT_CPP_FIMATHRECIPES void mpdiv(unsigned char q[], unsigned char r[], unsigned char u[],
	unsigned char v[], int n, int m);
EXPORT_CPP_FIMATHRECIPES void mpinv(unsigned char u[], unsigned char v[], int n, int m);
EXPORT_CPP_FIMATHRECIPES void mplsh(unsigned char u[], int n);
EXPORT_CPP_FIMATHRECIPES void mpmov(unsigned char u[], unsigned char v[], int n);
EXPORT_CPP_FIMATHRECIPES void mpmul(unsigned char w[], unsigned char u[], unsigned char v[], int n,
	int m);
EXPORT_CPP_FIMATHRECIPES void mpneg(unsigned char u[], int n);
EXPORT_CPP_FIMATHRECIPES void mppi(int n);
EXPORT_CPP_FIMATHRECIPES void mprove(double **a, double **alud, int n, int indx[], double b[],
	double x[]);
EXPORT_CPP_FIMATHRECIPES void mpsad(unsigned char w[], unsigned char u[], int n, int iv);
EXPORT_CPP_FIMATHRECIPES void mpsdv(unsigned char w[], unsigned char u[], int n, int iv, int *ir);
EXPORT_CPP_FIMATHRECIPES void mpsmu(unsigned char w[], unsigned char u[], int n, int iv);
EXPORT_CPP_FIMATHRECIPES void mpsqrt(unsigned char w[], unsigned char u[], unsigned char v[], int n,
	int m);
EXPORT_CPP_FIMATHRECIPES void mpsub(int *is, unsigned char w[], unsigned char u[], unsigned char v[],
	int n);
EXPORT_CPP_FIMATHRECIPES void mrqcof(double x[], double y[], double sig[], int ndata, double a[],
	int ia[], int ma, double **alpha, double beta[], double *chisq,
	void (*funcs)(double, double [], double *, double [], int));
EXPORT_CPP_FIMATHRECIPES void mrqmin(double x[], double y[], double sig[], int ndata, double a[],
	int ia[], int ma, double **covar, double **alpha, double *chisq,
	void (*funcs)(double, double [], double *, double [], int), double *alamda);
EXPORT_CPP_FIMATHRECIPES void newt(double x[], int n, int *check,
	void (*vecfunc)(int, double [], double []));
EXPORT_CPP_FIMATHRECIPES void odeint(double ystart[], int nvar, double x1, double x2,
	double eps, double h1, double hmin, int *nok, int *nbad,
	void (*derivs)(double, double [], double []),
	void (*rkqs)(double [], double [], int, double *, double, double,
	double [], double *, double *, void (*)(double, double [], double [])));
EXPORT_CPP_FIMATHRECIPES void orthog(int n, double anu[], double alpha[], double beta[], double a[],
	double b[]);
EXPORT_CPP_FIMATHRECIPES void pade(double cof[], int n, double *resid);
EXPORT_CPP_FIMATHRECIPES void pccheb(double d[], double c[], int n);
EXPORT_CPP_FIMATHRECIPES void pcshft(double a, double b, double d[], int n);
EXPORT_CPP_FIMATHRECIPES void pearsn(double x[], double y[], unsigned long n, double *r, double *prob,
	double *z);
EXPORT_CPP_FIMATHRECIPES void period(double x[], double y[], int n, double ofac, double hifac,
	double px[], double py[], int np, int *nout, int *jmax, double *prob);
EXPORT_CPP_FIMATHRECIPES void piksr2(int n, double arr[], double brr[]);
EXPORT_CPP_FIMATHRECIPES void piksrt(int n, double arr[]);
EXPORT_CPP_FIMATHRECIPES void pinvs(int ie1, int ie2, int je1, int jsf, int jc1, int k,
	double ***c, double **s);
EXPORT_CPP_FIMATHRECIPES double plgndr(int l, int m, double x);
EXPORT_CPP_FIMATHRECIPES double poidev(double xm, long *idum);
EXPORT_CPP_FIMATHRECIPES void polcoe(double x[], double y[], int n, double cof[]);
EXPORT_CPP_FIMATHRECIPES void polcof(double xa[], double ya[], int n, double cof[]);
EXPORT_CPP_FIMATHRECIPES void poldiv(double u[], int n, double v[], int nv, double q[], double r[]);
EXPORT_CPP_FIMATHRECIPES void polin2(double x1a[], double x2a[], double **ya, int m, int n,
	double x1, double x2, double *y, double *dy);
EXPORT_CPP_FIMATHRECIPES void polint(double xa[], double ya[], int n, double x, double *y, double *dy);
EXPORT_CPP_FIMATHRECIPES void powell(double p[], double **xi, int n, double ftol, int *iter, double *fret,
	double (*func)(double []));
EXPORT_CPP_FIMATHRECIPES void predic(double data[], int ndata, double d[], int m, double future[], int nfut);
EXPORT_CPP_FIMATHRECIPES double probks(double alam);
EXPORT_CPP_FIMATHRECIPES void psdes(unsigned long *lword, unsigned long *irword);
EXPORT_CPP_FIMATHRECIPES void pwt(double a[], unsigned long n, int isign);
EXPORT_CPP_FIMATHRECIPES void pwtset(int n);
EXPORT_CPP_FIMATHRECIPES double pythag(double a, double b);
EXPORT_CPP_FIMATHRECIPES void pzextr(int iest, double xest, double yest[], double yz[], double dy[],
	int nv);
EXPORT_CPP_FIMATHRECIPES double qgaus(double (*func)(double), double a, double b);
EXPORT_CPP_FIMATHRECIPES void qrdcmp(double **a, int n, double *c, double *d, int *sing);
EXPORT_CPP_FIMATHRECIPES double qromb(double (*func)(double), double a, double b);
EXPORT_CPP_FIMATHRECIPES double qromo(double (*func)(double), double a, double b,
	double (*choose)(double (*)(double), double, double, int));
EXPORT_CPP_FIMATHRECIPES void qroot(double p[], int n, double *b, double *c, double eps);
EXPORT_CPP_FIMATHRECIPES void qrsolv(double **a, int n, double c[], double d[], double b[]);
EXPORT_CPP_FIMATHRECIPES void qrupdt(double **r, double **qt, int n, double u[], double v[]);
EXPORT_CPP_FIMATHRECIPES double qsimp(double (*func)(double), double a, double b);
EXPORT_CPP_FIMATHRECIPES double qtrap(double (*func)(double), double a, double b);
/* double quad3d(double (*func)(double, double, double), double x1, double x2); */
EXPORT_CPP_FIMATHRECIPES void quadct(double x, double y, double xx[], double yy[], unsigned long nn,
	double *fa, double *fb, double *fc, double *fd);
EXPORT_CPP_FIMATHRECIPES void quadmx(double **a, int n);
EXPORT_CPP_FIMATHRECIPES void quadvl(double x, double y, double *fa, double *fb, double *fc, double *fd);
EXPORT_CPP_FIMATHRECIPES double ran0(long *idum);
EXPORT_CPP_FIMATHRECIPES double ran1(long *idum);
EXPORT_CPP_FIMATHRECIPES double ran2(long *idum);
EXPORT_CPP_FIMATHRECIPES double ran3(long *idum);
EXPORT_CPP_FIMATHRECIPES double ran4(long *idum);
EXPORT_CPP_FIMATHRECIPES void rank(unsigned long n, unsigned long indx[], unsigned long irank[]);
EXPORT_CPP_FIMATHRECIPES void ranpt(double pt[], double regn[], int n);
EXPORT_CPP_FIMATHRECIPES void ratint(double xa[], double ya[], int n, double x, double *y, double *dy);
EXPORT_CPP_FIMATHRECIPES void ratlsq(double (*fn)(double), double a, double b, int mm, int kk,
	double cof[], double *dev);
EXPORT_CPP_FIMATHRECIPES double ratval(double x, double cof[], int mm, int kk);
EXPORT_CPP_FIMATHRECIPES double rc(double x, double y);
EXPORT_CPP_FIMATHRECIPES double rd(double x, double y, double z);
EXPORT_CPP_FIMATHRECIPES void realft(double data[], unsigned long n, int isign);
EXPORT_CPP_FIMATHRECIPES void rebin(double rc, int nd, double r[], double xin[], double xi[]);
EXPORT_CPP_FIMATHRECIPES void red(int iz1, int iz2, int jz1, int jz2, int jm1, int jm2, int jmf,
	int ic1, int jc1, int jcf, int kc, double ***c, double **s);
EXPORT_CPP_FIMATHRECIPES void relax(double **u, double **rhs, int n);
EXPORT_CPP_FIMATHRECIPES void relax2(double **u, double **rhs, int n);
EXPORT_CPP_FIMATHRECIPES void resid(double **res, double **u, double **rhs, int n);
EXPORT_CPP_FIMATHRECIPES double revcst(double x[], double y[], int iorder[], int ncity, int n[]);
EXPORT_CPP_FIMATHRECIPES void reverse(int iorder[], int ncity, int n[]);
EXPORT_CPP_FIMATHRECIPES double rf(double x, double y, double z);
EXPORT_CPP_FIMATHRECIPES double rj(double x, double y, double z, double p);
EXPORT_CPP_FIMATHRECIPES void rk4(double y[], double dydx[], int n, double x, double h, double yout[],
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void rkck(double y[], double dydx[], int n, double x, double h,
	double yout[], double yerr[], void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void rkdumb(double vstart[], int nvar, double x1, double x2, int nstep,
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void rkqs(double y[], double dydx[], int n, double *x,
	double htry, double eps, double yscal[], double *hdid, double *hnext,
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void rlft3(double ***data, double **speq, unsigned long nn1,
	unsigned long nn2, unsigned long nn3, int isign);
EXPORT_CPP_FIMATHRECIPES double rofunc(double b);
EXPORT_CPP_FIMATHRECIPES void rotate(double **r, double **qt, int n, int i, double a, double b);
EXPORT_CPP_FIMATHRECIPES void rsolv(double **a, int n, double d[], double b[]);
EXPORT_CPP_FIMATHRECIPES void rstrct(double **uc, double **uf, int nc);
EXPORT_CPP_FIMATHRECIPES double rtbis(double (*func)(double), double x1, double x2, double xacc);
EXPORT_CPP_FIMATHRECIPES double rtflsp(double (*func)(double), double x1, double x2, double xacc);
EXPORT_CPP_FIMATHRECIPES double rtnewt(void (*funcd)(double, double *, double *), double x1, double x2,
	double xacc);
EXPORT_CPP_FIMATHRECIPES double rtsafe(void (*funcd)(double, double *, double *), double x1, double x2,
	double xacc);
EXPORT_CPP_FIMATHRECIPES double rtsec(double (*func)(double), double x1, double x2, double xacc);
EXPORT_CPP_FIMATHRECIPES void rzextr(int iest, double xest, double yest[], double yz[], double dy[], int nv);
EXPORT_CPP_FIMATHRECIPES void savgol(double c[], int np, int nl, int nr, int ld, int m);
EXPORT_CPP_FIMATHRECIPES void score(double xf, double y[], double f[]);
EXPORT_CPP_FIMATHRECIPES void scrsho(double (*fx)(double));
EXPORT_CPP_FIMATHRECIPES double rec_select(unsigned long k, unsigned long n, double arr[]); 
EXPORT_CPP_FIMATHRECIPES double selip(unsigned long k, unsigned long n, double arr[]);
EXPORT_CPP_FIMATHRECIPES void shell(unsigned long n, double a[]);
EXPORT_CPP_FIMATHRECIPES void shoot(int n, double v[], double f[]);
EXPORT_CPP_FIMATHRECIPES void shootf(int n, double v[], double f[]);
EXPORT_CPP_FIMATHRECIPES void simp1(double **a, int mm, int ll[], int nll, int iabf, int *kp,
	double *bmax);
EXPORT_CPP_FIMATHRECIPES void simp2(double **a, int n, int l2[], int nl2, int *ip, int kp, double *q1);
EXPORT_CPP_FIMATHRECIPES void simp3(double **a, int i1, int k1, int ip, int kp);
EXPORT_CPP_FIMATHRECIPES void simplx(double **a, int m, int n, int m1, int m2, int m3, int *icase,
	int izrov[], int iposv[]);
EXPORT_CPP_FIMATHRECIPES void simpr(double y[], double dydx[], double dfdx[], double **dfdy,
	int n, double xs, double htot, int nstep, double yout[],
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void sinft(double y[], int n);
EXPORT_CPP_FIMATHRECIPES void slvsm2(double **u, double **rhs);
EXPORT_CPP_FIMATHRECIPES void slvsml(double **u, double **rhs);
EXPORT_CPP_FIMATHRECIPES void sncndn(double uu, double emmc, double *sn, double *cn, double *dn);
EXPORT_CPP_FIMATHRECIPES double snrm(unsigned long n, double sx[], int itol);
EXPORT_CPP_FIMATHRECIPES void sobseq(int *n, double x[]);
EXPORT_CPP_FIMATHRECIPES void solvde(int itmax, double conv, double slowc, double scalv[],
	int indexv[], int ne, int nb, int m, double **y, double ***c, double **s);
EXPORT_CPP_FIMATHRECIPES void sor(double **a, double **b, double **c, double **d, double **e,
	double **f, double **u, int jmax, double rjac);
EXPORT_CPP_FIMATHRECIPES void sort(unsigned long n, double arr[]);
EXPORT_CPP_FIMATHRECIPES void sort2(unsigned long n, double arr[], double brr[]);
EXPORT_CPP_FIMATHRECIPES void sort3(unsigned long n, double ra[], double rb[], double rc[]);
EXPORT_CPP_FIMATHRECIPES void spctrm(FILE *fp, double p[], int m, int k, int ovrlap);
EXPORT_CPP_FIMATHRECIPES void spear(double data1[], double data2[], unsigned long n, double *d, double *zd,
	double *probd, double *rs, double *probrs);
EXPORT_CPP_FIMATHRECIPES void sphbes(int n, double x, double *sj, double *sy, double *sjp, double *syp);
EXPORT_CPP_FIMATHRECIPES void splie2(double x1a[], double x2a[], double **ya, int m, int n, double **y2a);
EXPORT_CPP_FIMATHRECIPES void splin2(double x1a[], double x2a[], double **ya, double **y2a, int m, int n,
	double x1, double x2, double *y);
EXPORT_CPP_FIMATHRECIPES void spline(double x[], double y[], int n, double yp1, double ypn, double y2[]);
EXPORT_CPP_FIMATHRECIPES void splint(double xa[], double ya[], double y2a[], int n, double x, double *y);
EXPORT_CPP_FIMATHRECIPES void spread(double y, double yy[], unsigned long n, double x, int m);
EXPORT_CPP_FIMATHRECIPES void sprsax(double sa[], unsigned long ija[], double x[], double b[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES void sprsin(double **a, int n, double thresh, unsigned long nmax, double sa[],
	unsigned long ija[]);
EXPORT_CPP_FIMATHRECIPES void sprspm(double sa[], unsigned long ija[], double sb[], unsigned long ijb[],
	double sc[], unsigned long ijc[]);
EXPORT_CPP_FIMATHRECIPES void sprstm(double sa[], unsigned long ija[], double sb[], unsigned long ijb[],
	double thresh, unsigned long nmax, double sc[], unsigned long ijc[]);
EXPORT_CPP_FIMATHRECIPES void sprstp(double sa[], unsigned long ija[], double sb[], unsigned long ijb[]);
EXPORT_CPP_FIMATHRECIPES void sprstx(double sa[], unsigned long ija[], double x[], double b[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES void stifbs(double y[], double dydx[], int nv, double *xx,
	double htry, double eps, double yscal[], double *hdid, double *hnext,
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void stiff(double y[], double dydx[], int n, double *x,
	double htry, double eps, double yscal[], double *hdid, double *hnext,
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void stoerm(double y[], double d2y[], int nv, double xs,
	double htot, int nstep, double yout[],
	void (*derivs)(double, double [], double []));
EXPORT_CPP_FIMATHRECIPES void svbksb(double **u, double w[], double **v, int m, int n, double b[],
	double x[]);
EXPORT_CPP_FIMATHRECIPES void svdcmp(double **a, int m, int n, double w[], double **v);
EXPORT_CPP_FIMATHRECIPES void svdfit(double x[], double y[], double sig[], int ndata, double a[],
	int ma, double **u, double **v, double w[], double *chisq,
	void (*funcs)(double, double [], int));
EXPORT_CPP_FIMATHRECIPES void svdvar(double **v, int ma, double w[], double **cvm);
EXPORT_CPP_FIMATHRECIPES void toeplz(double r[], double x[], double y[], int n);
EXPORT_CPP_FIMATHRECIPES void tptest(double data1[], double data2[], unsigned long n, double *t, double *prob);
EXPORT_CPP_FIMATHRECIPES void tqli(double d[], double e[], int n, double **z);
EXPORT_CPP_FIMATHRECIPES double trapzd(double (*func)(double), double a, double b, int n);
EXPORT_CPP_FIMATHRECIPES void tred2(double **a, int n, double d[], double e[]);
EXPORT_CPP_FIMATHRECIPES void tridag(double a[], double b[], double c[], double r[], double u[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES double trncst(double x[], double y[], int iorder[], int ncity, int n[]);
EXPORT_CPP_FIMATHRECIPES void trnspt(int iorder[], int ncity, int n[]);
EXPORT_CPP_FIMATHRECIPES void ttest(double data1[], unsigned long n1, double data2[], unsigned long n2,
	double *t, double *prob);
EXPORT_CPP_FIMATHRECIPES void tutest(double data1[], unsigned long n1, double data2[], unsigned long n2,
	double *t, double *prob);
EXPORT_CPP_FIMATHRECIPES void twofft(double data1[], double data2[], double fft1[], double fft2[],
	unsigned long n);
EXPORT_CPP_FIMATHRECIPES void vander(double x[], double w[], double q[], int n);
EXPORT_CPP_FIMATHRECIPES void vegas(double regn[], int ndim, double (*fxn)(double [], double), int init,
	unsigned long ncall, int itmx, int nprn, double *tgral, double *sd,
	double *chi2a);
EXPORT_CPP_FIMATHRECIPES void voltra(int n, int m, double t0, double h, double *t, double **f,
	double (*g)(int, double), double (*ak)(int, int, double, double));
EXPORT_CPP_FIMATHRECIPES void wt1(double a[], unsigned long n, int isign,
	void (*wtstep)(double [], unsigned long, int));
EXPORT_CPP_FIMATHRECIPES void wtn(double a[], unsigned long nn[], int ndim, int isign,
	void (*wtstep)(double [], unsigned long, int));
EXPORT_CPP_FIMATHRECIPES void wwghts(double wghts[], int n, double h,
	void (*kermom)(double [], double ,int));
EXPORT_CPP_FIMATHRECIPES int zbrac(double (*func)(double), double *x1, double *x2);
EXPORT_CPP_FIMATHRECIPES void zbrak(double (*fx)(double), double x1, double x2, int n, double xb1[],
	double xb2[], int *nb);
EXPORT_CPP_FIMATHRECIPES double zbrent(double (*func)(double), double x1, double x2, double tol);
EXPORT_CPP_FIMATHRECIPES void zrhqr(double a[], int m, double rtr[], double rti[]);
EXPORT_CPP_FIMATHRECIPES double zriddr(double (*func)(double), double x1, double x2, double xacc);
EXPORT_CPP_FIMATHRECIPES void zroots(fcomplex a[], int m, fcomplex roots[], int polish);
#endif /* _NR_H_ */
