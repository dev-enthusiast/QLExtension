#-------------------------------------------------------------------------------
rm(list=ls())
require(SecDb)
require(UsingR)
require(grDevices)

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
SimNum       <- 1e3
MWCapacity   <- 150
LiquidFactor <- 0.9
DateTermBeg  <- as.Date('2008-04-01')
DateTermEnd  <- as.Date('2008-06-30')
DateHistBeg  <- as.Date('2006-01-01')
DateHistEnd  <- as.Date('2007-12-31')
DatePricing  <- Sys.Date()
TSDB.Symbol.Sink   <- 'ICE_10xP_WE_Mona_Peak_Avg'
TSDB.Symbol.Source <- 'mid_firm_onp_prc'

#-------------------------------------------------------------------------------
HistDAPrice.Source <- tsdb.readCurve(TSDB.Symbol.Source,DateHistBeg,DateHistEnd)
HistDAPrice.Sink   <- tsdb.readCurve(TSDB.Symbol.Sink,DateHistBeg,DateHistEnd)
HistDAPrice        <- merge(HistDAPrice.Source,HistDAPrice.Sink,by='date')
names(HistDAPrice) <- c('Date','Source','Sink')
HistDAPrice$Year   <- format(HistDAPrice$Date,'%Y')
HistDAPrice$Month  <- format(HistDAPrice$Date,'%m')
HistDAPrice        <- subset(HistDAPrice, Month %in% unique(format(seq(DateTermBeg,DateTermEnd,by='1 mon'),'%m')))
HistDAPrice        <- subset(HistDAPrice, Year %in% c(2006,2007))
HistDASpread       <- pmax(0,0.9766*0.9552*HistDAPrice$Sink-HistDAPrice$Source)

#-------------------------------------------------------------------------------
windows()
Qtls = c(0.05,0.25,0.5,0.75,0.95)
layout.show(layout(matrix(c(1,2,3,3),4,1,byrow=TRUE), respect=FALSE))
plot(density(HistDASpread),xlim=quantile(HistDASpread,c(0,1)),type='l', col='blue', main='Empirical.PDF.HistDASpread', xlab='$/MWh', ylab='Empirical.PDF')
hist(HistDASpread, breaks='Sturges', col = 'wheat', xlim = range(HistDASpread), ylim = NULL, axes = TRUE, plot = TRUE,
     main = 'Histogram.HistDASpread', xlab = '$/MWh', ylab = paste('# outof',length(HistDASpread),'Observations'))
plot(ecdf(HistDASpread),xlim=quantile(HistDASpread,c(0,1)), main='Empirical.CDF.HistDASpread', xlab='$/MWh', ylab='Empirical.CDF')
points(quantile(HistDASpread,Qtls),Qtls, type='p', col='red', pch=19, cex=2)
text(quantile(HistDASpread,Qtls),Qtls, labels=paste(Qtls*100,'th Qtl: ',format(quantile(HistDASpread,Qtls),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)

#-------------------------------------------------------------------------------
windows()
HistMths <- as.Date(paste(HistDAPrice$Year,HistDAPrice$Month,1,sep='-'))
par(xaxt="n")
simple.violinplot(HistDASpread ~ HistMths, col="wheat")
Spread.Mean     <- tapply(HistDASpread,HistMths, mean)
Spread.50thQtl  <- tapply(HistDASpread,HistMths, quantile,0.50)
Zeros        <- rep(0,length(Spread.Mean))
names(Zeros) <- names(Spread.Mean)
points(Spread.Mean, pch=19, type='b',col='red')
points(Spread.50thQtl, pch=23, type='b',col='blue')
points(Zeros, type='l',col='green',lty=2,lwd=2)
par(xaxt="s")
axis(1, at=1:length(unique(HistMths)), labels=format(unique(HistMths),'%b%Y'), las=2, cex.axis=0.7, font=2)
mtext('Spread ($/MWh)', side=2, line=2.2)
mtext('HistSpread Mthly Distributions - MidC->Mona', side=3, line=0, font=2)
legend(list(x=length(unique(HistMths))/2.5,y=quantile(HistDASpread,0.995)), c('Mean','50th Qtl'), pch=c(19,23), lty=1, col=c('red','blue'),bg='wheat', merge=TRUE, trace=FALSE)

#-------------------------------------------------------------------------------
DatesFrom <- NULL
Payoffs   <- NULL
ThisDate  <- as.Date(secdb.dateRuleApply(DateTermBeg-1,'+1b'))
while( ThisDate <= DateTermEnd )
{
    DatesFrom <- c(DatesFrom,as.character(ThisDate))
    Payoffs   <- cbind(Payoffs,matrix(sample(HistDASpread,SimNum*16,replace=T),nrow=SimNum))
    ThisDate  <- as.Date(secdb.dateRuleApply(ThisDate,'+1b'))
}
Payoffs         <- 1e-6*Payoffs*MWCapacity*LiquidFactor*6/5
DiscountFactors <- GetDiscountFactor(as.Date(DatesFrom),DatePricing,DatePricing,'USD')
TermPayoffSims  <- apply(Payoffs*matrix(rep(rep(DiscountFactors$Value,each=16),SimNum),nrow=SimNum),1,sum)
windows()
layout.show(layout(matrix(c(1,2,3,3),4,1,byrow=TRUE), respect=FALSE))
Qtls = c(0.05,0.25,0.5,0.75,0.95)
#FileName <- 'H:/_Work/Programming. R/Functions/MidC2Mona.TermPayoff.Sims.pdf'
#pdf(file=FileName, width=7, height=3, onefile=TRUE, family ='Helvetica', paper='special', 
#    encoding='CP1250.enc', bg='transparent', pointsize=12, pagecentre=T)
plot(density(TermPayoffSims),xlim=quantile(TermPayoffSims,c(0,1)),type='l', col='blue', main='MidC->Mona.Empirical.PDF.TermPayoffs.Sim', xlab='$MM', ylab='Empirical.PDF')
hist(TermPayoffSims, breaks=100, col = 'wheat', xlim = range(TermPayoffSims), ylim = NULL, axes = TRUE, plot = TRUE,
     main = 'MidC->Mona.Histogram.TermPayoffs.Sim', xlab = '$MM', ylab = paste('# outof',length(TermPayoffSims),'Observations'))
plot(ecdf(TermPayoffSims),xlim=quantile(TermPayoffSims,c(0,1)), main='MidC->Mona.Empirical.CDF.TermPayoffs.Sim', xlab='$MM', ylab='Empirical.CDF')
points(quantile(TermPayoffSims,Qtls),Qtls, type='p', col='red', pch=19, cex=2)
text(quantile(TermPayoffSims,Qtls),Qtls, labels=paste(Qtls*100,'th Qtl: ',format(quantile(TermPayoffSims,Qtls),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
points(mean(TermPayoffSims),sum(TermPayoffSims<=mean(TermPayoffSims))/length(TermPayoffSims), type='p', col='red', pch=19, cex=2)
text(mean(TermPayoffSims),0.05+sum(TermPayoffSims<=mean(TermPayoffSims))/length(TermPayoffSims), labels=paste('Mean: ',format(mean(TermPayoffSims),digits=3)), pos=4, offset=1, col='red', font=10, cex=1.2)
#dev.off()