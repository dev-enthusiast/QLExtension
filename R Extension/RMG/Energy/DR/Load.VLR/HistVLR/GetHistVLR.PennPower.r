rm(list=ls())
setwd('H:/_Work/Programming. R')

source('H:/_Work/Programming. R/Functions/Utilities.TLP.R')
source('H:/_Work/Programming. R/Functions/Utilities.SecDB.TSDB.R')

LoadWizAccount <- 'PAPP SC_OHN'    
TariffDateBeg  <- as.Date('2008-06-01')
TariffDateEnd  <- as.Date('2009-05-31')
LoadWizInfo    <- GetLoadWizInfo(LoadWizAccount,TariffDateBeg,TariffDateEnd)

TSDBPriceSymbol <- 'PJMDAM_RECO_ZONE'
HistDateBeg     <- as.Date('2006-06-01')
HistDateEnd     <- as.Date('2007-05-31')
windows()
HistLoad        <- GetPlotHistLoads(LoadWizInfo$TSDBLOADHIST,'MW',HistDateBeg,HistDateEnd)
SpotPrices      <- GetPlotSpotPrices(TSDBPriceSymbol,'$/MWh',HistDateBeg,HistDateEnd)

HistData <- merge(HistLoad,SpotPrices,by<-'time')
windows()
layout.show(layout(matrix(c(1:2),2,1,byrow=TRUE), c(3,3), c(2,2), respect=FALSE))
par(mar=c(3,4,3,2))
plot( HistData[c('time','value.x')], type="l", col="blue",
      main=paste("HistLoad - ",LoadWizInfo$TSDBLOADHIST), xlab='Hist Date', ylab=paste('Hist Load (MW)') )
plot( HistData[c('time','value.y')], type="l", col="blue",
      main=paste("HistSpotPrice - ",TSDBPriceSymbol), xlab='Hist Date', ylab=paste('Hist SpotPrice ($/MWh)') )

HistMths  <- seq(HistDateBeg,HistDateEnd,by='1 month')
VLRMthly  <- NULL
VLRHourly <- NULL
FactorMth <- NULL
for( Mth in HistMths )
{
    class(Mth)  <- 'Date'
    MatFileName <- paste(LoadWizInfo$LoadSimPath,format(Mth,'%y%m'),'.mat',sep='')
    MthBeg      <- as.POSIXct(ISOdatetime(as.numeric(format(Mth,'%Y')),as.numeric(format(Mth,'%m')),1,0,0,0))
    MthEnd      <- as.POSIXct(secdb.dateRuleApply(MthBeg,'1m')-1)
    MthBeg      <- ISOdatetime(as.numeric(format(Mth,'%Y')),as.numeric(format(Mth,'%m')),1,0,0,0)
    MthEnd      <- secdb.dateRuleApply(MthBeg,'e')+24*60*60-1
    HourID      <- seq(MthBeg,MthEnd,by='hour')
    if( file.exists(MatFileName) )
    {
        LoadSim  <- ReadLoadSimMatFile(MatFileName)
        LoadAct  <- NULL
        PriceAct <- NULL
        for( h in HourID )
        {
            Idx      <- which(HistLoad$time==h)
            LoadAct  <- rbind(LoadAct,ifelse(length(Idx),HistLoad$value[Idx]*LoadWizInfo$SettleScalar,NA))
            Idx      <- which(as.POSIXlt(SpotPrices$time,tz='GMT')==h)
            PriceAct <- rbind(PriceAct,ifelse(length(Idx),SpotPrices$value[Idx],NA))
        }
        Idx       <- which(LoadAct>1e-3)[which(which(LoadAct>1e-3) %in% which(PriceAct>0&PriceAct<1e3))]
        ThisVLRs  <-(LoadAct[Idx]-LoadSim[Idx])*(PriceAct[Idx]-mean(PriceAct[Idx]))/LoadAct[Idx]
        ThisVLRs  <- ThisVLRs[which(!is.na(ThisVLRs))]
        ThisVLRs  <- ThisVLRs[which(ThisVLRs<quantile(ThisVLRs,0.9)&ThisVLRs>quantile(ThisVLRs,0.1))]
        VLRMthly  <- rbind(VLRMthly,mean(ThisVLRs))
        VLRHourly <- cbind(VLRHourly,ThisVLRs)
    }else
    {
        ThisVLRs  <- rep(NA,length(HourID))
        VLRMthly  <- rbind(VLRMthly,NA)
        VLRHourly <- cbind(VLRHourly,ThisVLRs)
    }
    FactorMth <- cbind(FactorMth,rep(Mth,length(ThisVLRs)))
}
windows()
layout.show(layout(matrix(c(1:2),2,1,byrow=TRUE), c(3,3), c(2,2), respect=FALSE))
par(mar=c(5,4,3,2))
plot( HistMths,VLRMthly, type="h", col="blue",
      main=paste('HistVLR Mthly Mean - LoadWizID =',LoadWizAccount), xlab='Hist Mths', ylab=paste('Hist VLR ($/MWh)') )
axis(1, at=1:length(unique(VLRHourly$FactorMth)), labels=format(unique(VLRHourly$FactorMth),'%b%Y'), las=2, cex.axis=0.7)
par(mar=c(5,4,3,2))
plot( ecdf(VLRMthly), do.points=FALSE, verticals=TRUE,
      main=paste('HistVLR Empirical CDF - LoadWizID =',LoadWizAccount), xlab='Hist VLRs ($/MWh)', ylab='CDF')
windows()
ViolinPlotHistVLR(data.frame(VLRHourly=as.vector(VLRHourly),FactorMth=as.vector(FactorMth)),LoadWizAccount)

windows()
layout.show(layout(matrix(c(1:3),3,1,byrow=TRUE), c(3,3,3), c(3,3,4), respect=FALSE))
par(mar=c(5,4,3,2))
plot( HistMths,VLRMthly, type="h", col="blue",
      main=paste('HistVLR Mthly Mean - LoadWizID =',LoadWizAccount), xlab='Hist Mths', ylab=paste('Hist VLR ($/MWh)') )
axis(1, at=HistMths, labels=format(HistMths,'%b%y'))
par(mar=c(5,4,3,2))
plot( ecdf(VLRMthly), do.points=FALSE, verticals=TRUE,
      main=paste('HistVLR Empirical CDF - LoadWizID =',LoadWizAccount), xlab='Hist VLRs ($/MWh)', ylab='CDF')
par(mar=c(5,4,3,1))
ViolinPlotHistVLR(data.frame(VLRHourly=as.vector(VLRHourly),FactorMth=as.vector(FactorMth)),LoadWizAccount)
