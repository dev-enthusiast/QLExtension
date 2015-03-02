/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Klaus Spanderen
 Copyright (C) 2005 StatPro Italia srl

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

#include <ql/risk/stochasticprocesspcaarray.hpp>
//#include <ql/math/matrixutilities/pseudosqrt.hpp>

namespace QuantLib {

	StochasticProcessPcaArray::StochasticProcessPcaArray(
        const std::vector<boost::shared_ptr<StochasticProcess1D> >& processes,
        const Matrix& pcasqrtcorrelation)
    : processes_(processes),
      sqrtCorrelation_(pcasqrtcorrelation) {

        QL_REQUIRE(!processes.empty(), "no processes given");
		QL_REQUIRE(pcasqrtcorrelation.rows() == processes.size(),
                   "mismatch between number of processes "
                   "and size of correlation matrix");
        for (Size i=0; i<processes_.size(); i++)
            registerWith(processes_[i]);
    }

	Size StochasticProcessPcaArray::size() const {
        return processes_.size();
    }

	Size StochasticProcessPcaArray::factorSize() const {
		return sqrtCorrelation_.columns();
	}

	Disposable<Array> StochasticProcessPcaArray::initialValues() const {
        Array tmp(size());
        for (Size i=0; i<size(); ++i)
            tmp[i] = processes_[i]->x0();
        return tmp;
    }

	Disposable<Array> StochasticProcessPcaArray::drift(Time t,
                                                    const Array& x) const {
        Array tmp(size());
        for (Size i=0; i<size(); ++i)
            tmp[i] = processes_[i]->drift(t, x[i]);
        return tmp;
    }

	// reconstruct sigma(t,X) from pca
	Disposable<Matrix> StochasticProcessPcaArray::diffusion(
                                               Time t, const Array& x) const {
        Matrix tmp = sqrtCorrelation_;
        for (Size i=0; i<size(); ++i) {
            Real sigma = processes_[i]->diffusion(t, x[i]);
            std::transform(tmp.row_begin(i), tmp.row_end(i),
                           tmp.row_begin(i),
                           std::bind2nd(std::multiplies<Real>(),sigma));
        }
        return tmp;
    }

	Disposable<Array> StochasticProcessPcaArray::expectation(Time t0,
                                                          const Array& x0,
                                                          Time dt) const {
        Array tmp(size());
        for (Size i=0; i<size(); ++i)
            tmp[i] = processes_[i]->expectation(t0, x0[i], dt);
        return tmp;
    }

	Disposable<Matrix> StochasticProcessPcaArray::stdDeviation(Time t0,
                                                            const Array& x0,
                                                            Time dt) const {
        Matrix tmp = sqrtCorrelation_;
        for (Size i=0; i<size(); ++i) {
            Real sigma = processes_[i]->stdDeviation(t0, x0[i], dt);
            std::transform(tmp.row_begin(i), tmp.row_end(i),
                           tmp.row_begin(i),
                           std::bind2nd(std::multiplies<Real>(),sigma));
        }
        return tmp;
    }

	Disposable<Matrix> StochasticProcessPcaArray::covariance(Time t0,
                                                          const Array& x0,
                                                          Time dt) const {
        Matrix tmp = stdDeviation(t0, x0, dt);
        return tmp*transpose(tmp);
    }

	// pca evolution
	// for futures, it doesn't consider the maturity dates
	Disposable<Array> StochasticProcessPcaArray::evolve(
                  Time t0, const Array& x0, Time dt, const Array& dw) const {
        const Array dz = sqrtCorrelation_ * dw;        // (n*3) (3*1)

        Array tmp(size());
        for (Size i=0; i<size(); ++i)
            tmp[i] = processes_[i]->evolve(t0, x0[i], dt, dz[i]);
        return tmp;
    }

	Disposable<Array> StochasticProcessPcaArray::apply(const Array& x0,
                                                    const Array& dx) const {
        Array tmp(size());
        for (Size i=0; i<size(); ++i)
            tmp[i] = processes_[i]->apply(x0[i],dx[i]);
        return tmp;
    }

	Time StochasticProcessPcaArray::time(const Date& d) const {
        return processes_[0]->time(d);
    }

    const boost::shared_ptr<StochasticProcess1D>&
		StochasticProcessPcaArray::process(Size i) const {
        return processes_[i];
    }

	Disposable<Matrix> StochasticProcessPcaArray::correlation() const {
        return sqrtCorrelation_ * transpose(sqrtCorrelation_);
    }
}
