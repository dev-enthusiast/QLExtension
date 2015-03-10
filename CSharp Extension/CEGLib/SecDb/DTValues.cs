using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace CEGLib.SecDb
{
    // char**
    // should be IntPtr[]
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct StringHandle
    {
        public string content;
    }

    // void**
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct Handle
    {
        public IntPtr pointer_;
    }

    public class DtValue : IDisposable
    {
        protected string typename_;
        public Handle chandle_;
        // protected object value_;     // in derived class
        protected bool tohandled_ = false;
        protected bool tovalued_ = false;

        public DtValue() 
        {
            chandle_ = new Handle();
        }

        public DtValue(string typename, bool allocate=true)
        {
            typename_ = typename;
            chandle_ = new Handle();

            // int ret = (int)Uberglue.DtValue_Allocate(typename_, chandle_);
        }

        public void FromHandle(Handle handle)
        {
            if (handle.pointer_ == IntPtr.Zero) return;

            StringHandle result = new StringHandle();
            Uberglue.DtValue_GetTypeName(handle.pointer_, result);

            if (chandle_.pointer_ != IntPtr.Zero)
            {
                Uberglue.ReleaseHandle(chandle_.pointer_);
            }

            typename_ = result.content;
            chandle_ = handle;
            tovalued_ = false;
            tohandled_ = false;
        }

        public string TypeName()
        {
            return typename_;
        }

        /// <summary>
        /// From chandle to value
        /// </summary>
        public virtual void ToValue()
        {
            tovalued_ = true;
        }

        /// <summary>
        /// from value to chandle
        /// </summary>
        public virtual void TocHandle()
        {
            tohandled_ = true;
        }

        public DtArray GetSubscripts()
        {
            DtArray ret = new DtArray();
            try
            {
                // Lazy marshal
                Uberglue.DtValue_ListSubscripts(chandle_.pointer_, ret.chandle_.pointer_);
            }
            catch(Exception)
            {
                ConfigManager.Debug("unable to get subscripts");
            }
            return ret;
        }

        public DtValue GetSubscriptValue(DtValue subscript)
        {
            DtValue ret = new DtValue();
            try
            {
                Uberglue.DtValue_GetSubscriptValue(chandle_.pointer_, subscript.chandle_.pointer_, ret.chandle_);
            }
            catch (Exception)
            {
                ConfigManager.Debug("unable to get one subscript");
            }

            return ret;
        }

        public void SetSubscriptValue(DtValue sbuscript, DtValue value)
        {
            try
            {
                Uberglue.DtValue_SetSubscriptValue(chandle_.pointer_, sbuscript.chandle_.pointer_, value.chandle_.pointer_);
            }
            catch (Exception)
            {

            }
        }

        public void DeleteSubscript(DtValue subscript, bool custom_exception = false)
        {
            try
            {
                Uberglue.DtValue_DestroySubscript(chandle_.pointer_, subscript.chandle_.pointer_);
            }
            catch (Exception)
            {

            }
        }

        public long GetSize()
        {
            return GetSize(chandle_.pointer_);
        }
        public static long GetSize(IntPtr ptr)
        {
            try
            {
                IntPtr ret = new IntPtr();
                Uberglue.DtValue_GetSize(ptr, ret);

                return ret.ToInt64();
            }
            catch (Exception e)
            {
                ConfigManager.Debug("unable to get size: " + e.Message);
                return 0;
            }
        }

        /// <summary>
        /// shouldn't be used
        /// </summary>
        public DtValue ConvertTo(string newtype)
        {
            // switch
            DtValue To = new DtValue(newtype);
            Uberglue.DtValue_Convert(chandle_.pointer_, newtype, To.chandle_);
            return To;
        }

        /*
        public static bool operator==(DtValue x, DtValue y)
        {
            DtValue ret = new DtValue();
            try
            {
                Uberglue.DtValue_AreEqual(x.chandle_.pointer_, y.chandle_.pointer_, ret.chandle_.pointer_);
            }
            catch (Exception e)
            {
                ConfigManager.Debug("unable to compare dtvalues. " + e.Message);
            }

            return ret.chandle_.pointer_.ToInt32() == 0 ? false : true;
        }*/

        private bool disposed = false;
        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                // Free other state (managed objects).
                if (disposing) { }
                
                // Free unmanaged objects
                // Set large fields to null
                if (chandle_.pointer_ != IntPtr.Zero)
                    Uberglue.ReleaseHandle(chandle_.pointer_);

                disposed = true;
            }
            
        }

        /// <summary>
        /// destructor / finalizer
        /// </summary>
        ~DtValue()
        {
            Dispose(false);
        }

        // Implement IDisposable
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
    }

    public class DtDouble : DtValue
    {
        private double value_;
        public double Value { get { return value_; } set { value_ = value; } }

        public DtDouble() : base("Double")
        {
            value_ = 0.0;
        }

        public DtDouble(double v) : base("Double")
        {
            value_ = v;
        }


        public override void TocHandle()
        {
            // already allocated
            if ((!tohandled_) && (chandle_.pointer_ != IntPtr.Zero))
            {
                try
                {
                    Uberglue.DtValue_SetNumber(chandle_.pointer_, (double)value_);
                }
                catch (Exception e)
                {
                    ConfigManager.Debug("unable to set up handle: " + typename_ + ". " + e.Message);
                }
            }

            base.TocHandle();
        }

        public override void ToValue()
        {
            if ((!tovalued_) && (chandle_.pointer_ != IntPtr.Zero))
            {
                try
                {
                    IntPtr res = new IntPtr();
                    double[] res2 = new double[1];
                    Uberglue.DtValue_GetNumber(chandle_, res);
                    Marshal.Copy(res, res2, 0, 1);
                    value_ = res2[0];
                }
                catch (Exception e)
                {
                    ConfigManager.Debug("unable to convert to Double: " + e.Message);
                }
            }

            base.ToValue();
        }
    }

    public class DtString : DtValue
    {
        private string value_;
        public string Value { get { return value_; } set { value_ = value; } }

        public DtString() : base("String")
        {
            value_ = "";
        }

        public DtString(string v) : base("String")
        {
            value_ = v;
        }

        public override void TocHandle()
        {
            if (!tohandled_)
                Uberglue.NewString(value_, chandle_);
            
            base.TocHandle();
        }

        public override void ToValue()
        {
            if (!tovalued_)
                value_ = Marshal.PtrToStringAnsi(chandle_.pointer_);
            // Uberglue.DtValue_GetPointer();
            base.ToValue();
        }
    }

    /// <summary>
    /// Array of DtValues
    /// </summary>
    public class  DtArray : DtValue
    {
        private DtValue[] value_;
        public DtArray() : base("Array")
        {
        }

        public long Length()
        {
            return value_.Length;
        }

        public override void TocHandle()
        {
            base.TocHandle();
        }

        public override void ToValue()
        {
            //Uberglue.DtValue_SetSubscriptValue()
            if (!tovalued_)
            {
                long size = GetSize();
                value_ = new DtValue[size];

                for (long i = 0; i < size; i++)
                {
                    value_[i] = GetSubscriptValue(new DtDouble(i));
                }    
            }
            base.ToValue();
        }

        public DtValue this[int index]
        {
            get
            {
                if (!tovalued_) ToValue();
                return value_[index];
            }
            set
            {
                // value_[index] = value;
                SetSubscriptValue(new DtDouble(index), value);
                tohandled_ = false;
            }
        }

        public void DeleteItem(int index)
        {
            Uberglue.Array_DestroyElements(chandle_.pointer_, (ulong)index, (ulong)1);
        }

        public void InsertItem(int index, DtValue value)
        {
            Uberglue.Array_InsertElement(chandle_.pointer_, (ulong)index, value.chandle_.pointer_);
        }
    }

    /*
    /// <summary>
    /// dictionary of DtValues
    /// </summary>
    public class DtStructure : DtValue
    {
        private Dictionary<string, DtValue> value_;

        public DtStructure() : base("Structure")
        {
        }

        public long Length()
        {
            return value_.Count;
        }

        public override void TocHandle()
        {
            base.TocHandle();
        }

        public override void ToValue()
        {
            //Uberglue.DtValue_SetSubscriptValue()
            if (!tovalued_)
            {
                long size = GetSize();
                value_ = new Dictionary<string, DtValue>((int)size);

                for (long i = 0; i < size; i++)
                {
                    // value_.Add();
                }    
            }
            base.ToValue();
        }

        public DtValue this[int index]
        {
            get
            {
                if (!tovalued_) ToValue();
                return value_[index];
            }
            set
            {
                // value_[index] = value;
                SetSubscriptValue(new DtDouble(index), value);
                tohandled_ = false;
            }
        }

        public void DeleteItem(string key)
        {
            DeleteSubscript(new DtString(key));
        }
    }

    /// <summary>
    /// Array of doubles
    /// </summary>
    public class DtVector : DtValue
    {
        private double[] value_;

        public DtVector(double[] value) : base("Vector")
        {
            value_ = new double[value.Length];

            Uberglue.NewVector();
        }

        public override void TocHandle()
        {
            base.TocHandle();
        }

        public override void ToValue()
        {
            base.ToValue();
        }

        public double this[int index]
        {
            get
            {
                if (!tovalued_) ToValue();
                return value_[index];
            }
            set
            {
                value_[index] = value;
                tohandled_ = false;
            }
        }
    }

    /// <summary>
    /// Matrix of doubles
    /// </summary>
    public class DtMatrix : DtValue
    {
        private double[,] value_;
    }

    public class SlangFunction : DtValue
    {
        private string slangcode_;
        private List<SlangFunctionArgument> arguments_;

        public SlangFunction() : base("Slang")
        {

        }

        public int GetReferenceCount()
        {
            return GetSubscriptValue("ReferenceCount");
        }

        public string GetExpression()
        {
            return GetSubscriptValue("Expression");
        }

        public double HasReturn()
        {

        }

        public void CallFunction()
        {

        }

        private class SlangFunctionArgument
        {
            private Dictionary<string, object> infostruct_ = new Dictionary<string, object>();

        }
    }*/
}
