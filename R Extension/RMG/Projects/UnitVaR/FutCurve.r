# Equivalent to FutCurve in Matlab

FutCurve <- function(MatlabServer, CommodCurve, DateParms)
{
  require(R.matlab)

  if (length(MatlabServer) == 0)
      { MatlabServer=Matlab(host="localhost",port=9999)
        if (!open(MatlabServer)){
           throw("MatlabServer is not Running: Waited 30 seconds")
           LocalMServer <- 1 }
      }else 
           LocalMServer <- 0
      
                                                              
  setVariable(MatlabServer,CommodCurve =  CommodCurve )
  setVariable(MatlabServer,DateParms =  DateParms )
  evaluate(MatlabServer, "StartCon = datenum(DateParms.StartCon)")
  evaluate(MatlabServer, "EndCon = datenum(DateParms.EndCon)")
  evaluate(MatlabServer, "StartDT = datenum(DateParms.StartDT)")
  evaluate(MatlabServer, "EndDT = datenum(DateParms.EndDT)")  
    
  evaluate(MatlabServer, "[FutDates,FrwrExp,FutCurves] = futCurve(char(CommodCurve),StartCon,EndCon,StartDT,EndDT)")
  evaluate(MatlabServer, "FutDates = datestr(FutDates,29)")
  evaluate(MatlabServer, "FrwrExp = datestr(FrwrExp,29)")
    
  evaluate(MatlabServer, "FutCurve = struct('FutDates',FutDates,'FutExp',FrwrExp,'FutCurves',FutCurves)")
  FutCurve <- getVariable(MatlabServer, "FutCurve")
  FutCurve <- FutCurve$FutCurve
  
  names(FutCurve) <- attr(FutCurve, "dimnames")[[1]]
                      
  if (LocalMServer == 1)
    close(MatlabServer)

  FutCurve


}