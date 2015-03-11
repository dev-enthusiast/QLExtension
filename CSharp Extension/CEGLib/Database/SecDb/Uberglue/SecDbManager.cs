using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib.SecDb
{
    public class SecDbManager
    {
        private static List<string> scopes_ = new List<string>();
        private static Dictionary<string, Handle> handles_ = new Dictionary<string, Handle>();
        private static Dictionary<string, SlangVariableScope> scopes_ = new Dictionary<string, SlangVariableScope>();

        public static List<string> GetKeys()
        {
            scopes_.Clear();

            Interface.EvaluateSlang("scopes()");

            return scopes_;
        }

        public static void AddHandle(string name, Handle handle)
        {
            handles_.Add(name, handle);
        }

        public static void RemoveHandle(string name)
        {
            if (handles_.ContainsKey(name))
            {
                Uberglue.ReleaseHandle(handles_[name].pointer_);
                handles_.Remove(name);
            }

        }

        public static void RemoveAllHandles()
        {
            foreach (var kvp in handles_)
            {
                Uberglue.ReleaseHandle(kvp.Value.pointer_);
            }
            handles_.Clear();
        }

        public static SlangVariableScope GetItem(string scopename)
        {
            if (scopes_.Contains(scopename))
                return new NamedSlangVariableScope();
        }

        public override string ToString()
        {
            return "AllSlangVariableScopes()";
        }
    }
}
