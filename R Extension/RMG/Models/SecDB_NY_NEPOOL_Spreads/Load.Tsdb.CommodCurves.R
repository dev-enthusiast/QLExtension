# Load Tsdb from Commod Curves

# Created by Wang Yu, 13-Jan-2006

Load.Tsdb.DAMCurves <- function(MatlabServer, CommodCurves, DateParms)

{
  require(R.matlab)
  
  if (length(MatlabServer) == 0)
      MatlabServer=Matlab(host="localhost",port=9999)
  
  if (!open(MatlabServer))
  throw("MatlabServer is not Running: Waited 30 seconds")
  
  setVariable(MatlabServer,CommodCurves =  CommodCurves )
  setVariable(MatlabServer,DateParms = DateParms)
  evaluate(MatlabServer, "DateParms.Start = datenum(DateParms.Start)")
  evaluate(MatlabServer, "DateParms.End = datenum(DateParms.End)")
  
  evaluate(MatlabServer, "TsdbCurves = Commod_to_tsdb_DAM_RMG(CommodCurves)")
  evaluate(MatlabServer, "TsdbInfo = LoadTsdb_RMG(DateParms,TsdbCurves)")
  evaluate(MatlabServer, "TsdbInfo = Recognize_Bucket_RMG(TsdbInfo)")
  evaluate(MatlabServer, "[Y,M,D,H] = datevec(TsdbInfo.TimeStamp)")
  evaluate(MatlabServer, "TsdbInfo.TimeStamp = [Y,M,D,H]")
  evaluate(MatlabServer, "TsdbInfo.HisStart = datestr(TsdbInfo.HisStart,29)")
  evaluate(MatlabServer, "TsdbInfo.HisEnd = datestr(TsdbInfo.HisEnd,29)")
  TsdbInfo = getVariable(MatlabServer, "TsdbInfo")
  TsdbInfo <- TsdbInfo$TsdbInfo
  names(TsdbInfo) = c("Young","HisStart","HisEnd","TimeStamp","TsdbHist",
                      "TsdbSymbols","TsdbSymbols_Unique","TsdbSymbols_Index","TsdbBuckets")
                      
  close(MatlabServer)

  TsdbInfo

}

Load.Tsdb.FutCurves <- function(MatlabServer, CommodCurves, DateParms)

{
  require(R.matlab)
  
  if (length(MatlabServer) == 0)
      MatlabServer=Matlab(host="localhost",port=9999)
  
  if (!open(MatlabServer))
  throw("MatlabServer is not Running: Waited 30 seconds")
  
  setVariable(MatlabServer,CommodCurves =  CommodCurves )
  setVariable(MatlabServer,DateParms = DateParms)
  evaluate(MatlabServer, "DateParms.Start = datenum(DateParms.Start)")
  evaluate(MatlabServer, "DateParms.End = datenum(DateParms.End)")
  
  evaluate(MatlabServer, "TsdbCurves = Commod_to_tsdb_RMG(CommodCurves)")
  evaluate(MatlabServer, "TsdbInfo = LoadTsdb_RMG(DateParms,TsdbCurves)")
  evaluate(MatlabServer, "TsdbInfo = Recognize_Bucket_RMG(TsdbInfo)")
  evaluate(MatlabServer, "[Y,M,D,H] = datevec(TsdbInfo.TimeStamp)")
  evaluate(MatlabServer, "TsdbInfo.TimeStamp = [Y,M,D,H]")
  evaluate(MatlabServer, "TsdbInfo.HisStart = datestr(TsdbInfo.HisStart,29)")
  evaluate(MatlabServer, "TsdbInfo.HisEnd = datestr(TsdbInfo.HisEnd,29)")
  TsdbInfo = getVariable(MatlabServer, "TsdbInfo")
  TsdbInfo <- TsdbInfo$TsdbInfo
  names(TsdbInfo) = c("Young","HisStart","HisEnd","TimeStamp","TsdbHist",
                      "TsdbSymbols","TsdbSymbols_Unique","TsdbSymbols_Index","TsdbBuckets")
                      
  close(MatlabServer)

  TsdbInfo

}