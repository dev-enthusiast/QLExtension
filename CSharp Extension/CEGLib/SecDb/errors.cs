using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;

namespace CEGLib.SecDB
{
    [Serializable]
    public class SecDbException : Exception
    {
        public SecDbException() { }
        public SecDbException(string message) : base(message) {}
        public SecDbException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SecDbIndexException : Exception
    {
        public SecDbIndexException() { }
        public SecDbIndexException(string message) : base(message) {}
        public SecDbIndexException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbIndexException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SecDbKeyException : Exception
    {
        public SecDbKeyException() { }
        public SecDbKeyException(string message) : base(message) {}
        public SecDbKeyException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbKeyException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SlangEvaluationException : Exception
    {
        public SlangEvaluationException() { }
        public SlangEvaluationException(string message) : base(message) {}
        public SlangEvaluationException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SlangEvaluationException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SecDbInvalidClassNameException : Exception
    {
        public SecDbInvalidClassNameException() { }
        public SecDbInvalidClassNameException(string message) : base(message) {}
        public SecDbInvalidClassNameException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbInvalidClassNameException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SecDbLargeResultException : Exception
    {
        public SecDbLargeResultException() { }
        public SecDbLargeResultException(string message) : base(message) {}
        public SecDbLargeResultException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbLargeResultException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }

    [Serializable]
    public class SecDbInvalidTimeException : Exception
    {
        public SecDbInvalidTimeException() { }
        public SecDbInvalidTimeException(string message) : base(message) {}
        public SecDbInvalidTimeException(string message, Exception innerException) : base(message, innerException) { }      
        // Ensure Exception is Serializable
        protected SecDbInvalidTimeException(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { }
    }
}
