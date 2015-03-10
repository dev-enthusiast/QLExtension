﻿using RDotNet.Internals;
using System;
using System.Security.Permissions;

namespace RDotNet
{
   /// <summary>
   /// Provides extension methods for <see cref="SymbolicExpression"/>.
   /// </summary>
   [SecurityPermission(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
   public static class SymbolicExpressionExtension
   {
      /// <summary>
      /// Gets whether the specified expression is list.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is list.</returns>
      public static bool IsList(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         // See issue 81. Rf_isList in the R API is NOT the correct thing to use (yes, hard to be more conter-intuitive)
         return (expression.Type == SymbolicExpressionType.List ||
            // ?is.list ==> "is.list returns TRUE if and only if its argument is a list or a pairlist of length > 0"
            (expression.Type == SymbolicExpressionType.Pairlist && expression.GetFunction<Rf_length>()(expression.DangerousGetHandle()) > 0)); 
      }

      /// <summary>
      /// Converts the specified expression to a GenericVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The GenericVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static GenericVector AsList(this SymbolicExpression expression)
      {
         return asList(expression);
      }

      /// <summary>
      /// A cache of the REngine - WARNING this assumes there can be only one per process, initialized once only.
      /// </summary>
      private static REngine engine = null;
      private static Function asListFunction = null;
      private static GenericVector asList(SymbolicExpression expression)
      {
         if (!object.ReferenceEquals(engine, expression.Engine) || engine == null)
         {
            engine = expression.Engine;
            asListFunction = null;
         }
         if(asListFunction==null) 
            asListFunction = engine.Evaluate("invisible(as.list)").AsFunction();
         var newExpression = asListFunction.Invoke(expression);
         return new GenericVector(newExpression.Engine, newExpression.DangerousGetHandle());
      }


      /// <summary>
      /// Gets whether the specified expression is data frame.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is data frame.</returns>
      public static bool IsDataFrame(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isFrame>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Converts the specified expression to a DataFrame.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The DataFrame. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static DataFrame AsDataFrame(this SymbolicExpression expression)
      {
         if (!expression.IsDataFrame())
         {
            return null;
         }
         return new DataFrame(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets whether the specified expression is an S4 object.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is an S4 object.</returns>
      public static bool IsS4(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isS4>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Coerce the specified expression to an S4 object.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The DataFrame. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static S4Object AsS4(this SymbolicExpression expression)
      {
         if (!expression.IsS4())
         {
            return null;
         }
         return new S4Object(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets whether the specified expression is vector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is vector.</returns>
      public static bool IsVector(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isVector>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Converts the specified expression to a DynamicVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The DynamicVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static DynamicVector AsVector(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), expression.Type);
         return new DynamicVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to a LogicalVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static LogicalVector AsLogical(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.LogicalVector);
         return new LogicalVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to an IntegerVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static IntegerVector AsInteger(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.IntegerVector);
         return new IntegerVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to a NumericVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static NumericVector AsNumeric(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.NumericVector);
         return new NumericVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to a CharacterVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static CharacterVector AsCharacter(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = IntPtr.Zero;
         if (expression.IsFactor())
            coerced = expression.GetFunction<Rf_asCharacterFactor>()(expression.DangerousGetHandle());
         else
            coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.CharacterVector);
         return new CharacterVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to a ComplexVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static ComplexVector AsComplex(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.ComplexVector);
         return new ComplexVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Converts the specified expression to a RawVector.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalVector. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static RawVector AsRaw(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }
         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.RawVector);
         return new RawVector(expression.Engine, coerced);
      }

      /// <summary>
      /// Gets whether the specified expression is matrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is matrix.</returns>
      public static bool IsMatrix(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isMatrix>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Converts the specified expression to a LogicalMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The LogicalMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static LogicalMatrix AsLogicalMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.LogicalVector)
            {
               return new LogicalMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.LogicalVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new LogicalMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Converts the specified expression to an IntegerMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The IntegerMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static IntegerMatrix AsIntegerMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.IntegerVector)
            {
               return new IntegerMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.IntegerVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new IntegerMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Converts the specified expression to a NumericMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The NumericMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static NumericMatrix AsNumericMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.NumericVector)
            {
               return new NumericMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.NumericVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new NumericMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Converts the specified expression to a CharacterMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The CharacterMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static CharacterMatrix AsCharacterMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.CharacterVector)
            {
               return new CharacterMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.CharacterVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new CharacterMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Converts the specified expression to a ComplexMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The ComplexMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static ComplexMatrix AsComplexMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.ComplexVector)
            {
               return new ComplexMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.ComplexVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new ComplexMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Converts the specified expression to a RawMatrix.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The RawMatrix. Returns <c>null</c> if the specified expression is not vector.</returns>
      public static RawMatrix AsRawMatrix(this SymbolicExpression expression)
      {
         if (!expression.IsVector())
         {
            return null;
         }

         int rowCount = 0;
         int columnCount = 0;

         if (expression.IsMatrix())
         {
            if (expression.Type == SymbolicExpressionType.RawVector)
            {
               return new RawMatrix(expression.Engine, expression.DangerousGetHandle());
            }
            else
            {
               rowCount = expression.GetFunction<Rf_nrows>()(expression.DangerousGetHandle());
               columnCount = expression.GetFunction<Rf_ncols>()(expression.DangerousGetHandle());
            }
         }

         if (columnCount == 0)
         {
            rowCount = expression.GetFunction<Rf_length>()(expression.DangerousGetHandle());
            columnCount = 1;
         }

         IntPtr coerced = expression.GetFunction<Rf_coerceVector>()(expression.DangerousGetHandle(), SymbolicExpressionType.RawVector);
         var dim = new IntegerVector(expression.Engine, new[] { rowCount, columnCount });
         SymbolicExpression dimSymbol = expression.Engine.GetPredefinedSymbol("R_DimSymbol");
         var matrix = new RawMatrix(expression.Engine, coerced);
         matrix.SetAttribute(dimSymbol, dim);
         return matrix;
      }

      /// <summary>
      /// Specifies the expression is an <see cref="REnvironment"/> object or not.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if it is an environment.</returns>
      public static bool IsEnvironment(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isEnvironment>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets the expression as an <see cref="REnvironment"/>.
      /// </summary>
      /// <param name="expression">The environment.</param>
      /// <returns>The environment.</returns>
      public static REnvironment AsEnvironment(this SymbolicExpression expression)
      {
         if (!expression.IsEnvironment())
         {
            return null;
         }
         return new REnvironment(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Specifies the expression is an <see cref="RDotNet.Expression"/> object or not.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if it is an expression.</returns>
      public static bool IsExpression(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isExpression>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets the expression as an <see cref="RDotNet.Expression"/>.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The expression.</returns>
      public static Expression AsExpression(this SymbolicExpression expression)
      {
         if (!expression.IsExpression())
         {
            return null;
         }
         return new Expression(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Specifies the expression is a symbol object or not.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if it is a symbol.</returns>
      public static bool IsSymbol(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isSymbol>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets the expression as a symbol.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The symbol.</returns>
      public static Symbol AsSymbol(this SymbolicExpression expression)
      {
         if (!expression.IsSymbol())
         {
            return null;
         }
         return new Symbol(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Specifies the expression is a language object or not.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if it is a language.</returns>
      public static bool IsLanguage(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isLanguage>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets the expression as a language.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The language.</returns>
      public static Language AsLanguage(this SymbolicExpression expression)
      {
         if (!expression.IsLanguage())
         {
            return null;
         }
         return new Language(expression.Engine, expression.DangerousGetHandle());
      }

      /// <summary>
      /// Specifies the expression is a function object or not.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if it is a function.</returns>
      public static bool IsFunction(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         return expression.GetFunction<Rf_isFunction>()(expression.DangerousGetHandle());
      }

      /// <summary>
      /// Gets the expression as a function.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The function.</returns>
      public static Function AsFunction(this SymbolicExpression expression)
      {
         switch (expression.Type)
         {
            case SymbolicExpressionType.Closure:
               return new Closure(expression.Engine, expression.DangerousGetHandle());

            case SymbolicExpressionType.BuiltinFunction:
               return new BuiltinFunction(expression.Engine, expression.DangerousGetHandle());

            case SymbolicExpressionType.SpecialFunction:
               return new SpecialFunction(expression.Engine, expression.DangerousGetHandle());

            default:
               throw new ArgumentException();
         }
      }

      /// <summary>
      /// Gets whether the specified expression is factor.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns><c>True</c> if the specified expression is factor.</returns>
      public static bool IsFactor(this SymbolicExpression expression)
      {
         if (expression == null)
         {
            throw new ArgumentNullException();
         }
         var handle = expression.DangerousGetHandle();
         return expression.GetFunction<Rf_isFactor>()(handle);
      }

      /// <summary>
      /// Gets the expression as a factor.
      /// </summary>
      /// <param name="expression">The expression.</param>
      /// <returns>The factor.</returns>
      public static Factor AsFactor(this SymbolicExpression expression)
      {
         if (!IsFactor(expression))
         {
            throw new ArgumentException("Not a factor.", "expression");
         }
         return new Factor(expression.Engine, expression.DangerousGetHandle());
      }
   }
}
