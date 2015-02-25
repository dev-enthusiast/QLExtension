#-------------------------------------------------------------------------------
rm(list=ls())
require(SecDb)
require(UsingR)
require(grDevices)
setwd('H:/_Work/_Deal Review/20080314.WestValley')

#-------------------------------------------------------------------------------
GetDiscountFactor <- function(DatesFrom,DatesTo,DatePricing,Currency)
{
    require(SecDb)
    DateLow  <- min(min(DatesFrom),min(DatesTo))
    DateHigh <- max(max(DatesFrom),max(DatesTo))
    if( DatePricing==as.Date(secdb.dateRuleApply(Sys.Date(),'-1b')) )
    {
        DFCurve <- tsdb.readCurve(paste(Currency,'_Discount_Factor',sep=''),DateLow,DateHigh)
    }else
        DFCurve <- as.data.frame(secdb.invoke('_LIB Matlab Functions','MATLAB::getDatedDF',
                                              DatePricing,DateLow,DateHigh,Currency))
    DiscountFactorsTo     <- approx(DFCurve$date,DFCurve$value,DatesTo)
    DiscountFactorsFrom   <- approx(DFCurve$date,DFCurve$value,DatesFrom)
    DiscountFactors       <- NULL
    DiscountFactors$Date  <- DatesFrom
    DiscountFactors$Value <- DiscountFactorsFrom$y/DiscountFactorsTo$y
    return(as.data.frame(DiscountFactors))
}

#-------------------------------------------------------------------------------
TsdbReadCurve <- function( curveName, startDate, endDate, debug=FALSE )
{
    curve = as.character(curveName)
    if (class(startDate)[1]=="Date")
      startDate <- as.POSIXct(as.POSIXlt(startDate), tz="EST5EDT")
    start <- as.POSIXct(startDate)
    if (class(endDate)[1]=="Date")
      endDate <- as.POSIXct(as.POSIXlt(endDate+1)-1, tz="EST5EDT")
    end   <- as.POSIXct(endDate)

    if(debug)
    {
        cat("Calling TSDBRead with ", curve, " ", start, " ", end, "\n")
    }

    returnValue <- secdb.invoke("_lib elec matlab dispatch fns", "TSDB::Read",
            curve, start, end)

    return(data.frame(returnValue))
}

#-------------------------------------------------------------------------------
GetHistMthlyAvgPWSpotPrice <- function(CurveName,HistDate)
{
    HistMthBeg     <- as.Date(secdb.dateRuleApply(HistDate,'e-1m'))+1
    HistMthEnd     <- as.Date(secdb.dateRuleApply(HistDate,'e'))
    HistHours      <- secdb.getHoursInBucket( "wscc", "6x16", HistMthBeg, HistMthEnd)
    HistPrices     <- TsdbReadCurve(CurveName,HistMthBeg,HistMthEnd)
    HistPeakPrices <- merge(HistPrices, as.data.frame(HistHours), by='time')
    return(mean(HistPeakPrices$value))
    return(mean(HistPrices$value))
}

#-------------------------------------------------------------------------------
GetHistMthlyAvgNGSpotPrice <- function(CurveName,HistDate)
{
    HistMthBeg <- as.Date(secdb.dateRuleApply(HistDate,'e-1m'))+1
    HistMthEnd <- as.Date(secdb.dateRuleApply(HistDate,'e'))
    HistPrices <- TsdbReadCurve(CurveName,HistMthBeg,HistMthEnd)
    return(mean(HistPrices$value))
}

#-------------------------------------------------------------------------------
GetFwdPrice <- function(CommodName,FwdMth,DatePricing)
{
#    CrtDateBeg <- as.Date(secdb.dateRuleApply(FwdMth,'e-1m'))+1
    CrtDateBeg <- FwdMth
    CrtDateEnd <- as.Date(secdb.dateRuleApply(FwdMth,'e'))
    FwdCurve   <- as.data.frame(secdb.invoke("_LIB Matlab Functions","MATLAB::GetCommodFutCurve",
                                             CommodName,CrtDateBeg,CrtDateEnd,DatePricing))
    return(FwdCurve$value)
}

#-------------------------------------------------------------------------------
GetAdjustmentRatio <- function(CurveName,HistDate,CommodName,FwdMth,DatePricing)
{
    if( length(grep('PW',toupper(CommodName))) )
    {
        HistPrice <- GetHistMthlyAvgPWSpotPrice(CurveName,HistDate)
    }else if( length(grep('NG',toupper(CommodName))) )
        HistPrice <- GetHistMthlyAvgNGSpotPrice(CurveName,HistDate)
    FwdPrice <- GetFwdPrice(CommodName,FwdMth,DatePricing)
    
    return(FwdPrice/HistPrice)
}

#-------------------------------------------------------------------------------
GetMatrixRatios <- function(CurveName,CommodName,HistMths,TermMths,DatePricing)
{
    MatrixRatios <- NULL
    for( HistM in HistMths )
    {
        class(HistM) <- 'Date'
        VectorRatios <- NULL
        for( TermM in TermMths )
        {
            class(TermM) <- 'Date'
            VectorRatios <- c(VectorRatios,GetAdjustmentRatio(CurveName,HistM,CommodName,TermM,DatePricing))
        }
        MatrixRatios <- rbind(MatrixRatios, VectorRatios)
    }
    return(MatrixRatios)
}

#-------------------------------------------------------------------------------
DatePricing      <- Sys.Date()
DateTermBeg      <- as.Date('2008-06-01')
DateTermEnd      <- as.Date('2012-05-31')
DateHistBeg      <- as.Date('2004-08-01')
DateHistEnd      <- as.Date('2007-12-31')
TermDays         <- seq(DateTermBeg,DateTermEnd,by='1 day')
TermMths         <- seq(DateTermBeg,DateTermEnd,by='1 month')
HistMths         <- seq(DateHistBeg,DateHistEnd,by='1 month')
HistMthsUnique   <- unique(HistMths)
MWCapacity       <- c(220.6, 217.7, 211.8, 208.8, 202.9, 197.0, 191.1, 194.0, 200.0, 205.9, 214.7, 220.6)
HeatRate         <- c(9.554, 9.554, 9.574, 9.603, 9.671, 9.739, 9.876, 9.817, 9.710, 9.632, 9.554, 9.554)
CapacityMthlyBus <- c(177, 172, 175, 180, 185, 193, 199, 199, 196, 191, 188, 183,
                      177, 172, 175, 180, 185, 193, 199, 199, 196, 191, 188, 183,
                      177, 172, 175, 180, 185, 193, 198, 198, 196, 190, 188, 182,
                      177, 172, 175, 180, 185, 193, 198, 198, 196, 190, 188, 182)
CapacityMthlyHub <- c(191, 185, 188, 194, 199, 208, 213, 213, 211, 205, 202, 196,
                      191, 185, 188, 193, 199, 208, 213, 213, 211, 205, 202, 196,
                      191, 185, 188, 193, 199, 208, 213, 213, 211, 205, 202, 196,
                      190, 185, 188, 193, 199, 208, 213, 213, 211, 205, 202, 196)
HeatRateMthly    <- c(9.939, 10.078, 10.019, 9.909, 9.830, 9.750, 9.761, 9.761, 9.761, 9.781, 9.810, 9.880,
                      9.950, 10.089, 10.029, 9.920, 9.840, 9.761, 9.761, 9.761, 9.761, 9.781, 9.810, 9.880,
                      9.950, 10.089, 10.040, 9.931, 9.851, 9.771, 9.771, 9.771, 9.771, 9.791, 9.821, 9.891,
                      9.960, 10.100, 10.040, 9.931, 9.851, 9.771, 9.771, 9.771, 9.771, 9.791, 9.832, 9.901)
StrikeMthlyHub   <- c(11.34, 11.34, 11.34, 11.34, 11.34, 11.34, 11.34, 11.48, 11.48, 11.48, 11.48, 11.48,
                      11.48, 11.48, 11.48, 11.48, 11.48, 11.48, 11.48, 11.63, 11.63, 11.63, 11.63, 11.63,
                      11.63, 11.63, 11.63, 11.63, 11.63, 11.63, 11.63, 11.79, 11.79, 11.79, 11.79, 11.79,
                      11.79, 11.79, 11.79, 11.79, 11.79, 11.79, 11.79, 11.94, 11.94, 11.94, 11.94, 11.94)
StrikeMthlyBus   <- c(4.50, 4.50, 4.50, 4.50, 4.50, 4.50, 4.50, 4.51, 4.51, 4.51, 4.51, 4.51,
                      4.51, 4.51, 4.51, 4.51, 4.51, 4.51, 4.51, 4.52, 4.52, 4.52, 4.52, 4.52,
                      4.52, 4.52, 4.52, 4.52, 4.52, 4.52, 4.52, 4.53, 4.53, 4.53, 4.53, 4.53,
                      4.53, 4.53, 4.53, 4.53, 4.53, 4.53, 4.53, 4.54, 4.54, 4.54, 4.54, 4.54)
TransLossBus     <- 0.00
TransLossHub     <- 0.04
Pct.Bus          <- 0.8
TSDB.Symbol.PW   <- 'PWIDX_Mona_Avg'
TSDB.Symbol.NG   <- 'GasDailyMean_KERN'
Commod.PW        <- 'Commod PWP 6x16 MONA Physical'
Commod.NG        <- 'Commod NG NORRM Gas-Dly Mean'
SimNum           <- 2e2

AdjRatios.PW   <- GetMatrixRatios(TSDB.Symbol.PW,Commod.PW,HistMths,TermMths,DatePricing)
AdjRatios.NG   <- GetMatrixRatios(TSDB.Symbol.NG,Commod.NG,HistMths,TermMths,DatePricing)

#-------------------------------------------------------------------------------
HistPrice.PW    <- TsdbReadCurve(TSDB.Symbol.PW,DateHistBeg,DateHistEnd)
HistPrice.NG    <- TsdbReadCurve(TSDB.Symbol.NG,DateHistBeg,DateHistEnd)
HistValue       <- NULL
VectorHistValue <- NULL
VectorHistMth   <- NULL
VectorPrice.PW  <- NULL
VectorPrice.NG  <- NULL
for( Mth in HistMths )
{
    class(Mth) <- 'Date'
    Payoff     <- NULL
    IdxMth.NG  <- which( format(HistPrice.NG$date,'%Y%m') == format(Mth,'%Y%m') )
    if( length(IdxMth.NG)>0 )
    {
        for( IdxDay.NG in IdxMth.NG )
        {
            IdxDay.PW <- which( format(HistPrice.PW$time,'%Y%m%d') == format(HistPrice.NG$date[IdxDay.NG],'%Y%m%d') )
            if( length(IdxDay.PW)>0 )
            {
                PayoffBus      <- sum(pmax(0, (1-TransLossBus)*HistPrice.PW$value[IdxDay.PW] - HeatRate[as.numeric(format(Mth,'%m'))]*HistPrice.NG$value[IdxDay.NG] - mean(StrikeMthlyBus)))
                PayoffHub      <- sum(pmax(0, (1-TransLossHub)*HistPrice.PW$value[IdxDay.PW] - HeatRate[as.numeric(format(Mth,'%m'))]*HistPrice.NG$value[IdxDay.NG] - mean(StrikeMthlyHub)))
                Payoff         <- c(Payoff, MWCapacity[as.numeric(format(Mth,'%m'))]*(Pct.Bus*PayoffBus+(1-Pct.Bus)*PayoffHub))
                VectorPrice.PW <- rbind(VectorPrice.PW, HistPrice.PW$value[IdxDay.PW])
                VectorPrice.NG <- rbind(VectorPrice.NG, rep(HistPrice.NG$value[IdxDay.NG],length(IdxDay.PW)))
            }
        }
    }
    HistValue[[format(Mth,'%y%m')]] <- Payoff
    VectorHistValue                 <- c(VectorHistValue,Payoff)
    VectorHistMth                   <- c(VectorHistMth,rep(Mth,length(Payoff)))
}
class(VectorHistMth) <- 'Date'
VectorHistValue      <- 1e-6*VectorHistValue

#-------------------------------------------------------------------------------
AdjRatios.Row        <- 1:length(HistMthsUnique)
names(AdjRatios.Row) <- HistMthsUnique
AdjRatios.Col        <- 1:length(TermMths)
names(AdjRatios.Col) <- TermMths

SimValue  <- NULL
TermMthID <- 0
for( m in TermMths )
{
    TermMthID <- TermMthID+1
    class(m)  <- 'Date'
    MthEnd    <- as.Date(secdb.dateRuleApply(m,'+1m'))-1
    DF        <- GetDiscountFactor(MthEnd,DatePricing,DatePricing,'USD')
    Idx       <- which( format(VectorHistMth,'%m')==format(m,'%m') )
    for( d in m:MthEnd )
    {
        SimIdx        <- sample(Idx,SimNum,replace=T)
        IdxRow        <- as.numeric(AdjRatios.Row[format(VectorHistMth[SimIdx],'%Y-%m-01')])
        IdxCol        <- as.numeric(AdjRatios.Col[format(m,'%Y-%m-01')])
        Price.PW      <- AdjRatios.PW[IdxRow,IdxCol]*VectorPrice.PW[SimIdx,]
        Price.NG      <- 1.1*AdjRatios.NG[IdxRow,IdxCol]*VectorPrice.NG[SimIdx,]+0.42
        OutageIdx     <- runif(length(Price.PW))<0.05
        Payoff.HR.Bus <- pmax(0, (1-OutageIdx)*(1-TransLossBus)*Price.PW - HeatRateMthly[TermMthID]*Price.NG - StrikeMthlyBus[TermMthID])
        Payoff.HR.Hub <- pmax(0, (1-OutageIdx)*(1-TransLossHub)*Price.PW - HeatRateMthly[TermMthID]*Price.NG - StrikeMthlyHub[TermMthID])
        Payoff.Bus    <- CapacityMthlyBus[TermMthID]*apply(matrix(Payoff.HR.Bus,nrow=dim(Price.PW)[1]),1,sum)
        Payoff.Hub    <- CapacityMthlyHub[TermMthID]*apply(matrix(Payoff.HR.Hub,nrow=dim(Price.PW)[1]),1,sum)
        SimValue      <- c(SimValue,Pct.Bus*Payoff.Bus+(1-Pct.Bus)*Payoff.Hub)
#        SimValue      <- c(SimValue,DF$Value*(Pct.Bus*Payoff.Bus+(1-Pct.Bus)*Payoff.Hub))
    }
}
SimTotalValue <- 1e-6*apply(matrix(SimValue,nrow=SimNum),1,sum)

#-------------------------------------------------------------------------------
windows()
par(xaxt="n")
simple.violinplot(VectorHistValue ~ VectorHistMth, col='wheat',bw='nrd0',kernel='rectangular',give.Rkern=T)
HistValue.Mean     <- tapply(VectorHistValue,VectorHistMth, mean)
HistValue.50thQtl  <- tapply(VectorHistValue,VectorHistMth, quantile,0.50)
Zeros        <- rep(0,length(HistValue.Mean))
names(Zeros) <- names(HistValue.Mean)
points(HistValue.Mean, pch=19, type='b',col='red')
points(HistValue.50thQtl, pch=23, type='b',col='blue')
points(Zeros, type='l',col='green',lty=2,lwd=2)
par(xaxt="s")
axis(1, at=1:length(unique(VectorHistMth)), labels=format(unique(VectorHistMth),'%b%Y'), las=2, cex.axis=0.7, font=2)
mtext('HistValue ($MM)', side=2, line=2.2)
mtext('HistValue Mthly Distributions', side=3, line=0, font=2)
legend(list(x=length(unique(VectorHistMth))/2.5,y=max(VectorHistValue)), c('Mean','50th Qtl'), pch=c(19,23), lty=1, col=c('red','blue'),bg='wheat', merge=TRUE, trace=FALSE)

windows()
layout.show(layout(matrix(c(1,2,3,3),4,1,byrow=TRUE), respect=FALSE))
Qtls   <- c(0.05,0.25,0.5,0.75,0.95)
sTitle <- paste('SimTotalValue - ',100*Pct.Bus,'%@Bus & ',100*(1-Pct.Bus),'%@Hub - HistDate[',DateHistBeg,' to ',DateHistEnd,']',sep='')
plot(density(SimTotalValue),xlim=quantile(SimTotalValue,c(0,1)),type='l', col='blue', main=sTitle, xlab='$MM', ylab='Empirical.PDF')
hist(SimTotalValue, breaks=SimNum/10, col = 'wheat', xlim = range(SimTotalValue), ylim = NULL, axes = TRUE, plot = TRUE,
     main=sTitle, xlab = '$MM', ylab = paste('# outof',length(SimTotalValue),'Observations'))
plot(ecdf(SimTotalValue),xlim=quantile(SimTotalValue,c(0,1)), main=sTitle, xlab='$MM', ylab='Empirical.CDF')
points(quantile(SimTotalValue,Qtls),Qtls, type='p', col='red', pch=19, cex=2)
text(quantile(SimTotalValue,Qtls),Qtls, labels=paste(Qtls*100,'th Qtl: ',format(quantile(SimTotalValue,Qtls),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
points(mean(SimTotalValue),sum(SimTotalValue<=mean(SimTotalValue))/length(SimTotalValue), type='p', col='red', pch=19, cex=2)
text(mean(SimTotalValue),0.05+sum(SimTotalValue<=mean(SimTotalValue))/length(SimTotalValue), labels=paste('Mean: ',format(mean(SimTotalValue),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)

windows()
layout.show(layout(matrix(c(1,2),2,1,byrow=TRUE), respect=FALSE))
Qtls   <- c(0.05,0.25,0.5,0.75,0.95)
sTitle <- paste('SimTotalValue - ',100*Pct.Bus,'%@Bus & ',100*(1-Pct.Bus),'%@Hub - HistDate[',DateHistBeg,' to ',DateHistEnd,']',sep='')
plot(ecdf(SimTotalValue),xlim=quantile(SimTotalValue,c(0,1)), main=sTitle, xlab='$MM', ylab='Empirical.CDF')
points(quantile(SimTotalValue,Qtls),Qtls, type='p', col='red', pch=19, cex=2)
text(quantile(SimTotalValue,Qtls),Qtls, labels=paste(Qtls*100,'th Qtl: ',format(quantile(SimTotalValue,Qtls),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
points(mean(SimTotalValue),sum(SimTotalValue<=mean(SimTotalValue))/length(SimTotalValue), type='p', col='red', pch=19, cex=2)
text(mean(SimTotalValue),0.05+sum(SimTotalValue<=mean(SimTotalValue))/length(SimTotalValue), labels=paste('Mean: ',format(mean(SimTotalValue),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
SimValue.kWMth <- 1e6*SimTotalValue/(length(TermMths)*200*1e3)
plot(ecdf(SimValue.kWMth),xlim=quantile(SimValue.kWMth,c(0,1)), main=sTitle, xlab='$/kW-Mth', ylab='Empirical.CDF')
points(quantile(SimValue.kWMth,Qtls),Qtls, type='p', col='red', pch=19, cex=2)
text(quantile(SimValue.kWMth,Qtls),Qtls, labels=paste(Qtls*100,'th Qtl: ',format(quantile(SimValue.kWMth,Qtls),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
points(mean(SimValue.kWMth),sum(SimValue.kWMth<=mean(SimValue.kWMth))/length(SimValue.kWMth), type='p', col='red', pch=19, cex=2)
text(mean(SimValue.kWMth),0.05+sum(SimValue.kWMth<=mean(SimValue.kWMth))/length(SimValue.kWMth), labels=paste('Mean: ',format(mean(SimValue.kWMth),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
