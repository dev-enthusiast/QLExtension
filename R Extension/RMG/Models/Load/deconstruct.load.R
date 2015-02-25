#
#
#
# Written by Adrian Dragulescu on 12-Feb-2004

deconstruct.load <- function(hdata, save, options){


  dayTypes <- c("WD", "WE")
  ind <- which(is.weekend(chron(as.character(hdata$hdays), format="m/d/y")))
  hdata$dayType <- "WD"; hdata$dayType[ind] <- "WE"
  myQ <- function(x){quantile(x, probs=c(0.05,0.50,0.95))}

  data <- hdata[,c("year","month", "day","hour","load")]
  colnames(data)[5] <- "x" 
  aux           <- remove.trend(data, save, options) 
  reg.DT        <- aux[[1]]      # regression for detrending
  hdata$load.dt <- aux[[2]]      # detrended load
  if (!options$detrend){         # detrend only if requested
    reg.DT        <- NULL  
    hdata$load.dt <- hdata$load
  }
  load.month=NULL;  aux.load=NULL;  reg.LT=NULL;

  for (dayType in dayTypes){     # loop over day types, WD & WE
     for (m in 1:12){            # loop over months
        ind <- which(hdata$month==m & hdata$dayType==dayType)
        aux <- hdata[ind,]    # cut data
        aux.load$prct <- tapply(aux$load.dt, aux$hour, myQ)
   #    aux.load$mean <- tapply(aux$load.dt, aux$hour, mean)
        aux.load$median <- tapply(aux$load.dt, aux$hour, median)
        aux.load$sd   <- tapply(aux$load.dt, aux$hour, sd)
        aux.load$hourly.shape <- aux.load$median/mean(aux.load$median)
        uDays <- levels(factor(aux$hdays))
   #    aux.load$day <- array(NA, dim=c(length(uDays),24))
        aux.load$avg <- NULL;  temp.day.avg <- NULL
        for (d in 1:length(uDays)){
          bux <- aux[which(as.character(aux$hdays)==uDays[d]),] # day only
   #      aux.load$day[d,bux$hour+1] <- bux$load.dt
          aux.load$avg[d] <- mean(bux$load.dt, na.rm=TRUE)
          temp.day.avg[d] <- mean(bux$temp, na.rm=TRUE)
        }
        #---------------- Do the regression -----------------
        options$m <- m; options$dayType <- dayType
        res <- regressLT(aux.load$avg, temp.day.avg, save, options)
        #-------------- Construct the list of results -------
        aux.load$avg <- NULL  # no need to save it
        if (dayType=="WE"){   # weekends
            load.month$WE[[m]] <- aux.load
            reg.LT$WE[[m]] <- res
        } else {              # weekdays
            load.month$WD[[m]] <- aux.load
            reg.LT$WD[[m]] <- res
        }
      } # end of loop over months
  } # end of loop over dayTypes
  res <- list(hdata, load.month, reg.LT, reg.DT)
  return(res)
}




