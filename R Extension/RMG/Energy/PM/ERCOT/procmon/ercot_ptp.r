# Get RT Prices for PTP file
#
#
# Written by Lori Simpson on 19-Jan-2015


#------------------------------------------------------------
# Functions
# startDate=STARTDT; endDate=ENDDT; conn=channel_zema_prod
#
get.interval.data <- function(startDate=NULL, endDate=NULL, conn)
{


     arrSp=read.csv("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/uploadPtpLocations.csv")

     ## Store log data
      rLog(paste("Start get.interval.data function at", Sys.time()))

     ## Pull RT SPPs
     	rLog(paste("Will attempt to pull SPPs"))
      # arrSPs=c('HB_NORTH','HB_SOUTH','HB_HOUSTON','HB_WEST','LZ_NORTH','LZ_SOUTH','LZ_HOUSTON','LZ_WEST','LZ_RAYBN')
      spps=pull.spps(startDate, endDate, arrSp$SP, conn);
      if(length(spps)==1) {rLog(paste("Returned no SPPs: ",spps[1])); stop}


     	rLog(paste("Will reorganize data"))
      sppsrt=spps
      colnames(sppsrt)[match("RTSPP",colnames(sppsrt))]="value"
      sppsrt$variable="rtspp"
      sppsrt=cast(sppsrt, OPR_DATE + HE + SP ~ variable, mean)
      sppsda=spps
      colnames(sppsda)[match("DASPP",colnames(sppsda))]="value"
      sppsda$variable="daspp"
      sppsda=cast(sppsda, OPR_DATE + HE + SP ~ variable, mean)

      sppsc=merge(sppsda, sppsrt)
      sppsm=melt(sppsc, id=c("OPR_DATE", "HE", "SP"))
      sppsc=cast(sppsm, OPR_DATE + HE ~SP + variable, mean)

     ## Store data by month-year
     	rLog(paste("Writing CSV file"))
        write.csv(sppsc, file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/ptp_prices.csv")



}



##############################################################################
##############################################################################
#
require(CEGbase)
options(width=150)
require(SecDb)
##Sys.setenv( SECDB_ORACLE_HOME=paste( "C:\\oracle\\ora90" ) )
##secdb.evaluateSlang( "Try( E ) { ODBCConnect(\"\",\"\"); } : {}" )
require(RODBC)
require(FTR)
source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
rLog(paste("Start proces at", Sys.time()))
# returnCode <- 0    # succeed = 0

# q( status = 0 )   ## Temporarily prevent from running



#------------------------------------------------------------
# Run the file to consolidate all RT data
# STARTDT=as.Date('2011-04-30'); ENDDT=as.Date('2011-05-01')

  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"),
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)

  ## Get Data
    STARTDT=Sys.Date() - 7
    ENDDT=Sys.Date() + 1
      rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,sep=""))
    res=get.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
      rLog(paste("Finished pulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,"\n",sep=""))


   odbcClose(channel_zema_prod)


#------------------------------------------------------------
# Force fail so procmon runs script hourly until final hour when needs to succeed
#
    rLog(paste("Hour is ",as.numeric(format(Sys.time(), "%H")),".  Return logic says: ", as.numeric(format(Sys.time(), "%H"))>=9)
    returnCode <- 99
    if (as.numeric(format(Sys.time(), "%H"))>=9)   returnCode <- 0

  # Interactive=true if ran by hand

  if( interactive() ){
    print( returnCode )
  } else {
    q( status = returnCode )
  }

