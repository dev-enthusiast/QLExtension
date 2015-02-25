# Pull data needed for DAM analyses
#   store.interval.data     # consolidates wind/load forecasts and actual into one file.  Granularity is by SCED interval
#
#
# Written by Lori Simpson on 02-May-2011


#------------------------------------------------------------
# Functions
# STARTDT=as.Date('2011-04-30'); ENDDT=as.Date('2011-05-01')
# GET.RT.INTERVAL
# consolidates wind/load forecasts and actual into one file.  Granularity is by SCED interval . Only allowed to scrape 15 days of data at a time.
# res=store.interval.data(as.Date("2010-12-01"), as.Date("2010-12-05")); res
# res=store.interval.data(startDate=as.Date("2011-04-14"), endDate=as.Date("2011-04-16")); res
# res=store.interval.data(); res
# startDate=as.Date("2011-08-01"); endDate=as.Date("2011-08-02")
# startDate=STARTDT; endDate=ENDDT; conn=channel_zema_prod 
# startDate=Sys.Date()-5; endDate=Sys.Date()-3; conn=channel_zema_prod
# 
store.interval.data <- function(startDate=NULL, endDate=NULL, conn)
{

     ## Store log data
      rLog(paste("Start store.interval.data function at", Sys.time()))

    ## Limit dates that can be queried
    	if (length(startDate)==0) startdate=max(min(as.Date(max(dfRT$OPR_DATE))-2, Sys.Date()),as.Date("2010-12-01")) else startdate=max(startDate, as.Date("2010-12-01"))
    	if (length(endDate)==0) lmtgdate=max(min(startdate+5, Sys.Date()), as.Date("2010-12-01"))  else lmtgdate=max(endDate, as.Date("2010-12-01"))
    	if (lmtgdate-startdate>30) {lmtgdate=startdate+15; rLog(paste("Asked for too much data--will only get 15 days of data.")); stop}
    	if (lmtgdate<=startdate) {rLog(paste("ERROR: The end date(",lmtgdate,") is greater then or equal to the start date (",startdate,").",sep="")); stop}
    	rLog(paste("Collecting ",lmtgdate-startdate+1," days of data.",sep=""))
      if(lmtgdate<startdate) {rLog(paste("End date must be greater than or equal to start date")); return("End date must be greater than or equal to start date")}
      STARTDT=startdate; LMTGDT=lmtgdate
      

    ## LMPS & platts prices
       	rLog(paste("Will attempt to pull LMPs"))

            ## Get RT lmps by SCED run and DA SPPs for hubs and load zones
              arrSPs=c('HB_NORTH','HB_SOUTH','HB_HOUSTON','HB_WEST','LZ_NORTH','LZ_SOUTH','LZ_HOUSTON','LZ_WEST','LZ_RAYBN')
              lmps=pull.lmps(STARTDT, LMTGDT, arrSPs, conn); if(length(lmps)==1) {rLog(paste("Returned no LMPs: ",lmps[1])); stop}
              lmps$OPR_MINUTE=NULL; colnames(lmps)[match("RTSPP", colnames(lmps))]="RTLMP"; 
              lmps=lmps[,c("OPR_DATE","HE","SCED_MINUTE_START","SP","RTLMP","dt_start","pk","fiveminbegin")]
              ## Add in interval number
                intmap=cbind(seq(0,55,by=5),c(rep(1,3),rep(2,3),rep(3,3),rep(4,3))); colnames(intmap)=c("fiveminbegin","int")
                lmps=merge(lmps, intmap)
            ## Pull RT SPPs
             	rLog(paste("Will attempt to pull SPPs"))
              spps=pull.spps(STARTDT, LMTGDT, arrSPs, conn); if(length(spps)==1) {rLog(paste("Returned no SPPs: ",spps[1])); stop}
              colnames(spps)[match("INTERVAL",colnames(spps))]="int"
            ## Combine
              spps=merge(lmps,spps,all.y=TRUE) 
              ## sort(unique(subset(spps, OPR_DATE==as.POSIXct('2011-03-13'))$HE))
                
              
            ## GET Platts data.  DO NOT YET HAVE SYMBOLS FOR DA LZ PRICES!
               	rLog(paste("Will attempt to pull platts data"))
                # Stopped producing MWDaily power indices in Dec2014
                  # symbolst=c('MWDaily_16HrDly_ERCOTHous', 'MWDaily_16HrDly_ERCOTNort', 'MWDaily_16HrDly_ERCOTSout', 'MWDaily_16HrDly_ERCOTWest', 'MWDaily_OPDly_ERCOTHous', 'MWDaily_OPDly_ERCOTNort', 'MWDaily_OPDly_ERCOTSout', 'MWDaily_OPDly_ERCOTWest', 'gasdailymean_ship', 'gasdailymean_henry', 'gasdailymean_waha', 'gasdailymean_katy')
                  # symbolnames=c('HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST', 'HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST', 'hsc', 'hh','waha','katy')
                  # symbolpk=c('pk','pk','pk','pk','op','op','op','op','gas','gas','gas','gas')
                # Non-power indices 
                  symbolst=c('gasdailymean_ship', 'gasdailymean_henry', 'gasdailymean_waha', 'gasdailymean_katy')
                  symbolnames=c('hsc', 'hh','waha','katy')
                  symbolpk=c('gas','gas','gas','gas')
                tsdbsymbols=cbind(symbolst,symbolnames); tsdbsymbols=cbind(tsdbsymbols,symbolpk); tsdbsymbols=as.data.frame(tsdbsymbols); 
                  colnames(tsdbsymbols)=c("variable", "SP", "pk")
                plattspwr=pull.tsdb(STARTDT, LMTGDT, symbolst, hourconvention=2)
                  if(length(plattspwr)==1) {rLog(paste(plattspwr[1])); stop}
                platts=merge(plattspwr, tsdbsymbols); 
                # plattspwr=subset(platts, pk!="gas")[,c(2:ncol(platts))]
                # colnames(plattspwr)[match("value",colnames(plattspwr))]="plattspwr"
              ## Get the gas
                plattsgas=subset(platts, pk=="gas")[,c(2,3,4)]; 
                plattsgas=cast(plattsgas, OPR_DATE~SP, mean)
                colnames(plattsgas)[2:5]=paste("platts_",colnames(plattsgas)[2:5],sep="")

           ## Combine DA/RT LMPs with Platts DA ICE data and add appropriate time/date mappings
              prices=spps
              prices$plattspwr=NA
              prices=merge(prices, plattsgas,all=TRUE)
              regionmap=cbind(c('HOUSTON','NORTH','SOUTH','WEST', 'HOUSTON','NORTH','SOUTH','WEST','NORTH'),c('LZ_HOUSTON','LZ_NORTH','LZ_SOUTH','LZ_WEST', 'HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST', 'LZ_RAYBN')); colnames(regionmap)=c("region","SP")
              prices=merge(prices, regionmap)
              zonemap=cbind(c('HOUSTON','NORTH','SOUTH','WEST', 'HOUSTON','NORTH','SOUTH','WEST','RAYBN'),c('LZ_HOUSTON','LZ_NORTH','LZ_SOUTH','LZ_WEST', 'HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST', 'LZ_RAYBN')); colnames(zonemap)=c("zone","SP")
              prices=merge(prices, zonemap)
              rm(plattsgas); rm(symbolpk);rm(symbolst);rm(tsdbsymbols); rm(lmps)
              ##write.csv(prices, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")



    ## RT WIND AND LOAD
            ## 5-minute wind and load data
             	rLog(paste("Will attempt to pull RT Load and Wind"))
              if(STARTDT>as.Date('2011-04-01'))
                {
                  windrt=pull.tsdb(STARTDT, LMTGDT, symbolst=c('ercot_wind_iccp'), hourconvention=1)
                  windrt=windrt[,c(3,4,5,6)]; colnames(windrt)[1]="value"; windrt$variable="windrt"; windrt=cast(windrt, OPR_DATE+HE+fiveminbegin~variable, mean)
                } else {windrt=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, windrt=0)[0,]}
              if(STARTDT>as.Date('2011-01-25'))
                {
                  loadrt=pull.tsdb(STARTDT, LMTGDT, symbolst=c('ercot_system_load'), hourconvention=1)
                  loadrt=loadrt[,c(3,4,5,6)]; colnames(loadrt)[1]="value"; loadrt$variable="loadrt"; loadrt=cast(loadrt, OPR_DATE+HE+fiveminbegin~variable, mean)
                } else {loadrt=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, loadrt=0)[0,]}
                if(nrow(loadrt)>0) for(i in 2:nrow(loadrt))  {if(is.na(loadrt$loadrt[i])) loadrt$loadrt[i]=loadrt$loadrt[i-1]}
            ## combine with prices, don't have ICCP data going back very far
              prices1=merge(prices, loadrt, all.x=TRUE)
              prices1=merge(prices1, windrt, all.x=TRUE)
   
  
     
    ## WIND - hourly & DA
           	rLog(paste("Will attempt to pull wind forecasts"))
            ## Regional wind forecast
              arrRes=pull.wind.reg(STARTDT, LMTGDT, conn); if(length(arrRes)==1) {rLog(paste(arrRes[1])); stop}
              if (nrow(arrRes)>0)
                {
                arrResT=cast(arrRes, OPR_DATE+HE~region, mean); arrResT$wind_act_total=rowSums(arrResT[,c(3:ncol(arrResT))])
                colnames(arrRes)[5]="wind_reg_act"
                arrRes=merge(arrRes, arrResT[,c("OPR_DATE","HE","wind_act_total")])
                } else 
                {
                arrRes=cbind(arrRes, arrRes[,5])
                colnames(arrRes)[5]="wind_reg_act"
                colnames(arrRes)[6]="wind_act_total"
                print("here")
                }              
             
            ## DA regional wind forecast
              arrRes2=pull.wind.reg.fcst(STARTDT, LMTGDT, DAct=1, regional=1, conn); if(length(arrRes2)==1) {rLog(paste(arrRes2[1])); stop}
              if(nrow(arrRes2)==0) {arrRes2=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, region=0, wind_reg_fcst_STWPF=0, wind_reg_fcst_WGRPP=0)[0,]}
              if(nrow(arrRes2)>0) {arrRes2=cast(arrRes2, OPR_DATE+HE+region~fcst_type, mean); colnames(arrRes2)[4:5]=paste("wind_reg_fcst",colnames(arrRes2)[4:5],sep="_"); }
              arrRes2=as.data.frame(arrRes2)
            ## DA total wind forecast
              arrRes3=pull.wind.reg.fcst(STARTDT, LMTGDT, DAct=1, regional=0, conn); if(length(arrRes3)==1) {rLog(paste(arrRes3[1])); stop}
              if(nrow(arrRes3)==0) {arrRes3=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, wind_STWPF=0, wind_WGRPP=0)[0,]}
              if(nrow(arrRes3)>0) {arrRes3=cast(arrRes3, OPR_DATE+HE~variable, mean); colnames(arrRes3)[3:4]=paste("wind",colnames(arrRes3)[3:4],sep="_"); }
            ## Combine
              wind=merge(arrRes, arrRes2)
              rLog(paste("Have pulled wind data."))

          ## Merge with prices
            ## Combine ercot wide-wind with prices
              prices2=merge(prices1, arrRes3, all.x=TRUE)
              ##DLT:  if(rowct1!=rowct2) {return("Not pulling enough DA ERCOT-wide wind forecasts (using pull.wind.reg.fcst function)."); break}
            ## Combine regional wind with prices
              prices2=merge(prices2, wind, all.x=TRUE)
              prices2[is.na(prices2$wind_reg_act)==TRUE, "wind_reg_act"]=0
              prices2[is.na(prices2$wind_reg_fcst_STWPF)==TRUE, "wind_reg_fcst_STWPF"]=0
              prices2[is.na(prices2$wind_reg_fcst_WGRPP)==TRUE, "wind_reg_fcst_WGRPP"]=0
          



    ## LOAD - hourly & DA
        ## Actuals by load zone
            ##  Will have NAs for all data inside the current month until populating the spreadsheet
            ##  uploadzonalload.csv  
            ##  This gives historical load by load zone, but only available after delivery at the ops site, reports, demand and energy
             	rLog(paste("Will attempt to pull regional load actuals"))
            loadz=pull.load.reg(STARTDT, LMTGDT); if(length(loadz)==1) {rLog(paste(arrRes[1])); return("Could not pull any actual regional load!")}
           	if(nrow(loadz)==0) rLog(paste("Need to update uploadzonalload.csv file with most recent data.", sep=""))
            colnames(loadz)[match("hour",colnames(loadz))]="HE"
            loadz=loadz[,c("OPR_DATE","HE","int","load_reg_act_sum","zone","load_reg_act")]
            if(nrow(loadz)>0) {loadza=aggregate(loadz[,c("load_reg_act_sum","load_reg_act")], by=list(loadz$OPR_DATE, loadz$HE, loadz$zone), sum); colnames(loadza)[c(1,2,3)]=c("OPR_DATE","HE","zone")}
            if(nrow(loadz)==0) loadza=loadz            
            prices3=merge(prices2, loadza, all.x=TRUE)
            
        ## Actuals by weather zone
            ##   select * from ze_data.ercot_actual_load;  actual load by weather zone hourly
            
        ## DA forecast by load zone (agg=0)
             	rLog(paste("Will attempt to pull regional load forecasts"))
            ldfc=pull.load.reg.fcst(STARTDT, LMTGDT, DAct=1, agg=0, conn); 
            if(length(ldfc)==1) {rLog(paste(ldfc[1])); prices3$load_reg_fcst=NA}
            if(length(ldfc)>1) 
              {
                ldfc$ZONE=toupper(ldfc$ZONE)
                colnames(ldfc)[3:4]=c("region","load_reg_fcst")
                prices3=merge(prices3, ldfc, all.x=TRUE)
              }
        ## DA forecast system-wide (agg=1)
             	rLog(paste("Will attempt to pull system load forecasts"))
            ldfcsys=pull.load.reg.fcst(STARTDT, LMTGDT, DAct=1, agg=1, conn); 
            if(length(ldfcsys)==1) {rLog(paste(ldfc[1])); prices3$load_sys_fcst=NA}
            if(length(ldfc)>1) 
              {
                colnames(ldfcsys)[3]=c("load_sys_fcst")
                prices3=merge(prices3, ldfcsys, all.x=TRUE)
              }

        ## Get actual system load (as the ICCP symbol doesn't go back far enough
             	rLog(paste("Will attempt to pull system load actuals"))
            ld=pull.load(STARTDT, LMTGDT, conn); if(length(ld)==1) {rLog(paste(ld[1,1])); stop}
            colnames(ld)[4]="load_sys_interval"
            prices3=merge(prices3, ld, all.x=TRUE)
            rLog(paste("Collected load data"))

    ## Add dt_he if don't have already
      if( nrow(prices3[is.na(prices3$dt_he)==FALSE, ])!=nrow(prices3)) prices3$dt_he=prices3$OPR_DATE+prices3$HE*60*60            
              
    ## WN limits
        ## pull from when binding, if binding.  if not, have to use the generic limits unless can interpolate that day's limtis
             	rLog(paste("Will attempt to pull W-N limits"))
              #            if(STARTDT>as.Date('2011-01-28'))
              #              {
              ### Temporary for processor failure: wnda=pull.tsdb(STARTDT, LMTGDT, symbolst=c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'), hourconvention=0); colnames(wnda)[3]="limit_wn_da"
              #              } else 
              wnda=data.frame(dt_he=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, limit_wn_da=0)[0,]
               prices4=merge(prices3, wnda[,c("dt_he","limit_wn_da")], all.x=TRUE)
              #            if(STARTDT>as.Date('2010-12-18'))
              #              {
              #               	rLog(paste("Will attempt to use pull.tsdb function"))
              #                wnrt=pull.tsdb(STARTDT-30, LMTGDT+3, symbolst=c('ZM_ERCOT_NODAL_SCED_SP_BTC_LIMIT_CONSTR_NAME_W_TO_N'), hourconvention=1)
              #                wnrt=subset(wnrt, OPR_DATE<=as.POSIXct(LMTGDT+1) & OPR_DATE>as.POSIXct(STARTDT))
              #                wnrt=wnrt[,c(3,4,5,6)]; colnames(wnrt)[1]="value"; 
              #                if(nrow(wnrt)>0) 
              #                  {wnrt$variable="limit_wn_rt"; wnrt=cast(wnrt, OPR_DATE+HE+fiveminbegin~variable, mean)} else
              #                  {wnrt=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, limit_wn_rt=0)[0,]}
              #              } else {wnrt=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, limit_wn_rt=0)[0,]}
              if( is.na(match("wnrt",ls())) )  wnrt=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, fiveminbegin=0, limit_wn_rt=0)[0,]
                dfRTnew=merge(prices4, wnrt, all.x=TRUE)
             rLog(paste("Collected wind limits"))
        dfRTnew$moyr=paste(as.POSIXlt(dfRTnew$OPR_DATE)$year+1900, sprintf("%02.0f", as.POSIXlt(dfRTnew$OPR_DATE)$mon+1), sep="_")

     ## Store data by month-year
        filestart="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_"
        fileend=".RData"     
        monthsdf=unique(dfRTnew$moyr)
        for (i in  1:length(monthsdf))
          {
            filesave=paste(filestart, monthsdf[i], fileend, sep="")
            if(file.exists(filesave))
              {
              dfRTnewi=subset(dfRTnew, moyr==monthsdf[i])
              load(file=filesave); 
              dfRT=rbind( subset(dfRT, (OPR_DATE<min(prices4$OPR_DATE) | OPR_DATE>max(prices4$OPR_DATE))),   dfRTnewi )
              save(dfRT, file=filesave); 
              }
            if(!file.exists(filesave))
              {
              dfRT=subset(dfRTnew, moyr==monthsdf[i])
              save(dfRT, file=filesave); 
              }
            rm(dfRT)
          }
          
     load(file=filesave); 
     rLog(paste("Combined interval file: ", min(dfRT$OPR_DATE), " : ", max(dfRT$OPR_DATE),".  File: ", filesave, sep=""))
          



      ### Store hourly data
      ## Get into interval averages
        rtInt=aggregate(subset(dfRTnew, select=c("RTSPP","DASPP","plattspwr","platts_hh","platts_hsc","platts_katy","platts_waha","loadrt","windrt","wind_STWPF","wind_WGRPP","wind_reg_act","wind_act_total","wind_reg_fcst_STWPF","wind_reg_fcst_WGRPP","load_reg_act_sum","load_reg_act","load_sys_fcst","load_reg_fcst","load_sys_interval","limit_wn_da","limit_wn_rt"))
          , by=list(dfRTnew$int, dfRTnew$OPR_DATE, dfRTnew$HE, dfRTnew$pk, dfRTnew$SP, dfRTnew$dt_he, dfRTnew$moyr, dfRTnew$region, dfRTnew$zone), mean)
        colnames(rtInt)[1:9]=c("int","OPR_DATE","HE","pk","SP","dt_he", "moyr","region","zone")  
      ## Get hourly averages  
        rthourly=aggregate(subset(rtInt, select=c("RTSPP","DASPP","plattspwr","platts_hh","platts_hsc","platts_katy","platts_waha","loadrt","windrt","wind_STWPF","wind_WGRPP","wind_reg_act","wind_act_total","wind_reg_fcst_STWPF","wind_reg_fcst_WGRPP","load_reg_act_sum","load_reg_act","load_sys_fcst","load_reg_fcst","load_sys_interval","limit_wn_da","limit_wn_rt"))
          , by=list(rtInt$OPR_DATE, rtInt$HE, rtInt$pk, rtInt$SP, rtInt$dt_he, rtInt$moyr, rtInt$region, rtInt$zone), mean)
        colnames(rthourly)[1:8]=c("OPR_DATE","HE","pk","SP","dt_he", "moyr","region","zone")  
        rthourly=rthourly[order(rthourly$HE),]; rthourly=rthourly[order(rthourly$OPR_DATE),]
          rthourly$yr=as.POSIXlt(rthourly$OPR_DATE)$year+1900

      ## Store file by year
          filestart="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_"
          fileend=".RData"     
          yearsdf=unique(rthourly$yr)
          for (i in  1:length(yearsdf))
            {
              filesave=paste(filestart, yearsdf[i], fileend, sep="")
              if(file.exists(filesave))
                {
                rthourlyi=subset(rthourly, yr==yearsdf[i])
                load(file=filesave); 
                rthourly=rbind( subset(rthourly, (OPR_DATE<min(rthourlyi$OPR_DATE) | OPR_DATE>max(rthourlyi$OPR_DATE))),   rthourlyi )
                save(rthourly, file=filesave); 
                }
              if(!file.exists(filesave))
                {
                rthourly=subset(rthourly, yr==yearsdf[i])
                save(rthourly, file=filesave); 
                }
              rm(rthourly)
            }



     load(file=filesave); 
     rLog(paste("Combined hourly file: ", min(rthourly$OPR_DATE), " : ", max(rthourly$OPR_DATE),".  File: ", filesave, sep=""))

     return(rthourly)

}





##############################################################################
##############################################################################
#
require(CEGbase)
options(width=150)     
require(SecDb)
##Sys.setenv( SECDB_ORACLE_HOME=paste( "C:\\oracle\\ora90" ) )
##secdb.evaluateSlang( "Try( E ) { ODBCConnect(\"\",\"\"); } : {}" )
require(RODBC)
require(FTR)
source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

# q( status = 0 )   ## Temporarily prevent from running



#------------------------------------------------------------
# Run the file to consolidate all RT data
# STARTDT=as.Date('2011-04-30'); ENDDT=as.Date('2011-05-01')

  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)

  ## Get Data
    STARTDT=Sys.Date() - 5
    ENDDT=Sys.Date() - 0
      rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,sep=""))
    res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
      rLog(paste("Finished pulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,"\n",sep=""))

  ## Get old data
    ## 1 week back
      # STARTDT=Sys.Date() - 10; ENDDT=Sys.Date() -8
      # rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT,sep=""))
      # res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
    ## 2 months back
      STARTDT=Sys.Date() - 60; ENDDT=Sys.Date() - 58
      rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT,sep=""))
      res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
    ## 1 year back
      STARTDT=max(Sys.Date() - 343, as.Date('2010-12-01')); ENDDT=max(Sys.Date() - 342, as.Date('2010-12-03'))
      rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT,sep=""))
      res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
  

  odbcClose(channel_zema_prod)
  
  

#------------------------------------------------------------
# Log finish
#
rLog(paste("Done at ", Sys.time()))


#------------------------------------------------------------
# Interactive=true if ran by hand
#
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}




## #------------------------------------------------------------
## # Backfill data
## #
     STARTDT=as.Date("2013-01-01")
     FINALENDDT=as.Date("2013-12-31")
     FINALENDDT=Sys.Date()
     ENDDT=STARTDT+4
     backfill=FALSE

 if(backfill)
   {
     for(i in 1:3)
       {                                                                                                        
         cat(paste("Start: ",STARTDT,"\tEnd: ",ENDDT,"\n",sep=""))
         rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,sep="")); flush.console()
         res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
         rLog(paste("\n\nPulling data from ",STARTDT," to ", ENDDT," at ", Sys.time() ,sep="")); flush.console()
         STARTDT=ENDDT+1
         ENDDT=min(STARTDT+4,FINALENDDT)
         if(ENDDT<STARTDT) stop("ended")
       }
   }    
    


## DLT BELOW  - to backfill the hourly files
#moyr="2011_05"
#yearr=2011
#        filestart="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_"
#        fileend=".RData"     
#            filesave=paste(filestart, moyr, fileend, sep="")
#        load(file=filesave); 
#        dfRTnew=dfRT
#      rthourly=aggregate(subset(dfRTnew, select=c("RTSPP","DASPP","plattspwr","plattsgas","loadrt","windrt","wind_STWPF","wind_WGRPP","wind_reg_act","wind_act_total","wind_reg_fcst_STWPF","wind_reg_fcst_WGRPP","load_reg_act_sum","load_reg_act","load_sys_fcst","load_reg_fcst","load_sys_interval","limit_wn_da","limit_wn_rt"))
#        , by=list(dfRTnew$OPR_DATE, dfRTnew$HE, dfRTnew$pk, dfRTnew$SP, dfRTnew$dt_he, dfRTnew$moyr, dfRTnew$region, dfRTnew$zone), mean)
#      colnames(rthourly)[1:8]=c("OPR_DATE","HE","pk","SP","dt_he", "moyr","region","zone")  
#      rthourly=rthourly[order(rthourly$HE),]; rthourly=rthourly[order(rthourly$OPR_DATE),]
#      rthourly$plattshr=rthourly$plattspwr/rthourly$plattsgas
#      rthourly$yr=as.POSIXlt(rthourly$OPR_DATE)$year+1900
#
#      ## Store file by year
#          filestart="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_"
#          fileend=".RData"     
#          filesave=paste(filestart, yearr, fileend, sep="")
#              if(file.exists(filesave))
#                {
#                rthourlyi=rthourly
#                load(file=filesave); 
#                rthourly=rbind( subset(rthourly, (OPR_DATE<min(rthourlyi$OPR_DATE) | OPR_DATE>max(rthourlyi$OPR_DATE))),   rthourlyi )
#                save(rthourly, file=filesave); 
#                }
#              if(!file.exists(filesave))
#                {
#                rthourly=subset(rthourly, yr==yearsdf[i])
#                save(rthourly, file=filesave); 
#                }
### DLT ABOVE     
    
