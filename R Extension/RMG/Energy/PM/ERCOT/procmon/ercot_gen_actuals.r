# Convert gen actuals to an hourly quantity we actually get paid on
#     Price weights gen actuals
#     Discounts quantity for any basepoint deviation charges
#
# Written by Lori Simpson on 29-Jan-2013

#------------------------------------------------------------
# MAIN
#

require(FTR)
strHeader = "H:/user/R/RMG/Energy/PM/ERCOT/"
source(paste(strHeader,"rt/lib.gen_actuals.r",paste="",sep=""))
source(paste(strHeader,"dam/lib.dam.r",paste="",sep=""))
require(CEGbase)
require(Tsdb)



require(SecDb)
require(PM)
options(width=150)
require(RODBC)
require(reshape)
require(methods)
#Sys.setenv( SECDB_ORACLE_HOME=paste( "C:\\oracle\\ora90" ) )
#secdb.evaluateSlang( "Try( E ) { ODBCConnect(\"\",\"\"); } : {}" )




 ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
       channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
 
 

#------------------------------------------------------------
# Collect and upload data
#

    ## Run Gen Actuals File (make ignoreLmpErrors==TRUE if there are periods longer than 10 minutes without updated LMPs)
      # strDtStart=Sys.Date()-3; strDtEnd=Sys.Date(); ignoreLmpErrors=FALSE; conn=channel_zema_prod
      rLog(paste("Start proces at", Sys.time()))
      if(as.POSIXlt(Sys.time())$hour==1) (DtStart=Sys.Date()-1) else DtStart=Sys.Date()
      DtEnd=Sys.Date()+1 ## DtEnd=DtStart+1
      do_gen_actuals(strDtStart=DtStart, strDtEnd=DtEnd, ignoreLmpErrors=FALSE, conn=channel_zema_prod)

    ## Log finish
      rLog(paste("Done at ", Sys.time()))
      odbcCloseAll()

#------------------------------------------------------------
# Force fail so procmon runs script hourly until final hour when needs to succeed
#
    returnCode <- 99
    if (as.numeric(format(Sys.time(), "%H"))==23)   returnCode <- 0   
    q(status=returnCode)
