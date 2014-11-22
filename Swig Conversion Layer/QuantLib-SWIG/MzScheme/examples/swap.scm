
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

; utility functions

(define (i-th i l)
  (if (= n 0)
      (car l)
      (i-th (- n 1) (cdr l))))

(define (my-first n l)
  (if (= n 0)
      '()
      (cons (car l) (my-first (- n 1) (cdr l)))))
(define (trim-front n l)
  (if (= n 0)
      l
      (trim-front (- n 1) (cdr l))))
(define (trim-back n l)
  (reverse (trim-front n (reverse l))))

(define (format-rate v digits)
  (format "~a %" (round-with-digits (* v 100) digits)))

(define (format-price p digits)
  (format "~a %" (round-with-digits p digits)))


; global data
(define calendar (new-TARGET))
(define todays-date (new-Date 6 11 2001))
(Settings-evaluation-date-set! (Settings-instance) todays-date)
(define settlement-date (new-Date 8 11 2001))

; market values

(define deposit-data
  (list (list 1 (Weeks)  (new-SimpleQuote 0.0382))
        (list 1 (Months) (new-SimpleQuote 0.0372))
        (list 3 (Months) (new-SimpleQuote 0.0363))
        (list 6 (Months) (new-SimpleQuote 0.0353))
        (list 9 (Months) (new-SimpleQuote 0.0348))
        (list 1 (Years)  (new-SimpleQuote 0.0345))))

(define fra-data
  (list (list 3 6 (new-SimpleQuote 0.037125))
        (list 6 9 (new-SimpleQuote 0.037125))
        (list 9 12 (new-SimpleQuote 0.037125))))

(define futures-data
  (list (list (new-Date 19 12 2001) (new-SimpleQuote 96.2875))
        (list (new-Date 20 3 2002) (new-SimpleQuote 96.7875))
        (list (new-Date 19 6 2002) (new-SimpleQuote 96.9875))
        (list (new-Date 18 9 2002) (new-SimpleQuote 96.6875))
        (list (new-Date 18 12 2002) (new-SimpleQuote 96.4875))
        (list (new-Date 19 3 2003) (new-SimpleQuote 96.3875))
        (list (new-Date 18 6 2003) (new-SimpleQuote 96.2875))
        (list (new-Date 17 9 2003) (new-SimpleQuote 96.0875))))

(define 5-years-swap (new-SimpleQuote 0.0443))
(define swap-data
  (list (list  2 (Years) (new-SimpleQuote 0.037125))
        (list  3 (Years) (new-SimpleQuote 0.0398))
        (list  5 (Years) 5-years-swap)
        (list 10 (Years) (new-SimpleQuote 0.05165))
        (list 15 (Years) (new-SimpleQuote 0.055175))))

; build rate helpers

(define depo-day-counter (new-Actual360))
(define depo-settlement-days 2)
(define deposit-helpers
  (map (lambda (datum)
         (apply (lambda (n units quote)
                  (new-DepositRateHelper (new-QuoteHandle quote)
                                         (new-Period n units)
                                         depo-settlement-days
                                         calendar (ModifiedFollowing)
                                         #f depo-day-counter))
                datum))
       deposit-data))

(define fra-day-counter (new-Actual360))
(define fra-settlement-days 2)
(define fra-helpers
  (map (lambda (datum)
         (apply (lambda (n m quote)
                  (new-FraRateHelper (new-QuoteHandle quote)
                                     n m fra-settlement-days
                                     calendar (ModifiedFollowing)
                                     #f fra-day-counter))
                datum))
       fra-data))

(define futures-day-counter (new-Actual360))
(define futures-months 3)
(define futures-helpers
  (map (lambda (datum)
         (apply (lambda (date quote)
                  (new-FuturesRateHelper (new-QuoteHandle quote)
                                         date futures-months
                                         calendar (ModifiedFollowing)
                                         #t futures-day-counter
                                         (new-QuoteHandle
                                          (new-SimpleQuote 0.0))))
                datum))
       futures-data))

(define fixed-leg-frequency (Annual))
(define fixed-leg-tenor (new-Period 1 (Years)))
(define fixed-leg-adjustment (Unadjusted))
(define fixed-leg-day-counter (new-Thirty360))
(define floating-leg-frequency (Semiannual))
(define floating-leg-tenor (new-Period 6 (Months)))
(define floating-leg-adjustment (ModifiedFollowing))
(define swap-helpers
  (map (lambda (datum)
         (apply (lambda (n units quote)
                  (new-SwapRateHelper (new-QuoteHandle quote)
                                      (new-Period n units)
                                      calendar
                                      fixed-leg-frequency fixed-leg-adjustment
                                      fixed-leg-day-counter
                                      (new-Euribor6M)))
                datum))
       swap-data))

; term structure handles

(define discount-term-structure (new-RelinkableYieldTermStructureHandle))
(define forecast-term-structure (new-RelinkableYieldTermStructureHandle))

; term-structure construction

(define depo-futures-swap-curve (new-PiecewiseFlatForward
                                 settlement-date
                                 (append
                                  (my-first 2 deposit-helpers)
                                  futures-helpers
                                  (trim-front 1 swap-helpers))
                                 (new-Actual360)))

(define depo-fra-swap-curve (new-PiecewiseFlatForward
                             settlement-date
                             (append
                              (my-first 3 deposit-helpers)
                              fra-helpers
                              swap-helpers)
                             (new-Actual360)))

; swaps to be priced

(define swap-engine (new-DiscountingSwapEngine discount-term-structure))

(define nominal 1000000)
(define length 5)
(define maturity (Calendar-advance calendar settlement-date length (Years)))
(define pay-fixed (VanillaSwap-Payer))

(define fixed-rate 0.04)
(define spread 0.0)
(define index-fixing-days 2)
(define index (new-Euribor6M forecast-term-structure))
(define floating-leg-day-counter (InterestRateIndex-day-counter index))

(define fixed-schedule (new-Schedule settlement-date maturity
                                     fixed-leg-tenor calendar
                                     fixed-leg-adjustment fixed-leg-adjustment
                                     (DateGeneration-Forward) #f))
(define floating-schedule (new-Schedule settlement-date maturity
                                        floating-leg-tenor calendar
                                        floating-leg-adjustment
                                        floating-leg-adjustment
                                        (DateGeneration-Forward) #f))

(define spot (new-VanillaSwap pay-fixed nominal
                              fixed-schedule fixed-rate fixed-leg-day-counter
                              floating-schedule index spread
                              floating-leg-day-counter))
(Instrument-pricing-engine-set! spot swap-engine)

(define forward-start (Calendar-advance calendar settlement-date 1 (Years)))
(define forward-end (Calendar-advance calendar forward-start length (Years)))
(define fixed-fwd-schedule (new-Schedule forward-start forward-end
                                         fixed-leg-tenor calendar
                                         fixed-leg-adjustment
                                         fixed-leg-adjustment
                                         (DateGeneration-Forward) #f))
(define floating-fwd-schedule (new-Schedule forward-start forward-end
                                            floating-leg-tenor calendar
                                            floating-leg-adjustment
                                            floating-leg-adjustment
                                            (DateGeneration-Forward) #f))
(define forward (new-VanillaSwap pay-fixed nominal
                                 fixed-fwd-schedule fixed-rate
                                 fixed-leg-day-counter
                                 floating-fwd-schedule
                                 index spread
                                 floating-leg-day-counter))
(Instrument-pricing-engine-set! forward swap-engine)

; display results

(define fmt '((14 r) (17 r) (11 r) (15 r)))
(define sep " | ")

(define header (tabulate fmt sep
                         "term structure"
                         "net present value"
                         "fair spread"
                         "fair fixed rate"))
(define (rule)
  (let ((r (make-string (string-length header) #\-)))
    (display r) (newline)))
(define (dblrule)
  (let ((r (make-string (string-length header) #\=)))
    (display r) (newline)))
(define (tab)
  (let ((t (make-string 8 #\space)))
    (display t)))

(define (report swap name)
  (display (tabulate fmt sep
                     name
                     (format-price (Instrument-NPV swap) 2)
                     (format-rate (VanillaSwap-fair-spread swap) 4)
                     (format-rate (VanillaSwap-fair-rate swap) 4)))
  (newline))

; price the spot swap

(dblrule)
(display (format "5-year market swap-rate = ~a~n"
                 (format-rate (Quote-value 5-years-swap) 2)))
(dblrule)

(tab)
(display (format "5-years swap paying ~a~n"
                 (format-rate fixed-rate 2)))
(display header) (newline)
(rule)

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-futures-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-futures-swap-curve)
(report spot "depo-fut-swap")

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-fra-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-fra-swap-curve)
(report spot "depo-fra-swap")

(rule)

; price the 1-year forward swap

(tab)
(display (format "5-years 1-year forward swap paying ~a~n"
                 (format-rate fixed-rate 2)))
(rule)

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-futures-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-futures-swap-curve)
(report forward "depo-fut-swap")

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-fra-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-fra-swap-curve)
(report forward "depo-fra-swap")

; modify the 5-years swap rate and reprice

(SimpleQuote-value-set! 5-years-swap 0.046)

(dblrule)
(display (format "5-year market swap-rate = ~a~n"
                 (format-rate (Quote-value 5-years-swap) 2)))
(dblrule)

(tab)
(display (format "5-years swap paying ~a~n"
                 (format-rate fixed-rate 2)))
(display header) (newline)
(rule)

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-futures-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-futures-swap-curve)
(report spot "depo-fut-swap")

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-fra-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-fra-swap-curve)
(report spot "depo-fra-swap")

(rule)

(tab)
(display (format "5-years 1-year forward swap paying ~a~n"
                 (format-rate fixed-rate 2)))
(rule)

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-futures-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-futures-swap-curve)
(report forward "depo-fut-swap")

(RelinkableYieldTermStructureHandle-link-to! discount-term-structure
                                             depo-fra-swap-curve)
(RelinkableYieldTermStructureHandle-link-to! forecast-term-structure
                                             depo-fra-swap-curve)
(report forward "depo-fra-swap")

