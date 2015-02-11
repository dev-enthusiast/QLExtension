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

public class CmsRateBond : Bond {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal CmsRateBond(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NQuantLibcPINVOKE.CmsRateBond_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(CmsRateBond obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~CmsRateBond() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_CmsRateBond(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public CmsRateBond(uint settlementDays, double faceAmount, Schedule schedule, SwapIndex index, DayCounter paymentDayCounter, BusinessDayConvention paymentConvention, uint fixingDays, DoubleVector gearings, DoubleVector spreads, DoubleVector caps, DoubleVector floors, bool inArrears, double redemption, Date issueDate) : this(NQuantLibcPINVOKE.new_CmsRateBond__SWIG_0(settlementDays, faceAmount, Schedule.getCPtr(schedule), SwapIndex.getCPtr(index), DayCounter.getCPtr(paymentDayCounter), (int)paymentConvention, fixingDays, DoubleVector.getCPtr(gearings), DoubleVector.getCPtr(spreads), DoubleVector.getCPtr(caps), DoubleVector.getCPtr(floors), inArrears, redemption, Date.getCPtr(issueDate)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public CmsRateBond(uint settlementDays, double faceAmount, Schedule schedule, SwapIndex index, DayCounter paymentDayCounter, BusinessDayConvention paymentConvention, uint fixingDays, DoubleVector gearings, DoubleVector spreads, DoubleVector caps, DoubleVector floors, bool inArrears, double redemption) : this(NQuantLibcPINVOKE.new_CmsRateBond__SWIG_1(settlementDays, faceAmount, Schedule.getCPtr(schedule), SwapIndex.getCPtr(index), DayCounter.getCPtr(paymentDayCounter), (int)paymentConvention, fixingDays, DoubleVector.getCPtr(gearings), DoubleVector.getCPtr(spreads), DoubleVector.getCPtr(caps), DoubleVector.getCPtr(floors), inArrears, redemption), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public CmsRateBond(uint settlementDays, double faceAmount, Schedule schedule, SwapIndex index, DayCounter paymentDayCounter, BusinessDayConvention paymentConvention, uint fixingDays, DoubleVector gearings, DoubleVector spreads, DoubleVector caps, DoubleVector floors, bool inArrears) : this(NQuantLibcPINVOKE.new_CmsRateBond__SWIG_2(settlementDays, faceAmount, Schedule.getCPtr(schedule), SwapIndex.getCPtr(index), DayCounter.getCPtr(paymentDayCounter), (int)paymentConvention, fixingDays, DoubleVector.getCPtr(gearings), DoubleVector.getCPtr(spreads), DoubleVector.getCPtr(caps), DoubleVector.getCPtr(floors), inArrears), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public CmsRateBond(uint settlementDays, double faceAmount, Schedule schedule, SwapIndex index, DayCounter paymentDayCounter, BusinessDayConvention paymentConvention, uint fixingDays, DoubleVector gearings, DoubleVector spreads, DoubleVector caps, DoubleVector floors) : this(NQuantLibcPINVOKE.new_CmsRateBond__SWIG_3(settlementDays, faceAmount, Schedule.getCPtr(schedule), SwapIndex.getCPtr(index), DayCounter.getCPtr(paymentDayCounter), (int)paymentConvention, fixingDays, DoubleVector.getCPtr(gearings), DoubleVector.getCPtr(spreads), DoubleVector.getCPtr(caps), DoubleVector.getCPtr(floors)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
