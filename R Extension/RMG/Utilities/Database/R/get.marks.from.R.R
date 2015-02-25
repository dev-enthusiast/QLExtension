# Load prices used by R in the overnight VaR.  Will work with the new design
# using filehash package and having each curve on an individual file.
# Where QQ is a data.frame with colnames = c("curve.name", "contract.dt")
# If QQ has vol.type as column name, the program will pull the historical vols.
#   vol.type can be "D" or "M"
#
# Written by Adrian Dragulescu on 11-Jul-2007

.demo.get.prices.from.R <- function(){

  require(filehash); require(SecDb)
  options <- NULL
  options$calc$hdays <- seq(as.Date("2005-01-01"), by="1 day", length.out=27)
  
  QQ <- expand.grid(curve.name=c("COMMOD NG EXCHANGE"),
    contract.dt= seq(as.Date("2005-02-01"), by="month", length.out=4))

  hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")
  
  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
  res <- get.prices.from.R(QQ, options, hPP.env)

  return(res)
  
}
#-------------------------------------------------- name wrappers -------
get.prices.from.R <- function(QQ, options, hPP.env=NULL){
  if (!is.environment(hPP.env)){
    hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")
  }
  res <- .get.marks.from.R(QQ, options, hPP.env)
  return(res)
}
get.vols.from.R <- function(QQ, options){
   if (!("vol.type" %in% names(QQ))){               # user wants vols
     stop("The input data.frame does not contain vol.type.  Exiting.")
   }
   res <- .get.marks.from.R(QQ, options)
  return(res)
}
#########################################################################
# Workhorse function to get historical prices, or vols from the filehash 
# for prices and vols.  It is recommended you pass in the hPV filehash
# as an environment
#
.get.marks.from.R <- function(QQ, options, hPV=NULL)
{
#   source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
#   load.packages( "SecDb" )
#   require(filehash)
   if (length(ls(hPV))==0){
     print("The environment", path.to.filehash, "is empty.  Please check!")
   }
   if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()}
   calendar <- "CPS-BAL"
   skipWeekends <- FALSE
   ldays <- lapply(as.list(as.character(options$calc$hdays)), as.Date)
   aux <- secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends)
   options$calc$hdays <- options$calc$hdays[unlist(aux)==0]
   if (length(options$save$prices.mkt.dir)==0){
     options$save$prices.mkt.dir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"}

   options$calc$hdays <- sort(options$calc$hdays)       # not necessary
   ind <- which(options$calc$hdays > options$asOfDate)  # if you get carried away
   if (length(ind)>0){options$calc$hdays <- options$calc$hdays[-ind]}
  
   QQ$curve.name <- toupper(QQ$curve.name)
   rownames(QQ) <- NULL            # will need good rownames later
   
   # create an empty results matrix to be filled
   res <- matrix(NA, nrow=nrow(QQ), ncol=length(options$calc$hdays),
     dimnames=list(NULL, as.character(options$calc$hdays)))
   uCurves <- sort(unique(QQ$curve.name))
   for (ind.c in 1:length(uCurves)){                    # loop over the curves
     aux   <- subset(QQ, curve.name==uCurves[ind.c])
     ind.m <- which(hPV[[tolower(uCurves[ind.c])]]$contract.dt %in% aux$contract.dt)
     bux   <- hPV[[tolower(uCurves[ind.c])]][ind.m,]  # the contracts that you need!
     IND   <- merge(cbind(aux$contract.dt, ind.aux=as.numeric(rownames(aux))),
                    cbind(bux$contract.dt, ind.hPV=1:nrow(bux)))     
     ind.n <- which(names(bux) %in% as.character(options$calc$hdays)) # get the hist days
     if (length(ind.n)==0){
       print("No historical data in the pricehash for the days requested!")}
     bux <- bux[,names(bux)[ind.n], drop=F] # get only the history you want 
     res[IND$ind.aux, colnames(bux)] <- as.matrix(bux)
   }
   res <- cbind(QQ, res)  # no index mangling because I use rownames(aux)
   return(res)
}



################################################################################
# Load up the file hash and return it as an env
#
.getFileHashEnv <- function( path )
{
    filehashOption(defaultType = "RDS")      # need this, dbInit will fail
    endWithSlash = which(regexpr("/$", path) != -1)
    path[endWithSlash] = substr(path[endWithSlash], 1, nchar(path[endWithSlash])-1)
    db  <- dbInit(path)
    env <- db2env(db)
    if (!is.environment(env))
    {
        rError("Could not load the price filehash environment.\n")
        stop("Exiting.\n")
    }
    
    return(env)
}


##    if ("vol.type" %in% names(QQ)){                   # user wants vols
##      db  <-  dbInit("//nas-msw-07/rmgapp/Vols/Historical")
##      if (!exists("hVV") || !is.environment(hVV)){       # if not cached
##        hVV <<- db2env(db)  # put all vols in a global environment (takes 5 seconds)
##      }
##      hPV <- hVV
##    } else {                                          # user wants prices 
##      db  <- dbInit("//nas-msw-07/rmgapp/Prices/Historical")
##      if (!exists("hPP") || !is.environment(hPP)){       # if not cached
##        hPP <<- db2env(db)  # put all prices in a global environment (takes 5 seconds)
##      }
##      hPV <- hPP
##    }
   
