# Do different relevant plots for temperature. 
#
#
# Written by Adrian Dragulescu on 4-Jun-2004


plot.hist.temp <- function(save, options){

save$plots <- 1
setwd(save$dir$calib)
options$filename <- paste("parms.",options$airport.name,".Rdata",sep="")
load(options$filename)

#------------------------------------------------------
#               Plot the monthly temperature 
#------------------------------------------------------
fName <- paste(save$dir$plots,options$airport.name,"_hT-month.pdf", sep="")
if (save$plots){pdf(fName, width=8.0, heigh=5.0)}
xLim <- c(1,12);
yLim <- c(min(temp.month$mean-temp.month$sd),
          max(temp.month$mean+temp.month$sd))
plot(xLim,yLim, type="n", main=options$airport.name,
     xlab="Month", ylab="Monthly temperature",
     xlim=xLim, ylim=yLim, xaxt="n", cex.main=1)
lines(temp.month$mean, type="b", col="blue")
lines(temp.month$mean-temp.month$sd, type="b", lty=2, col="red")
lines(temp.month$mean+temp.month$sd, type="b", lty=2, col="red")
axis(side=1, at=1:12, labels=month.abb)
if (save$plots){dev.off()}

#-------------------------------------------------------
#      Plot the spline interpolated daily temperature
#-------------------------------------------------------
fName <- paste(save$dir$plots,options$airport.name,"_hT-day.pdf", sep="")
if (save$plots){pdf(fName, width=8.0, heigh=5.0)}
yLim <- c(min(temp.day$mean-temp.day$sd),
          max(temp.day$mean+temp.day$sd))
plot(temp.day$mean, type="n", ylim=yLim,
        xlab="Day of the year", ylab="Daily temperature",
        main=options$airport.name, cex.main=1, xaxs="i",
        xlim=c(0,367))
lines(temp.day$mean, col="blue")
lines(temp.day$mean+temp.day$sd, lty=2, col="red")
lines(temp.day$mean-temp.day$sd, lty=2, col="red")
if (save$plots){dev.off()}

#-------------------------------------------------------
#          Plot this year in the bands
#-------------------------------------------------------
aux <- as.numeric(temp.hist.scenarios)
ind <- rep(1:366, each=dim(temp.hist.scenarios)[1])
day.max <- tapply(aux, ind, max)
day.min <- tapply(aux, ind, min)

thisYear   <- hdata$year[nrow(hdata)]
ind        <- which(hdata$year==thisYear)
temp.thisyear <- (hdata$Tmax[ind] + hdata$Tmin[ind])/2
hdata$Tavg <- (hdata$Tmax + hdata$Tmin)/2

fName <- paste(save$dir$plots,options$airport.name,"_envelope.pdf", sep="")
if (save$plots){pdf(fName, width=8.0, heigh=5.0)}
plot(1:366, day.max, type="n", ylim=c(min(day.min),max(day.max)),
     xlim=c(1,366), xlab="Day of year", ylab="Temperature",
     main=options$airport.name, cex.main=1)
polygon(c(1:366,366:1),c(day.max,rev(day.min)), col="lavender",
        border="gray")
lines(1:366,temp.day$mean, col="gray")
lines(1:length(temp.thisyear), temp.thisyear, col="blue")
if (save$plots){dev.off()}

#-------------------------------------------------------
#       Plot monthly temperature vs. year by month
#-------------------------------------------------------
plot.monthly.T <- function(hist.avg, m, options){
  # Where: - hist.avg is the monthly average by year
  #        - m is the month you are interested to plot
  main <- paste(options$city, ", ", month.abb[m], sep="")
  fileName <- paste(save$dir$plots, options$city, "-",
                    month.abb[m],".pdf", sep="")
  pdf(fileName, width=7.0, height=5.0)
  q <- as.numeric(quantile(hist.avg[,m], probs=c(0.1,0.5,0.9), na.rm=TRUE))
  plot(rownames(hist.avg), hist.avg[,m], xlab="year", col="blue",
     ylab="Average monthly temperature", main=main, cex.main=1)
  points(thisYear, hist.avg[as.character(thisYear),m], pch=8, col="red")
  abline(h=q[c(1,3)], col="pink")
  abline(h=q[2], col="grey")
  mtext(c("10%", "50%", "90%"), side=4, at=q)
  dev.off()
  return(rbind(t(t(q)), hist.avg["2004",1]))
}
hist.avg <- NULL
uYears   <- unique(hdata$year)
hist.avg <- matrix(NA, ncol=12, nrow=length(uYears))
for (yr in 1:length(uYears)){
   ind <- which(hdata$year==uYears[yr])
   aux <- tapply(hdata[ind,"Tavg"], hdata[ind,"month"], mean)
   hist.avg[yr,as.numeric(names(aux))] <- as.matrix(aux)   
}
rownames(hist.avg) <- uYears
colnames(hist.avg) <- month.abb
hist.avg <- signif(hist.avg, digits=3)

options$city <- options$airport.name
for (m in 1:12){
  aux <- plot.monthly.T(hist.avg, m, options)  
}
  
#-------------------------------------------------------
#          Trellis Plot temperature by month -- big file
#-------------------------------------------------------
monthF <- ordered(month.abb[hdata$month], levels=month.abb)
fName <- paste(save$dir$plots,options$airport.name,"_trellis.pdf", sep="")
if (save$plots){pdf(fName, width=5.0, heigh=5.0)}
aux <- trellis.par.get()
bkg.col <- trellis.par.get("background")
bkg.col$col <- "white"
trellis.par.set("background", bkg.col)
print(xyplot(hdata$Tavg ~ hdata$day | monthF,
       panel=function(x,y){
         hF <- factor(x)
         my <- tapply(y,hF,median)
         panel.xyplot(x,y, col="gray", pch=".")
         panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
       }, 
       col="grey", pch=".", bg="white", 
       xlab="Day of month", ylab="Historical temperature",
       main=options$main))
if (save$plots){dev.off()}

## #-------------------------------------------------------
## #          BW Plot of the last 10 years 
## #-------------------------------------------------------
## fName <- paste(save$dir$plots,options$airport.name,"_trellis.pdf", sep="")
## if (save$plots){pdf(fName, width=5.0, heigh=5.0)}
## aux <- trellis.par.get()
## bkg.col <- trellis.par.get("background")
## bkg.col$col <- "white"
## trellis.par.set("background", bkg.col)
## print(xyplot(hdata$Tavg ~ hdata$day | monthF,
##        panel=function(x,y){
##          hF <- factor(x)
##          my <- tapply(y,hF,median)
##          panel.xyplot(x,y, col="gray", pch=".")
##          panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
##        }, 
##        col="grey", pch=".", bg="white", 
##        xlab="Day of month", ylab="Historical temperature",
##        main=options$main))
## if (save$plots){dev.off()}



#----------------------------------------------------------
#    Quantile plot of daily changes by month -- big file 
#----------------------------------------------------------
fName <- paste(save$dir$plots,options$airport.name,
               "_daily_Temp_changes_quantile.pdf", sep="")
N  <- dim(hdata)[1]
dT <- diff(hdata$Tavg)
if (save$plots){pdf(fName, width=8.75, heigh=6.0)}
aux <- trellis.par.get()
bkg.col <- trellis.par.get("background")
bkg.col$col <- "white"
trellis.par.set("background", bkg.col)
print(xyplot( dT ~ dT | monthF[1:(N-1)],
       panel=function(x,y){
         NN <- length(x)
         probF <- seq(1/(2*NN),1-1/(2*NN), length=NN)         
         reg <- lm(sort(x)~qnorm(probF))
         panel.xyplot(qnorm(probF), sort(x), col="blue", pch=1)
         panel.xyplot(qnorm(probF), predict(reg), col="grey", type="l",
                      lwd=2)},
       xlim=c(-4,4), xlab="Quantile standard normal",
       ylab="Quantile daily temperature changes",
       main=options$airport.name, cex.main=1))
if (save$plots){dev.off()}
  
}
