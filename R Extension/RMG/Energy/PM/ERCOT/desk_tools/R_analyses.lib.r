#------------------------------------------------------------
# R_analyses.lib.r
#   to be used with R_analyses to hold all functions









#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(reshape)
require(FTR)


#source(paste(strHeader,"pnl/lib.pnl.R",paste="",sep=""))  ## For testing
## source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/pnl/lib.pnl.R")  ## For production
## source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
#require(RODBC)
#require(Tsdb)
## source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
## source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
#rLog(paste("Start proces at", Sys.time()))
#returnCode <- 0    # succeed = 0
#










#------------------------------------------------------------
# Functions
#





#------------------------------------------------------------
# Rank-ordered plot of heat rates for a given month and years
# arrHistorical=historical; startyear=Startyear; endyear=Endyear; include_months=Include_months; exclude_years=Exclude_years; bket=Bucket; sp_choice=SP_analysis; ymin=FALSE; ymax=FALSE; combine_years_on_graph=FALSE; market=Market; gas=Gas
# ; startyear=Startyear; endyear=Endyear; include_months=Include_months; exclude_years=Exclude_years; bket=Bucket; sp_choice=SP_analysis
#
do.analysis = function(arrHistorical, startyear=2006, endyear=2011, conn=channel_zema_prod, include_months=0, exclude_years=0, bket="7x24", sp_choice="HB_NORTH", ymin=FALSE, ymax=FALSE, combine_years_on_graph=TRUE, market="RT", gas="HSC")
    {

      #rLog("Get Data")
      if(endyear<startyear) return("Must choose an endyear greater than or equal to the start year!")

      ## Get correct location, bucket, years
        hist=subset(arrHistorical, SP==sp_choice & yr>=startyear & yr<=endyear)
        if(bket!="7x24" & bket!="wrap") hist=subset(hist, bucket==bket); 
        if(bket=="wrap") hist=subset(hist, bucket=="7x8" | bucket=="2x16h"); 
      ## Only include_months:
              monthmap=data.frame(month=include_months)
              hist=merge(hist, monthmap)
      ## Exclude years
              for(i in 1:length(exclude_years))
                {
                 if(exclude_years[i]<=2010 & exclude_years[i]>=startyear) hist=subset(hist, yr!=exclude_years[i])
                }

    ## Which market desired?
      ## Power
         if(market=="RT") {hist$pwrvariable=hist$RTSPP; variablename="RTHR"}
         if(market=="DA") {hist$pwrvariable=hist$DASPP; variablename="DAHR"}
         if(market=="OTC") {hist$pwrvariable=hist$plattspwr; variablename="OTCHR"}
      ## Gas
         if(gas=="HH") hist$gasvariable=hist$platts_hh
         if(gas=="HSC") hist$gasvariable=hist$platts_hsc
    
    ## Get HR and correct bucket
        hist$variable="HR"
       hist$value=hist$pwrvariable/hist$gasvariable
       combbgc=cast(hist, OPR_DATE+month+yr~variable, mean)
       combbgc=combbgc[order(combbgc$HR, decreasing=TRUE),]
       if(length(combbgc[is.na(combbgc)==TRUE,4])>0) return(paste("ERROR: missing data for ", paste(combbgc[is.na(combbgc[,4])==TRUE,1], collapse=" ,"),sep=""))
       ## subset(hist, OPR_DATE==combbgc[is.na(combbgc[,4])==TRUE,1])

    ## Graph results on one graph
    if (combine_years_on_graph==TRUE)
      {
       windows(14,12)
       if(ymin!=FALSE) ylimitsmin=ymin else ylimitsmin=min(combbgc$HR)
       if(ymax!=FALSE) ylimitsmax=ymax else ylimitsmax=max(combbgc$HR)
       plot(combbgc$HR, pch=20, bty="l", xlab="Count of days", ylab=variablename, main=paste(bket," ",SP_analysis," heatrate.  Months: ",paste(include_months,collapse=","),"  Years: ",paste(sort(unique(combbgc$yr)),collapse=","),sep=""), ylim=c(ylimitsmin,ylimitsmax))
       plotlines=quantile(combbgc$HR, c(0.05,0.25,0.5,0.75,0.95))
       qlevels=(c(0.05,0.25,0.5,0.75,0.95))
       for(i in 1:5)  {abline(h=plotlines[i],col="red"); text(nrow(combbgc)*((i-1)/5)+5, plotlines[i]+1,paste(qlevels[i],round(plotlines[i],1),sep=": "))}
       abline(h=mean(combbgc$HR), lwd=3,col="red"); text(nrow(combbgc)/2, mean(combbgc$HR)+1,paste("Mean",round(mean(combbgc$HR),2),sep=": "), cex=1.3)

       ## Get data
         resData=rbind(as.data.frame(plotlines), c(mean(combbgc$HR)))
         resData=round(resData,2)
         rownames(resData)[nrow(resData)]="mean"
      }
      
    ## Graph results on one graph per year
    if (combine_years_on_graph==FALSE)
      {
       windows(14,12)
       uniqueyears=unique(hist$yr); uniqueyears=sort(uniqueyears)
       rtct=length(uniqueyears)+1; if(rtct<=1) rc=1;  if(rtct<=4 & rtct>1) rc=2; if(rtct<=9 & rtct>4) rc=3; if(rtct<=16 & rtct>9) rc=4; if(rtct<=25 & rtct>16) rc=5; if(rtct<=36 & rtct>25) rc=6
       par(mfrow=c(rc,rc),mar=c(3,3,3,3))  
       if(ymin!=FALSE) ylimitsmin=ymin else ylimitsmin=min(combbgc$HR)
       if(ymax!=FALSE) ylimitsmax=ymax else ylimitsmax=max(combbgc$HR)
       ## Get data
         resData=combbgc[0,1:3]; colnames(resData)=c("yr","type","value")
       
       ## Plot indl years
       for (yeari in 1:length(uniqueyears))
        {
           ## Plot data
             combbgci = subset(combbgc, yr==uniqueyears[yeari])
             plot(combbgci$HR, pch=20, bty="l", xlab="Count of days", ylab=variablename, main=paste(bket," ",SP_analysis," ",variablename,".  Mos: ",paste(include_months,collapse=","),"  Yr: ",paste(sort(unique(combbgci$yr)),collapse=","),sep=""), ylim=c(ylimitsmin,ylimitsmax))
             plotlines=quantile(combbgci$HR, c(0.05,0.25,0.5,0.75,0.95))
             qlevels=(c(0.05,0.25,0.5,0.75,0.95))
           ## Set up data frame
            resDatai=data.frame(yr=rep(uniqueyears[yeari],5), type=qlevels, value=round(plotlines,2))
            resDatai[nrow(resDatai)+1,]=c(uniqueyears[yeari], "mean", round(mean(combbgci$HR),2))
            resData=rbind(resData,resDatai)
           ## Plot percentiles 
             for(i in 1:5)  {abline(h=plotlines[i],col="grey90"); text(nrow(combbgci)*((i-1)/5)+5, plotlines[i]+1,paste(qlevels[i],round(plotlines[i],1),sep=": "))}
             abline(h=mean(combbgci$HR), lwd=3,col="red"); text(nrow(combbgci)/2, mean(combbgci$HR)+3,paste("Mean",round(mean(combbgci$HR),2),sep=": "),cex=1.4)
        }


      }


    return(resData)

    }





#------------------------------------------------------------
# OLD CODE:
#

## Get Zonal MCPes
#    Get_mcpe_data = function()
#      {
#                if(startyear<=2010) {if(endyear>=2010) enddate="2010-12-01" else enddate=paste(min(endyear,2010),"01-01",sep="-")}
#                if(startyear<=2010) {startdate=paste(startyear,"01-01",sep="-")}
#                if(startyear<=2010) {mcperaw=pull.mcpe(as.Date(startdate), as.Date(enddate));
#                                      colnames(mcperaw)[match("MCPe",colnames(mcperaw))]="value"; mcperaw$variable="RTSPP"
#                                      #mcpe=cast(mcperaw, OPR_DATE+HE+SP+year+month~variable, mean)
#                                      mcpe=aggregate( mcperaw[,c("value")], by=list(mcperaw$OPR_DATE,mcperaw$HE,mcperaw$SP,mcperaw$year,mcperaw$month), mean)
#                                      colnames(mcpe)=c("OPR_DATE","HE","SP","yr","month","RTSPP")
#                                      mcpe$OPR_DATE=as.POSIXct(mcpe$OPR_DATE)
#                                      #mcpe=mcperaw[,c("OPR_DATE","HE","SP","MCPe","INTERVAL","MCPe","int_end","minute","year","month")];
#                                      #colnames(mcpe)[c(4,6,8)]=c("DASPP","RTSPP","pk")
#                                      mcpeak=subset(mcpe, HE>=7 & HE<=22); mcpeak$pk="pk"; mcop=subset(mcpe, HE<7 | HE>22); mcop$pk="OP"
#                                      mcpe=rbind(mcpeak, mcop)
#                                      }
#           ## Combine with buckets
#                mcpe=arrResc
#                mcpe$DASPP=mcpe$RTSPP
#                colnames(mcpe)[match("year",colnames(mcpe))]="yr"
#                colnames(mcpe)[match("mon",colnames(mcpe))]="month"
#                mcpe$moyr=paste(mcpe$yr,sprintf("%02.0f", mcpe$month),sep="-")
#                mcpe=mcpe[,c("OPR_DATE","HE","SP","DASPP","RTSPP","dt_he","yr","moyr","month")]
#                bkt=bkts; bkt$OPR_DATE=as.POSIXct(bkt$OPR_DATE)
#                mcpeb=merge(mcpe, bkt[,c("OPR_DATE","HE","bucket","dayname")])
#
#           ## Save results
#              # load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/mcpe.RData")
#              # save(mcpe, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/mcpe.RData")
#              # save(mcpeotc, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/dlt.RData")
#
#          ## Add gas
#             start_date=as.Date(paste(2005,"01-01",sep="-"))
#             end_date=min(as.Date(paste(2010,"11-30",sep="-")), Sys.Date())
#             gas=pull.tsdb(start_date, end_date, c('gasdailymean_ship'), hourconvention=2)
#             colnames(gas)[3]="plattsgas"
#             mcpeg=merge(mcpeb, gas[,c("OPR_DATE","plattsgas")],all=TRUE)
#             str(mcpeg[is.na(mcpeg$plattsgas)==TRUE,])
#             str(mcpeg[is.na(mcpeg$RTSPP)==TRUE,])
#             str(mcpeg)
#
#          ## Add nymex
#             start_date=as.Date(paste(2005,"01-01",sep="-"))
#             end_date=min(as.Date(paste(2010,"11-30",sep="-")), Sys.Date())
#             gasn=pull.tsdb(start_date, end_date, c('gasdailymean_henry'), hourconvention=2)
#             colnames(gasn)[3]="plattshenry"
#             ## Missing nymex gas
#              mcpehenry=merge(mcpeg, gasn[,c("OPR_DATE","plattshenry")],all=TRUE)
#              misd=unique(mcpehenry[is.na(mcpehenry$plattshenry)==TRUE,]$OPR_DATE)
#              for(i in 1:length(misd)) {gasn=rbind(gasn, subset(gasn, OPR_DATE==(misd[i]+1*60*60*24))); gasn$OPR_DATE[nrow(gasn)]=misd[i]}
#             mcpehenry=merge(mcpeg, gasn[,c("OPR_DATE","plattshenry")],all=TRUE)
#             str(mcpehenry[is.na(mcpehenry$plattsgas)==TRUE,])
#             str(mcpehenry[is.na(mcpehenry$plattshenry)==TRUE,])
#             str(mcpehenry)
#
#
#          ## Add load
#             #loadraw=pull.tsdb(start_date, end_date, c('hourlyavg(ercrt_systemload)','ercrt_shorttermfcast'), hourconvention=0)
#             #loadrawhrly=pull.tsdb(start_date, end_date, c('hourlyavg(ercrt_systemload)'), hourconvention=0)
#             #write.csv(loadraw, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")
#             loadread=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/zonal_loads_ercotwide.csv", as.is=TRUE)
#             loadread$dt_he=as.POSIXct(loadread$dt_he)
#             loadread[which(is.na(loadread$dt_he)==TRUE),"dt_he"]=loadread[which(is.na(loadread$dt_he)==TRUE)+1,"dt_he"]-1*60*60
#             mcpeload=merge(mcpehenry, loadread, all=TRUE)
#             mcpeload$loadrt=as.numeric(mcpeload$loadrt)
#             str(mcpeload[is.na(mcpeload$plattsgas)==TRUE,])
#             str(mcpeload[is.na(mcpeload$loadrt)==TRUE,])
#             str(mcpeload)
#             # arrtest=mcpeload; arrtest$value=1; arrtest$variable="ct"
#             # arrtest=cast(arrtest, dt_he~variable, sum); tail(arrtest[order(arrtest$dt_he),])
#             # subset(mcpeload, OPR_DATE==as.POSIXct('2008-03-09') & HE==2)
#
#         ## ADD OTC prices
#             platts=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt1.csv", as.is=TRUE)
#             mcpeotc=merge(mcpeload, platts[,c(1:4)],all=TRUE) 
#             str(mcpeotc[is.na(mcpeotc$plattsgas)==TRUE,])
#             str(mcpeotc[is.na(mcpeotc$plattspwr)==TRUE,])
#             str(mcpeotc)
#
#
#          ## Add wind
#             wind=pull.tsdb(start_date, end_date, c('erct_wind_output'), hourconvention=0)
#             wind$dt_hea=wind$dt_he+60*60*0.25
#             wind$dt_hec=round(wind$dt_hea, "hour")
#             wind$dt_hec=as.POSIXct(wind$dt_hec)
#             windc=cast(wind, dt_hec~variable, mean)
#             colnames(windc)=c("dt_he","windrt")
#             # write.csv(windc, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt3.csv")
#             mcpewind=merge(mcpeotc, windc, all=TRUE)
#             str(mcpewind[is.na(mcpewind$plattsgas)==TRUE,])
#             str(mcpewind[is.na(mcpewind$windrt)==TRUE,])
#             str(mcpewind)
#
#
#
#      }
                              
