# Construct the weather normalized load.
# 
#
# Written by Adrian Dragulescu on 13-Feb-2004

construct.load <- function(load.month, reg.LT, reg.DT, save, options){

  startDate <- as.Date(options$forecast$startDate)
  endDate   <- as.Date(options$forecast$endDate)
  
  f.days <- seq(startDate,endDate, by="days") # forecast days
  YMD <- ymd(f.days, rep=1)    # table of year month day
  YMD <- cbind(YMD, f.days)
  uYears <- unique(YMD$year)   # unique years
  no.days <- length(f.days)
  fdata <- NULL;  fdata.year=NULL

  load(options$file.hist.temp) # load saved T regression results

  for (yr in uYears){          # loop over unique years
    yrDD <- substr(as.character(yr),3,4)
    start.year <- chron(paste("1/1/",yrDD,sep=""))
    end.year   <- chron(paste("12/31/",yrDD,sep=""))
    days.year  <-  as.numeric(end.year-start.year+1)
    fdata.year=NULL
    #------------------------------------------------------------    
    aux <- YMD[YMD$year==yr,]         # one year data.frame   
    uMonths <- unique(aux$month)      # unique number of months in year
    for (m in uMonths){               # loop over months
       bux <- aux[aux$month==m,]      # one month data.frame
       chron.fdays <- chron(format(bux$f.days, "%m/%d/%y"))
       we <- which(as.numeric(is.weekend(chron.fdays))==1)
       wd <- which(as.numeric(is.weekend(chron.fdays))==0)
       #------------------ Calculate weather normalized load ----
       fdata.day=NULL
       day.of.year <- as.numeric(chron.fdays[wd]-start.year)+1       
       fdata.day[wd] <- predict(reg.LT$WD[[m]],
                        data.frame(temp=temp.day$mean[day.of.year]))
       day.of.year <- as.numeric(chron.fdays[we]-start.year)+1      
       fdata.day[we] <- predict(reg.LT$WE[[m]],
                        data.frame(temp=temp.day$mean[day.of.year]))
       # plot(fdata.day, type="b")
       #------------------- Add the hourly shape ----------------
       fdata.mdh=matrix(NA, nrow=dim(bux)[1], ncol=24)
       lm <- load.month$WD[[m]]
       fdata.mdh[wd,] <- fdata.day[wd] %o% lm$hourly.shape
       lm <- load.month$WE[[m]]       
       fdata.mdh[we,] <- fdata.day[we] %o% lm$hourly.shape
       fdata.mdh <- as.numeric(t(fdata.mdh))
       # plot(fdata.mdh, type="l")

#browser()
       cux <- ymd(bux$f.days, rep=24)
       cux <- cbind(cux, fdays=rep(as.character(bux$f.days), each=24))
       fdata.month <- cbind(cux, load=fdata.mdh)       
       fdata.year <- rbind(fdata.year, fdata.month) # add this month
     } # end of loop over unique months
     fdata <- rbind(fdata, fdata.year)
  }  # end of loop over unique years
  rownames(fdata) <- 1:dim(fdata)[1]

  if (options$detrend){
    colnames(fdata)[6] <- "x"
    fdata <- add.trend(fdata, reg.DT, options)  # add the trend
    colnames(fdata)[6] <- "load"
    fdata <- fdata[,-7]     # you don't need the adder
  }
  
  return(fdata)  
} # end of file










# plot(fdata$load, type="l")

# monthF <- factor(fdata$month, labels=month.abb[unique(fdata$month)])
# fdaysCH <- chron(as.character(fdata$fdays))
# wd <- which(as.numeric(is.weekend(fdaysCH))==0)
# we <- which(as.numeric(is.weekend(fdaysCH))==1)
# dayType=NULL; dayType[wd] <- "WD";  dayType[we] <- "WE"
# fdata <- cbind(fdata, dayType=dayType)

# plot(rownames(fdata)[wd], fdata$load[wd], col="blue")
# points(rownames(fdata)[we], fdata$load[we], col="red")

# xyplot( fdata$load[wd] ~ fdata$hour[wd] | monthF[wd],
#        panel=function(x,y){
#          hF <- factor(x)
#          my <- tapply(y,hF,mean)
#          panel.xyplot(x,y, col="gray")
#          panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
#        }, 
#        col="grey", pch=1, bg="white", layout=c(6,2), 
#        xlab="Hour", ylab="Weekday Load, MW",
#        main=paste("Forecasted",options$loadName,"load")
