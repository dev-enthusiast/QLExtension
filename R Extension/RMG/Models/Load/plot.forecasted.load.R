# Plot the forecasted load in different ways
#
#
# Last modified by Adrian Dragulescu on 23-Feb-2004

plot.forecasted.load <- function(hdata, fdata.RMG, fdata.TLP,
                                 hs.load, reg.LT, save, options){

  #-------------------- Table: TLP forecast load by month ---------
  fload.TLP.month=NULL
  uYears <- unique(fdata.TLP$year)
  fload.TLP.month <- matrix(NA, nrow=12, ncol=length(uYears))
  for (y in 1:length(uYears)){
    ind <- which(fdata.TLP$year==uYears[y])
    aux <- tapply(fdata.TLP[ind,"load"],factor(fdata.TLP[ind,"month"]), sum)
    fload.TLP.month[as.numeric(names(aux)),y] <- as.matrix(aux)
  }
  rownames(fload.TLP.month) <- month.abb
  colnames(fload.TLP.month) <- uYears
  fload.TLP.month <- as.data.frame(fload.TLP.month)

  fileName <- paste(save$dir$Rtables,"tlp.load.month.tex", sep="")
  print.xtable(xtable(fload.TLP.month,
    caption="Total TLP forecasted load (MWh) by month.",
    display=c("s",rep("f",ncol(fload.TLP.month))),
    digits=c(0,rep(options$digits.table,length(uYears)))), file=fileName)
  
  #-------- Table Historical, RMG, TLP loads by month --------
  options$one.column <- 1
  fload.RMG.month <- summarize.monthly(cbind(fdata.RMG, x=fdata.RMG$load),
                                       options)
  if (nrow(fload.RMG.month)>12){
     fload.RMG.month <- fload.RMG.month[1:12,]}

  fload.TLP.month <- summarize.monthly(cbind(fdata.TLP, x=fdata.TLP$load),
                                       options)

  months.RMG <- paste(fload.RMG.month[,1], fload.RMG.month[,2],sep="-")
  months.TLP <- paste(fload.TLP.month[,1], fload.TLP.month[,2],sep="-")
  ind <- which(is.element(months.TLP, months.RMG))
  fload.TLP.month <- fload.TLP.month[ind,]
  fload.TLP.month[,"month"] <- month.abb[fload.TLP.month[,"month"]]
  outT <- cbind(fload.TLP.month, fload.RMG.month[,"x"])
  outT <- cbind(outT, round(100*(outT[,4]-outT[,3])/outT[,3],1))
  colnames(outT) <- c("year","month","TLP","RMG","diff(\\%)")
  outT
  
  fileName <- paste(save$dir$Rtables,"fload.TLP.RMG.month.tex", sep="")
  print.xtable(xtable(outT,
   caption="Forecasted TLP load and RMG load (MWh) by month for the first 12 months.",
   display=c("s","f","f","f","f","f"),
   digits=c(0,0,0,rep(options$digits.table,2),1)), file=fileName)

  #------------------------- Histogram of first 12 mths load --------
  hs.load.year=NULL
  no.years.hist   <- dim(hs.load)[2]-3
  fload.RMG.12mth <- sum(fload.RMG.month$x)
  fload.TLP.12mth <- sum(fload.TLP.month$x)
  hs.load.12mth   <- apply(as.matrix(hs.load[,3+(1:no.years.hist)]),2,sum)
  pdf.load.year   <- density(hs.load.12mth)
  q2 <- as.numeric(quantile(pdf.load.year$y, 0.50))

  fileName <- paste(save$dir$plots,"pdfHS+TLP.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8.0, heigh=4.0)}
  plot(pdf.load.year, main=options$main, xlab="First 12 months load, MWh",
       cex.main=1, xlim=range(c(fload.TLP.12mth, hs.load.12mth)) )
  rug(hs.load.12mth)
  lines(c(fload.RMG.12mth, fload.RMG.12mth), c(0, q2), col="red")
  text(fload.RMG.12mth, q2, "RMG", col="red", pos=3)  
  lines(c(fload.TLP.12mth, fload.TLP.12mth), c(0, q2), col="blue")
  text(fload.TLP.12mth, q2, "TLP", col="blue", pos=3)
  if (save$Analysis$all){dev.off()}

  #-------------------- Plot TLP forecast and min-max RMG limits ---
  main=paste("Forecast ",options$loadName,",",sep="")
  fileName <- paste(save$dir$plots,"tlpload_minmax.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8.0, heigh=5.5)}

  uMonths <- unique(paste(hs.load$year, hs.load$month, sep="-"))
  yyyy.mm <- paste(hs.load$year, hs.load$month, sep="-")
  yyyy.mm <- factor(yyyy.mm, levels=uMonths)
  hs.load.month <- matrix(NA, nrow=length(uMonths), ncol=no.years.hist)
  for (s in 1:no.years.hist){
     hs.load.month[,s] <- tapply(hs.load[,s+3], yyyy.mm, sum)
  }
  myQ <- function(x){return(c(min(x),median(x),max(x)))}
  hs.load.qtile <- t(apply(hs.load.month, 1, myQ))
  colnames(hs.load.qtile) <- c("min","median","max")
  rownames(hs.load.qtile) <- uMonths
  hs.load.qtile
  no.uMonths <- length(uMonths)
  uMonthsPOS <- as.Date(paste(uMonths,"-1",sep=""))
  mth.label  <- format(uMonthsPOS, format="%b%y")
        
  main=paste("Forecast ",options$loadName,sep="")
  fileName <- paste(save$dir$plots,"tlpload_minmax.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8.0, heigh=5.5)}
  plot(1:12, hs.load.qtile[1:12,1], type="n", xaxt="n", xlim=c(1,13),
       ylim=range(hs.load.qtile, fload.TLP.month$x, fload.RMG.month$x),
       ylab="Monthly load, MWh", main=main, cex.main=1, xlab="")
  for (m in 1.5:12.5){
    rect(m-0.5,hs.load.qtile[m,1],m+0.5, hs.load.qtile[m,3],
         border="wheat", col="wheat")
  }
  xticks <- 1.5:12.5
  points(xticks,fload.TLP.month$x, pch=19, col="blue")
  points(xticks,fload.RMG.month$x, col="red")
  axis(1, xticks, labels=format(uMonthsPOS[1:12],  format="%b%y"), las=2)
  ind <- grep("-4",uMonths)  # place the legend on April
  if (length(ind)!=0){ind <- ind[1]}else{ind <- 10}
  legend(ind,max(hs.load.qtile), c("TLP", "RMG"),
       col=c("blue","red"), bty="n", pch=c(19,1))
  if (save$Analysis$all){dev.off()}

  #-------------------- Plot RMG R^2 for the regressions ---
  S.WD=S.WE <- array(NA, 12)
  for (m in 1:12){
    reg   <- reg.LT$WD[[m]]
    S.WD[m] <- reg$s
    reg   <- reg.LT$WE[[m]]
    S.WE[m] <- reg$s
  }

  fileName <- paste(save$dir$plots,"rmg_load_sd.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8.0, heigh=4.0)}
  layout(matrix(1:2, 1,2))
  par(mar=c(3.5,4,1.5,1.5))  # distance between the plots    
  plot(S.WD, main="Weekdays", xaxt="n", xlab="",
       ylab="sd of residuals", col="blue", cex.main=1)
  axis(1, 1:12, labels=month.abb, las=2)
  plot(S.WE, main="Weekends", xaxt="n", xlab="",
       ylab="sd of residuals", col="blue", cex.main=1)
  axis(1, 1:12, labels=month.abb, las=2)
  if (save$Analysis$all){dev.off()}
  
  #-------------------- Plot RMG Slope of the regressions ---
  S.WD=S.WE <- array(NA, 12)
  for (m in 1:12){
    reg   <- reg.LT$WD[[m]]
    S.WD[m] <- reg$coefficients[2]
    reg   <- reg.LT$WE[[m]]
    S.WE[m] <- reg$coefficients[2]
  }

  fileName <- paste(save$dir$plots,"rmg_load_slope.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8.0, heigh=4.0)}
  layout(matrix(1:2, 1,2))
  par(mar=c(3.5,4,1.5,1.5))  # distance between the plots    
  plot(S.WD, main="Weekdays", xaxt="n", xlab="",
       ylab="slope of regression", col="blue", cex.main=1)
  axis(1, 1:12, labels=month.abb, las=2)
  plot(S.WE, main="Weekends", xaxt="n", xlab="",
       ylab="slope of regression", col="blue", cex.main=1)
  axis(1, 1:12, labels=month.abb, las=2)
  if (save$Analysis$all){dev.off()}


  
} # end of function

# #------------------  ANALYZE BVU FORECAST ------------------
# setwd("S:/Risk/2004, Q1/Structured Deals/Bristol")
# fdata.BVU <- read.csv("fdata.BVU.csv", skip=1, 
#              header = TRUE, sep =",", row.names = NULL )
# setwd("C:/R/Work/Energy/StructuredDeals/Bristol")
# N <- dim(fdata.BVU)[1]

# datesCH <- chron(unique(as.character(fdata.BVU$Day)))
# YMD <- ymd(datesCH, rep=1)

# uYears <- 2005:2009
# aux=NULL
# for (yr in 1:length(uYears)){
#   aux <- cbind(aux, tapply(fdata.BVU[,2+yr], fdata.BVU$Day, sum))
# }
# colnames(aux) <- uYears; fload.BVU.day <- cbind(YMD,aux)
# rm(aux); rm(YMD)
# fload.BVU.day[1:10,]

# fload.BVU.month=NULL; fload.BVU.year=NULL
# for (yr in 1:length(uYears)){
#   aux <- tapply(fload.BVU.day[,3+yr],fload.BVU.day$month, mean, na.rm=TRUE)
#   fload.BVU.month <- cbind(fload.BVU.month, aux)
#   aux <- tapply(fload.BVU.day[,3+yr],fload.BVU.day$year, sum, na.rm=TRUE)  
#   fload.BVU.year <- cbind(fload.BVU.year, aux )
# }
# colnames(fload.BVU.month) <- uYears; colnames(fload.BVU.year) <- uYears
# rownames(fload.BVU.month) <- month.abb
# fload.BVU.month <- fload.BVU.month/1000/24   # average value
# fload.BVU.year <- fload.BVU.year/1000
# fload.BVU.month <- signif(fload.BVU.month, digits=3)
# fload.BVU.month; fload.BVU.year

# #-------------------- Save table with BVU monthly averages ------
# fileName <- paste(save$dir$Rtables,"fload.U.month.tex", sep="")
# print.xtable(xtable(fload.BVU.month, caption="Average hourly load (MWh) from the BVU load forecast by month.",
#   display=c("s",rep("f",length(uYears))), digits=c(0,1,1,1,1,1)),
#              file=fileName)

# #-------------------- Plot BVU forecast and min-max wn limits ---
# hs.load.qtile=NULL
# for (m in 1:12){
#   ind <- which(hs.load$month==m)
#   aux <- as.matrix(hs.load[ind,4:(3+no.years.hist)])
#   bux=NULL
#   for (s in 1:ncol(aux)){ bux[s] <- sum(aux[,s])}
#   cux <- c(min(bux),median(bux),max(bux))/(nrow(aux)*24)
#   hs.load.qtile <- rbind(hs.load.qtile, cux)
# }
# hs.load.qtile <- rbind(hs.load.qtile, cux) # add one more
# colnames(hs.load.qtile) <- c("min","median","max")
# rownames(hs.load.qtile) <- c(1:12,12)

# for (yr in 1:lenght(uYears)){
#   main=paste(paste("Forecast ",options$loadName,",",sep=""),uYears[yr])
#   fileName <- paste(save$dir$plots,"wnload_minmax.pdf", sep="")
#   if (save$Analysis$all){pdf(fileName, width=8.0, heigh=5.5)}
 
#   matplot(hs.load.qtile[,c(1,3)], type="s", lty=1, 
#           xaxt="n", col=c("pink","pink"),
#           xlab="Month", ylab="Average hourly load, MWh", main=main, cex.main=1)
#   xticks <- 1.5:12.5
#   points(xticks,fload.BVU.month[,1], pch=19, col="blue")
#   points(xticks,hs.load.qtile[1:12,2], col="grey")
#   axis(side=1, at=xticks, labels=month.abb)
#   legend(8,95, c("BVU forecast", "Weather normalized load"),
#            col=c("blue","grey"), bty="n", pch=c(19,1))
#   if (save$Analysis$all){dev.off()}
# }



#   rownames(hs.load.month) <- mth.label  
#   rownames(hs.load.month) <- mth.label
#   aux <- stack(as.data.frame(t(hs.load.month)))
#   boxplot(values ~ ind, data=aux, col="lavender", xaxt="n")
#   axis(1, 1:12, labels=mth.label, las=2)

#   matplot(cbind(1:(no.uMonths+1),1:(no.uMonths+1)),
#       rbind(hs.load.qtile[,c(1,3)], hs.load.qtile[no.uMonths,c(1,3)]),
#       type="s", lty=1, xaxt="n", col=c("pink","pink"), xlim=c(1,13),
#       xlab="", ylab="Monthly load, MWh", main=main, cex.main=1,
#       ylim=range(hs.load.qtile, fload.TLP.month$x, fload.RMG.month$x))
 


