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

public class HullWhite : ShortRateModel {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal HullWhite(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NQuantLibcPINVOKE.HullWhite_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(HullWhite obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~HullWhite() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_HullWhite(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public HullWhite(YieldTermStructureHandle termStructure, double a, double sigma) : this(NQuantLibcPINVOKE.new_HullWhite__SWIG_0(YieldTermStructureHandle.getCPtr(termStructure), a, sigma), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public HullWhite(YieldTermStructureHandle termStructure, double a) : this(NQuantLibcPINVOKE.new_HullWhite__SWIG_1(YieldTermStructureHandle.getCPtr(termStructure), a), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public HullWhite(YieldTermStructureHandle termStructure) : this(NQuantLibcPINVOKE.new_HullWhite__SWIG_2(YieldTermStructureHandle.getCPtr(termStructure)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public double discount(double t) {
    double ret = NQuantLibcPINVOKE.HullWhite_discount(swigCPtr, t);
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
