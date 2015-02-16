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

/*! \file svi.hpp
    \brief SVI functions
*/

#ifndef quantlib_svi_hpp
#define quantlib_svi_hpp

#include <ql/types.hpp>

namespace QuantLib {

    Real unsafeSviVolatility(Rate strike,
                              Rate forward,
                              Time expiryTime,
							  Real a,
							  Real b,
							  Real rho,
							  Real m,
							  Real sigma);

    Real sviVolatility(Rate strike,
                        Rate forward,
                        Time expiryTime,
						Real a,
						Real b,
						Real rho,
						Real m,
						Real sigma);

    void validateSviParameters(Real a,
                                Real b,
								Real rho,
								Real m,
								Real sigma);

}

#endif
