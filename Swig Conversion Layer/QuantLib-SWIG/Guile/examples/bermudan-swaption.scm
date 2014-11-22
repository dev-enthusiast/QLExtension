
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

(define swaption-vols
  (list ; maturity,          length,             volatility
   (list (new-Period 1 (Years)) (new-Period 5 (Years)) 0.1148)
   (list (new-Period 2 (Years)) (new-Period 4 (Years)) 0.1108)
   (list (new-Period 3 (Years)) (new-Period 3 (Years)) 0.1070)
   (list (new-Period 4 (Years)) (new-Period 2 (Years)) 0.1021)
   (list (new-Period 5 (Years)) (new-Period 1 (Years)) 0.1000)))

(define (format-vol v digits)
  (format #f "~a %" (round-with-digits (* v 100) digits)))

(define (format-price p digits)
  (format #f "~a" (round-with-digits p digits)))

(define (average xs)
  (define (sum xs accum)
    (if (null? xs)
        accum
        (sum (cdr xs) (+ accum (car xs)))))
  (/ (sum xs 0.0) (length xs)))

(define (insert-unique x xs)
  (cond ((null? xs) (list x))
        ((= x (car xs)) xs)
        ((< x (car xs)) (cons x xs))
        (else (cons (car xs) (insert-unique x (cdr xs))))))
(define (insert-many-unique ys xs)
  (if (null? ys)
      xs
      (insert-many-unique (cdr ys) (insert-unique (car ys) xs))))
(define (collect-times helpers)
  (define (collect-iter hs ts)
    (if (null? hs)
        ts
        (collect-iter (cdr hs) (insert-many-unique
                                (vector->list (SwaptionHelper-times (car hs)))
                                ts))))
  (collect-iter helpers '()))


(define (calibrate model helpers l name)
  (let* ((fmt '((12 r) (12 r) (12 r) (12 r) (12 r)))
         (sep " |")
         (header (tabulate fmt sep
                           "maturity" "length" "volatility" "implied" "error"))
         (rule (make-string (string-length header) #\-))
         (dblrule (make-string (string-length header) #\=)))
    (newline)
    (display dblrule) (newline)
    (display name) (newline)
    (display rule) (newline)

    (let ((method (new-Simplex l)))
      (ShortRateModel-calibrate! model helpers method
                                 (new-EndCriteria 1000 250 1e-7 1e-7 1e-7))

      (display (format #f "Parameters: ~a" (Array->string
                                            (ShortRateModel-params model))))
      (newline)
      (display rule) (newline)
      (display header) (newline)
      (display rule) (newline)

      (let ((errors
             (map (lambda (swaption helper)
                    (let* ((maturity (car swaption))
                           (length (cadr swaption))
                           (vol (caddr swaption))
                           (NPV (CalibrationHelper-model-value helper))
                           (implied (CalibrationHelper-implied-volatility
                                     helper NPV 1.0e-4 1000 0.05 0.50))
                           (error (- implied vol)))
                      (display (tabulate fmt sep
                                         (Period->string maturity)
                                         (Period->string length)
                                         (format-vol vol 4)
                                         (format-vol implied 4)
                                         (format-vol error 4)))
                      (newline)
                      (abs error)))
                  swaption-vols helpers)))
        (display rule) (newline)
        (display (tabulate (list (list (string-length header) 'r)) ""
                           (string-append "Average error: "
                                          (format-vol (average errors) 4))))
        (newline)
        (display dblrule) (newline)))))


(define todays-date (new-Date 15 2 2002))
(Settings-evaluation-date-set! (Settings-instance) todays-date)
(define calendar (new-TARGET))
(define settlement-date (new-Date 19 2 2002))

; flat yield term structure impling 1x5 swap at 5%
(define rate (new-QuoteHandle (new-SimpleQuote 0.04875825)))
(define term-structure (new-YieldTermStructureHandle
                        (new-FlatForward settlement-date rate
                                         (new-Actual365Fixed))))

; define the ATM/OTM/ITM swaps

(define swap-engine (new-DiscountingSwapEngine term-structure))

(define fixed-leg-frequency (Annual))
(define fixed-leg-tenor (new-Period 1 (Years)))
(define fixed-leg-convention (Unadjusted))
(define floating-leg-convention (ModifiedFollowing))
(define fixed-leg-day-counter (new-Thirty360 (Thirty360-European)))
(define floating-leg-frequency (Semiannual))
(define floating-leg-tenor (new-Period 6 (Months)))

(define pay-fixed (VanillaSwap-Payer))
(define fixing-days 2)
(define index (new-Euribor6M term-structure))
(define floating-leg-day-counter (InterestRateIndex-day-counter index))

(define swap-start (Calendar-advance calendar settlement-date 1 (Years)
                                     floating-leg-convention))
(define swap-end (Calendar-advance calendar swap-start 5 (Years)
                                   floating-leg-convention))

(define fixed-schedule (new-Schedule swap-start swap-end
                                     fixed-leg-tenor calendar
                                     fixed-leg-convention
                                     fixed-leg-convention
                                     (DateGeneration-Forward) #f))
(define floating-schedule (new-Schedule swap-start swap-end
                                        floating-leg-tenor calendar
                                        floating-leg-convention
                                        floating-leg-convention
                                        (DateGeneration-Forward) #f))

(define dummy (new-VanillaSwap pay-fixed 100.0 fixed-schedule 0.0
                               fixed-leg-day-counter floating-schedule
                               index 0.0
                               floating-leg-day-counter))
(Instrument-pricing-engine-set! dummy swap-engine)
(define atm-rate (VanillaSwap-fair-rate dummy))

(define atm-swap (new-VanillaSwap pay-fixed 1000.0 fixed-schedule atm-rate
                                  fixed-leg-day-counter floating-schedule
                                  index 0.0
                                  floating-leg-day-counter))
(define otm-swap (new-VanillaSwap pay-fixed 1000.0 fixed-schedule
                                  (* 1.2 atm-rate)
                                  fixed-leg-day-counter floating-schedule
                                  index 0.0
                                  floating-leg-day-counter))
(define itm-swap (new-VanillaSwap pay-fixed 1000.0 fixed-schedule
                                  (* 0.8 atm-rate)
                                  fixed-leg-day-counter floating-schedule
                                  index 0.0
                                  floating-leg-day-counter))
(Instrument-pricing-engine-set! atm-swap swap-engine)
(Instrument-pricing-engine-set! otm-swap swap-engine)
(Instrument-pricing-engine-set! itm-swap swap-engine)

(define helpers
  (map (lambda (swaption-data)
         (apply
          (lambda (maturity length vol)
            (new-SwaptionHelper maturity length
                                (new-QuoteHandle (new-SimpleQuote vol))
                                index (InterestRateIndex-tenor index)
                                (InterestRateIndex-day-counter index)
                                (InterestRateIndex-day-counter index)
                                term-structure))
          swaption-data))
       swaption-vols))

(define grid (new-TimeGrid (collect-times helpers) 30))

(define G2 (new-G2 term-structure))
(define HW (new-HullWhite term-structure))
(define HW2 (new-HullWhite term-structure))
(define BK (new-BlackKarasinski term-structure))

(display "Calibrating...") (newline)

(for-each (lambda (h) (CalibrationHelper-pricing-engine-set!
                       h (new-G2SwaptionEngine G2 6.0 16)))
          helpers)
(calibrate G2 helpers 0.05 "G2 (analytic formulae)")

(for-each (lambda (h) (CalibrationHelper-pricing-engine-set!
                       h (new-JamshidianSwaptionEngine HW)))
          helpers)
(calibrate HW helpers 0.05 "Hull-White (analytic formulae)")

(for-each (lambda (h) (CalibrationHelper-pricing-engine-set!
                       h (new-TreeSwaptionEngine HW2 grid)))
          helpers)
(calibrate HW2 helpers 0.05 "Hull-White (numerical calibration)")

(for-each (lambda (h) (CalibrationHelper-pricing-engine-set!
                       h (new-TreeSwaptionEngine BK grid)))
          helpers)
(calibrate BK helpers 0.05 "Black-Karasinski (numerical calibration)")


; price Bermudan swaptions on defined swaps

(define bermudan-dates (Schedule-map fixed-schedule (lambda (d) d)))
(define exercise (new-BermudanExercise
                  (reverse (cdr (reverse bermudan-dates)))))

(define fmt '((17 r) (17 r) (17 r) (17 r)))
(define sep " |")
(define header (tabulate fmt sep
                         "model" "in-the-money"
                         "at-the-money" "out-of-the-money"))
(define rule (make-string (string-length header) #\-))
(define dblrule (make-string (string-length header) #\=))

(newline)
(display dblrule) (newline)
(display "Pricing Bermudan swaptions...") (newline)
(display rule) (newline)
(display header) (newline)
(display rule) (newline)

(define atm-swaption (new-Swaption atm-swap exercise))
(define otm-swaption (new-Swaption otm-swap exercise))
(define itm-swaption (new-Swaption itm-swap exercise))

(Instrument-pricing-engine-set! atm-swaption
                                (new-TreeSwaptionEngine G2 50))
(Instrument-pricing-engine-set! otm-swaption
                                (new-TreeSwaptionEngine G2 50))
(Instrument-pricing-engine-set! itm-swaption
                                (new-TreeSwaptionEngine G2 50))

(display (tabulate fmt sep
                   "G2 analytic"
                   (format-price (Instrument-NPV itm-swaption) 2)
                   (format-price (Instrument-NPV atm-swaption) 2)
                   (format-price (Instrument-NPV otm-swaption) 2)))
(newline)

(Instrument-pricing-engine-set! atm-swaption
                                (new-TreeSwaptionEngine HW 50))
(Instrument-pricing-engine-set! otm-swaption
                                (new-TreeSwaptionEngine HW 50))
(Instrument-pricing-engine-set! itm-swaption
                                (new-TreeSwaptionEngine HW 50))

(display (tabulate fmt sep
                   "HW analytic"
                   (format-price (Instrument-NPV itm-swaption) 2)
                   (format-price (Instrument-NPV atm-swaption) 2)
                   (format-price (Instrument-NPV otm-swaption) 2)))
(newline)

(Instrument-pricing-engine-set! atm-swaption
                                (new-TreeSwaptionEngine HW2 50))
(Instrument-pricing-engine-set! otm-swaption
                                (new-TreeSwaptionEngine HW2 50))
(Instrument-pricing-engine-set! itm-swaption
                                (new-TreeSwaptionEngine HW2 50))

(display (tabulate fmt sep
                   "HW numerical"
                   (format-price (Instrument-NPV itm-swaption) 2)
                   (format-price (Instrument-NPV atm-swaption) 2)
                   (format-price (Instrument-NPV otm-swaption) 2)))
(newline)

(Instrument-pricing-engine-set! atm-swaption
                                (new-TreeSwaptionEngine BK 50))
(Instrument-pricing-engine-set! otm-swaption
                                (new-TreeSwaptionEngine BK 50))
(Instrument-pricing-engine-set! itm-swaption
                                (new-TreeSwaptionEngine BK 50))

(display (tabulate fmt sep
                   "BK numerical"
                   (format-price (Instrument-NPV itm-swaption) 2)
                   (format-price (Instrument-NPV atm-swaption) 2)
                   (format-price (Instrument-NPV otm-swaption) 2)))
(newline)

(display dblrule) (newline)

