# Code used for obtaining AS procured by ERCOT for all Texas plants
#
#
# Written by Chelsea Yeager on 15-Dec-2011






#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(reshape)
require(FTR)

strHeader = "S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/"  ## as variable passed into the functions
source(paste(strHeader,"rt/lib.ercot_as.r",paste="",sep=""))  ## For testing
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0






#------------------------------------------------------------
# Collect and upload data
#

  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_yeager.dsn;", "UID=RISKDATREAD;", "PWD=RISK_DAT_R3AD;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)


  ## Dates requested for data pull
      start_date = '2011-01-01' ## Will return data for this day also
      end_date = '2011-11-30'  ## Will return data for this day also
      
  ## Get AS procured by ERCOT Gets written here:  S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/Output/AncAll.csv
      Ancillaries = get.all.anc(start_date, end_date,channel_nmktp, channel_teslap, channel_zema_prod)       
    
 
  ## Close database connection
      odbcClose(channel_zema_prod)


  ## Log finish
      rLog(paste("Done at ", Sys.time()))





#------------------------------------------------------------
# Other connections not used in this query
#

  ## Connect to Tesla DB
        #DSN_PATH_teslap = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/TESLAP_ERCOT.dsn;", "UID=ercot_read;", "PWD=ercot_read;", sep="")
        #channel_teslap = odbcDriverConnect(DSN_PATH_teslap)
  ## Connect to N-Market DB
        #DSN_PATH_nmktp = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/nMarket.dsn;", "UID=dat_user;", "PWD=datusertx;", sep="")
        #channel_nmktp = odbcDriverConnect(DSN_PATH_nmktp)
        
  ## Close database connection
        #odbcClose(channel_teslap)
        #odbcClose(channel_nmktp)