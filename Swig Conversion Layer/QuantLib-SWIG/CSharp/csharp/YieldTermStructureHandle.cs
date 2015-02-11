//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace QuantLib {

public class YieldTermStructureHandle : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal YieldTermStructureHandle(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(YieldTermStructureHandle obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~YieldTermStructureHandle() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_YieldTermStructureHandle(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public YieldTermStructureHandle(YieldTermStructure arg0) : this(NQuantLibcPINVOKE.new_YieldTermStructureHandle__SWIG_0(YieldTermStructure.getCPtr(arg0)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public YieldTermStructureHandle() : this(NQuantLibcPINVOKE.new_YieldTermStructureHandle__SWIG_1(), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public YieldTermStructure __deref__() {
    YieldTermStructure ret = new YieldTermStructure(NQuantLibcPINVOKE.YieldTermStructureHandle___deref__(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool empty() {
    bool ret = NQuantLibcPINVOKE.YieldTermStructureHandle_empty(swigCPtr);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Observable asObservable() {
    Observable ret = new Observable(NQuantLibcPINVOKE.YieldTermStructureHandle_asObservable(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DayCounter dayCounter() {
    DayCounter ret = new DayCounter(NQuantLibcPINVOKE.YieldTermStructureHandle_dayCounter(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Calendar calendar() {
    Calendar ret = new Calendar(NQuantLibcPINVOKE.YieldTermStructureHandle_calendar(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Date referenceDate() {
    Date ret = new Date(NQuantLibcPINVOKE.YieldTermStructureHandle_referenceDate(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Date maxDate() {
    Date ret = new Date(NQuantLibcPINVOKE.YieldTermStructureHandle_maxDate(swigCPtr), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public double maxTime() {
    double ret = NQuantLibcPINVOKE.YieldTermStructureHandle_maxTime(swigCPtr);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public double discount(Date arg0, bool extrapolate) {
    double ret = NQuantLibcPINVOKE.YieldTermStructureHandle_discount__SWIG_0(swigCPtr, Date.getCPtr(arg0), extrapolate);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public double discount(Date arg0) {
    double ret = NQuantLibcPINVOKE.YieldTermStructureHandle_discount__SWIG_1(swigCPtr, Date.getCPtr(arg0));
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public double discount(double arg0, bool extrapolate) {
    double ret = NQuantLibcPINVOKE.YieldTermStructureHandle_discount__SWIG_2(swigCPtr, arg0, extrapolate);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public double discount(double arg0) {
    double ret = NQuantLibcPINVOKE.YieldTermStructureHandle_discount__SWIG_3(swigCPtr, arg0);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(Date d, DayCounter arg1, Compounding arg2, Frequency f, bool extrapolate) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_0(swigCPtr, Date.getCPtr(d), DayCounter.getCPtr(arg1), (int)arg2, (int)f, extrapolate), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(Date d, DayCounter arg1, Compounding arg2, Frequency f) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_1(swigCPtr, Date.getCPtr(d), DayCounter.getCPtr(arg1), (int)arg2, (int)f), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(Date d, DayCounter arg1, Compounding arg2) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_2(swigCPtr, Date.getCPtr(d), DayCounter.getCPtr(arg1), (int)arg2), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(double t, Compounding arg1, Frequency f, bool extrapolate) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_3(swigCPtr, t, (int)arg1, (int)f, extrapolate), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(double t, Compounding arg1, Frequency f) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_4(swigCPtr, t, (int)arg1, (int)f), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate zeroRate(double t, Compounding arg1) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_zeroRate__SWIG_5(swigCPtr, t, (int)arg1), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(Date d1, Date d2, DayCounter arg2, Compounding arg3, Frequency f, bool extrapolate) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_0(swigCPtr, Date.getCPtr(d1), Date.getCPtr(d2), DayCounter.getCPtr(arg2), (int)arg3, (int)f, extrapolate), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(Date d1, Date d2, DayCounter arg2, Compounding arg3, Frequency f) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_1(swigCPtr, Date.getCPtr(d1), Date.getCPtr(d2), DayCounter.getCPtr(arg2), (int)arg3, (int)f), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(Date d1, Date d2, DayCounter arg2, Compounding arg3) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_2(swigCPtr, Date.getCPtr(d1), Date.getCPtr(d2), DayCounter.getCPtr(arg2), (int)arg3), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(double t1, double t2, Compounding arg2, Frequency f, bool extrapolate) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_3(swigCPtr, t1, t2, (int)arg2, (int)f, extrapolate), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(double t1, double t2, Compounding arg2, Frequency f) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_4(swigCPtr, t1, t2, (int)arg2, (int)f), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public InterestRate forwardRate(double t1, double t2, Compounding arg2) {
    InterestRate ret = new InterestRate(NQuantLibcPINVOKE.YieldTermStructureHandle_forwardRate__SWIG_5(swigCPtr, t1, t2, (int)arg2), true);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void enableExtrapolation() {
    NQuantLibcPINVOKE.YieldTermStructureHandle_enableExtrapolation(swigCPtr);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public void disableExtrapolation() {
    NQuantLibcPINVOKE.YieldTermStructureHandle_disableExtrapolation(swigCPtr);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool allowsExtrapolation() {
    bool ret = NQuantLibcPINVOKE.YieldTermStructureHandle_allowsExtrapolation(swigCPtr);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
