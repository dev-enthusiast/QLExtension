/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Mario Pucci
 Copyright (C) 2006 StatPro Italia srl

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

#include <ql/termstructures/volatility/svi.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/math/comparison.hpp>
#include <ql/errors.hpp>

namespace QuantLib {

    Real unsafeSviVolatility(Rate strike,
                              Rate forward,
                              Time expiryTime,
							  Real a,
							  Real b,
							  Real rho,
							  Real m,
							  Real sigma) {
		Real k = std::log(strike / forward);
		Real totalVariance =  ( a + b*(rho*(k-m) + std::sqrt((k-m)*(k-m)+sigma*sigma)) );
		return std::sqrt(totalVariance / expiryTime);
    }

	void validateSviParameters(Real a,
								Real b,
								Real rho,
								Real m,
								Real sigma) {
        QL_REQUIRE(b>=0.0, "b must be non negative: "
                              << b << " not allowed");
		QL_REQUIRE(rho*rho<1.0, "rho square must be less than one: "
							<< rho << " not allowed");
        QL_REQUIRE(sigma>0.0, "sigma must be positive: "
                            << sigma << " not allowed");
		// QL_REQUIRE((a + b*sigma*std::sqrt(1-rho*rho))>=0.0, "total variance must be non negative: ");
        
    }

    Real sviVolatility(Rate strike,
                        Rate forward,
                        Time expiryTime,
						Real a,
						Real b,
						Real rho,
						Real m,
						Real sigma) {
        QL_REQUIRE(strike>0.0, "strike must be positive: "
                               << io::rate(strike) << " not allowed");
        QL_REQUIRE(forward>0.0, "at the money forward rate must be "
                   "positive: " << io::rate(forward) << " not allowed");
        QL_REQUIRE(expiryTime>=0.0, "expiry time must be non-negative: "
                                   << expiryTime << " not allowed");
        validateSviParameters(a, b, rho, m, sigma);
        return unsafeSviVolatility(strike, forward, expiryTime,
                                    a, b, rho, m, sigma);
    }

}
