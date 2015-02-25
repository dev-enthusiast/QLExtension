#
#
#
# Last modified by Adrian Dragulescu on 19-Feb-2004


plot.hist.load <- function(hdata,save,options){

weCH <- is.weekend(chron(as.character(hdata$hdays)))
we <- which(as.numeric(weCH)==1)  # weekends
wd <- which(as.numeric(weCH)==0)  # weekdays
N  <- dim(hdata)[1] 
hdata$dayType <- array(0, dim=c(N,1))
hdata$dayType[we] <- "WE";  hdata$dayType[wd] <- "WD";   
monthF <- factor(month.abb[hdata$month],
                 levels=month.abb[sort(unique(hdata$month))])
uYears <- unique(hdata$year)
yRange <- paste(uYears[1],uYears[length(uYears)], sep="-")

bkg.col <- trellis.par.get("background")
bkg.col$col <- "white"
trellis.par.set("background", bkg.col)

#-------------------Violin plot on all data--------------------
library(Simple)
main=paste(paste("Detrended historical ",options$loadName,",",sep=""),yRange)
fileName <- paste(save$dir$plots,"histload_violin.pdf", sep="")
if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
#aux <- sort(hdata$month, index.return=TRUE)
cat(simple.violinplot( hdata$load.dt ~ monthF, col="wheat1"))
title(main)
mtext("Average hourly load, MWh", side=2, line=2.2)
hload.month <- tapply(hdata$load.dt, factor(hdata$month), mean)
points(1:12, hload.month, pch=19)
if (save$Analysis$all){dev.off()}

#-------------------- Table: Historical data by month ---------
uYears <- unique(hdata$year)
hload.month <- matrix(NA, nrow=12, ncol=length(uYears))
for (y in 1:length(uYears)){
  ind <- which(hdata$year==uYears[y])
  aux <- tapply(hdata[ind,"load"],factor(hdata[ind,"month"]), sum)
  hload.month[as.numeric(names(aux)),y] <- as.matrix(aux)
}
rownames(hload.month) <- month.abb
colnames(hload.month) <- uYears
hload.month <- as.data.frame(hload.month)

fileName <- paste(save$dir$Rtables,"hist.load.month.tex", sep="")
print.xtable(xtable(hload.month,
   caption="Total historical load (MWh) by month.",
   display=c("s",rep("f",ncol(hload.month))),
   digits=c(0,rep(options$digits.table,length(uYears)))), file=fileName)

#------------Trellis of load vs hour | month for WD------------
main=paste(paste("Detrended historical ",options$loadName,",",sep=""),yRange)
fileName <- paste(save$dir$plots,"histload_hour_WD.pdf", sep="")
if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
print(xyplot( hdata$load.dt[wd] ~ hdata$hour[wd] | monthF[wd],
       panel=function(x,y){
         hF <- factor(x)
         my <- tapply(y,hF,mean)
         panel.xyplot(x,y, col="gray", pch=".")
         panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
       },
       layout=c(6,2),
       col="grey", pch=1, bg="white", 
       xlab="Hour of day", ylab="Weekday Load, MW",
       main=main))
if (save$Analysis$all){dev.off()}

#------------Trellis of load vs hour | month for WE------------
main=paste(paste("Detrended historical ",options$loadName,",",sep=""),yRange)
fileName <- paste(save$dir$plots,"histload_hour_WE.pdf", sep="")
if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
print(xyplot( hdata$load.dt[we] ~ hdata$hour[we] | monthF[we],
       panel=function(x,y){
         hF <- factor(x)
         my <- tapply(y,hF,mean)
         panel.xyplot(x,y, col="gray", pch=".")
         panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
       },
       layout=c(6,2), 
       col="grey", pch=1, bg="white", 
       xlab="Hour of day", ylab="Weekend Load, MW",
       main=main))
if (save$Analysis$all){dev.off()}

#-----------Historical load one year a page--------------------
for (yr in uYears){
  main=paste(paste("Historical ",options$loadName,",",sep=""),"Year", yr)
  fileName <- paste(save$dir$plots,"histload_day_",yr,".pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
  aux=hdata[which(hdata$year==yr),]
  aux.MF <- factor(aux$month, labels=month.abb[unique(aux$month)])
  print(xyplot(aux$load ~ aux$day | aux.MF,
       layout=c(4,3), type="l", aspect="xy",
       col="blue", pch=1, 
       xlab="Day of month", ylab="Load, MW",
       main=main))
  if (save$Analysis$all){dev.off()}
}
} # end of function
