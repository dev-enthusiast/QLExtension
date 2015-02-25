#########################################################################
# use the RODBC package to read a sheet
# 
xlsReadSheet <- function(filename, sheetname, use="RODBC")
{
  if (use=="RODBC"){
    con    <- odbcConnectExcel(filename)
    data   <- sqlFetch(con, sheetname)
    odbcCloseAll()
  }
  
  return(data)
}
