; Copyright (C) 2002, 2003 RiskMap srl
; Copyright (C) 2007 StatPro Italia srl
;
; This file is part of QuantLib, a free-software/open-source library
; for financial quantitative analysts and developers - http://quantlib.org/
;
; QuantLib is free software developed by the QuantLib Group; you can
; redistribute it and/or modify it under the terms of the QuantLib License;
; either version 1.0, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; QuantLib License for more details.
;
; You should have received a copy of the QuantLib License along with this
; program; if not, please email quantlib-dev@lists.sf.net
;
; The QuantLib License is also available at <http://quantlib.org/license.shtml>
; The members of the QuantLib Group are listed in the QuantLib License

(load "unittest.scm")
(load "common.scm")

(define (TermStructure-test tag)
  (define (make-test-structure-handle)
    (let ((settlement-days 2)
          (deposit-data (list (list 1 (Months) 4.581)
                              (list 2 (Months) 4.573)
                              (list 3 (Months) 4.557)
                              (list 6 (Months) 4.496)
                              (list 9 (Months) 4.490)))
          (swap-data '((1 4.54)
                       (5 4.99)
                       (10 5.47)
                       (20 5.89)
                       (30 5.96))))
      (let* ((real-today (Date-todays-date))
             (calendar (new-TARGET))
             (today (Calendar-adjust calendar real-today))
             (settlement (Calendar-advance calendar today
                                           settlement-days (Days)))
             (day-counter (new-Actual360))
             (day-counter-2 (new-Thirty360))
             (deposits (map
                        (lambda (datum)
                          (let-at-once ((n units rate) datum)
                            (new-DepositRateHelper
                             (new-QuoteHandle (new-SimpleQuote (/ rate 100)))
                             (new-Period n units) settlement-days
                             calendar (ModifiedFollowing)
                             #f day-counter)))
                        deposit-data))
             (swaps (map
                     (lambda (datum)
                       (let-at-once ((length rate) datum)
                         (new-SwapRateHelper
                          (new-QuoteHandle (new-SimpleQuote (/ rate 100)))
                          (new-Period length (Years)) calendar
                          1 (Unadjusted) day-counter-2 (new-Euribor6M))))
                     swap-data))
             (term-structure (new-PiecewiseFlatForward
                              settlement
                              (append deposits swaps)
                              day-counter)))
        (new-RelinkableYieldTermStructureHandle term-structure))))
  ; setup
  (let ((term-structure (make-test-structure-handle)))
    (cond ((equal? tag 'implied-obs)
           ; check implied term structure observability
           (let ((flag #f))
             (let* ((calendar (new-TARGET))
                    (settlement (YieldTermStructureHandle-reference-date
                                 term-structure))
                    (new-settlement (Calendar-advance
                                     calendar settlement 3 (Years)))
                    (day-counter (YieldTermStructureHandle-day-counter
                                  term-structure))
                    (implied (new-ImpliedTermStructure term-structure
                                                       new-settlement))
                    (obs (new-Observer (lambda () (set! flag #t)))))
               (let ((temp (YieldTermStructure->Observable implied)))
                 (Observer-register-with obs temp))
               (let ((new-term-structure (new-FlatForward settlement
                                                          0.05
                                                          day-counter)))
                 (RelinkableYieldTermStructureHandle-link-to!
                  term-structure
                  new-term-structure))
               (if (not flag)
                   (error
                    "Observer was not notified of term structure change")))))
          ((equal? tag 'fw-spreaded-obs)
           ; check forward-spreaded term structure observability
           (let ((flag #f)
                 (tolerance 1.0e-10))
             (let* ((settlement (YieldTermStructureHandle-reference-date
                                 term-structure))
                    (day-counter (YieldTermStructureHandle-day-counter
                                  term-structure))
                    (me (new-SimpleQuote 0.01))
                    (h (new-QuoteHandle me))
                    (spreaded (new-ForwardSpreadedTermStructure
                               term-structure h))
                    (obs (new-Observer (lambda () (set! flag #t)))))
               (let ((temp (YieldTermStructure->Observable spreaded)))
                 (Observer-register-with obs temp))
               (let ((new-term-structure (new-FlatForward settlement
                                                          0.05
                                                          day-counter)))
                 (RelinkableYieldTermStructureHandle-link-to!
                  term-structure
                  new-term-structure))
               (if (not flag)
                   (error
                    "Observer was not notified of term structure change"))
               (set! flag #f)
               (SimpleQuote-value-set! me 0.005)
               (if (not flag)
                   (error "Observer was not notified of spread change")))))
          ((equal? tag 'z-spreaded-obs)
           ; check zero-spreaded term structure observability
           (let ((flag #f)
                 (tolerance 1.0e-10))
             (let* ((settlement (YieldTermStructureHandle-reference-date
                                 term-structure))
                    (day-counter (YieldTermStructureHandle-day-counter
                                  term-structure))
                    (me (new-SimpleQuote 0.01))
                    (h (new-QuoteHandle me))
                    (spreaded (new-ZeroSpreadedTermStructure
                               term-structure h))
                    (obs (new-Observer (lambda () (set! flag #t)))))
               (let ((temp (YieldTermStructure->Observable spreaded)))
                 (Observer-register-with obs temp))
               (let ((new-term-structure (new-FlatForward settlement
                                                          0.05
                                                          day-counter)))
                 (RelinkableYieldTermStructureHandle-link-to!
                  term-structure
                  new-term-structure))
               (if (not flag)
                   (error
                    "Observer was not notified of term structure change"))
               (set! flag #f)
               (SimpleQuote-value-set! me 0.005)
               (if (not flag)
                   (error "Observer was not notified of spread change"))))))))

(define (Implied-term-structure-observability-test)
  (TermStructure-test 'implied-obs))
(define (Forward-spreaded-term-structure-observability-test)
  (TermStructure-test 'fw-spreaded-obs))
(define (Zero-spreaded-term-structure-observability-test)
  (TermStructure-test 'z-spreaded-obs))

(define TermStructure-suite
  (make-test-suite
   "Term-structure tests"
   (make-test-case/msg "Testing observability of implied term structure"
                       (Implied-term-structure-observability-test))
   (make-test-case/msg
    "Testing observability of forward-spreaded term structure"
    (Forward-spreaded-term-structure-observability-test))
   (make-test-case/msg "Testing observability of zero-spreaded term structure"
                       (Zero-spreaded-term-structure-observability-test))))

