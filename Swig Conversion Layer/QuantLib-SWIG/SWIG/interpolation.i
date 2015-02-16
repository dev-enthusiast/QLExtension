
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2002, 2003 Ferdinando Ametrano
 Copyright (C) 2003, 2004, 2008 StatPro Italia srl
 Copyright (C) 2005 Dominic Thuillier

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef quantlib_interpolation_i
#define quantlib_interpolation_i

%include linearalgebra.i
%include optimizers.i

%{
using QuantLib::Extrapolator;
%}

%ignore Extrapolator;
class Extrapolator {
    #if defined(SWIGRUBY)
    %rename("enableExtrapolation!")  enableExtrapolation;
    %rename("disableExtrapolation!") disableExtrapolation;
    %rename("allowsExtrapolation?")  allowsExtrapolation;
    #elif defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("enable-extrapolation")  enableExtrapolation;
    %rename("disable-extrapolation") disableExtrapolation;
    %rename("allows-extrapolation")  allowsExtrapolation;
    #endif
  public:
    void enableExtrapolation();
    void disableExtrapolation();
    bool allowsExtrapolation();
};

// interpolations

%{
// safe versions which copy their arguments
template <class I>
class SafeInterpolation {
  public:
    SafeInterpolation(const Array& x, const Array& y)
    : x_(x), y_(y), f_(x_.begin(),x_.end(),y_.begin()) {}
    Real operator()(Real x, bool allowExtrapolation=false) {
        return f_(x, allowExtrapolation);
    }
    Array x_, y_;
    I f_;
};
%}

%define make_safe_interpolation(T,Alias)
%{
typedef SafeInterpolation<QuantLib::T> Safe##T;
%}
%rename(Alias) Safe##T;
class Safe##T {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE) \
     || defined(SWIGCSHARP) || defined(SWIGPERL)
    %rename(call) operator();
    #endif
  public:
    Safe##T(const Array& x, const Array& y);
    Real operator()(Real x, bool allowExtrapolation=false);
};
%enddef

%{
// sabr
// safe versions which copy their arguments
class SafeInterpolation_sabr {
  public:
    SafeInterpolation_sabr(const Array& x, const Array& y, const double expiry, const double forward, 
		const double alpha, const double beta, const double volvol, const double rho,
		EndCriteria& endCriteria, OptimizationMethod& optimizationMethod,
		const bool isalphafixed = false, const bool isbetafixed = false, const bool isvolvolfixed = false, const bool isrhofixed = false, const bool vegaweighted = true)
    : x_(x), y_(y), f_(x_.begin(),x_.end(),y_.begin(), expiry, forward, alpha, beta, volvol, rho, isalphafixed, isbetafixed, isvolvolfixed, isrhofixed, vegaweighted, 
		boost::shared_ptr<EndCriteria>(&endCriteria), boost::shared_ptr<OptimizationMethod>(&optimizationMethod)) 
		{
			
		}
    Real operator()(Real x, bool allowExtrapolation=true) {
        return f_(x, allowExtrapolation);
    }
	Real update() {
		f_.update();
		return f_.rmsError();
	}

	Real alpha() { return f_.alpha();}
	Real beta() { return f_.beta(); }
	Real nu() { return f_.nu(); }
	Real rho() { return f_.rho(); }
	Real rmsError() { return f_.rmsError(); }
	Real maxError() { return f_.maxError(); }
    Array x_, y_;
    QuantLib::SABRInterpolation f_;
};
%}

%define make_safe_interpolation2(Alias)
%{
typedef SafeInterpolation_sabr Safe2##T;
%}
%rename(Alias) Safe2##T;
class Safe2##T {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE) \
     || defined(SWIGCSHARP) || defined(SWIGPERL)
    %rename(call) operator();
    #endif
  public:
    Safe2##T(const Array& x, const Array& y, const double expiry, const double forward, 
		const double alpha, const double beta, const double volvol, const double rho,
		EndCriteria& endCriteria, OptimizationMethod& optimizationMethod,
		const bool isalphafixed = false, const bool isbetafixed = false, const bool isvolvolfixed = false, const bool isrhofixed = false, const bool vegaweighted = true);
		// const EndCriteria& endCriteria = new EndCriteria(100000, 100, 1e-8, 1e-8, 1e-8));
		// OptimizationMethod& optimizationMethod = new Simplex(0.01));

    Real operator()(Real x, bool allowExtrapolation=true);

	Real alpha();
	Real beta();
	Real nu();
	Real rho();
	Real rmsError();
	Real maxError();
	Real update();
};
%enddef

%{
// svi
// safe versions which copy their arguments
class SafeInterpolation_svi {
  public:
    SafeInterpolation_svi(const Array& x, const Array& y, const double expiry, const double forward, 
		const double a, const double b, const double rho, const double m, const double sigma,
		EndCriteria& endCriteria, OptimizationMethod& optimizationMethod, const bool vegaweighted = true)
    : x_(x), y_(y), f_(x_.begin(),x_.end(),y_.begin(), expiry, forward, a, b, rho, m, sigma, vegaweighted, 
		boost::shared_ptr<EndCriteria>(&endCriteria), boost::shared_ptr<OptimizationMethod>(&optimizationMethod), 1E-10) 
		{
			
		}
    Real operator()(Real x, bool allowExtrapolation=true) {
        return f_(x, allowExtrapolation);
    }
	Real update() {
		f_.update();
		return f_.rmsError();
	}

	Real a() { return f_.a();}
	Real b() { return f_.b(); }
	Real rho() { return f_.rho(); }
	Real m() { return f_.m(); }
	Real sigma() { return f_.sigma(); }
	Real rmsError() { return f_.rmsError(); }
	Real maxError() { return f_.maxError(); }
    Array x_, y_;
    QuantLib::SVIInterpolation f_;
};
%}

%define make_safe_interpolation3(Alias)
%{
typedef SafeInterpolation_svi Safe3##T;
%}
%rename(Alias) Safe3##T;
class Safe3##T {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE) \
     || defined(SWIGCSHARP) || defined(SWIGPERL)
    %rename(call) operator();
    #endif
  public:
    Safe3##T(const Array& x, const Array& y, const double expiry, const double forward, 
		const double a, const double b, const double rho, const double m, const double sigma,
		EndCriteria& endCriteria, OptimizationMethod& optimizationMethod, const bool vegaweighted = true);

    Real operator()(Real x, bool allowExtrapolation=true);

	Real a();
	Real b();
	Real rho();
	Real m();
	Real sigma();
	Real rmsError();
	Real maxError();
	Real update();
};
%enddef

make_safe_interpolation(LinearInterpolation,LinearInterpolation);
make_safe_interpolation(LogLinearInterpolation,LogLinearInterpolation);

make_safe_interpolation(BackwardFlatInterpolation,BackwardFlatInterpolation);
make_safe_interpolation(ForwardFlatInterpolation,ForwardFlatInterpolation);

make_safe_interpolation(CubicNaturalSpline,CubicNaturalSpline);
make_safe_interpolation(LogCubicNaturalSpline,LogCubicNaturalSpline);
make_safe_interpolation(MonotonicCubicNaturalSpline,MonotonicCubicNaturalSpline);
make_safe_interpolation(MonotonicLogCubicNaturalSpline,MonotonicLogCubicNaturalSpline);

make_safe_interpolation(KrugerCubic,KrugerCubic);
make_safe_interpolation(KrugerLogCubic,KrugerLogCubic);

make_safe_interpolation(FritschButlandCubic,FritschButlandCubic);
make_safe_interpolation(FritschButlandLogCubic,FritschButlandLogCubic);

make_safe_interpolation(Parabolic,Parabolic);
make_safe_interpolation(LogParabolic,LogParabolic);
make_safe_interpolation(MonotonicParabolic,MonotonicParabolic);
make_safe_interpolation(MonotonicLogParabolic,MonotonicLogParabolic);

make_safe_interpolation2(SABRInterpolation);
make_safe_interpolation3(SVIInterpolation);

%define extend_spline(T)
%extend Safe##T {
    Real derivative(Real x, bool extrapolate = false) {
        return self->f_.derivative(x,extrapolate);
    }
    Real secondDerivative(Real x, bool extrapolate = false) {
        return self->f_.secondDerivative(x,extrapolate);
    }
    Real primitive(Real x, bool extrapolate = false) {
        return self->f_.primitive(x,extrapolate);
    }
}
%enddef

extend_spline(CubicNaturalSpline);
extend_spline(LogCubicNaturalSpline);
extend_spline(MonotonicCubicNaturalSpline);
extend_spline(MonotonicLogCubicNaturalSpline);

extend_spline(KrugerCubic);
extend_spline(KrugerLogCubic);

extend_spline(FritschButlandCubic);
extend_spline(FritschButlandLogCubic);

extend_spline(Parabolic);
extend_spline(LogParabolic);
extend_spline(MonotonicParabolic);
extend_spline(MonotonicLogParabolic);

%{
// safe versions which copy their arguments
template <class I>
class SafeInterpolation2D {
  public:
    SafeInterpolation2D(const Array& x, const Array& y, const Matrix& m)
    : x_(x), y_(y), m_(m), f_(x_.begin(),x_.end(),y_.begin(),y_.end(),m_) {}
    Real operator()(Real x, Real y, bool allowExtrapolation=false) {
        return f_(x,y, allowExtrapolation);
    }
  protected:
    Array x_, y_;
    Matrix m_;
    I f_;
};
%}

%define make_safe_interpolation2d(T,Alias)
%{
typedef SafeInterpolation2D<QuantLib::T> Safe##T;
%}
%rename(Alias) Safe##T;
class Safe##T {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE) \
     || defined(SWIGCSHARP) || defined(SWIGPERL)
    %rename(call) operator();
    #endif
  public:
    Safe##T(const Array& x, const Array& y, const Matrix& m);
    Real operator()(Real x, Real y, bool allowExtrapolation=false);
};
%enddef

make_safe_interpolation2d(BilinearInterpolation,BilinearInterpolation);
make_safe_interpolation2d(BicubicSpline,BicubicSpline);


// interpolation traits

%{
using QuantLib::BackwardFlat;
using QuantLib::ForwardFlat;
using QuantLib::Linear;
using QuantLib::LogLinear;
using QuantLib::Cubic;
%}

struct BackwardFlat {};
struct ForwardFlat {};
struct Linear {};
struct LogLinear {};
struct Cubic {};


#endif
