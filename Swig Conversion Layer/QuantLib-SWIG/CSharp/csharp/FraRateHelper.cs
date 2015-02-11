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

public class FraRateHelper : RateHelper {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal FraRateHelper(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NQuantLibcPINVOKE.FraRateHelper_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(FraRateHelper obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~FraRateHelper() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_FraRateHelper(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public FraRateHelper(QuoteHandle rate, uint monthsToStart, uint monthsToEnd, uint fixingDays, Calendar calendar, BusinessDayConvention convention, bool endOfMonth, DayCounter dayCounter) : this(NQuantLibcPINVOKE.new_FraRateHelper__SWIG_0(QuoteHandle.getCPtr(rate), monthsToStart, monthsToEnd, fixingDays, Calendar.getCPtr(calendar), (int)convention, endOfMonth, DayCounter.getCPtr(dayCounter)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public FraRateHelper(double rate, uint monthsToStart, uint monthsToEnd, uint fixingDays, Calendar calendar, BusinessDayConvention convention, bool endOfMonth, DayCounter dayCounter) : this(NQuantLibcPINVOKE.new_FraRateHelper__SWIG_1(rate, monthsToStart, monthsToEnd, fixingDays, Calendar.getCPtr(calendar), (int)convention, endOfMonth, DayCounter.getCPtr(dayCounter)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public FraRateHelper(QuoteHandle rate, uint monthsToStart, IborIndex index) : this(NQuantLibcPINVOKE.new_FraRateHelper__SWIG_2(QuoteHandle.getCPtr(rate), monthsToStart, IborIndex.getCPtr(index)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public FraRateHelper(double rate, uint monthsToStart, IborIndex index) : this(NQuantLibcPINVOKE.new_FraRateHelper__SWIG_3(rate, monthsToStart, IborIndex.getCPtr(index)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
