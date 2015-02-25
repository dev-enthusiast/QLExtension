# Upload Bentek Gas Nominations data
#  There is one number for the entire day--however I give that number to each hour of the day so can graph with hourly data
#  Units are billion btus
#
# Written by Lori Simpson on 13-May-2011




#------------------------------------------------------------
# Functions
# startDate=STARTDT; endDate=ENDDT; conn=channel_bentek
#

store.bentek.data.tx = function(startDate, endDate, conn)
    {
     rLog("Inside store.bentek.data.tx function")
    strQry=paste("select flows.measurement_date, flows.nomination_cycle, sum(flows.scheduled_volume)/1000 flows
                  from bentek.flowdata flows, bentek.points points
                  where flows.point_id=points.point_id
                  and measurement_date>=to_date('",startDate,"', 'YYYY-MM-DD') and measurement_date<=to_date('",endDate,"', 'YYYY-MM-DD')
                  and points.btk_customer_type=3 and points.state='TX'
                  group by flows.measurement_date, flows.nomination_cycle
                  order by nomination_cycle, measurement_date", sep="")
		nomsraw <- sqlQuery(conn, strQry);
     rLog("Finished SQL query")

    ## Return empty if cannot find data
      if(nrow(subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(startDate),"day") & NOMINATION_CYCLE=='I2'))==0) {rLog(paste("Did not find data for desired date of ",startDate,sep="")); return(); break; stop}
     rLog(paste("Collected ", nrow(subset(nomsraw, MEASUREMENT_DATE>=round(as.POSIXct(startDate),"day"))), " rows of data", sep=""))
    
    ## Upload pm_ercot_bentek_texas_i2
      symbol <- "pm_ercot_bentek_texas_i2"
      dfNoms  <- data.frame(time=seq(round(as.POSIXct(startDate),"day"), by="1 hour", length.out=25),
                            value=subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(startDate),"day") & NOMINATION_CYCLE=='I2')[,"FLOWS"])[2:25,]
      rLog("Made dataframe")                            
      tsdbUpdate(symbol, dfNoms)
      rLog(paste("Tried to upload I2 data to TSDB for date",round(dfNoms[1,1],"day")," --success uncertain",sep=""))

    ## Upload pm_ercot_bentek_texas_e
      symbol <- "pm_ercot_bentek_texas_e"
      dfNoms  <- data.frame(time=seq(round(as.POSIXct(endDate),"day"), by="1 hour", length.out=25),
                            value=subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(endDate),"day") & NOMINATION_CYCLE=='E')[,"FLOWS"])[2:25,]
      tsdbUpdate(symbol, dfNoms)
      rLog(paste("Tried to upload E data to TSDB for date",round(dfNoms[1,1],"day")," --success uncertain",sep=""))

    }





#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
# source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
# source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0





#------------------------------------------------------------
# Collect and upload gas data
#

  ## Connect to Nodal N-Market
      DSN_PATH_bentek = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/BENTEK.dsn"), 
                              ";UID=Bentekread;", "PWD=Bentekread;", sep="")
      channel_bentek = odbcDriverConnect(DSN_PATH_bentek)

  ## Get Data -- Function found below
    STARTDT=Sys.Date()-1
    ENDDT=Sys.Date()    
    res=store.bentek.data.tx(startDate=STARTDT, endDate=ENDDT, conn=channel_bentek)

  ## Close database connection
  odbcClose(channel_bentek)

  # Log finish
  rLog(paste("Done at ", Sys.time()))




#------------------------------------------------------------
# Interactive=true if ran by hand
#
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}










## BACKFILL DATA
#for (i in 500:505)
#{
#   STARTDT=Sys.Date()-i
#   ENDDT=Sys.Date()-i+1
#
#    res=store.bentek.data.tx(startDate=STARTDT, endDate=ENDDT, conn=channel_bentek)
#    print(res)
#}



