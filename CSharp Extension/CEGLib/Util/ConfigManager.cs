using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RDotNet;

namespace CEGLib
{
    public sealed class ConfigManager
    {
        static readonly ConfigManager _instance = new ConfigManager();
        private REngine rengine_;
        public static ConfigManager Instance
        {
            get { return _instance; }
        }
        public REngine CalcEngine
        {
            get { return rengine_; }
        }

        private ConfigManager()
        {
            REngine.SetEnvironmentVariables();
            rengine_ = REngine.GetInstance();
        }

        ~ConfigManager()
        {
            if (rengine_ != null)
            {
                rengine_.Close();
                rengine_.Dispose();
            }
        }

        

        public delegate void VoidStringDelegate(string msg);
        public VoidStringDelegate debughandler_;

        public void Debug(string msg)
        {
            if (debughandler_ != null)
            {
                debughandler_(msg);
            }
        }
    }
}
