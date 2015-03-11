using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;

namespace CEGLib.SecDb
{
    [Serializable]
    public class SecDbException : Exception
    {
        private ErrorCode errorcode_;

        public SecDbException(int ecode) { errorcode_ = (ErrorCode)ecode; }
        public SecDbException(int ecode, string message) : base(message) { errorcode_ = (ErrorCode)ecode; }
        public SecDbException(int ecode, string message, Exception innerException) : base(message, innerException) { errorcode_ = (ErrorCode)ecode; }      
        // Ensure Exception is Serializable
        protected SecDbException(int ecode, SerializationInfo info, StreamingContext ctxt) : base(info, ctxt) { errorcode_ = (ErrorCode)ecode; }
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

    public enum ErrorCode : int
    {
        NO_ERROR = 0,
        UNKNOWN_CPP_ERROR = 1,
        ASSERTION_ERROR = 2,
        BAD_ARGUMENT_ERROR = 3,
        BAD_DATABASE_NAME_ERROR = 4,
        BAD_SECURITY_NAME_ERROR = 5,
        MEMORY_ERROR = 6,
        SECDB_CALL_FAILED = 7,
        SLANG_EVAL_FAILED = 8,
        BAD_SUBSCRIPT = 9,
        BAD_SCOPE_NAME = 10,
        BAD_VARIABLE_NAME = 11,
        OVERFLOW_ERROR = 12,
        NO_LAST_ERROR = 13,
        VARIABLE_IS_PROTECTED = 14,
        EMPTY_STACK_ERROR = 15,
        LIST_IS_TOO_LARGE_ERROR = 16,
        BAD_CLASS_NAME_ERROR = 17
    };
}
