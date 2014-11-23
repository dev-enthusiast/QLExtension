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
    public class RTDTimeServer : ExcelRtdServer
    {
        private readonly List<Topic> topics = new List<Topic>();
       private Timer timer;

       public RTDTimeServer()
       {
          timer = new Timer(Callback);
       }
 
       private void Start()
       {
          timer.Change(500, 500);
       }
 
       private void Stop()
       {
          timer.Change(-1, -1);
       }
 
       private void Callback(object o)
       {            
          Stop();
          foreach (Topic topic in topics)
             topic.UpdateValue(GetTime());
          Start();
       }
 
       private static string GetTime()
       {
          return DateTime.Now.ToString("HH:mm:ss.fff");
       }
 
       protected override void ServerTerminate()
       {
          timer.Dispose();
          timer = null;
       }
 
       protected override object ConnectData(Topic topic, IList<string> topicInfo, ref bool newValues)
       {
          topics.Add(topic);
          Start();
          return GetTime();
       }
 
       protected override void DisconnectData(Topic topic)
       {
          topics.Remove(topic);
          if (topics.Count == 0)
             Stop();
       }
    }
}
