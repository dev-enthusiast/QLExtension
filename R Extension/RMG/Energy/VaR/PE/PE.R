################################################################################
# This file defines the environment that will be used throughout the PE 
# simulation.
#
# Please place all settings in the PE environment to avoid name conflicts


################################################################################
# External Libraries
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")


################################################################################
# File Namespace
#
PE = new.env(hash=TRUE)


################################################################################
# Constants
#

# The root directory for the PE project
PE$ROOT_DIR <- "//nas-msw-07/rmgapp/PotentialExposure/"

# The source code directory for the PE project
PE$SRC_DIR = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/PE/"

PE$FC_SRC_DIR = paste( "//ceg/cershare/All/Risk/Software/R/",
    "prod/Models/Price/ForwardCurve/Network/", sep="" )

# The base directory for all the simulation data
PE$BASE_SIM_DIR = "//nas-msw-07/rmgapp/SimPrices/Network/"

# The base data file read to generate the simulated blocks
PE$COUNTERPARTY_POSITION_FILE = paste(PE$ROOT_DIR, 
  "PositionAndPricingData.RData", sep="")

PE$VCV_POSITION_DIR <- "//ceg/cershare/All/Risk/Data/VaR/prod/Positions/"

# How we want to output the PE curve pictures
PE$CURVE_PICTURE_EXT = ".pdf"

# Where the condor files are stored for the PE Curve Simulation
PE$CONDOR_DIR = paste(PE$ROOT_DIR, "Condor/", sep="")


################################################################################
# loads the options variable for use in storing configuration parameters, 
# recreates it if it doesn't exist
#
PE$loadPEOptions <- function( asOfDate=overnightUtils.getAsOfDate(),
  isInteractive=FALSE )
{
  if( is.null(PE$options) )
  {
    PE$reloadPEOptions(asOfDate, isInteractive)
  } else
  {
    if( as.Date(asOfDate) != PE$options$asOfDate )
    {
      PE$reloadPEOptions(asOfDate, isInteractive)
    } 
  }
  
  invisible(PE$options)
}


################################################################################
# Creates the options variable for use in storing configuration parameters
#
PE$reloadPEOptions <- function( asOfDate=overnightUtils.getAsOfDate(),
  isInteractive )
{
  PE$options = overnightUtils.loadOvernightParameters( as.Date(asOfDate),
    connectToDB=FALSE )
    
  PE$options$fp = ForwardPriceOptions$create(asOfDate)
  
  runMode = ifelse(isInteractive, "interactive/", "overnight/")
  cat("\nPE Run Mode Set To:", runMode, "\n")
  
  PE$options$save$reports.dir = paste(PE$ROOT_DIR, PE$options$asOfDate,
     "/", runMode, sep="")
     
  # Where the simulation data is stored.  By default look at the overnight dir.
  PE$CURVE_DATA_DIR = paste( PE$BASE_SIM_DIR, PE$options$asOfDate, 
    "/overnight/result/", sep="")
  
  # make overnight directory 
  if (file.access(PE$options$save$reports.dir)!=0)
  {  
    shell(paste("mkdir", PE$options$save$reports.dir), 
      translate=TRUE, invisible=TRUE)
  }

  invisible(PE$options)
}

