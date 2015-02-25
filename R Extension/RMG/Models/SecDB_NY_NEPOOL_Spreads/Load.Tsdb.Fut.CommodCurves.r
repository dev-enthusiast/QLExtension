# Load historical data FutCurves given a Commod Name

Load.Tsdb.Fut.CommodCurves <- function(MatlabServer, CommodCurves, DateParms)

{
  require(R.matlab)

  if (length(MatlabServer) == 0)
      { MatlabServer=Matlab(host="localhost",port=9999)
        if (!open(MatlabServer)){
           throw("MatlabServer is not Running: Waited 30 seconds")
           LocalMServer <- 1 }
      }else 
           LocalMServer <- 0
      
                                                              
  setVariable(MatlabServer,CommodCurves =  CommodCurves )
  setVariable(MatlabServer,DateParms =  DateParms )
  evaluate(MatlabServer, "StartCon = datenum(DateParms.StartCon)")
  evaluate(MatlabServer, "EndCon = datenum(DateParms.EndCon)")
  evaluate(MatlabServer, "StartDT = datenum(DateParms.StartDT)")
  evaluate(MatlabServer, "EndDT = datenum(DateParms.EndDT)")  
  
  evaluate(MatlabServer, "TsdbCurves = Commod_to_tsdb_RMG(CommodCurves)")
  
  evaluate(MatlabServer, "[FutDates,FrwrExp,FutCurves] = futCurve(char(TsdbCurves),StartCon,EndCon,StartDT,EndDT)")
  evaluate(MatlabServer, "FutDates = datestr(FutDates,29)")
  evaluate(MatlabServer, "FrwrExp = datestr(FrwrExp,29)")
    
  evaluate(MatlabServer, "FutInfo = struct('FutDates',FutDates,'FutExp',FrwrExp,'FutCurves',FutCurves')")
  FutInfo <- getVariable(MatlabServer, "FutInfo")
  FutInfo <- FutInfo$FutInfo
  
  names(FutInfo) <- attr(FutInfo, "dimnames")[[1]]
                      
  if (LocalMServer == 1)
    close(MatlabServer)

  FutInfo

}