# Code used for calculating PNL for PTPs & Energy Offer Bids
# Uses Energy/PM/ERCOT/pnl/lib.dam.R
#
#
# Written by Chelsea Yeager & Lori Simpson on 12-Jul-2011










#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(reshape)

strHeader = "S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/"  ## as variable passed into the functions
source(paste(strHeader,"pnl/lib.pnl.R",paste="",sep=""))  ## For testing
# source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/pnl/lib.pnl.R")  ## For production
source(paste(strHeader,"dam/lib.dam.R",paste="",sep=""))   ## For testing
# source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0








#------------------------------------------------------------
# Collect and upload gas data
#

  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_yeager.dsn;", "UID=RISKDATREAD;", "PWD=RISK_DAT_R3AD;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
  ## Connect to Tesla DB
      DSN_PATH_teslap = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/TESLAP_ERCOT.dsn;", "UID=ercot_read;", "PWD=ercot_read;", sep="")
      channel_teslap = odbcDriverConnect(DSN_PATH_teslap)
  ## Connect to N-Market DB
      DSN_PATH_nmktp = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/nMarket.dsn;", "UID=dat_user;", "PWD=datusertx;", sep="")
      channel_nmktp = odbcDriverConnect(DSN_PATH_nmktp)

  ## Get positions and calculate DA/RT P&L
      start_date = '2011-01-01' ## Will return data for this day also
      end_date = '2011-07-26'  ## Will return data for this day also
      arrPtps = get.ptp.mws(start_date, end_date, channel_nmktp, channel_teslap, channel_zema_prod)    

  ## Close database connection
    odbcClose(channel_zema_prod)
    odbcClose(channel_teslap)
    odbcClose(channel_nmktp)

  # Log finish
    rLog(paste("Done at ", Sys.time()))
