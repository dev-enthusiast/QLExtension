using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib.SecDb
{
    public struct ErrorCode
    {
        public int NO_ERROR = 0;
        public int UNKNOWN_CPP_ERROR = 1;
        public int ASSERTION_ERROR = 2;
        public int BAD_ARGUMENT_ERROR = 3;
        public int BAD_DATABASE_NAME_ERROR = 4;
        public int BAD_SECURITY_NAME_ERROR = 5;
        public int MEMORY_ERROR = 6;
        public int SECDB_CALL_FAILED = 7;
        public int SLANG_EVAL_FAILED = 8;
        public int BAD_SUBSCRIPT = 9;
        public int BAD_SCOPE_NAME = 10;
        public int BAD_VARIABLE_NAME = 11;
        public int OVERFLOW_ERROR = 12;
        public int NO_LAST_ERROR = 13;
        public int VARIABLE_IS_PROTECTED = 14;
        public int EMPTY_STACK_ERROR = 15;
        public int LIST_IS_TOO_LARGE_ERROR = 16;
        public int BAD_CLASS_NAME_ERROR = 17;
    };
}
