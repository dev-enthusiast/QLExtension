using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using ExcelDna.Integration.Rtd;
using System.Threading.Tasks;
using System.Threading;

/// http://putridparrot.com/blog/a-real-time-data-rtd-server-using-excel-dna-in-excel/
namespace QLXLL
{
    [ComVisible(true)]
    public class RTDTimerServer : IRtdServer
    {
        public class PayLoad
        {
            public bool IsUpdated = false;
            public double interval;
            public string functionName;
            public System.Timers.Timer timer;
            List<string> parameters;        // the first one is function name and the rest is its parameters

            public object result;

            public event EventHandler OnComplete;

            public PayLoad(double interval_, string function_, List<string> parameters_)
            {
                interval = interval_;
                functionName = function_;
                parameters = parameters_;
                timer = new System.Timers.Timer();
                timer.Interval = (int)interval * 1000;
                timer.Elapsed += new System.Timers.ElapsedEventHandler(timer_Elapsed);
                timer.Start();
            }

            void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
            {
                if (functionName == "qlRefreshVolParams")
                {
                    //qlEquityVolatility.qlEquityParamsFromDB(parameters[0]);
                    //object resultInArray = qlEquityVolatility.qlDisplayEquityVolParams("EParam@" + parameters[0], 0, null);
                    // we need to convert result into the format of {1, 2, 3, 4; 10, 20, 30, 40; 100, 200, 300, 400}
                    //result = ConvertToExcelFormat((Array)resultInArray);
                    result = "";
                    this.IsUpdated = true;
                    OnComplete(this, null);
                }
                else if (functionName == "NOW")
                {
                    result = DateTime.Now;
                    this.IsUpdated = true;
                    OnComplete(this, null);
                }
            }

            private string ConvertToExcelFormat(Array array)
            {
                string result = string.Empty;
                for (int j = 0; j < array.GetLength(1); j++)
                {
                    for (int i = 0; i < array.GetLength(0); i++)
                    {
                        if (i == 0)
                            result = result + array.GetValue(i, j);
                        else
                            result = result + "," + array.GetValue(i, j);
                    }
                    if (j < array.GetLength(1) - 1)
                        result = result + ";";
                }
                result = "{ " + result + " }";
                return result;
            }

            public void stop()
            {
                timer.Stop();
            }
        }

        private IRTDUpdateEvent _callback;
        private Dictionary<int, PayLoad> taskMap;   // we use forms.Timer as it calls back on native window's thread

        #region IRtdServer Members
        /* we assume that the input strings are: repeat interval in seconds, function name, and input parameters */
        public object ConnectData(int topicId, ref Array Strings, ref bool GetNewValues)
        {
            if (taskMap.ContainsKey(topicId))
            {
                // return "rtd task already set";
                return DateTime.Now;
            }

            if (Strings.Length < 2)
            {
                throw new Exception("input parameters should at least contains interval and function names");
            }

            double interval = Double.Parse((string)Strings.GetValue(0));
            string functionName = (string)Strings.GetValue(1);

            List<string> parameters = new List<string>();
            if (Strings.Length > 2)
            {
                for (int i = 2; i < Strings.Length; ++i)
                {
                    parameters.Add((string)Strings.GetValue(i));
                }
            }

            PayLoad task = new PayLoad(interval, functionName, parameters);
            task.OnComplete += new EventHandler(task_OnComplete);
            taskMap.Add(topicId, task);
            return DateTime.Now;
        }

        void task_OnComplete(object sender, EventArgs e)
        {
            _callback.UpdateNotify();
        }

        public void DisconnectData(int topicId)
        {

        }

        public int Heartbeat()
        {
            return 1;
        }

        public Array RefreshData(ref int TopicCount)
        {
            object[,] results = new object[2, taskMap.Keys.Count];
            TopicCount = 0;

            foreach(int topicID in taskMap.Keys)
            {
                if (taskMap[topicID].IsUpdated == true)
                {
                    results[0, TopicCount] = topicID;
                    results[1, TopicCount] = taskMap[topicID].result;
                    taskMap[topicID].IsUpdated = false;     // mark it back to false
                    TopicCount++;
                }
            }

            object[,] temp = new object[2, TopicCount];
            for (int i = 0; i < TopicCount; i++)
            {
                temp[0, i] = results[0, i];
                temp[1, i] = results[1, i];
            }

            return temp;
        }

        public int ServerStart(IRTDUpdateEvent CallbackObject)
        {
            _callback = CallbackObject;
            taskMap = new Dictionary<int, PayLoad>();
            return 1;
        }

        public void ServerTerminate()
        {
            foreach(PayLoad task in taskMap.Values)
            {
                task.stop();
            }
        }
        #endregion

        private string GetTime()
        {
            return DateTime.Now.ToString("HH:mm:ss.fff");
        }
    }
}
