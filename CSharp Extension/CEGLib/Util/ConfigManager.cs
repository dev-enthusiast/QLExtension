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
                rengine_.Dispose();
            }
        }

        public void ConnectToSecDb(string dbname, string appname, string username)
        {
            if (string.IsNullOrEmpty(dbname))
                dbname = "!Bal Prod 1;ReadOnly{!Bal Prod 2}";

            if (string.IsNullOrEmpty(appname))
                appname = "CEGLib";

            if (string.IsNullOrEmpty(username))
                username = "E053608";

            Exelon.Data.SecDb.SecDbAPI.SecDbMsgOutputHandler msgouthandle = new Exelon.Data.SecDb.SecDbAPI.SecDbMsgOutputHandler(Debug);
            Exelon.Data.SecDb.SecDbAPI.SecDbMsgOutputHandler errormsgouthandle = new Exelon.Data.SecDb.SecDbAPI.SecDbMsgOutputHandler(Debug);

            Exelon.Data.SecDb.SecDbAPI.Database.SecDbInitialize(dbname, appname, username, msgouthandle, errormsgouthandle);
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
