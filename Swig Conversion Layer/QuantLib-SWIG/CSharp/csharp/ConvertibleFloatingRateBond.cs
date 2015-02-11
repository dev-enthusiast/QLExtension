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

public class ConvertibleFloatingRateBond : Bond {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal ConvertibleFloatingRateBond(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NQuantLibcPINVOKE.ConvertibleFloatingRateBond_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ConvertibleFloatingRateBond obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ConvertibleFloatingRateBond() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NQuantLibcPINVOKE.delete_ConvertibleFloatingRateBond(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public ConvertibleFloatingRateBond(Exercise exercise, double conversionRatio, DividendSchedule dividends, CallabilitySchedule callability, QuoteHandle creditSpread, Date issueDate, int settlementDays, IborIndex index, int fixingDays, DoubleVector spreads, DayCounter dayCounter, Schedule schedule, double redemption) : this(NQuantLibcPINVOKE.new_ConvertibleFloatingRateBond__SWIG_0(Exercise.getCPtr(exercise), conversionRatio, DividendSchedule.getCPtr(dividends), CallabilitySchedule.getCPtr(callability), QuoteHandle.getCPtr(creditSpread), Date.getCPtr(issueDate), settlementDays, IborIndex.getCPtr(index), fixingDays, DoubleVector.getCPtr(spreads), DayCounter.getCPtr(dayCounter), Schedule.getCPtr(schedule), redemption), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

  public ConvertibleFloatingRateBond(Exercise exercise, double conversionRatio, DividendSchedule dividends, CallabilitySchedule callability, QuoteHandle creditSpread, Date issueDate, int settlementDays, IborIndex index, int fixingDays, DoubleVector spreads, DayCounter dayCounter, Schedule schedule) : this(NQuantLibcPINVOKE.new_ConvertibleFloatingRateBond__SWIG_1(Exercise.getCPtr(exercise), conversionRatio, DividendSchedule.getCPtr(dividends), CallabilitySchedule.getCPtr(callability), QuoteHandle.getCPtr(creditSpread), Date.getCPtr(issueDate), settlementDays, IborIndex.getCPtr(index), fixingDays, DoubleVector.getCPtr(spreads), DayCounter.getCPtr(dayCounter), Schedule.getCPtr(schedule)), true) {
    if (NQuantLibcPINVOKE.SWIGPendingException.Pending) throw NQuantLibcPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
