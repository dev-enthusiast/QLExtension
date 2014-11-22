
; Copyright (C) 2004, 2005, 2006, 2007 StatPro Italia srl
;
; This file is part of QuantLib, a free-software/open-source library
; for financial quantitative analysts and developers - http://quantlib.org/
;
; QuantLib is free software: you can redistribute it and/or modify it under the
; terms of the QuantLib license.  You should have received a copy of the
; license along with this program; if not, please email
; <quantlib-dev@lists.sf.net>. The license is also available online at
; <http://quantlib.org/license.shtml>.
;
; This program is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
; FOR A PARTICULAR PURPOSE.  See the license for more details.

(require (lib "quantlib.ss" "quantlib"))
(load "tabulate.scm")

(define-syntax with-pricing-engine
  (syntax-rules ()
    ((with-pricing-engine (instrument engine) body1 body2 ...)
     (begin
       (Instrument-pricing-engine-set! instrument engine)
       body1 body2 ...))))


; global data
(define todays-date (new-Date 15 5 1998))
(Settings-evaluation-date-set! (Settings-instance) todays-date)
(define settlement-date (new-Date 17 5 1998))
(define risk-free-rate (new-FlatForward settlement-date 0.06
                                        (new-Actual365Fixed)))

; option parameters
(define exercise (new-AmericanExercise settlement-date (new-Date 17 5 1999)))
(define payoff (new-PlainVanillaPayoff (Option-Put) 40.0))

; market data
(define underlying (new-SimpleQuote 36.0))
(define volatility (new-BlackConstantVol todays-date (new-TARGET)
                                         0.20 (new-Actual365Fixed)))
(define dividend-yield (new-FlatForward settlement-date 0.00
                                        (new-Actual365Fixed)))

; report
(define fmt '((20 r) (17 r 5) (17 r 4) (17 r 4)))
(define sep " |")
(define header (tabulate fmt sep
                         "method" "value" "estimated error" "actual error"))
(newline)
(display header) (newline)
(display (make-string (string-length header) #\-)) (newline)

(define ref-value 4.48667344)
(define (report method x)
  (display (tabulate fmt sep
                     method x "n/a" (abs (- x ref-value))))
  (newline))

; good to go

(define process (new-BlackScholesMertonProcess
                 (new-QuoteHandle underlying)
                 (new-YieldTermStructureHandle dividend-yield)
                 (new-YieldTermStructureHandle risk-free-rate)
                 (new-BlackVolTermStructureHandle volatility)))

(define option (new-VanillaOption payoff exercise))

(report "reference value" ref-value)

; method: analytic

(with-pricing-engine (option (new-BaroneAdesiWhaleyEngine process))
  (report "Barone-Adesi-Whaley" (Instrument-NPV option)))

(with-pricing-engine (option (new-BjerksundStenslandEngine process))
  (report "Bjerksund-Stensland" (Instrument-NPV option)))

; method: finite differences
(define timeSteps 801)
(define gridPoints 801)

(with-pricing-engine (option (new-FDAmericanEngine
                              process timeSteps gridPoints #f))
  (report "finite differences" (Instrument-NPV option)))

; method: binomial
(define timeSteps 801)

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "jr" timeSteps))
  (report "binomial (JR)" (Instrument-NPV option)))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "crr" timeSteps))
  (report "binomial (CRR)" (Instrument-NPV option)))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "eqp" timeSteps))
  (report "binomial (EQP)" (Instrument-NPV option)))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "trigeorgis" timeSteps))
  (report "bin. (Trigeorgis)" (Instrument-NPV option)))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "tian" timeSteps))
  (report "binomial (Tian)" (Instrument-NPV option)))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "lr" timeSteps))
  (report "binomial (LR)" (Instrument-NPV option)))

