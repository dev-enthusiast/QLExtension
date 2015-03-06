
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl

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

#ifndef quantlib_r_i
#define quantlib_r_i

 %{
 /* Includes the header in the wrapper code */
 #include <ql/math/Rinternals.h>
 %}
 
 %{
 using QuantLib::primsxp_struct;
 using QuantLib::symsxp_struct;
 using QuantLib::listsxp_struct;
 using QuantLib::envsxp_struct;
 using QuantLib::closxp_struct;
 using QuantLib::promsxp_struct;
 %}
 
 /* Parse the header file to generate wrappers */
 %include <ql/math/Rinternals.h>

#endif
