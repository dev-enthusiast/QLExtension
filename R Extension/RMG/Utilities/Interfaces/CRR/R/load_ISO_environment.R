#
#
#
#

load_ISO_environment <- function(region="NEPOOL")
{
  switch(toupper(region),
    MISO   = .load_ISO_env_MISO(),
    NEPOOL = .load_ISO_env_NEPOOL(),
    NYPP   = .load_ISO_env_NYPP(),
    PJM    = .load_ISO_env_PJM(),
    SPP    = .load_ISO_env_SPP()
  )
}


.load_ISO_env_NEPOOL <- function() {
  NEPOOL <<- new.env(hash=TRUE)
  NEPOOL$region <- "NEPOOL"
  NEPOOL$DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"
  .load_MAP( NEPOOL )
 }

.load_ISO_env_NYPP <- function() {
  NYPP <<- new.env(hash=TRUE)
  NYPP$region <- "NYPP"
  NYPP$DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"
  .load_MAP( NYPP )
  
  NYPP
}



.load_ISO_env_PJM <- function() {
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.PJM.iso.R")
  PJM$MAP <- PJM$.loadMAP()
}

.load_ISO_env_MISO <- function() {
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.MISO.iso.R")
  MISO$MAP <- MISO$.loadMAP()
}

.load_ISO_env_SPP <- function() {
  SPP <<- new.env(hash=TRUE)
  SPP$region <- "SPP"
  SPP$DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/SPP/"
  .load_MAP( SPP )
  
  SPP
}


.load_MAP <- function(env) {
  fname <- paste(env$DIR, "RData/map.RData", sep="")
  if ( file.exists(fname) ) {
    load(fname, env)
  } else {
    rLog("Cannot find file", fname)
  }
}


