# Code used for calculating backcast profile load for Stream & STX
#
#
# Written by Chelsea Yeager on 13-Mar-2012








#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(reshape)
require(FTR)
require(SecDb)
require(PM)



strHeader = "S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/"  ## as variable passed into the functions
source(paste(strHeader,"rt/lib.load.r",paste="",sep=""))  ## For testing
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0








#------------------------------------------------------------
# Collect and upload data
#

  ## Connect to Loadwiz DB
      DSN_PATH_loadwiz_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/LOADWIZP_yeager.dsn;", "UID=loadwizops;", "PWD=loadwizops;", sep="")
      conlwiz <- odbcDriverConnect(DSN_PATH_loadwiz_prod)
      
  ## Connect to CPS Ercot DB
      DSN_PATH_cpsercot_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/CPSERCOT_yeager.dsn;", "UID=CPS_OPS;", "PWD=CPS_OPS;", sep="")
      concps <- odbcDriverConnect(DSN_PATH_cpsercot_prod)



  ## Get data and calculate reserve %
      start_date = '2012-03-30' ## Will return data for this day also
      end_date = '2012-04-01'  ## Will return data for this day also
      
  ## Get Balance Account  Gets written here:  S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/pnl/Output/XXX.csv
     get.load(start_date, end_date, channel_loadwiz_prod)       
    
 
  ## Close database connection
    odbcClose(channel_Loadwiz_prod)

  # Log finish
    rLog(paste("Done at ", Sys.time()))
