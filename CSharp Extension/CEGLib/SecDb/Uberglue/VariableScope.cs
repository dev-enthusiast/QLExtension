using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib.SecDb
{
    public abstract class SlangVariableScope
    {
        protected string name_;
        protected DtValue scope_;
        public SlangVariableScope(string name)
        {
            name_ = name;
            scope_ = new DtValue();
        }

        public void GetKeys()
        {
            DtValue value = new DtArray();
            try
            {
                Uberglue.VariableScope_ListVariables(scope_.chandle_.pointer_, value.chandle_);

                value.ToValue();
                value.GetSubscripts();
            }
            catch (Exception e) 
            {
                ConfigManager.Debug("can't get keys from scope " + name_);
            }
        }

        public DtValue GetItem(string variablename)
        {
            DtValue ret = new DtValue();
            try
            {
                Uberglue.SlangVariable_GetValue(scope_.chandle_.pointer_, variablename, ret.chandle_);
            }
            catch (Exception e)
            {
                ConfigManager.Debug("can't get variable " + variablename + " from scope " + name_);
            }
            return ret;
        }

        public void SetItem(string variablename, DtValue value)
        {
            try
            {
                Uberglue.SlangVariable_SetValue(scope_.chandle_.pointer_, variablename, value.chandle_.pointer_);
            }
            catch (Exception e)
            {
                ConfigManager.Debug("can't set variable " + variablename + " from scope " + name_);
            }
        }

        public void DeleteItem(string variablename)
        {
            try
            {
                Uberglue.SlangVariable_Destroy(scope_.chandle_.pointer_, variablename);
            }
            catch (Exception e)
            {
                ConfigManager.Debug("can't delete variable " + variablename + " from scope " + name_);
            }
        }

        /// <summary>
        /// Evaluate Slang code in the current variable scope.
        /// </summary>
        /// <param name="slangcode"></param>
        public DtValue Evaluate(string slangcode)
        {
            DtValue ret = new DtValue();
            try
            {
                DtValue ret_code = new DtValue();
                DtValue ret_type = new DtValue();

                Uberglue.PushVariableScope(scope_.chandle_.pointer_);

                Uberglue.SlangEval(slangcode, ret_code.chandle_.pointer_, ret_type.chandle_.pointer_, ret.chandle_);
            }
            catch (Exception e)
            {
                ConfigManager.Debug("can't execute slang code " + slangcode + " in scope " + name_);
            }
            finally
            {
                Uberglue.PopVariableScope();
            }

            return ret;
        }
    }
    public class NamedSlangVariableScope : SlangVariableScope
    {
        private int create_;
        public NamedSlangVariableScope(string name) : base(name)
        {
            // If the named scope does not already exists, should we create it (True), or raise a KeyError (False)
            create_ = 1;
            Uberglue.GetNamedVariableScope(name_, create_, scope_.chandle_);
        }

        public override string ToString()
        {
            return "NamedSlangVariableScope({!r})."+name_;
        }
    }

    /// <summary>
    ///  Every local scope is unique; in particular, even if two local
    ///  variables scopes share the same name, they contain different
    ///  variable bindings.  The name argument is purely for informational
    ///  purposes, and this will be the "function name" that appears in the
    ///  array produced by the Slang function CallStack().
    /// </summary>
    public class LocalSlangVariableScope : SlangVariableScope
    {
        public LocalSlangVariableScope(string name) : base(name)
        {
            Uberglue.CreateLocalVariableScope(name_, scope_.chandle_);
        }

        public override string ToString()
        {
            return "LocalSlangVariableScope({!r})." + name_;
        }
    }
}
