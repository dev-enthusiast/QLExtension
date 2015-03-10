﻿using RDotNet.Internals;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security.Permissions;

namespace RDotNet
{
   /// <summary>
   /// A collection of integers from <c>-2^31 + 1</c> to <c>2^31 - 1</c>.
   /// </summary>
   /// <remarks>
   /// The minimum value of IntegerVector is different from that of System.Int32 in .NET Framework.
   /// </remarks>
   [SecurityPermission(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
   public class IntegerVector : Vector<int>
   {
      /// <summary>
      /// Creates a new empty IntegerVector with the specified length.
      /// </summary>
      /// <param name="engine">The <see cref="REngine"/> handling this instance.</param>
      /// <param name="length">The length.</param>
      /// <seealso cref="REngineExtension.CreateIntegerVector(REngine, int)"/>
      public IntegerVector(REngine engine, int length)
         : base(engine, SymbolicExpressionType.IntegerVector, length)
      { }

      /// <summary>
      /// Creates a new IntegerVector with the specified values.
      /// </summary>
      /// <param name="engine">The <see cref="REngine"/> handling this instance.</param>
      /// <param name="vector">The values.</param>
      /// <seealso cref="REngineExtension.CreateIntegerVector(REngine, IEnumerable{int})"/>
      public IntegerVector(REngine engine, IEnumerable<int> vector)
         : base(engine, SymbolicExpressionType.IntegerVector, vector)
      { }

      /// <summary>
      /// Creates a new IntegerVector with the specified values.
      /// </summary>
      /// <param name="engine">The <see cref="REngine"/> handling this instance.</param>
      /// <param name="vector">The values.</param>
      /// <seealso cref="REngineExtension.CreateIntegerVector(REngine, IEnumerable{int})"/>
      public IntegerVector(REngine engine, int[] vector)
         : base(engine, SymbolicExpressionType.IntegerVector, vector.Length)
      {
         Marshal.Copy(vector, 0, DataPointer, vector.Length);
      }

      /// <summary>
      /// Creates a new instance for an integer vector.
      /// </summary>
      /// <param name="engine">The <see cref="REngine"/> handling this instance.</param>
      /// <param name="coerced">The pointer to an integer vector.</param>
      protected internal IntegerVector(REngine engine, IntPtr coerced)
         : base(engine, coerced)
      { }

      /// <summary>
      /// Gets or sets the element at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index of the element to get or set.</param>
      /// <returns>The element at the specified index.</returns>
      public override int this[int index]
      {
         get
         {
            if (index < 0 || Length <= index)
            {
               throw new ArgumentOutOfRangeException();
            }
            using (new ProtectedPointer(this))
            {
               int offset = GetOffset(index);
               return Marshal.ReadInt32(DataPointer, offset);
            }
         }
         set
         {
            if (index < 0 || Length <= index)
            {
               throw new ArgumentOutOfRangeException();
            }
            using (new ProtectedPointer(this))
            {
               int offset = GetOffset(index);
               Marshal.WriteInt32(DataPointer, offset, value);
            }
         }
      }

      /// <summary>
      /// Efficient conversion from R vector representation to the array equivalent in the CLR
      /// </summary>
      /// <returns>Array equivalent</returns>
      protected override int[] GetArrayFast()
      {
         var res = new int[this.Length];
         Marshal.Copy(DataPointer, res, 0, res.Length);
         return res;
      }

      /// <summary>
      /// Efficient initialisation of R vector values from an array representation in the CLR
      /// </summary>
      protected override void SetVectorDirect(int[] values)
      {
         Marshal.Copy(values, 0, DataPointer, values.Length);
      }

      /// <summary>
      /// Gets the size of an integer in byte.
      /// </summary>
      protected override int DataSize
      {
         get { return sizeof(int); }
      }

      /// <summary>
      /// Copies the elements to the specified array.
      /// </summary>
      /// <param name="destination">The destination array.</param>
      /// <param name="length">The length to copy.</param>
      /// <param name="sourceIndex">The first index of the vector.</param>
      /// <param name="destinationIndex">The first index of the destination array.</param>
      public new void CopyTo(int[] destination, int length, int sourceIndex = 0, int destinationIndex = 0)
      {
         if (destination == null)
         {
            throw new ArgumentNullException("destination");
         }
         if (length < 0)
         {
            throw new IndexOutOfRangeException("length");
         }
         if (sourceIndex < 0 || Length < sourceIndex + length)
         {
            throw new IndexOutOfRangeException("sourceIndex");
         }
         if (destinationIndex < 0 || destination.Length < destinationIndex + length)
         {
            throw new IndexOutOfRangeException("destinationIndex");
         }

         int offset = GetOffset(sourceIndex);
         IntPtr pointer = IntPtr.Add(DataPointer, offset);
         Marshal.Copy(pointer, destination, destinationIndex, length);
      }

      /// <summary>
      /// Gets the code used for NA for integer vectors
      /// </summary>
      protected int NACode { get { return int.MinValue; } }
   }
}