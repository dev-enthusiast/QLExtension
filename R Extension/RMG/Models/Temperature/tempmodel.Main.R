# Analysis of historical temperature
#  
#
# written by Adrian Dragulescu on 13-Feb-2004

require(lattice); require(splines); require(SecDb)
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/TimeFunctions/mean.by.month.R")
source("H:/user/R/RMG/Models/Temperature/plot.hist.temp.R")

#------------------------------------------------------
#                     General settings 
#------------------------------------------------------
tempmodel.Main <- function(save=NULL,options=NULL){

#require(MASS);

save$dir$root  <- "S:/All/Risk/Data/Weather/"
save$dir$plots <- "S:/All/Risk/Data/Weather/Plots/"
save$dir$calib <- "S:/All/Risk/Data/Weather/Calibrations/"   
save$filename <- paste("parms.",options$airport.name,".Rdata",sep="")   

#------------------------------------------------------
#                     Load the data 
#------------------------------------------------------
if (length(options$LIM.symbol)>0){
  query <- paste("SHOW 1: HighTemp of front", options$LIM.symbol,
                 "     2: LowTemp  of front", options$LIM.symbol,
     "when date is after 1969")
  options$colnames <- c("Tmax", "Tmin")
  hdata <- read.LIM(query, options)
}
if (length(options$tsdb.symbol)>0){
  aux <- tsdb.readCurve(options$tsdb.symbol[1], as.Date("1970-01-01"),
                        Sys.Date())
  hdata <- tsdb.readCurve(options$tsdb.symbol[2], as.Date("1970-01-01"),
                        Sys.Date())
  hdata <- merge(aux, hdata, by="date")
  colnames(hdata) <- c("Date", "Tmax","Tmin")
}

hdata$year  <- as.numeric(format(hdata$Date, "%Y"))
hdata$month <- as.numeric(format(hdata$Date, "%m"))
hdata$day   <- as.numeric(format(hdata$Date, "%d"))

hdata$Tavg <- (hdata$Tmax+hdata$Tmin)/2
monthF <- factor(hdata$month, labels=month.abb[unique(hdata$month)])

#------------------------------------------------------
#                     Analyze data 
#------------------------------------------------------
myQ <- function(x){quantile(x, probs=c(0.05,0.50,0.95))}
temp.month=NULL; aux.T=NULL
for (m in 1:12){            # loop over months
   aux <- subset(hdata, month==m)        # cut data
   aux.T$prct <- tapply(aux$Tavg, aux$day, myQ)
   aux.T$sd   <- tapply(aux$Tavg, aux$day, sd)
   temp.month$mean[m] <- mean(aux$Tavg)
   temp.month$sd[m]   <- mean(aux.T$sd)
}

#--------calculate weather normalized daily temperature------ 
temp.day=NULL                   # weather nomalized daily mean and sd    
days.year <- 366                # number of days in year
YF <- c(1:days.year)/days.year  # where I want to interpolate
yf <- seq(from=-1/24, to=27/24, by=1/12)   # year fractions
T.month <- data.frame(yf,
           T=c(temp.month$mean[12], temp.month$mean, temp.month$mean[1:2]))
T.spline <- interpSpline(T.month$yf, T.month$T, period=1)
temp.day$mean <- predict(T.spline, YF)$y   
temp.day$mean[days.year] <- temp.day$mean[1] 
sd.month <- data.frame(yf,
           sd=c(temp.month$sd[12], temp.month$sd, temp.month$sd[1:2]))
sd.spline <- interpSpline(T.month$yf, sd.month$sd, period=1)
temp.day$sd <- predict(sd.spline, YF)$y   
temp.day$sd[days.year] <- temp.day$sd[1] 

#--------gather historical weather daily scenarios-------- 
temp.hist.scenarios=NULL         # daily weather scenarios    
uYears <- unique(hdata$year)
for (yr in uYears){
   aux <- hdata[which(hdata$year==yr),] # cut this year
   T.yr <- aux$Tavg
   if (length(T.yr)>363){
     if (length(T.yr)==364){
         T.yr[365] <- T.yr[364]
         T.yr[366] <- T.yr[1]
     } else if (length(T.yr)==365){
         T.yr[366] <- T.yr[1]
     } 
     temp.hist.scenarios <- rbind(temp.hist.scenarios, T.yr)
   } else {next}   
}
rownames(temp.hist.scenarios) <- NULL
temp.hist.scenarios

#--------Mean temperature by month---------------------------
hdata$x <- hdata$Tavg
save$Analysis$all  <- 0
options$one.column <- 1
htemp.month <- mean.by.month(hdata, options, save)
hdata$x <- NULL   # I should replace this shameful function...
htemp <- hdata

#------------------------------------------------------------
#              Save the calibration structure
#------------------------------------------------------------
filename <- paste(save$dir$calib,save$filename, sep="")
save(list=c("temp.month", "temp.day", "temp.hist.scenarios",
       "htemp.month", "htemp"),  file=filename)

rLog(paste("Saved calibration to:", filename))

#------------------------------------------------------------
#rLog(paste("Saved plots in:", save$dir$plots))
}
