
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

(use-modules (QuantLib))
(load "tabulate.scm")

(define-macro with-pricing-engine
  (lambda (binding . body)
    (let ((instrument (car binding))
          (engine (cadr binding)))
      `(begin
         (Instrument-pricing-engine-set! ,instrument ,engine)
         ,@body))))

; global data
(define todays-date (new-Date 15 5 1998))
(Settings-evaluation-date-set! (Settings-instance) todays-date)
(define settlement-date (new-Date 17 5 1998))
(define risk-free-rate (new-FlatForward settlement-date 0.05
                                        (new-Actual365Fixed)))

; option parameters
(define exercise (new-EuropeanExercise (new-Date 17 5 1999)))
(define payoff (new-PlainVanillaPayoff (Option-Call) 8.0))

; market data
(define underlying (new-SimpleQuote 7.0))
(define volatility (new-BlackConstantVol todays-date (new-TARGET)
                                         0.10 (new-Actual365Fixed)))
(define dividend-yield (new-FlatForward settlement-date 0.05
                                        (new-Actual365Fixed)))

; report
(define fmt '((20 r) (17 r 5) (17 r 4) (17 r 4)))
(define sep " |")
(define header (tabulate fmt sep
                         "method" "value" "estimated error" "actual error"))
(newline)
(display header) (newline)
(display (make-string (string-length header) #\-)) (newline)

(define ref-value #f)
(define (report method x e)
  (display (tabulate fmt sep
                     method x e (abs (- x ref-value))))
  (newline))

; good to go

(define process (new-BlackScholesMertonProcess
                 (new-QuoteHandle underlying)
                 (new-YieldTermStructureHandle dividend-yield)
                 (new-YieldTermStructureHandle risk-free-rate)
                 (new-BlackVolTermStructureHandle volatility)))

(define option (new-VanillaOption payoff exercise))

; method: analytic

(with-pricing-engine (option (new-AnalyticEuropeanEngine process))
  (let ((value (Instrument-NPV option)))
    (set! ref-value value)
    (report "analytic" value "n/a")))

; method: finite differences
(define timeSteps 801)
(define gridPoints 800)

(with-pricing-engine (option (new-FDEuropeanEngine
                              process timeSteps gridPoints))
  (report "finite diff." (Instrument-NPV option) "n/a"))

; method: integral

(with-pricing-engine (option (new-IntegralEngine process))
  (report "integral" (Instrument-NPV option) "n/a"))

; method: binomial
(define timeSteps 801)

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "jr" timeSteps))
  (report "binomial (JR)" (Instrument-NPV option) "n/a"))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "crr" timeSteps))
  (report "binomial (CRR)" (Instrument-NPV option) "n/a"))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "eqp" timeSteps))
  (report "binomial (EQP)" (Instrument-NPV option) "n/a"))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "trigeorgis" timeSteps))
  (report "bin. (Trigeorgis)" (Instrument-NPV option) "n/a"))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "tian" timeSteps))
  (report "binomial (Tian)" (Instrument-NPV option) "n/a"))

(with-pricing-engine (option (new-BinomialVanillaEngine
                              process "lr" timeSteps))
  (report "binomial (LR)" (Instrument-NPV option) "n/a"))

; method: finite differences
; not yet implemented

; method: Monte Carlo
(with-pricing-engine (option (new-MCEuropeanEngine process
                                                   "pseudorandom" 1 #f #f
                                                   #f #f 0.02 #f 42))
  (report "MC (crude)"
          (Instrument-NPV option) (Instrument-error-estimate option)))

(with-pricing-engine (option (new-MCEuropeanEngine process
                                                   "lowdiscrepancy" 1 #f
                                                   #f #f 32768))
  (report "MC (Sobol)" (Instrument-NPV option) "n/a"))
