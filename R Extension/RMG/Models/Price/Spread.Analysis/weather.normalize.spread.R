# Calculate and plot the sensitivity of the spread to temperature
# where OutT is the table with mean by month
# 
# Written by Adrian Dragulescu on 31-Jan-2005


weather.normalize.spread <- function(MM, OutT, options, save){

  fName <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.",
                 options$airport.name, ".Rdata",sep="")
  load(fName)
  options$one.column <- 1 
  hspread.month <- mean.by.month(MM, options, save)
  hspread.month <- na.omit(hspread.month)
  dSpread <- as.Date(paste(hspread.month$year,hspread.month$month,"1",sep="-"))
  dTemp   <- as.Date(paste(htemp.month$year,htemp.month$month,"1",sep="-"))
  aux  <- intersect.ts(dTemp,   htemp.month$Mean,
                           dSpread, hspread.month$Mean)[[1]]
  x.WN <- temp.month$mean[as.numeric(format(aux$dates, "%m"))]
  X    <- sort(x.WN, index.return=T)
  aux  <- aux[X$ix,]
  y    <- aux$y; x <- aux$x; x.WN <- x.WN[X$ix]
  abs.delta.x <- x-x.WN
#browser()  
  if (options$polynomial==2){ type <- "quadratic"
    if (options$regression=="rlm"){
      type <- paste("robust", type)
      reg  <- rlm(y ~ x.WN + I(x.WN^2) + abs.delta.x)
    } else {reg  <- lm(y ~ x.WN + I(x.WN^2) + abs.delta.x)}
  } else if (options$polynomial==1){ type <- "linear"
    if (options$regression=="rlm"){
      type <- paste("robust", type); reg  <- rlm(y ~ x.WN + abs.delta.x)
    } else {reg  <- lm(y ~ x.WN + abs.delta.x)}
  }
  
  if (save$Analysis$all){
    fName <- "wn_spread.pdf"
    pdf(fName)
    plot(x.WN,y, col="blue", xlab="Monthly WN temperature",
         ylab="Average monthly spread", xlim=c(0.97*min(x.WN), 1.03*max(x.WN)))
    points(x.WN, predict(reg, newdata=data.frame(x.WN, 0)), col="red", pch=2)
    data <- data.frame(x=x.WN, yEx=y, yTh=predict(reg, data.frame(x.WN)),
                       label=format(aux$dates, "%b%y"))
    label.outliers(na.omit(data))
    dev.off()
   }

   #-----------------Table: mean by month -----------------
   xnew <- data.frame(x.WN=temp.month$mean, abs.delta.x=0)
   spread.WN <- predict(reg, xnew)
   OutT <- cbind(OutT, WN=c(spread.WN, mean(spread.WN)))
   fileName <- paste(save$dir,"hMean_month.tex", sep="")
   caption  <- paste("Historical mean spread by month for ", options$between, ".", sep="")
   caption  <- paste(caption,"The column labeled WN is the weather normalized spread.")
   print(xtable(OutT, caption=caption), file=fileName)

   fileName <- paste(save$dir,"wn_spread_regression.tex", sep="")
   caption  <- paste("Details of the", type, "regression of monthly spread vs. monthly temperature.")
   if (options$regression=="lm"){
     print(xtable(summary(reg), caption=caption), file=fileName)}
   if (options$regression=="rlm"){
     source("C:/R/Work/MyLibrary/Statistics/xtable.summary.rlm.R")
     print(xtable.summary.rlm(summary(reg), caption=caption), file=fileName)}

   #-----------------Save to file -----------------
   fileName <- paste(save$dir,"sumreg_monthly.tex", sep="")
   sink(fileName)
   cat("\\begin{Verbatim}[baselinestretch=1, fontsize=\\footnotesize]\n")
   print(summary(reg))
   cat("Correlation matrix of coefficients: \n")
   print(round(cov2cor(vcov(reg)),3))
   cat("\\end{Verbatim}\n")
   sink()
    
  
   return(reg)
}
