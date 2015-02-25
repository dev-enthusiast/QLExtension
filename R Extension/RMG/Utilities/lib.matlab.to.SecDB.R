# Libraries with R functions wraps for the Matlab to SecDB matlab
# functions:
#    sdbGetValue 
#    tsdbRead
#    futStrip
#
# Written by Adrian Dragulescu on 13-Jan-2005

#-------------------------------------------------------------------
# Convert dates from Matlab to R and viceversa.
#
convertDates <- function(dates.in, to){
  if (to=="R"){                    # dates.in should be a numeric vector 
     aux <- sum(dates.in-floor(dates.in))
     if (aux<10^(-4)){             # most likely it's a Date
        dates.out  <- dates.in - 730486 + as.Date("2000-01-01")
     } else {                      # most likely it's POSIX
##         dates.out  <- (round((dates.in-floor(dates.in))*3600*24) +  
##           (floor(dates.in)-730486)*3600*24 +
##            strptime("2000-01-01", format="%Y-%m-%d"))
##                        ISOdate(2000,1,1,12,0,0, tz=""))
        dates.out <- (dates.in-730486)*3600*24+ISOdate(2000,1,1,0,0,0,tz="")
        dates.out <- as.POSIXct(round(dates.out, "mins"))
   }
  }
  if (to=="Matlab"){      # dates.out should be a vector of R Dates
     dates.out  <- as.numeric(dates.in) - 10957 +  730486
  }
  return(dates.out)
}


#-------------------------------------------------------------------
sdbGetValue <- function(object, component, matlab){
   command <- paste("res=sdbGetValue('",object,"', '",component, "')", sep="")
   evaluate(matlab, command)
   obj <- getVariable(matlab, "res")
   return(obj)
}

#-------------------------------------------------------------------
sdbInvoke <- function(){

}

#-------------------------------------------------------------------
sdbEval <- function(){
    
}

#-------------------------------------------------------------------
tsdbRead <- function(symbol, start.dt, end.dt, options){
## symbol   <- c("PRC_NG_Exchange_0607", "PRC_PWJ_5x16_West_Phys_0607")
## start.dt <- as.Date("2005-01-01")
## end.dt   <- as.Date("2005-12-31")
## source("H:/user/R/RMG/Utilities/lib.matlab.to.SecDB.R")
## aux <- tsdbRead(symbol, start.dt, end.dt)
##
## I had to use Matlab datestr because the DST hours were calculated
## wrong if you use the HE convention. To differentiate the "extra" hour
## in October, I've added a second to the date.

## Written by Adrian Dragulescu on 18-Jan-2006  
   if (nargs()==3){options=NULL;}
   if (length(options$table)==0){options$table <- 1}
   stDt.mat  <- format(start.dt,  "%d-%b-%Y")  # Matlab dates
   endDt.mat <- format(end.dt, "%d-%b-%Y")
   res <- NULL
   for (s in 1:length(symbol)){
     query <- paste("out=tsdbRead('", symbol[s], "', datenum('", stDt.mat,
       "'), datenum('", endDt.mat, "')); dates=datestr(out(:,1));", sep="")
     evaluate(options$matlab, query)
     aux      <- as.data.frame(getVariable(options$matlab, "out")$out)
     mdates   <- getVariable(options$matlab, "dates")$dates
     if (nchar(mdates[1])==11){
       aux[,1]  <- as.POSIXct(strptime(mdates, format="%d-%b-%Y"))
     } else {
       aux[,1]  <- as.POSIXct(strptime(mdates, format="%d-%b-%Y %H:%M:%S"))
     }
     res[[s]] <- aux
   }
   if (options$table==1){#-------------arrange data in table format
     OutT <- as.data.frame(res[[1]])
     colnames(OutT) <- c("date", symbol[1])
     ind <- which(duplicated(OutT$date))
     if (length(ind)>0){OutT$date[ind] <- OutT$date[ind]+1} # break the tie!
     if (length(symbol)>1){
       for (s in 2:length(symbol)){
         aux <- as.data.frame(res[[s]])
         colnames(aux) <- c("date", symbol[s])
         ind <- which(duplicated(aux$date))
         if (length(ind)>0){aux$date[ind] <- aux$date[ind]+1} # break the tie! 
         OutT <- merge(OutT, aux, all=TRUE)
       }
     }
     res <- OutT
   }
  return(res)
}



#-------------------------------------------------------------------
futStrip <- function(commod, loc, bucket, stDt, endDt, prcDt, servType, matlab){
##  outCurve  =  futStrip( commod, loc, bucket, stDt, endDt, prcDt, servType, cacheOn )
##  Calls secdb Futures Strip to get a forward curve as of pricing date "prcDt". Note:
##  prices must have been archived for the given pricing date or the fucntion will fail.
##  Where commod = commodity name (e.g., 'PWR PJM Physical')
##        loc    = location name (e.g., 'BGE')
##   bucket      =   bucket name (e.g., '5x16')
##   stDt        =   first contract (e.g., datenum('01Jan06'))
##   endDt       =   last contract (e.g., datenum('01Dec06'))
##   prcDt       =   pricing date
##   servType    =   service type (optional; defaults to 'Energy')
##   cacheOn     =   use cache (optional; defaults to 1)
##   Returns:      Curve between startDT and endDT
##   futStrip( 'PWR NEPOOL Physical', 'PTF', '5x16', today, today + 365, today - 1 )
##   futStrip( 'PWR PJM Physical', 'BGE', '5x16', today, today + 365, today - 1, 'Spin' )
  
##    source("H:/user/R/RMG/Utilities/lib.matlab.to.SecDB.R")
##    commod   <- "PWR PJM Physical"
##    loc      <- "BGE"
##    bucket   <- "5x16"
##    stDt     <- as.Date("2006-01-01")
##    endDt    <- as.Date("2006-12-31")
##    prcDt    <- as.Date("2006-01-12")
##    servType <- "Energy"
## 
##    require(R.matlab)
##    matlab <- Matlab(host="localhost")
##    open(matlab)
##    out <- futStrip(commod, loc, bucket, stDt, endDt, prcDt, servType, matlab)

   stDt.mat  <- format(stDt,  "%d-%b-%Y")  # Matlab dates
   endDt.mat <- format(endDt, "%d-%b-%Y")
   prcDt.mat <- format(prcDt, "%d-%b-%Y")

   query <- paste("out=futStrip('", commod, "','", loc, "','", bucket, "',datenum('",
       stDt.mat, "'), datenum('", endDt.mat, "'), datenum('", prcDt.mat,
       "'), '", servType, "')", sep="")
   evaluate(matlab, query)
   res <- as.data.frame(getVariable(matlab, "out")$out)
   colnames(res) <- c("contract.dt", "values")
   res[,"contract.dt"] <- convertDates(res[,"contract.dt"], to="R")
   return(as.data.frame(res))
}


