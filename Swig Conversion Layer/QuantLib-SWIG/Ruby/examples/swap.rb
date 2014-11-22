
# Copyright (C) 2004, 2005, 2006, 2007 StatPro Italia srl
#
# This file is part of QuantLib, a free-software/open-source library
# for financial quantitative analysts and developers - http://quantlib.org/
#
# QuantLib is free software: you can redistribute it and/or modify it under the
# terms of the QuantLib license.  You should have received a copy of the
# license along with this program; if not, please email
# <quantlib-dev@lists.sf.net>. The license is also available online at
# <http://quantlib.org/license.shtml>.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the license for more details.

require 'QuantLib'
include QuantLib

# global data
calendar = TARGET.new
todaysDate = Date.new(6,November,2001);
Settings.instance.evaluationDate = todaysDate
settlementDate = Date.new(8,November,2001);

# market quotes
deposits = [ [1, Weeks,  SimpleQuote.new(0.0382)],
             [1, Months, SimpleQuote.new(0.0372)],
             [3, Months, SimpleQuote.new(0.0363)],
             [6, Months, SimpleQuote.new(0.0353)],
             [9, Months, SimpleQuote.new(0.0348)],
             [1, Years,  SimpleQuote.new(0.0345)] ]

fras = { [3,6]  => SimpleQuote.new(0.037125),
         [6,9]  => SimpleQuote.new(0.037125),
         [9,12] => SimpleQuote.new(0.037125) }

futures = { Date.new(19,12,2001) => SimpleQuote.new(96.2875),
            Date.new(20,3,2002)  => SimpleQuote.new(96.7875),
            Date.new(19,6,2002)  => SimpleQuote.new(96.9875),
            Date.new(18,9,2002)  => SimpleQuote.new(96.6875),
            Date.new(18,12,2002) => SimpleQuote.new(96.4875),
            Date.new(19,3,2003)  => SimpleQuote.new(96.3875),
            Date.new(18,6,2003)  => SimpleQuote.new(96.2875),
            Date.new(17,9,2003)  => SimpleQuote.new(96.0875) }

swaps = { [ 2,Years] => SimpleQuote.new(0.037125),
          [ 3,Years] => SimpleQuote.new(0.0398),
          [ 5,Years] => SimpleQuote.new(0.0443),
          [10,Years] => SimpleQuote.new(0.05165),
          [15,Years] => SimpleQuote.new(0.055175) }

# build rate helpers

dayCounter = Actual360.new
settlementDays = 2
depositHelpers = deposits.map { |n,unit,v|
  DepositRateHelper.new(QuoteHandle.new(v),
                        Period.new(n,unit), settlementDays,
                        calendar, ModifiedFollowing,
                        false, dayCounter)
}

dayCounter = Actual360.new
settlementDays = 2
fraHelpers = fras.map { |(n,m),v|
  FraRateHelper.new(QuoteHandle.new(v),
                    n, m, settlementDays,
                    calendar, ModifiedFollowing,
                    false, dayCounter)
}

dayCounter = Actual360.new
months = 3
futuresHelpers = futures.map { |d,v|
  FuturesRateHelper.new(QuoteHandle.new(v),
                        d, months,
                        calendar, ModifiedFollowing,
                        true, dayCounter,
                        QuoteHandle.new(SimpleQuote.new(0.0)))
}

settlementDays = 2
fixedLegFrequency = Annual
fixedLegAdjustment = Unadjusted
fixedLegDayCounter = Thirty360.new
floatingLegFrequency = Semiannual
floatingLegAdjustment = ModifiedFollowing
swapHelpers = swaps.map {|(n,unit),v|
  SwapRateHelper.new(QuoteHandle.new(v),
                     Period.new(n,unit),
                     calendar, fixedLegFrequency, fixedLegAdjustment,
                     fixedLegDayCounter, Euribor6M.new)
}

# term structure handles

discountTermStructure = RelinkableYieldTermStructureHandle.new
forecastTermStructure = RelinkableYieldTermStructureHandle.new

# term-structure construction

helpers = depositHelpers[0..1] + futuresHelpers + swapHelpers[1..-1]
depoFuturesSwapCurve = PiecewiseFlatForward.new(settlementDate,
                                                helpers, Actual360.new)

helpers = depositHelpers[0..2] + fraHelpers + swapHelpers
depoFraSwapCurve = PiecewiseFlatForward.new(settlementDate,
                                            helpers, Actual360.new)

# swaps to be priced

swapEngine = DiscountingSwapEngine.new(discountTermStructure)

nominal = 1000000
length = 5
maturity = calendar.advance(settlementDate,length,Years)
payFixed = VanillaSwap::Payer

fixedLegFrequency = Annual
fixedLegTenor = Period.new(1,Years)
fixedLegAdjustment = Unadjusted
fixedLegDayCounter = Thirty360.new
fixedRate = 0.04

floatingLegFrequency = Semiannual
floatingLegTenor = Period.new(6,Months)
spread = 0.0
fixingDays = 2
index = Euribor6M.new(forecastTermStructure)
floatingLegAdjustment = ModifiedFollowing
floatingLegDayCounter = index.dayCounter

fixedSchedule = Schedule.new(settlementDate, maturity,
                             fixedLegTenor, calendar,
                             fixedLegAdjustment, fixedLegAdjustment,
                             DateGeneration::Forward, false)
floatingSchedule = Schedule.new(settlementDate, maturity,
                                floatingLegTenor, calendar,
                                floatingLegAdjustment, floatingLegAdjustment,
                                DateGeneration::Forward, false)

spot = VanillaSwap.new(payFixed, nominal,
                       fixedSchedule, fixedRate, fixedLegDayCounter,
                       floatingSchedule, index, spread,
                       floatingLegDayCounter)
spot.pricingEngine = swapEngine

forwardStart = calendar.advance(settlementDate,1,Years)
forwardEnd = calendar.advance(forwardStart,length,Years)
fixedSchedule = Schedule.new(forwardStart, forwardEnd,
                             fixedLegTenor, calendar,
                             fixedLegAdjustment, fixedLegAdjustment,
                             DateGeneration::Forward, false)
floatingSchedule = Schedule.new(forwardStart, forwardEnd,
                                floatingLegTenor, calendar,
                                floatingLegAdjustment, floatingLegAdjustment,
                                DateGeneration::Forward, false)

forward = VanillaSwap.new(payFixed, nominal,
                          fixedSchedule, fixedRate, fixedLegDayCounter,
                          floatingSchedule, index, spread,
                          floatingLegDayCounter)
forward.pricingEngine = swapEngine

# price on the bootstrapped curves

def formatPrice(p,digits=2)
  format = "%.#{digits}f"
  return sprintf(format, p)
end

def formatRate(r,digits=2)
  format = "%.#{digits}f %%"
  return sprintf(format,r*100)
end

Format = '%17s |%17s |%17s |%17s'
header = sprintf(Format,"term structure", "net present value",
                 "fair spread", "fair fixed rate")
width = header.length

rule = "-" * width
dblrule = "=" * width
tab = " " * 8

def report(swap, name)
  puts sprintf(Format, name, formatPrice(swap.NPV,2),
               formatRate(swap.fairSpread,4),
               formatRate(swap.fairRate,4))
end

puts dblrule

puts "5-year market swap-rate = #{formatRate(swaps[[5,Years]].value)}"
puts dblrule

# price on two different term structures

puts "#{tab}5-years swap paying #{formatRate(fixedRate)}"
puts header
puts rule

discountTermStructure.linkTo!(depoFuturesSwapCurve)
forecastTermStructure.linkTo!(depoFuturesSwapCurve)
report(spot,'depo-fut-swap')

discountTermStructure.linkTo!(depoFraSwapCurve)
forecastTermStructure.linkTo!(depoFraSwapCurve)
report(spot,'depo-FRA-swap')

puts rule

# price the 1-year forward swap

puts "#{tab}5-years, 1-year forward swap paying #{formatRate(fixedRate)}"
puts rule

discountTermStructure.linkTo!(depoFuturesSwapCurve)
forecastTermStructure.linkTo!(depoFuturesSwapCurve)
report(forward,'depo-fut-swap')

discountTermStructure.linkTo!(depoFraSwapCurve)
forecastTermStructure.linkTo!(depoFraSwapCurve)
report(forward,'depo-FRA-swap')

# modify the 5-years swap rate and reprice

swaps[[5,Years]].value = 0.046

puts dblrule
puts "5-year market swap-rate = #{formatRate(swaps[[5,Years]].value)}"
puts dblrule

puts "#{tab}5-years swap paying #{formatRate(fixedRate)}"
puts header
puts rule

discountTermStructure.linkTo!(depoFuturesSwapCurve)
forecastTermStructure.linkTo!(depoFuturesSwapCurve)
report(spot,'depo-fut-swap')

discountTermStructure.linkTo!(depoFraSwapCurve)
forecastTermStructure.linkTo!(depoFraSwapCurve)
report(spot,'depo-FRA-swap')

puts rule

puts "#{tab}5-years, 1-year forward swap paying #{formatRate(fixedRate)}"
puts rule

discountTermStructure.linkTo!(depoFuturesSwapCurve)
forecastTermStructure.linkTo!(depoFuturesSwapCurve)
report(forward,'depo-fut-swap')

discountTermStructure.linkTo!(depoFraSwapCurve)
forecastTermStructure.linkTo!(depoFraSwapCurve)
report(forward,'depo-FRA-swap')
