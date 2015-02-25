# Library of analyses
# 
#
#
# Analysis
#   1) DART SPREADS         
#   2) COMPARE DAM PRICES TO OTC TO AS MCPs         
#   3) WHAT IS THE LOAD AND HOW MANY SCED INTERVALS WHEN HITTING PBP
#   4) PLOT MULTIPLE DAY'S HOURLY RT PRICES ON THE SAME GRAPH
#   5) PLOT HOURLY DA PRICES ON THE SAME GRAPH
#   6) PLOT DAILY MAX TEMPS VS LOAD VS RT LMPS
#   7) PLOT TEMPERATURES
#   8) WEST TO NORTH ANALYSES
#   9) MAP ELECTRICAL BUSES AND CORRESPONDING INFORMATION 
#
#







rm(list=ls())
require(RODBC)
require("SecDb"); require(reshape)
require(CEGbase)
require(zoo)


setwd("S:/All/Risk/Software/R/RMG.Class/O_R to SecDB interface.2007-10-12/")
source("S:/All/Risk/Software/R/prod/Utilities/Packages/load.packages.R")
load.packages("SecDb")
source("S:/all/Risk/Software/R/Prod/Utilities/write.xls.R")
args(write.xls)
args(write.csv)
args(read.xls)
args(read.csv)
require(SecDb)
require(lattice)
source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
require(RODBC)
require(FTR)








##  GET HOURLY DATA
        load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData"); 
        file1=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_", as.POSIXlt(Sys.Date())$year+1900, ".RData", sep="")
        file2=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_", as.POSIXlt(Sys.Date())$year+1899, ".RData", sep="")
        load(file=file1); 
        rthourly1=subset(rthourly, SP=="HB_NORTH")
        load(file=file2); 
        rthourly2=subset(rthourly, SP=="HB_NORTH")
        rthourly=rbind(rthourly1, rthourly2)
        rthourly=merge(rthourly, bkts[,c("OPR_DATE","HE","bucket","dayname")])


##  GET INTERVAL DATA
        load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData"); 
        file1=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_", sprintf("%02.0f", as.POSIXlt(Sys.Date())$mon+1), ".RData", sep="")
        file2=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_", sprintf("%02.0f", as.POSIXlt(Sys.Date())$mon), ".RData", sep="")
        load(file=file1); 
        rt1=subset(dfRT, SP=="HB_NORTH")
        load(file=file2); 
        rt2=subset(dfRT, SP=="HB_NORTH")
        rt=rbind(rt1, rt2)
        rt=merge(rt, bkts[,c("OPR_DATE","HE","bucket","dayname")])









#################################################
#################################################
#################################################
## DA-RT LOAD BIDDING ANALYSIS   ############
##  Need to confirm that the daily data gets assigned to the correct date of data (e.g. hsc dly gas)
##      1a) How does 5 min changes in load wind lead to power balance penalty
##      1b) How does wind output lead to west congestion
##      1c) How do ICE DA heat rates lead to capacity sufficiency/insufficiency?  Is there a given heat rate dependent on temps
##      1d) How do differences in load amongst zones affect basis?
#################################################
#################################################








#################################################
## 1) DART SPREADS                   ############
##    A) hourly dart returns
##    B) bucket dart
##    C) Look at hourly returns for each day's peak load hour      
##    D) Hourly RT prices
#################################################

startdate=as.POSIXct('2011-10-01')


## A) Look at hourly DART returns
      rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02')  
                              & OPR_DATE!=as.POSIXct('2011-02-04') & OPR_DATE>=startdate
                               & (bucket=="5x16" | bucket=="7x8") )
      rtgood$dart=rtgood$RTSPP-rtgood$DASPP
      colnames(rtgood)[ncol(rtgood)]="value"
      par(mfrow=c(5,5), mar=c(2,2,3,1))
      for (i in 1:24)
        {
          rtgoodi=subset(rtgood, HE==i)
          rtgoodi=rtgoodi[order(rtgoodi$value, decreasing=TRUE),]
          if(round(mean(rtgoodi$value),1)>1) plotcolor="red" else plotcolor="black"
          plot(rtgoodi$value, type="l", bty="l", 
              main=paste("Hour",i,".   Ct RT>$150:",nrow(subset(rtgoodi, RTSPP>=150)),"\nAvg RT-DA:",round(mean(rtgoodi$value),1),"\nMdn RT-DA:",round(median(rtgoodi$value),1)),
              lwd=4, col=plotcolor)
          abline(h=0)
        }
        plot(1,1,col="white", axes=FALSE); text(1,1, paste(min(rtgood$OPR_DATE)," : ",max(rtgood$OPR_DATE),sep=""), cex=1.5)
      rtgood=rtgood[order(rtgood$value, decreasing=TRUE),]
      head(rtgood)
## B) Look at bucket DART
      rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02')  
                              & OPR_DATE!=as.POSIXct('2011-02-04') & OPR_DATE>=startdate
                               & (bucket=="5x16") )
      par(mfrow=c(2,1), mar=c(4,2,3,1))
      ## Plot DART
        rtgood$value=rtgood$RTSPP-rtgood$DASPP; rtgood$variable="dart"
        rtgoodi=cast(rtgood, OPR_DATE~variable, mean)
        rtgoodi=rtgoodi[order(rtgoodi[,2], decreasing=TRUE),]
        plot(rtgoodi[,2], pch=20, bty="l", main=paste("RT-DA  (5x16) ",min(rtgoodi[,1]),":",max(rtgoodi[,1])), lwd=4, xlab="")
        text(nrow(rtgoodi)/2, max(rtgoodi[,2]), paste("Ct DART>$150:",nrow(subset(rtgoodi, dart>=100)),".   Avg RT-DA:",round(mean(rtgoodi$dart),1),".   Mdn RT-DA:",round(median(rtgoodi$dart),1)))
            abline(h=0)
      ## Plot ICEDA
        rtgood$value=rtgood$DASPP-rtgood$plattspwr; rtgood$variable="iceda"
        rtgoodi=cast(rtgood, OPR_DATE~variable, mean)
        rtgoodi=rtgoodi[order(rtgoodi[,2], decreasing=TRUE),]
        plot(rtgoodi[,2], pch=20, bty="l", main=paste("DA-ICE  (5x16) ",min(rtgoodi[,1]),":",max(rtgoodi[,1])), lwd=4, xlab="")
        text(nrow(rtgoodi)/2, max(rtgoodi[,2])*0.9, paste("Ct ICEDA>$150:",nrow(subset(rtgoodi, iceda>=100)),".   Avg DA-ICE:",round(mean(rtgoodi[,2]),1),".   Mdn DA-ICE:",round(median(rtgoodi[,2]),1)))
            abline(h=0)
      ## Plot time series DART
      par(mfrow=c(1,1), mar=c(4,2,3,1))
        rtgood$value=rtgood$RTSPP-rtgood$DASPP; rtgood$variable="dart"
        rtgoodi=cast(rtgood, OPR_DATE~variable, mean)
        plot(rtgoodi$OPR_DATE, rtgoodi$dart, pch=20, bty="l", main=paste("RT-DA  (5x16) ",min(rtgoodi[,1]),":",max(rtgoodi[,1])), lwd=4, xlab="")
        abline(h=0)
        text(mean(rtgoodi$OPR_DATE), max(rtgoodi$dart)*0.9, paste("Ct DART>$150:",nrow(subset(rtgoodi, dart>=100)),".   Avg RT-DA:",round(mean(rtgoodi$dart),1),".   Mdn RT-DA:",round(median(rtgoodi$dart),1)))
            abline(h=0)
            
          
## C) Look at hourly returns for each day's peak load hour      
      ## 1)  What is the max hour using actual load
          rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02') 
                        & OPR_DATE>=startdate)
          rtgood$dart=rtgood$RTSPP-rtgood$DASPP
          rtld=rtgood
          colnames(rtld)[match("loadrt",colnames(rtld))]="value"; rtld$variable="ld"
          rtld=cast(rtld, OPR_DATE~variable, max)
          rtld$max_hr=0
          rtgood=merge(rtgood, rtld, all=TRUE)
          rtgood[is.na(rtgood$max_hr)==TRUE,"max_hr"]=1000      
          rtgood=rtgood[order(rtgood$HE),]; rtgood=rtgood[order(rtgood$OPR_DATE),]
          for (i in 2:(nrow(rtgood)-1))
            {
            if(rtgood$OPR_DATE[i-1]==rtgood$OPR_DATE[i]) {if(rtgood$max_hr[i]==0)  {rtgood$max_hr[i-1]=-1; rtgood$max_hr[i+1]=1}}
            }
          round(tapply(rtgood$dart, rtgood$max_hr, mean),0); tapply(rtgood$dart, rtgood$max_hr, length)
          ## Plot max hour
              par(mfrow=c(2,2), mar=c(2,2,3,1))
              for (i in c(-1,0,1,1000))
              {
              rtgoodi=subset(rtgood, max_hr==i)
              plot(rtgoodi$HE, rtgoodi$dart, pch=20, bty="l", main=paste("Hour",i,"\nAvg RT-DA:",round(mean(rtgoodi$dart),1),"\nMdn RT-DA:",round(median(rtgoodi$dart),1)))
              abline(h=0)
              }          
      ## 2)  What is the max hour using forecasted load
          rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02') & OPR_DATE>=as.POSIXct('2011-01-01'))
          rtgood$dart=rtgood$RTSPP-rtgood$DASPP
          rtld=rtgood
          colnames(rtld)[match("loadfc",colnames(rtld))]="value"; rtld$variable="loadfc"
          rtld=cast(rtld, OPR_DATE~variable, max)
          rtld$max_hr=0
          rtgood=merge(rtgood, rtld, all=TRUE)
          rtgood[is.na(rtgood$max_hr)==TRUE,"max_hr"]=1000      
          rtgood=rtgood[order(rtgood$HE),]; rtgood=rtgood[order(rtgood$OPR_DATE),]
          for (i in 2:(nrow(rtgood)-1))
            {
            if(rtgood$OPR_DATE[i-1]==rtgood$OPR_DATE[i]) {if(rtgood$max_hr[i]==0)  {rtgood$max_hr[i-1]=-1; rtgood$max_hr[i+1]=1}}
            }
          round(tapply(rtgood$dart, rtgood$max_hr, mean),0); tapply(rtgood$dart, rtgood$max_hr, length)
          ## Plot max hour
              par(mfrow=c(2,2), mar=c(2,2,3,1))
              for (i in c(-1,0,1,1000))
              {
              rtgoodi=subset(rtgood, max_hr==i)
              plot(rtgoodi$HE, rtgoodi$dart, pch=15, bty="l", main=paste("Hour",i,"\nAvg RT-DA:",round(mean(rtgoodi$dart),1),"\nMdn RT-DA:",round(median(rtgoodi$dart),1)))
              abline(h=0)
              }          
              par(mfrow=c(1,1), mar=c(3,3,3,3))    
      ## 3) Multiple peak load hours using actual load
              ## Get rid of missing data
                  rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02') & OPR_DATE>=as.POSIXct('2011-01-01'))
                  rtgood$dart=rtgood$RTSPP-rtgood$DASPP      
                  baddates=rtgood[is.na(rtgood$ld)==TRUE,c("OPR_DATE","SP")]; baddates$dategood=0
                  rtgood=merge(rtgood, baddates, all.x=TRUE)
                  rtgood=rtgood[is.na(rtgood$dategood)==TRUE,]      
                  rtgood=rtgood[order(rtgood$HE),]; rtgood=rtgood[order(rtgood$OPR_DATE),]
                  rtgood$rownum=1; for  (i in 2:nrow(rtgood)) rtgood$rownum[i]=1+rtgood$rownum[i-1]
                  if (min(tapply(rtgood$SP, rtgood$OPR_DATE, length))<24) print("ERROR--SOME DAYS HAVE LESS THAN 24 HOURS")
              ## Assign order of load hours by highest to lowest
                  rtgood$orderhr=1000
                  for (i in 2:(nrow(rtgood)-1))
                    {
                    if(rtgood$OPR_DATE[i-1]==rtgood$OPR_DATE[i] & rtgood$OPR_DATE[i+1]==rtgood$OPR_DATE[i]) 
                      {
                        if(rtgood$ld[i]>rtgood$ld[i-1] & rtgood$ld[i]>rtgood$ld[i+1])  rtgood$orderhr[i]=0
                      }
                    }
                  for (j in 1:24)
                    {
                      for (i in 1:nrow(rtgood))
                        {
                        ## Negative numbers
                        if((i+j)<=nrow(rtgood)) 
                            {if(rtgood$OPR_DATE[i+j]==rtgood$OPR_DATE[i] & rtgood$orderhr[i+j]==0 & rtgood$orderhr[i]==1000) rtgood$orderhr[i]=-j } 
                        ## Positive numbers
                        if((i-j)>0) 
                            {if(rtgood$OPR_DATE[i-j]==rtgood$OPR_DATE[i] & rtgood$orderhr[i-j]==0 & rtgood$orderhr[i]==1000) rtgood$orderhr[i]=j  }
                        }
                    }
                rtgoodi=subset(rtgood, dart>=100)
                round(tapply(rtgood$dart, rtgood$orderhr, mean),0)
                round(tapply(rtgood$dart, rtgood$orderhr, median),0)
                round(tapply(rtgoodi$dart, rtgoodi$orderhr, length),0)        
                plot(rtgood$orderhr, rtgood$dart, bty="l", pch=20, main="Peak hour (0) DART spreads")
              par(mfrow=c(1,1), mar=c(3,3,3,3))    
      ## 4) Multiple peak load hours using forecasted load
              ## Get rid of missing data
                  rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02') & OPR_DATE>=as.POSIXct('2011-01-01'))
                  rtgood$dart=rtgood$RTSPP-rtgood$DASPP      
                  baddates=rtgood[is.na(rtgood$loadfc)==TRUE,c("OPR_DATE","SP")]; baddates$dategood=0
                  rtgood=merge(rtgood, baddates, all.x=TRUE)
                  rtgood=rtgood[is.na(rtgood$dategood)==TRUE,]      
                  rtgood=rtgood[order(rtgood$HE),]; rtgood=rtgood[order(rtgood$OPR_DATE),]
                  rtgood$rownum=1; for  (i in 2:nrow(rtgood)) rtgood$rownum[i]=1+rtgood$rownum[i-1]
                  gooddates=as.data.frame(tapply(rtgood$SP, rtgood$OPR_DATE, length)); 
                  gooddates$OPR_DATE=rownames(gooddates)                  
                  colnames(gooddates)[1]="hrct"
                  gooddates=subset(gooddates, hrct==24)
                  rtgood=merge(rtgood, gooddates)                  
                  if (min(tapply(rtgood$SP, rtgood$OPR_DATE, length))<24) print("ERROR--SOME DAYS HAVE LESS THAN 24 HOURS")
                  rtgood=rtgood[order(rtgood$HE),]; rtgood=rtgood[order(rtgood$OPR_DATE),]
              ## Assign order of load hours by highest to lowest
                  rtgood$orderhr=1000
                  for (i in 2:(nrow(rtgood)-1))
                    {
                    if(rtgood$OPR_DATE[i-1]==rtgood$OPR_DATE[i] & rtgood$OPR_DATE[i+1]==rtgood$OPR_DATE[i]) 
                      {
                        if(rtgood$loadfc[i]>rtgood$loadfc[i-1] & rtgood$loadfc[i]>rtgood$loadfc[i+1])  rtgood$orderhr[i]=0
                      }
                    }
              rtgood[1:24,c("OPR_DATE","HE","loadfc","orderhr")]; 
                  for (j in 1:24)
                    {
                      for (i in 1:nrow(rtgood))
                        {
                        ## Negative numbers
                        if((i+j)<=nrow(rtgood)) 
                            {if(rtgood$OPR_DATE[i+j]==rtgood$OPR_DATE[i] & rtgood$orderhr[i+j]==0 & rtgood$orderhr[i]==1000) rtgood$orderhr[i]=-j } 
                        ## Positive numbers
                        if((i-j)>0) 
                            {if(rtgood$OPR_DATE[i-j]==rtgood$OPR_DATE[i] & rtgood$orderhr[i-j]==0 & rtgood$orderhr[i]==1000) rtgood$orderhr[i]=j  }
                        }
                    }
                rtgoodi=subset(rtgood, dart>=100)
                round(tapply(rtgood$dart, rtgood$orderhr, mean),0)
                round(tapply(rtgood$dart, rtgood$orderhr, median),0)
                round(tapply(rtgoodi$dart, rtgoodi$orderhr, length),0)        
                plot(rtgood$orderhr, rtgood$dart, bty="l", pch=20, main="Peak hour (0) DART spreads using load forecast")
        
                  
              rtgood[(nrow(rtgood)-24):(nrow(rtgood)),c("OPR_DATE","HE","ld","orderhr","rownum")]
              rtgood[1513,c("OPR_DATE","HE","ld","orderhr")]
## D) Look at hourly prices
      rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02')  
                              & OPR_DATE!=as.POSIXct('2011-02-04') & OPR_DATE>=as.POSIXct('2010-12-01'))
      str(rtgood)
      plot ()
                              
      par(mfrow=c(2,1), mar=c(4,2,3,1))
              








           





#################################################
## 2) COMPARE DAM PRICES TO OTC TO AS MCPs ######
#################################################

      # Get AS clearing prices
        arrAS=pull.tsdb(as.Date("2011-07-01"), as.Date("2011-08-11"), symbolst=c('zm_ercot_nodal_dam_clr_pr_cpt_mcpc_ancillary_type_regup', 'zm_ercot_nodal_dam_clr_pr_cpt_mcpc_ancillary_type_regdn','zm_ercot_nodal_dam_clr_pr_cpt_mcpc_ancillary_type_rrs','zm_ercot_nodal_dam_clr_pr_cpt_mcpc_ancillary_type_nspin'), 0)
        arrAS=cast(arrAS, dt_he~variable, mean)    
        colnames(arrAS)[2:5]=c("urs","drs","rrs","nsrs")
        arrasrt=merge(arrAS, rthourly)
        str(arrasrt)
       # PLot OTC vs DAM
        plot(arrasrt$)
       # Plot AS vs DAM LMP  14:19
              arrasrt=arrasrtfull
              arrasrt=arrasrtsummer
              par(mfrow=c(4,6))
              for (i in 1:24)
                {
                  arrasrti=subset(arrasrt, HE==i & dayname!="Saturday" & dayname!="Saturday" & OPR_DATE!=as.POSIXct('2011-02-02') 
                          & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-04')  & OPR_DATE!=as.POSIXct('2011-02-10'))
                  plot(arrasrti$DASPP, arrasrti$rrs, bty="l", pch=15, main=paste(i,sep=""),xlab="")
                  points(arrasrti$DASPP, arrasrti$rrs+50, pch=15, col="green")
                  abline(0,1,col="red")
                }  
              par(mfrow=c(1,1))
        #  Plot share of AS price contributed by hour
            moyrs=unique(arrasrt$moyr)
             par(mfrow=c(3,3))
              for (i in 1:9)
                {
                  arrasrti=subset(arrasrt, moyr==moyrs[i] & dayname!="Saturday" & dayname!="Saturday" & OPR_DATE!=as.POSIXct('2011-02-02') 
                          & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-04')  & OPR_DATE!=as.POSIXct('2011-02-10'))
                  barplot(tapply(arrasrti$rrs, arrasrti$HE, mean), main=paste("Month = ",moyrs[i],sep=""))
                  abline(h=0)
                }  
              par(mfrow=c(1,1))
            
          











      


###############################################
##  DART BASIS SPREADS

#      startdate=as.POSIXct('2011-07-01')
#      rtgood=subset(rthourly, SP=="HB_NORTH" & OPR_DATE!=as.POSIXct('2011-02-03') & OPR_DATE!=as.POSIXct('2011-02-02')  
#                              & OPR_DATE!=as.POSIXct('2011-02-04') & OPR_DATE>=startdate
#                               & (bucket=="5x16" | bucket=="7x8") )
#      rtgood$dart=rtgood$RTSPP-rtgood$DASPP
#      colnames(rtgood)[ncol(rtgood)]="value"
#      par(mfrow=c(5,5), mar=c(2,2,3,1))
#      for (i in 1:24)
#        {
#          rtgoodi=subset(rtgood, HE==i)
#          rtgoodi=rtgoodi[order(rtgoodi$value, decreasing=TRUE),]
#          if(round(mean(rtgoodi$value),1)>1) plotcolor="red" else plotcolor="black"
#          plot(rtgoodi$value, type="l", bty="l", 
#              main=paste("Hour",i,".   Ct RT>$150:",nrow(subset(rtgoodi, RTSPP>=150)),"\nAvg RT-DA:",round(mean(rtgoodi$value),1),"\nMdn RT-DA:",round(median(rtgoodi$value),1)),
#              lwd=4, col=plotcolor)
#          abline(h=0)
#        }
#        plot(1,1,col="white", axes=FALSE); text(1,1, paste(min(rtgood$OPR_DATE)," : ",max(rtgood$OPR_DATE),sep=""), cex=1.5)
#      rtgood=rtgood[order(rtgood$value, decreasing=TRUE),]
#      head(rtgood)

  
  
  
  
  
  
        
    
#################################################
## 3) WHAT IS THE LOAD AND HOW MANY SCED INTERVALS WHEN HITTING PBP ######
#################################################
    loadz=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/loadwind.csv", as.is=TRUE)
    loadz=as.data.frame(loadz)
    loadz[,3]=as.numeric(loadz[,3]);loadz[,4]=as.numeric(loadz[,4]);loadz[,5]=as.numeric(loadz[,5])
    str(loadz)
    days=unique(loadz$day); cols=c("grey80","grey65","grey50","grey30","black","red")
    for (i in 1:length(days))
      {
        daysi=subset(loadz, day==days[i])
        if(i==1) plot(daysi$ld_wind, daysi$lmp, pch=15, bty='l', ylab="5-minute SCED LMP HB_NORTH", xlab="5-minute Avg Load-Wind", main="LMPs vs Load-Wind", col=cols[i], ylim=c(0,3100),xlim=c(0,70000))
        if(i>1) points(daysi$ld_wind, daysi$lmp, pch=15, col=cols[i])
      }
    par(mfrow=c(6,1), mar=c(2,2,2,2))
    for (i in 1:length(days))
      {
        daysi=subset(loadz, day==days[i])
        plot(daysi$ld_wind, daysi$lmp, pch=15, bty='l', ylab="5-minute SCED LMP HB_NORTH", xlab="5-minute Avg Load-Wind", main=days[i], col=cols[i], ylim=c(0,3100),xlim=c(60000,68000))
        if(i==1)  text(67000,3000,"LMPs vs Load-Wind, 5-min interval data",cex=2)
      }
    par(mfrow=c(1,1))
    par(mfrow=c(6,1), mar=c(2,2,2,2))
    for (i in 1:length(days))
      {
        daysi=subset(loadz, day==days[i])
        plot(daysi$load, daysi$lmp, pch=15, bty='l', ylab="5-minute SCED LMP HB_NORTH", xlab="5-minute Avg Load-Wind", main=days[i], col=cols[i], ylim=c(0,3100),xlim=c(60000,69000))
        if(i==1)  text(65000,3000,"LMPs vs Load, 5-min interval data",cex=2)
      }
    par(mfrow=c(1,1))
    






















#################################################
## 4) PLOT MULTIPLE DAY'S HOURLY RT PRICES ON THE SAME GRAPH ######
#################################################

      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
    ## Inputs
        lmphigh=150
        STARTDT=as.Date("2011-10-01")
        ENDDATE=as.Date("2011-10-19")
        bucketid="5x16"
    ## Get data
      arrRes=pull.spps(startdate=STARTDT, lmtgdate=ENDDATE, arrSPs=c('HB_NORTH','HB_SOUTH'), conn=channel_zema_prod); str(arrRes)
        load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData"); str(bkts)
       arrRes=merge(arrRes, bkts[,c("OPR_DATE","HE","bucket")])   
      peak=subset(arrRes , bucket==bucketid & SP=='HB_NORTH')    
      colnames(peak)[match("RTSPP",colnames(peak))]="value"; 
      peak$variable="value"; peak=cast(peak, OPR_DATE+HE~variable, mean)
      peak$variable="max"; test=cast(peak, OPR_DATE~variable, max); 
      colnames(peak)[match("value",colnames(peak))]="rt"; 
      peakkeep=peak
    ## Plot high LMP days and today
        test1=subset(test, max>=lmphigh | OPR_DATE==max(test$OPR_DATE))
        peak=merge(test1,peakkeep)
        peak=peak[order(peak$HE),]
        dates=unique(peak$OPR_DATE)
        peaki=subset(peak, OPR_DATE==dates[length(dates)])
        plot(peaki$HE, peaki$rt, bty="l", type="l", ylim=c(0, min(max(peak$rt),200)), lwd=4, col="red", xlim=c(7,22))
        print(dates[1]) ; print(round(mean(peaki$rt),0))
        for (i in 1:(length(dates)-1))
        {
        peaki=subset(peak, OPR_DATE==dates[i]); print(mean(peaki$rt))
        print(dates[i]) ; print(round(mean(peaki$rt),0))
        lines(peaki$HE, peaki$rt, type="l", lwd=2)
        # readline(prompt = "Pause. Press <Enter> to continue...") 
        }
    ## Plot non-high LMP days in light greay
        test2=subset(test, max<lmphigh & OPR_DATE!=max(test$OPR_DATE))
        peak=merge(test2,peakkeep); peak=peak[order(peak$HE),]
        peak=peak[order(peak$HE),]
        dates=unique(peak$OPR_DATE)
        for (i in 1:length(dates))
        {
        peaki=subset(peak, OPR_DATE==dates[i])
        lines(peaki$HE, peaki$rt, type="l", lwd=1, col="grey")
        }
    ## Plot HOURLY SPPs by day  
            rt=subset(rthourly, moyr=="2011_08" & bucket=="5x16")
            rt=rt[order(rt$HE),]
            dates=unique(rt$OPR_DATE)
            rti=subset(rt, OPR_DATE==dates[1])
            plot(rti$HE, rti$DASPP, type="l", bty="l", ylim=c(0,max(rt$RTSPP)),lwd=3)
            for(i in 2:length(dates))
            {
              rti=subset(rt, OPR_DATE==dates[i])
              lines(rti$HE, rti$DASPP, ylim=c(0,max(rt$RTSPP)),lwd=3)
            }
        ## Get the shape increase for load
            rt=subset(rthourly, moyr=="2011_08" & bucket=="5x16")
            arrMin=as.data.frame(tapply(rt$DASPP, rt$OPR_DATE, mean)); arrMin=arrMin[order(arrMin[,1]),]; mindate=rownames(arrMin)[1]
            colnames(rt)[match("DASPP",colnames(rt))]="value"
            rt=cast(rt, HE~OPR_DATE, mean)
            rt$min=rt[,match(mindate,colnames(rt))]
            rtmin=rt
            for(j in 2:(ncol(rtmin)-1))
              {
                for(i in 1:nrow(rtmin))
                {
                  rtmin[i,j]=rtmin[i,j]/rtmin[i,ncol(rtmin)]
                }
              }
            rt$min=apply(rt, 1, function(x)  {min(x[2:(length(x)-1)])} )
            rtmin=rt
            rtmin[,2]=rtmin[,2]/rtmin[,ncol(rtmin)]
            
        
        rt=cast(rt, HE~OPR_DATE)
        plot()
        
        
        

        
        


#################################################
## 5) PLOT HOURLY DA PRICES ON THE SAME GRAPH ######
#################################################

      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
    ## Inputs
        lmphigh=150
        STARTDT=as.Date("2011-07-01")
        ENDDATE=Sys.Date()
        bucketid="5x16"
    ## Get data
      arrRes=pull.spps(startdate=STARTDT, lmtgdate=ENDDATE, arrSPs=c('HB_NORTH','HB_SOUTH'), conn=channel_zema_prod); str(arrRes)
        load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData"); str(bkts)
       arrRes=merge(arrRes, bkts[,c("OPR_DATE","HE","bucket")])   
      peak=subset(arrRes , bucket==bucketid & SP=='HB_NORTH')    
      peak=peak[order(peak$int_end),]
      peak$time=peak$HE+peak$INTERVAL/4
    ## Plot
      dates=unique(peak$OPR_DATE)
      peaki=subset(peak, OPR_DATE==dates[length(dates)])
      plot(peaki$time, peaki$DASPP, lwd=5, col="red", bty="l", xlim=c(7,22),type="l",ylim=c(0,100))
      for (i in (length(dates)-1):1)
        {
          readline(prompt = "Pause. Press <Enter> to continue...") 
          peaki=subset(peak, OPR_DATE==dates[i])
          lines(peaki$time, peaki$DASPP, lwd=5-(length(dates)-1-i)) 
          print(dates[i]); print(5-(length(dates)-1-i)) 
        }
      
      
      
 



###############################################
##  PLOT MONTHLY HEAT RATES
  str(rthourly)
  peak=subset(rthourly, bucket=="5x16")
  round(tapply(peak$plattshr, peak$moyr, mean),1)
  tapply(peak$plattshr, peak$moyr, length)










#################################################
## 6) PLOT DAILY MAX TEMPS VS LOAD VS RT LMPS ######
#################################################

  ## Variables
    datestart=as.Date("2011-07-01")
    datestart=as.Date("2010-12-01")
    dateend=as.Date("2011-08-18")
    ngprice=4
  ## GET rt load and lmps
        str(rthourly)
        if(datestart<)
  ## Get actual temps
        require(CEGbase)
        options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
        require(SecDb)
        require(RODBC)
        require(FTR)
        source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
        arrTemperatures=pull.tsdb(datestart, dateend, symbolst=c('Wethr_ENGYcst_HOUI_TX_Temp', 'Wethr_ENGYcst_DALF_TX_Temp'), 0); str(arrTemperatures)
        arrTemperatures=cast(arrTemperatures, dt_he~variable, mean)    ; colnames(arrTemperatures)[2:3]=c("hou","dfw")
  ## Merge
        rt=merge(rthourly, arrTemperatures)
        rt=subset(rt, bucket=="5x16")
        rt[is.na(rt$loadrt),]
        rt[is.na(rt$loadrt),"loadrt"]= rt[is.na(rt$loadrt),"load_sys_interval"]
        rtmax=rt[is.na(rt$loadrt)==FALSE,]
        rtmax=aggregate(rtmax[,c("loadrt","hou","dfw")], by=list(rtmax$OPR_DATE, rtmax$dayname), max); colnames(rtmax)[1:2]=c("OPR_DATE","dayname")
        rtavg=aggregate(rt[,c("RTSPP", "plattsgas")], by=list(rt$OPR_DATE, rt$dayname), length); colnames(rtavg)[1:2]=c("OPR_DATE","dayname")
        if(min(rtavg[,3:4])!=16 | max(rtavg[,3:4])!=16) {"WARNING: NOT FINDING 16 LMPS PER DAY"}
        rtavg=aggregate(rt[,c("RTSPP", "plattsgas")], by=list(rt$OPR_DATE, rt$dayname), mean); colnames(rtavg)[1:2]=c("OPR_DATE","dayname")
        rtavg=subset(rtavg, OPR_DATE<as.POSIXct(Sys.Date()))
        rtavg$htrt=rtavg$RTSPP/rtavg$plattsgas
        str(rtavg)
        rt=merge(rtavg, rtmax)
        rt$temp=rt$hou/3+rt$dfw*2/3
  ## PLot load vs temps
        plot(rt$temp, rt$loadrt, bty="l", pch=15, xlab="2/3 DFW + 1/3 Hou Daily Max Temperature", ylab="Daily Peak System Load", main=paste("Load vs Temperatures 5x16 for ",min(rt$OPR_DATE)," to ",max(rt$OPR_DATE),sep="")
          , xlim=c(70,110))
        ## First order equation
          fit=lm(loadrt~temp,data=rt)  
          lines(rt$temp,fitted(fit),col="red")
          text(max(rt$temp)*0.95,max(rt$loadrt)*0.8,paste("Equation: ",round(fit$coefficients[2],2),"x+",round(fit$coefficients[1],0),".  R2=",round(summary(fit)$r.squared,2),sep=""))
        ##  2nd order equation
          rtfit=subset(rt, temp>=90)
          rtfit$temp2=rtfit$temp^2
          fit2=lm(loadrt~temp+temp2,data=rtfit)
          fitres=data.frame(temp=rtfit$temp, fitvalues=fit2$fitted.values)
          fitres=fitres[order(fitres$temp), ]
          points(fitres$temp, fitres$fitvalues, type="l", col="red", lwd=4)
          text(max(rt$temp)*0.95,max(rt$loadrt)*0.8,paste("Equation: ",round(fit2$coefficients[2],2),"x+",round(fit2$coefficients[1],0),".  R2=",round(summary(fit2)$r.squared,2),sep=""))
        ##  3rd order equation
          rt$temp3=rt$temp^3
          fit=lm(loadrt~temp+temp2+temp3,data=rt)
          fitres=data.frame(temp=rt$temp, fitvalues=fit$fitted.values)
          fitres=fitres[order(fitres$temp), ]
          points(fitres$temp, fitres$fitvalues, type="l", col="green", lwd=4)
          text(max(rt$temp)*0.95,max(rt$loadrt)*0.8,paste("Equation: ",round(fit$coefficients[2],2),"x+",round(fit$coefficients[1],0),".  R2=",round(summary(fit)$r.squared,2),sep=""))
        ## Plot only Fridays
          rtfri=subset(rt, dayname=='Friday')
          points(rtfri$temp, rtfri$loadrt, pch="x", col="red",cex=2)
  ## Plot HR vs load
        plot(rt$loadrt, rt$htrt, bty="l", pch=15, xlab="Peak hourly Load", ylab="1x16 Heat Rate", main=paste("RT Heat Rate vs Load 5x16 for ",min(rt$OPR_DATE)," to ",max(rt$OPR_DATE),sep=""))
        abline(v=66000,col="red"); text(66000,75,"Exp Load=66GW",col="red")
        abline(h=21.5,col="blue"); text(50000,25,"OTC heat rate = 21.5",col="blue")
          
  ## Find expected HR at the forecasted loads
        ## Get forecasted load
            arrLdFcst=pull.tsdb(as.Date("2011-08-12"), as.Date("2011-08-31"), symbolst=c('ERCOT_LD_CCG_STfcst_WestLZ', 'ERCOT_LD_CCG_STfcst_SouthLZ','ERCOT_LD_CCG_STfcst_NorthLZ','ERCOT_LD_CCG_STfcst_HoustonLZ'), 0)
            arrLdFcst=as.data.frame(arrLdFcst)
            arrLdFcst=cast(arrLdFcst, dt_he~variable, mean)
            arrLdFcst$sys=apply() arrLdFcst[]
            str(arrLdFcst)


  ## PLot lmps vs temps
        rt$lmp=rt$htrt*ngprice
        plot(rt$loadrt, rt$lmp, bty="l", pch=15, xlab="Daily Peak System Load", ylab=paste("Average heat rate times $",ngprice," gas",sep=""), main=paste("Heat rates vs Load 5x16 for ",min(rt$OPR_DATE)," to ",max(rt$OPR_DATE),sep=""))
        plot(rt$loadrt, rt$RTSPP, bty="l", pch=15, xlab="Daily Peak System Load", ylab=paste("Average 7-22 price",sep=""), main=paste("Heat rates vs Load 5x16 for ",min(rt$OPR_DATE)," to ",max(rt$OPR_DATE),sep=""))
        #fit=lm(lmp~loadrt,data=rt)  
        #lines(rt$loadrt,fitted(fit),col="red")
        #text(max(rt$temp)*0.95,max(rt$loadrt)*0.8,paste("Equation: ",round(fit$coefficients[2],2),"x+",round(fit$coefficients[1],0),".  R2=",round(summary(fit)$r.squared,2),sep=""))
        ## Zoom in
          plot(rt$loadrt, rt$lmp, bty="l", pch=15, xlab="Daily Peak System Load", ylab="Average heat rate times 4.25 gas", main=paste("Zoomed in: Heat rates vs Load 5x16 for ",min(rt$OPR_DATE)," to ",max(rt$OPR_DATE),sep=""),ylim=c(0,100))
  
  ## How much wind blows in the peak load hour?
        str(rthourly)
        rt=subset(rthourly, OPR_DATE>=as.POSIXct('2011-07-01') & OPR_DATE<as.POSIXct('2011-08-18'))
        rt=rt[is.na(rt$loadrt)==FALSE,]
        rt1=aggregate( rt[,c("loadrt")], by=list(rt$OPR_DATE), max)      ; colnames(rt1)=c("OPR_DATE","loadrt")  
        rt1=merge(rt1, rt)
        round(quantile(rt1$windrt,c(0.05,0.25,0.5,0.75,0.95)),0); round(mean(rt1$windrt),0)
  







#################################################
## 7) PLOT TEMPERATURES ######
#################################################

  ## GET rt load and lmps
  month1=7
  month2=8
    str(rthourly)
  ## Get actual temps
    require(CEGbase)
    options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
    require(SecDb)
    require(RODBC)
    require(FTR)
    source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
    arrTemperatures=pull.tsdb(as.Date("2005-07-01"), as.Date("2011-08-03"), symbolst=c('Wethr_ENGYcst_HOUI_TX_Temp', 'Wethr_ENGYcst_DALF_TX_Temp'), 0); str(arrTemperatures)
    arrTemperatures=cast(arrTemperatures, dt_he~variable, mean)    ; colnames(arrTemperatures)[2:3]=c("hou","dfw")
    arrTemperatures$month=as.POSIXlt(arrTemperatures$dt_he)$mon+1
    arrTemperatures$year=as.POSIXlt(arrTemperatures$dt_he)$year+1900
    arrTemperatures$day=as.POSIXlt(arrTemperatures$dt_he)$mday
    arrTemperatures$date=paste(arrTemperatures$year, sprintf("%02.0f",arrTemperatures$month), sprintf("%02.0f",arrTemperatures$mday),sep="-")
    arrTemperatures$moyr=paste(arrTemperatures$year, sprintf("%02.0f",arrTemperatures$month),sep="-")
    temp=subset(arrTemperatures, month==month1|month==month2)
    tempmin=as.Date(min(temp$dt_he)); tempmax=as.Date(max(temp$dt_he))
  ## Plot combo of Dallas and Houston
    temp$value=temp$dfw*2/3 + temp$hou/3
    temp=temp[order(temp$dt_he),]
    for (i in 2:nrow(temp))  {  if(is.na(temp$value[i])) temp$value[i]=temp$value[i-1]}
    temp$variable="temp_avg"
    temp=cast(temp, date+moyr+day+month+year~variable, mean)
    temp=temp[order(temp$temp_avg,decreasing=TRUE),]
    temp$order=seq(1, nrow(temp))
    plot(temp$order, temp$temp_avg, pch=20, bty="l", xlab="Rank-ordered daily max", ylab="2/3 DFW + 1/3 Hou Temperature", main=paste("Summer temps from ",tempmin," to ",tempmax,sep=""))
    tempi=subset(temp, year==2011)
    points(tempi$order, tempi$temp_avg, pch="X", col="red",cex=2)
    tempi=subset(temp, year==2010)
    points(tempi$order, tempi$temp_avg, pch=20, col="grey",cex=2)
    round(tapply(temp$temp_avg, temp$moyr, mean),0)
    round(tapply(temp$temp_avg, temp$moyr, median),0)


















































#################################################
## 8)   WEST TO NORTH ANALYSES ######
#################################################

## CHOOSE VARIABLES
	lmtgdate=Sys.Date() ## Date with all data, >6/1/10 have lmp data, >7/8/10 have AS data
	startdate=as.Date('2010-12-01')
	rescrape=TRUE
	writecsv=FALSE
#################################################
########### Connect to Nodal N-Market
DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
#################################################
  ##  VALIMP
    ########### Connect to Nodal N-Market
    DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
    channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
    ## GET SOUTHERN WEATHER ZONE WIND
     if (rescrape==TRUE)
      	{
       		strQry=paste(
                  "select opr_date od,opr_hour HB,OPr_minute MB,value ld from ze_data.ercot_nodal_FC_load_weather ld
                    where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') AND opr_date<='07-mar-11' and zone='Southern';",sep="")
         		ldraw1 <- sqlQuery(channel_zema_prod, strQry);
      	} else print("NO")  



########### WEST-NORTH CONGESTION
    ########### Connect to Nodal N-Market
    DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
    channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
    ## GET WIND AND LOAD DATA
      	lmtgdate=Sys.Date() ## Date with all data, >6/1/10 have lmp data, >7/8/10 have AS data
       	startdate=as.Date('2010-12-25')    ## Don't have complete data before then
     if (rescrape==TRUE)
      	{
      	 ## LOAD FORECAST
               		strQry=paste(
                          "select opr_date od,opr_hour HB, zone, value, publish_date pd, to_date(publish_time,'HH24:MI')  pt
                            from ze_data.ercot_nodal_FC_load_weather ld
                            where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and zone in ('FarWest','West');",sep="")
                 		fcstdfraw <- sqlQuery(channel_zema_prod, strQry);
                	## Get the latest forecast
         	          fcstdfraw$value=fcstdfraw$PD+as.POSIXlt(fcstdfraw$PT)$hour*60*60            
                  	fcstdf=fcstdfraw; 
                    fcstdf$variable="value"; 
                  	fcstdf=cast(fcstdf, OD+HB+ZONE~variable, max); 
                  	fcstdfraw$value=as.numeric(fcstdfraw$value)
                  	fcstdf=merge(fcstdf, fcstdfraw); 
                    fcstdf$dt=fcstdf$OD+(fcstdf$HB+1)*60*60; 
                    fcstdf=fcstdf[c("OD","HB","dt","ZONE","VALUE")]       	
                  	colnames(fcstdf)[match("VALUE",colnames(fcstdf))]="value"
                  	ctzones=length(unique(fcstdf$ZONE))
                  ## Customize  
                    varname="ld"            
                    fcstdf=cast(fcstdf, dt+OD+HB~ZONE, mean)
                    colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))]=paste(varname,colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))],sep="_")
                    ld=fcstdf       
                  ## PLOT LD FORECAST:   plot(ld$dt, ld$ld_FarWest, xlim=c(as.POSIXct('2011-03-01'), as.POSIXct('2011-03-08')),lwd=2, type="l",ylim=c (920,1240)); abline(v=as.POSIXct('2011-03-13'))       
      	 ## WIND ACTUALS
               		strQry=paste(
                      "select opr_date od,opr_hour HB, value, region zone, publish_date pd from ze_data.ercot_nodal_reg_wind_prod ld
                      where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and region='ACTUAL_WEST';",sep="")
                 		fcstdfraw <- sqlQuery(channel_zema_prod, strQry);
                	## Get the latest forecast
         	          fcstdfraw$value=fcstdfraw$PD
                  	fcstdf=fcstdfraw; 
                    fcstdf$variable="value"; 
                  	fcstdf=cast(fcstdf, OD+HB+ZONE~variable, max); 
                  	fcstdfraw$value=as.numeric(fcstdfraw$value)
                  	fcstdf=merge(fcstdf, fcstdfraw); 
                    fcstdf$dt=fcstdf$OD+(fcstdf$HB+1)*60*60; 
                    fcstdf=fcstdf[c("OD","HB","dt","ZONE","VALUE")]       	
                  	colnames(fcstdf)[match("VALUE",colnames(fcstdf))]="value"
                  	ctzones=length(unique(fcstdf$ZONE))
                  ## Customize  
                    varname="wind"            
                    fcstdf=cast(fcstdf, dt+OD+HB~ZONE, mean)
                    colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))]=paste(varname,colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))],sep="_")
                    wind=fcstdf            
       	 ## COMBINE
            west=merge(ld, wind); str(west) 	
         ## W-N TRANSMISSION LIMITS
            w2=FTR:::FTR.load.tsdb.symbols(c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'), as.POSIXct(startdate), as.POSIXct(lmtgdate), aggHourly=FALSE)
            w2=as.data.frame(w2); colnames(w2)[1]="wn_limit"
            w2$dt=as.POSIXct(rownames(w2))
            head(w2)            
       	 ## COMBINE
            west=merge(west, w2); str(west) 	            
       	}
       	
      # W-N BASIS      	
        rtwest=subset(rthourly, (SP=="HB_NORTH" | SP=='HB_WEST' ) & OPR_DATE>=as.POSIXct(startdate))
        colnames(rtwest)[match("RTSPP", colnames(rtwest))]="value"; rtwest$dt=as.POSIXct(rtwest$OPR_DATE)+rtwest$HE*60*60
        rtwest=cast(rtwest, dt~SP, mean); str(rtwest)
     ## Combine
        west=merge(west, rtwest)
        west$basis=west$HB_NORTH-west$HB_WEST
                 ## Trial 0:  Plot basis vs west wind  less farwest and west load
                    windows(10,12) ; 
                    par(mfrow=c(2,1))
                    west$adjwind=west$wind_ACTUAL_WEST - west$ld_FarWest - west$ld_West
                    west1=subset(west, adjwind>=0 & adjwind<6000 & basis<100 & basis>0)
                    plot(west1$adjwind, west1$basis,pch=15, main="Wind vs Basis"); 
                    fit=lm(basis~adjwind, data=west1)
                    abline(fit$coefficients[1], fit$coefficients[2], col="red",lwd=2); summary(fit)
                    west1$windsqrd=west1$adjwind*west1$adjwind
                    fit01=lm(basis~adjwind+windsqrd, data=west1); summary(fit01); 
                    west1$basisfit01=fit01$coefficients[1]+fit01$coefficients[2]*west1$adjwind+fit01$coefficients[3]*west1$windsqrd
                    west1=west1[order(west1$basisfit01),]
                    lines(west1$adjwind, west1$basisfit01, col="blue", lwd=3)
     # Trial 1:  Plot basis vs west wind less far west load
        west$adjwind=west$wind_ACTUAL_WEST - west$ld_FarWest
        west1=subset(west, adjwind>=0 & adjwind<4000 & basis<100 & basis>=0)
        plot(west1$adjwind, west1$basis,pch=15, main="Wind less farWest load vs Basis"); 
        fit1=lm(basis~adjwind, data=west1)
        abline(fit1$coefficients[1], fit1$coefficients[2], col="red",lwd=2); summary(fit1); text(500, 55, paste("Basis=",round(fit1$coefficients[2],4),"*AdjWind",round(fit1$coefficients[1],1),sep=""),cex=2)
        windows(12,10); resid=as.data.frame(fit1$residuals) ; plot(resid[order(resid[,1], decreasing=TRUE),1],pch=20); abline(h=0)
        west1$windsqrd=west1$adjwind*west1$adjwind
        abline(fit1$coefficients[1], fit1$coefficients[2], col="red",lwd=2); summary(fit1); text(500, 55, paste("Basis=",round(fit1$coefficients[2],4),"*AdjWind",round(fit1$coefficients[1],1),sep=""),cex=2)
        windows(12,10); resid=as.data.frame(fit1$residuals) ; plot(resid[order(resid[,1], decreasing=TRUE),1],pch=20); abline(h=0); sum(fit1$residuals)
      ## Fit a second degree equation to trail 1:  
        fit12=lm(basis~adjwind+windsqrd, data=west1); summary(fit12); 
        west1$basisfit12=fit12$coefficients[1]+fit12$coefficients[2]*west1$adjwind+fit12$coefficients[3]*west1$windsqrd
        plot(west1$adjwind, west1$basis,pch=15, main="Wind less farWest load vs Basis");         
        west1=west1[order(west1$basisfit12),]
        lines(west1$adjwind, west1$basisfit12, col="red", lwd=3)        ; sum(fit12$residuals)     
      ## Fit a third degree equation to trial 1:  
        west1$windcubed=west1$adjwind*west1$windsqrd
        fit13=lm(basis~adjwind+windsqrd+windcubed, data=west1); summary(fit13); 
        west1$basisfit13=fit13$coefficients[1]+fit13$coefficients[2]*west1$adjwind+fit13$coefficients[3]*west1$windsqrd+fit13$coefficients[4]*west1$windcubed
        plot(west1$adjwind, west1$basis,pch=15, main="Wind less farWest load vs Basis");         
        west2=subset(west1, dt>=as.POSIXct('2011-02-15')); points(west2$adjwind, west2$basis,pch=15, col="green")
        west1=west1[order(west1$basisfit13),]
        lines(west1$adjwind, west1$basisfit13, col="red", lwd=3)        ; sum(fit12$residuals)        
        plot(west1$basis, west1$basisfit13,pch=15); abline(0,1,col="red",lwd=2)
   ## Trial 5: Fit a second degree equation west limit - west wind vs far west load:  
        west$adjwind2=(west$wind_ACTUAL_WEST - west$ld_FarWest) - west$wn_limit
        west$adjwind22= west$adjwind2*west$adjwind2
        west5=subset(west, adjwind>=0 & adjwind<4000 & basis<100 & basis>=0)
         fit5=lm(basis~adjwind2, data=west5); summary(fit5); 
         fit52=lm(basis~adjwind2+adjwind22, data=west5); summary(fit52); 
        west5$basisfit=fit5$coefficients[1]+fit5$coefficients[2]*west5$adjwind2
        west5$basisfit2=fit52$coefficients[1]+fit52$coefficients[2]*west5$adjwind2 +fit52$coefficients[3]*west5$adjwind22
        windows(12,10)
        plot(west5$adjwind2, west5$basis,pch=15, main="Wind less farWest load less limit vs Basis");         
        west5=west5[order(west5$basisfit),]
        lines(west5$adjwind2, west5$basisfit, col="red", lwd=3)        ; sum(fit5$residuals)
        west5=west5[order(west5$basisfit2),]
        lines(west5$adjwind2, west5$basisfit2, col="pink", lwd=3)        ; sum(fit5$residuals)
   ## Trial 6: Fit a second degree equation west limit - west wind vs far west load and west load:  
        west$adjwind6=(west$wind_ACTUAL_WEST - west$ld_FarWest - west$ld_West) - west$wn_limit
        west$adjwind62= west$adjwind6*west$adjwind6
        west6=subset(west, adjwind>=0 & adjwind<4000 & basis<100 & basis>=0)
         fit6=lm(basis~adjwind6, data=west6); summary(fit6); 
         fit62=lm(basis~adjwind6+adjwind62, data=west6); summary(fit62); 
        west6$basisfit =fit6$coefficients[1] +fit6$coefficients[2] *west6$adjwind6
        west6$basisfit2=fit62$coefficients[1]+fit62$coefficients[2]*west6$adjwind6 +fit62$coefficients[3]*west6$adjwind62
        windows(12,10)
        plot(west6$adjwind6, west6$basis,pch=15, main="Wind less farWest load less West load less limit vs Basis");         
        west6=west6[order(west6$basisfit),]
        lines(west6$adjwind6, west6$basisfit, col="red", lwd=3)        ; sum(fit6$residuals)
        west6=west6[order(west6$basisfit2),]
        lines(west6$adjwind6, west6$basisfit2, col="pink", lwd=3)        ; sum(fit6$residuals)
          #     ## Trial 2:  Plot west lmp vs west wind less west load
          #        west$adjwind=west$wind_ACTUAL_WEST - west$ld_FarWest - west$ld_West
          #        west1=subset(west, adjwind>=1500 & adjwind<4000 & basis<100 & basis>=0)
          #        plot(west1$adjwind, west1$HB_WEST,pch=15, main="Wind less farWest load vs West Hub LMP", ylim=c(-50,50)); 
          #        fit2=lm(HB_WEST~adjwind, data=west1); abline(h=0,col="grey")
          #        abline(fit2$coefficients[1], fit2$coefficients[2], col="red",lwd=2); summary(fit2)
          #
     ## Trial 4:  Get basis when wind is low
        west$adjwind=west$wind_ACTUAL_WEST - west$ld_FarWest
        west1=subset(west, adjwind<4000 & basis<0 & basis>=-100)
        par(mfrow=c(2,2))
        plot(west1$adjwind, west1$basis,pch=15, main="Wind less farWest load vs Basis", ylim=c(-40,0)); 
        plot(west1$wind_ACTUAL_WEST, west1$basis,pch=15, main="Wind load vs Basis", ylim=c(-40,0)); 
        plot(west1$ld_FarWest, west1$basis,pch=15, main="farWest load vs Basis", ylim=c(-40,0)); 
        par(mfrow=c(1,1))
        fit1=lm(basis~adjwind, data=west1)
        abline(fit1$coefficients[1], fit1$coefficients[2], col="red",lwd=2); summary(fit1); text(500, 55, paste("Basis=",round(fit1$coefficients[2],4),"*AdjWind",round(fit1$coefficients[1],1),sep=""),cex=2)
        windows(12,10); resid=as.data.frame(fit1$residuals) ; plot(resid[order(resid[,1], decreasing=TRUE),1],pch=20); abline(h=0)
        west1$windsqrd=west1$adjwind*west1$adjwind
        abline(fit1$coefficients[1], fit1$coefficients[2], col="red",lwd=2); summary(fit1); text(500, 55, paste("Basis=",round(fit1$coefficients[2],4),"*AdjWind",round(fit1$coefficients[1],1),sep=""),cex=2)
        windows(12,10); resid=as.data.frame(fit1$residuals) ; plot(resid[order(resid[,1], decreasing=TRUE),1],pch=20); abline(h=0); sum(fit1$residuals)
    ## GET WIND FORECAST DATA
       	 ## WIND FORECAST
               		strQry=paste("select opr_date od,opr_hour HB, value, region zone, publish_date pd from ze_data.ercot_nodal_reg_wind_prod ld
                      where opr_date=to_date('",dt,"', 'YYYY-MM-DD') and region in ('STWPF_WEST','WGRPP_WEST');",sep="")
                 		fcstdfraw <- sqlQuery(channel_zema_prod, strQry);
                	## Get the latest forecast
         	          fcstdfraw$value=fcstdfraw$PD
                  	fcstdf=fcstdfraw; 
                    fcstdf$variable="value"; 
                  	fcstdf=cast(fcstdf, OD+HB+ZONE~variable, max); 
                  	fcstdfraw$value=as.numeric(fcstdfraw$value)
                  	fcstdf=merge(fcstdf, fcstdfraw); 
                    fcstdf$dt=fcstdf$OD+(fcstdf$HB+1)*60*60; 
                    fcstdf=fcstdf[c("OD","HB","dt","ZONE","VALUE")]       	
                  	colnames(fcstdf)[match("VALUE",colnames(fcstdf))]="value"
                  	ctzones=length(unique(fcstdf$ZONE))
                  ## Customize  
                    varname="wind"            
                    fcstdf=cast(fcstdf, dt+OD+HB~ZONE, mean)
                    colnamesdf=paste(varname,colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))],sep="_")
                    colnames(fcstdf)[c((ncol(fcstdf)-ctzones+1):ncol(fcstdf))]=colnamesdf
                    wind=fcstdf   
     ## Use coefficients from trial 1 to estimate basis
        todaydt=dt
        wind$windfcst=(wind[,c(colnamesdf[1])] + wind[,c(colnamesdf[1])])/2        
        wind$basis=fit1$coefficients[1]+fit1$coefficients[2]*wind$windfcst


        







#################################################
## 9)   MAP ELECTRICAL BUSES AND CORRESPONDING INFORMATION          ######
#################################################

rm(list=ls())
setwd("S:/All/Risk/Software/R/RMG.Class/O_R to SecDB interface.2007-10-12/")
require("SecDb"); require(reshape)
source("S:/All/Risk/Software/R/prod/Utilities/Packages/load.packages.R")
load.packages("SecDb")
source("S:/all/Risk/Software/R/Prod/Utilities/write.xls.R")
args(write.xls)
args(write.csv)
args(read.xls)
args(read.csv)
require(RODBC)
require(SecDb)
require(zoo)
require(CEGbase)
require(lattice)



##  GET LOAD & LMPS ON ELECTRICAL BUS
## CHOOSE VARIABLES
	lmtgdate=Sys.Date() ## Date with all data, >6/1/10 have lmp data, >7/8/10 have AS data
	startdate=as.Date('2011-09-21')
	rescrape=TRUE
	writecsv=FALSE
#################################################
########### Connect to Nodal N-Market
DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
#################################################
## GET LOAD, PRICES, AND EB MAP

	ebmap=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/uploadEbCoordiantes.csv",as.is=TRUE)
	ebmap=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/uploadLat.csv",as.is=TRUE)
	ebmap2=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/uploadlatlz.csv",as.is=TRUE)	
  ## GET LMPs  hour is HE, Hours 1:24 all belong to same opr_date
 	if (rescrape==TRUE)
    		{
    		strQry=paste("select opr_date, OPr_hour HB, opr_minute MB, electrical_bus LmpEB, lmp 
                      from ze_data.ercot_nodal_rt_lmp_eb 
                      where opr_date='21-sep-11' and opr_hour>=13 and opr_hour<=18 ",sep="")  ##and opr_minute>=15 and opr_minute<=35
    			eblmpsraw <- sqlQuery(channel_zema_prod, strQry);
    		} else print("NO")
          max(eblmpsraw$HB)
          max(eblmpsraw$MB)
          unique(eblmpsraw$MB)
        spps=subset(eblmpsraw,MB==0); colnames(spps)[match("LMPEB", colnames(spps))]='LmpEB'
      ## MERGE RAW LMPS AND MAP
        sppsorig=merge(spps,ebmap2,all.x=TRUE); spps=sppsorig

  ## VIEW LMPS FOR A GIVEN POINT
        spps=spps[order(spps$OPR_MINUTE),]; spps=spps[order(spps$HB),]; spps=spps[order(spps$MB),]
        subset(spps, LmpEB=="CBEC_AG1");  

  ## PLOT EB LMPS
      ## GET DESIRED DATE
        spps=subset(sppsorig, OPR_DATE==as.POSIXct("2011-09-07") & HB==17 & MB==30)
        spps=spps[is.na(spps$long)==FALSE,]; spps=spps[is.na(spps$lat)==FALSE,]
      ## PLOT BY PRICE
        #windows(12,10); 
        plot(spps$long, spps$lat, pch=20, bty="l", col="grey")
        maxlmp=max(spps$LMP);minlmp=min(spps$LMP); lmprange=maxlmp-minlmp;
      ## CHOOSE OWN RANGE        maxlmp=100; minlmp=-10; lmprange=maxlmp-minlmp;
        round(minlmp,0); round(maxlmp,0)
        i=1
            sppsi=subset(spps, LMP<=(minlmp+lmprange*i/5)); points(sppsi$long, sppsi$lat, pch=20, col="blue"); 
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB)
        i=2
            sppsi=subset(spps, LMP>(minlmp+lmprange*(i-1)/5) & LMP<=(minlmp+lmprange*i/5)); 
            points(sppsi$long, sppsi$lat, pch=20, col="green")
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB)
        i=3
            sppsi=subset(spps, LMP>(minlmp+lmprange*(i-1)/5) & LMP<=(minlmp+lmprange*i/5)); points(sppsi$long, sppsi$lat, pch=20, col="yellow")
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB)
        i=4
            sppsi=subset(spps, LMP>(minlmp+lmprange*(i-1)/5) & LMP<=(minlmp+lmprange*i/5)); points(sppsi$long, sppsi$lat, pch=20, col="orange")
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB)
        i=5
            sppsi=subset(spps, LMP>(minlmp+lmprange*(i-1)/5) & LMP<=(minlmp+lmprange*i/5)); points(sppsi$long, sppsi$lat, pch=20, col="red")
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB); unique(sppsi$LMP)
       sppsi=subset(spps, LmpEB=="CBEC_AG1"); points(sppsi$long, sppsi$lat, pch=15, col="black"); sppsi
            print(round(minlmp+lmprange*(i-1)/5,0)); print(round(minlmp+lmprange*i/5,0)); #unique(sppsi$LmpEB); unique(sppsi$LMP)
        abline(v=-95.5)
      ## PLOT LOAD ZONES
        plot(spps$long, spps$lat, pch=20, bty="l", col="grey")
        spps1=subset(spps, lz=='LZ_SOUTH'); points(spps1$long, spps1$lat, pch=20, col="blue")
        spps1=subset(spps, lz=='LZ_NORTH'); points(spps1$long, spps1$lat, pch=20, col="red")
        spps1=subset(spps, lz=='LZ_HOUSTON'); points(spps1$long, spps1$lat, pch=20, col="orange")
        spps1=subset(spps, lz=='LZ_WEST'); points(spps1$long, spps1$lat, pch=20, col="green")
        spps1=subset(spps, load=='LZ_RAYBN'); points(spps1$long, spps1$lat, pch=20, col="black")                  
        spps1=subset(spps, load=='LZ_AEN'); points(spps1$long, spps1$lat, pch=20, col="black")                  
        spps1=subset(spps, load=='LZ_LCRA'); points(spps1$long, spps1$lat, pch=20, col="grey")                  
        spps1=subset(spps, load=='LZ_CPS'); points(spps1$long, spps1$lat, pch=20, col="yellow")                          
      # PLOT SOUTH NOIES
        windows(12,10); 
        plot(spps$long, spps$lat, pch=20, bty="l", col="grey")
        spps1=subset(spps, lz=='LZ_SOUTH'); points(spps1$long, spps1$lat, pch=20, col="blue")
        spps1=subset(spps, load=='LZ_AEN'); points(spps1$long, spps1$lat, pch=20, col="purple")
        spps1=subset(spps, load=='LZ_CPS'); points(spps1$long, spps1$lat, pch=20, col="red")
        spps1=subset(spps, load=='LZ_LCRA'); points(spps1$long, spps1$lat, pch=20, col="orange")        
        maxlmp=max(spps$LMP);minlmp=min(spps$LMP); lmprange=maxlmp-minlmp;
        i=5
        sppsi=subset(spps, LMP>1000); points(sppsi$long, sppsi$lat, pch=20, col="black")
        abline(v=-95.5)        
      ## Plot
        plot(spps$long, spps$lat, pch=20, bty="l")
  ## GET LOAD ON EB: 
 	if (rescrape==TRUE)
    		{
    		strQry=paste("select ELEC_BUS_name, se_mw from ze_data.ercot_nodal_se_tsl_eb eb 
                      where opr_date='10-feb-11' and opr_Hour=6 and opr_minute=3;",sep="")
    			ebldraw <- sqlQuery(channel_zema_prod, strQry);
    		} else print("NO")
        ebld=ebldraw; colnames(ebld)[1]='LoadEB'
        ebld=merge(ebld, ebmap2, all.x=TRUE)
      ## PLOT 
        plot(ebld$long, ebld$lat, pch=20, bty="l", col="grey")
        ebld1=subset(ebld, lz=='LZ_SOUTH'); points(ebld1$long, ebld1$lat, pch=20, col="blue")
        ebld1=subset(ebld, load=='LZ_AEN'); points(ebld1$long, ebld1$lat, pch=20, col="purple")
        ebld1=subset(ebld, load=='LZ_CPS'); points(ebld1$long, ebld1$lat, pch=20, col="red")
        ebld1=subset(ebld, load=='LZ_LCRA'); points(ebld1$long, ebld1$lat, pch=20, col="orange")
        ebld1=subset(ebld, load=='LZ_SOUTH'); points(ebld1$long, ebld1$lat, pch=20, col="orange")        
      ## COMBINE WITH PRICES
        ebld=ebldraw; colnames(ebld)[1]='LoadEB'
        ebld=merge(ebld, ebmap2, all=TRUE)
        spps=lmpsraw; colnames(spps)[1]='LmpEB'
        eb=merge(spps,ebld,all=TRUE)        
        eb[is.na(eb)==TRUE]=0
        round(tapply(eb$SE_MW, eb$load, sum),0)
        ebi=subset(eb, LMP>1000)
        round(tapply(ebi$SE_MW, ebi$load, sum),0)
      ## CALCULATE SOUTH_LZ PRICE
        ebi=subset(eb, load=='LZ_SOUTH')
        ebi$ldlmp=ebi$LMP * ebi$SE_MW
        sum(ebi$ldlmp)/sum(ebi$SE_MW)
   
## CLOSE DATABASE CONNECTIONS
	odbcCloseAll()




















#################################################
## ?)   ?                                  ######
#################################################

## SCED-LEVEL ANALYSES
  rtdata=subset(priceswll, SP=="HB_NORTH")
  rtdata$hourending=paste(sprintf("%02.0f", rtdata$HE),":00",sep="")  
  rtdata$adjld=rtdata$se_mw - rtdata$windgs
  rtdata=rtdata[order(rtdata$dt),]
  str(rtdata)
  


## CHECK LOAD AND WIND
  ## CHECK SE LOAD VS SYS LOAD
    plot(rtdata$dt, rtdata$ldiccp+1700, type="p", pch=20, xlim=c(as.POSIXct('2011-01-10 06:30'), as.POSIXct('2011-01-10 08:00')),ylim=c(40000,45000))
    lines(as.POSIXct(rtdata$dt), rtdata$se_mw, col="red")
  ## CHCK WINDACT VS WINDGS
    plot(as.POSIXct(rtdata$dt), rtdata$WINDACT, type="p", pch=20, xlim=c(as.POSIXct('2011-01-10 02:00'), as.POSIXct('2011-01-10 22:00')))
    lines(as.POSIXct(rtdata$dt), rtdata$windgs, col="red")
    



## PLOT RT LMPS VS LOAD BY HOUR
      windows(12,10)
      str(rthourly)
          xyplot( RTSPP ~ ld | hourending, data=rtdata, as.table=TRUE, pch=20, ylim=c(0,1000))
            xyplot( st ~ ordering | strdn, data=winddlt, as.table=TRUE,main="Actual wind minus STWPF forecast",
              panel = function(x, y) 
                {
                  panel.abline(h=0, col="grey", lty=2)
                  panel.xyplot(x,y,type="l",col="black")
                  panel.abline(h=mean(),type="l",col="black")
                }           
            )
  
  str(rtdata)
  

## PLOT LMP PRICE SPIKES 
    str(rtdata)
    rtdata=subset(rthourly, SP=="HB_NORTH" & RTSPP>100 & OPR_DATE!=as.POSIXct('2011-02-02') & OPR_DATE!=as.POSIXct('2011-02-03'))
    plot(rtdata$HE, rtdata$RTSPP, pch=20, bty="l")
    plot(rtdata$DASPP, rtdata$RTSPP, pch=20, bty="l")
    round(tapply(rtdata$RTSPP, rtdata$HE, length),0);round(tapply(rtdata$RTSPP, rtdata$HE, mean),0)
## WHAT IS THE BREAK-EVEN PRICE FOR HE7
    rtdata=subset(rthourly, SP=="HB_NORTH" & RTSPP>100 & OPR_DATE!=as.POSIXct('2011-02-02') & OPR_DATE!=as.POSIXct('2011-02-03'))
    


## CHECK PRICE SPIKES
  rtdata=subset(priceswll, SP=="HB_NORTH" & RTSPP>=1000)
  rtdata$dthr=rtdata$OPR_DATE+rtdata$HE*60*60
  length(unique(rtdata$dthr))
  str(rtdata)





















#################################################
## ?)   ?                                  ######
#################################################
## HOURLY ANALYSES

  ## PLOT WIND VS RT SPPs
    combi=subset(pricesl, OPR_DATE>=as.POSIXct('2010-12-11'))
    plot(as.POSIXct(combi$dt), combi$RTSPP, pch=20, bty="l", xlim=c(as.POSIXct('2010-12-11'), as.POSIXct('2011-01-10')))
    combi=subset(pricesl, OPR_DATE>=as.POSIXct('2010-12-11')&OPR_HOUR>=19&OPR_HOUR<=20)
    points(as.POSIXct(combi$dt), combi$RTSPP, pch=20, col="red")
    combi=subset(pricesl, OPR_DATE>=as.POSIXct('2010-12-11')&OPR_HOUR>=21&OPR_HOUR<=22)
    points(as.POSIXct(combi$dt), combi$RTSPP, pch=20, col="blue")
    par(new=TRUE)
    plot(as.POSIXct(combi$dt), combi$wind, col="red", type="l", bty="l", xlim=c(as.POSIXct('2010-12-11'), as.POSIXct('2010-12-22')),axes=FALSE)
    axis(4)
    abline(h=1500)
    comb$dart=comb$RTSPP-comb$DASPP
    plot(comb$OPR_HOUR, comb$dart, pch=20,ylim=c(-50,50)); abline(h=0)
    points(comb$OPR_HOUR, comb$RTSPP, pch=15, col="red")


## ANALYSIS OF WIND
            ## WIND ACTUAL V FORECAST
             winddlt=wind
             winddlt$dt=winddlt$OPR_DATE+(winddlt$OPR_HOUR-1)*60*60
             winddlt=winddlt[order(winddlt$dt),]
            winddlt$st=winddlt$WIND-winddlt$STWPF_VALUE
            winddlt$wg=winddlt$WIND-winddlt$WGRPP_VALUE
            winddlt$st2=winddlt$st*winddlt$st
            winddlt$wg2=winddlt$wg*winddlt$wg
            winddlt$strd=round(winddlt$STWPF_VALUE,-3); length(unique(winddlt$strd)); winddlt$strdn=paste(winddlt$strd," MW",sep="")
            winddlt$wgrd=round(winddlt$WGRPP_VALUE,-3); winddlt$wgrdn=paste(winddlt$wgrd," MW",sep="")                         
            print("STWPF");sqrt(sum(winddlt[is.na(winddlt$st2)==FALSE,9]))
            print("WGRPP");sqrt(sum(winddlt[is.na(winddlt$wg2)==FALSE,10]))            
            
            
            
             ## plot time series
               plot(as.POSIXct(winddlt$dt), winddlt$WIND, bty="l", type="l", xlim=c(as.POSIXct('2010-12-01'),as.POSIXct('2011-01-03') ) )
                lines(as.POSIXct(winddlt$dt), winddlt$STWPF_VALUE, col="red")
                lines(as.POSIXct(winddlt$dt), winddlt$WGRPP_VALUE, col="blue")              
             ## plot rank-ordered
             winddlt=winddlt[order(winddlt$WIND, decreasing=TRUE),]
             plot(winddlt$WIND, bty="l", type="l")
              lines(winddlt$STWPF_VALUE, col="red")
              lines(winddlt$WGRPP_VALUE, col="blue")    
            winddlt=winddlt[order(winddlt$STWPF_VALUE, decreasing=TRUE),]
             plot(winddlt$WIND, bty="l", type="l")
              lines(winddlt$STWPF_VALUE, col="red",lwd=2)
              text(100,500, "STWPF", col="red")
            ## plot errors  
             winddlt=winddlt[order(winddlt$st, decreasing=TRUE),]
             plot(winddlt$st, bty="l", type="l",lwd=2)
             winddlt=winddlt[order(winddlt$wg, decreasing=TRUE),]
              lines(winddlt$wg, col="red", lwd=2)
            ## plot errors by wind output level
            ## Plot wind vs stwpf
              plot(winddlt$WIND, winddlt$STWPF_VALUE, pch=20, bty="l")
              dlt=winddlt[is.na(winddlt$WIND)==FALSE,]
              dlt$windsqrd=dlt$WIND*dlt$WIND
              fit=lm(STWPF_VALUE~WIND+windsqrd, data=dlt)                        
              points(dlt$WIND, fit$fitted.values, col="red",pch=20)
              abline(0,1,col="blue", lwd=2)
              lines(dlt$WIND, fitted(fit), col="red")
#################################              
 ## What is the error for a given STWPF or WGRRP forecast range?   
    ## GOOD add in the avg error line
            ##  PLOT the forecast error at various forecast levels
              xyplot( WINDACT ~ STWPF_VALUE | strd, data=winddlt, as.table=TRUE, pch=20)            
              winddlt=winddlt[order(winddlt$st,decreasing=TRUE),]
             winddlt=winddlt[order(winddlt$strd, decreasing=TRUE),]
            winddlt$ordering=1; 
            for(i in (2:nrow(winddlt)))          
              {
                if (winddlt$strd[i]==winddlt$strd[i-1]) {winddlt$ordering[i]= winddlt$ordering[i-1]+1}
              } 
            str(winddlt)
            xyplot( st ~ ordering | strdn, data=winddlt, as.table=TRUE, pch=20)
            xyplot( st ~ ordering | strdn, data=winddlt, as.table=TRUE,main="Actual wind minus STWPF forecast",
              panel = function(x, y) 
                {
                  panel.abline(h=0, col="grey", lty=2)
                  panel.xyplot(x,y,type="l",col="black")
                  panel.abline(h=mean(),type="l",col="black")
                }           
            )


    #################################
    ## Do early hours have more accurate wind forecasts?
             winddlt=winddlt[order(winddlt$st, decreasing=TRUE),]
              winddlt=winddlt[order(winddlt$OPR_HOUR),]
            winddlt$ordering=1; 
            for(i in (2:nrow(winddlt)))          
              {
                if (winddlt$OPR_HOUR[i]==winddlt$OPR_HOUR[i-1]) {winddlt$ordering[i]= winddlt$ordering[i-1]+1}
              } 
            str(winddlt)
          winddlt$hr=paste(sprintf("%02.0f", winddlt$OPR_HOUR),"hour",sep="_")
              winddlt=winddlt[order(winddlt$OPR_HOUR),]
          par(mfrow=c(1,2))
          hist(winddlt$st)  ; abline(v=0,lwd=3,col="red"); hist(winddlt$wg); abline(v=0,lwd=3,col="red");par(mfrow=c(1,1))
          xyplot( st ~ ordering | hr, data=winddlt, as.table=TRUE, pch=20)
          histogram( st | hr, data=winddlt, as.table=TRUE, pch=20)
          densityplot(~st | hr,data=winddlt, as.table=TRUE,)          

          xyplot( st ~ ordering | hr, data=winddlt, as.table=TRUE,
              panel = function(x, y) 
                {
                  panel.abline(h=0, col="grey", lty=2)
                  panel.xyplot(x,y,type="l",col="black")
                }           
            )

                    
        xyplot( value ~ ordering | location, data=dart, 
          groups=market, ylim=c(0,10),
          panel = function(x, y, …) 
            {
            panel.abline(h=0, col="grey", lty=2)
            panel.xyplot(x,y,…,type="l",col=c("black","red"))
            } 
         )
              str(winddlt)





#################################              
## view wind forecast
p1=subset(pricesw, SP=='HB_NORTH' & OPR_DATE==as.POSIXct('2010-01-08')); str(p1)
w=subset(winda, OPR_DATE==max(wind$OPR_DATE)); str(w)
fcstdate=as.Date('2011-01-08')
     		strQry=paste(
                  "  select publish_date, publish_hour, forecast_date OPR_DATE, forecast_hour OPR_HOUR, STWPF_VALUE value, WGRPP_VALUE 
                    from ze_data.ercot_nodal_aggr_wgrppf 
                    where forecast_date>=to_date('",fcstdate,"', 'YYYY-MM-DD') ",sep="")
         		windfcst <- sqlQuery(channel_zema_prod, strQry);
         		str(windfcst); windfcst$variable="stwpff"; windfcst$publish=windfcst$publish_date+windfcst$publish_hour*60*60
         		winds=cast(windfcst, OPR_DATE+OPR_HOUR~variable, max)
         		
         		



#################################              
## avg price by slope in wind
p1=subset(pricesw, SP=='HB_NORTH')
p1=p1[is.na(p1$WINDACT)==FALSE,]
p1lmp=p1;colnames(p1lmp)[6]="value"; p1lmp$variable="rtlmp"; p1lmp=cast(p1lmp, OPR_DATE+OPR_HOUR~variable, mean)
p1w=p1;colnames(p1w)[11]="value"; p1w$variable="wind"; p1w=cast(p1w, OPR_DATE+OPR_HOUR~variable, mean); 
p1=merge(p1lmp, p1w)
p1=p1[order(p1$OPR_HOUR),];p1=p1[order(p1$OPR_DATE),]
p1$dwind=0
for (i in 2:nrow(p1))
  {
    p1$dwind[i]=p1$wind[i]-p1$wind[i-1]
  }
for (i in 1:(nrow(p1)-1))
  {
    p1$dwind[i]=p1$wind[i+1]-p1$wind[i]
  }
p1$dwindrd=paste(round(p1$dwind/2,-2)*2,"MW"); unique(p1$dwindrd)
str(p1)
plot(p1$dwind, p1$rtlmp, pch=15,bty="l")

xyplot( value ~ ordering | location, data=dart, 
          groups=market, ylim=c(0,10),
          panel = function(x, y, …) 
            {
            panel.abline(h=0, col="grey", lty=2)
            panel.xyplot(x,y,…,type="l",col=c("black","red"))
            } 
         )

p1=subset(pricesw, SP=='HB_NORTH'& )


 

         
             
                                   
             
        str(prices)
## PLOT MORNING RAMP HOURS
p1=subset(pricesl, SP=='HB_NORTH')
plot(as.POSIXct(p1$dt), p1$RTSPP,pch=20,ylim=c(0,200))
p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR>6&OPR_HOUR<10))
p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR==24))
points(as.POSIXct(p1$dt), p1$RTSPP,pch=15, col="red")

## PLOT EVENING RAMP HOURS
p1=subset(prices, SP=='HB_NORTH')
plot(as.POSIXct(p1$dt), p1$RTSPP,pch=20,ylim=c(0,200))
i=22
imax=i+1
imin=i-1
p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR>imin&OPR_HOUR<imax))
points(as.POSIXct(p1$dt), p1$RTSPP,pch=20, col="red")
j=i+1
p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR==j))
points(as.POSIXct(p1$dt), p1$RTSPP,pch=20, col="orange")
k=i+2
p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR==k))
points(as.POSIXct(p1$dt), p1$RTSPP,pch=20, col="yellow")




i=6
for (i in 6:10)
{p1=subset(prices, SP=='HB_NORTH'&(OPR_HOUR==i)&OPR_DATE>=as.POSIXct('2010-12-01'))
print(paste("Hour ",i,": $",round(mean(p1$RTSPP),0),sep=""))}

 unique(prices$OPR_HOUR)

p1=subset(prices, SP=='HB_NORTH' & OPR_HOUR==6)
p1=p1[order(p1$RTSPP,decreasing=TRUE),]
plot(p1$RTSPP[3:nrow(p1)],type="l",bty="l",lwd=2)   ; abline(h=mean(p1$RTSPP))
print(mean(p1$RTSPP))
p1=subset(prices, SP=='HB_NORTH' & OPR_HOUR==7)
p1=p1[order(p1$RTSPP,decreasing=TRUE),]
lines(p1$RTSPP[3:nrow(p1)],type="l",lwd=2,col="darkred")                       ; abline(h=mean(p1$RTSPP),col="darkred")
print(mean(p1$RTSPP))
p1=subset(prices, SP=='HB_NORTH' & OPR_HOUR==8)
p1=p1[order(p1$RTSPP,decreasing=TRUE),]
lines(p1$RTSPP[3:nrow(p1)],type="l",lwd=2,col="red")                           ; abline(h=mean(p1$RTSPP),col="red")
print(mean(p1$RTSPP))

for ()







## What are the highest priced hours on Saturdays (6) and Sundays (0)
head(pricesi)                                ; tail(pricesi)
dayweekday=1
prices$wday=as.POSIXlt(prices$OPR_DATE)$wday 
if (dayweekday==1)  pricesi=subset(prices, (wday>0 & wday<7) & OPR_HOUR>=5 & OPR_HOUR<=12 & SP=='HB_NORTH') else pricesi=subset(prices, (wday==dayweekday) & OPR_HOUR>=5 & OPR_HOUR<=12 & SP=='HB_NORTH') 
colnames(pricesi)[match("RTSPP",colnames(pricesi))]="value"; pricesi$variable="RT"
pricesi=cast(pricesi, OPR_DATE+OPR_HOUR+wday ~ variable, mean)
pricesi=pricesi[order(pricesi$RT, decreasing=TRUE),]; pricesi=pricesi[order(pricesi$OPR_DATE),]
pricesi$rank=1
for (i in 2:nrow(pricesi))
  {
      if(pricesi$OPR_DATE[i]==pricesi$OPR_DATE[i-1])  pricesi$rank[i]=pricesi$rank[i-1]+1
  }
if (dayweekday==0 | dayweekday==6) {colnames(pricesi)[match("rank",colnames(pricesi))]="value"; pricesi$variable="avgrank";  if(dayweekday==6) print("Saturday"); if(dayweekday==0) print("Sunday"); cast(pricesi, OPR_DATE~OPR_HOUR, mean); cast(pricesi, OPR_HOUR~variable, mean); colnames(pricesi)[match("value",colnames(pricesi))]="rank"; colnames(pricesi)[match("RT",colnames(pricesi))]="value";  dlt1=cast(pricesi, OPR_DATE~OPR_HOUR, mean); dlt1[,2:ncol(dlt1)]=round(dlt1[,2:ncol(dlt1)],0); dlt1; dlt2=cast(pricesi, OPR_HOUR~variable, mean); dlt2[,2:ncol(dlt2)]=round(dlt2[,2:ncol(dlt2)],0); dlt2}
colnames(pricesi)[match("rank",colnames(pricesi))]="value"; pricesi$variable="avgrank";  dlt2=cast(pricesi, OPR_HOUR~variable, mean); dlt2[,2:ncol(dlt2)]=round(dlt2[,2:ncol(dlt2)],1); dlt2; colnames(pricesi)[match("value",colnames(pricesi))]="rank"; colnames(pricesi)[match("RT",colnames(pricesi))]="value";  dlt2=cast(pricesi, OPR_HOUR~variable, mean); dlt2[,2:ncol(dlt2)]=round(dlt2[,2:ncol(dlt2)],0); dlt2
dlt1=cast(pricesi, OPR_DATE~OPR_HOUR, mean); dlt1[,2:ncol(dlt1)]=round(dlt1[,2:ncol(dlt1)],0); dlt1; 






