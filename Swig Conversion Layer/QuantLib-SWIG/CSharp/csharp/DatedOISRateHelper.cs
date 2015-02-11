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

public class DatedOISRateHelper : RateHelper {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal DatedOISRateHelper(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NQuantLibcPINVOKE.DatedOISRateHelper_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(DatedOISRateHelper obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~DatedOISRateHelper() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_DatedOISRateHelper(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public DatedOISRateHelper(Date startDate, Date endDate, QuoteHandle rate, OvernightIndex index, YieldTermStructureHandle discountingCurve) : this(NQuantLibcPINVOKE.new_DatedOISRateHelper__SWIG_0(Date.getCPtr(startDate), Date.getCPtr(endDate), QuoteHandle.getCPtr(rate), OvernightIndex.getCPtr(index), YieldTermStructureHandle.getCPtr(discountingCurve)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public DatedOISRateHelper(Date startDate, Date endDate, QuoteHandle rate, OvernightIndex index) : this(NQuantLibcPINVOKE.new_DatedOISRateHelper__SWIG_1(Date.getCPtr(startDate), Date.getCPtr(endDate), QuoteHandle.getCPtr(rate), OvernightIndex.getCPtr(index)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
