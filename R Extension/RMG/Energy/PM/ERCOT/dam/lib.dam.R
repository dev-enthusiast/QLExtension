# Library of functions for daily inc-dec trading
# Used by Energy/PM/ERCOT/DAM/main.dam.R
# 
#
# Functions: 
#
#
# Utility Functions:
#   pull.lmps               # this will get the DA SPPs and RT LMPs for a given set of SPs, start date, and end date
#   pull.spps               # pulls interval SPPs
#   pull.mcpe               # pulls zonal MCPe
#   pull.tsdb               # returns tsdb data as a melted dataframe  (hourconvention:  0=HE,    1=dt_start (like SCED interval);     2=daily (like Platts data))
#   pull.wind.reg           # pulls the actual wind output by zone (data was n,w,s <apr11 and n-w, s-h >apr11).  Won't return any data if you choose dates <=12/11/10 as have no data
#   pull.wind.reg.fcst      # pulls the wind forecast from the regional wind table.  can specify x days ahead and whether you want regional or ercot-wide numbers (data was n,w,s <apr11 and n-w, s-h >apr11). Won't return any data if you choose dates <=12/11/10 as have no data
#   pull.load.reg           # pulls 15-minute actual load by load zone.  don't have zema scraper so manually populate a csv file from the ops website after the operating month ends
#   pull.load.reg.fcst      # pulls the load forecast X days ahead of time (default=1).  agg is aggregation.  agg=0 is load zone, agg=1 is ercot total, agg=2 is weather zone. Won't return any data if you choose dates <=12/20/10 as have no data
#   pull.60d.sced.data      # pulls data from the ercot_nodal_60dscedgrd sced table.  
#   pull.60d.mne.data       # pulls data from the ercot menetered net energy from gen resources
#   alert.SPs               # emails plant information for updating my plant info sheet
#   get.nodal.zonal.data    # combines hourly lmps (da,rt,otc), load, wind, gas, and load forecast from zonal and nodal, 2005-present
#   get.gen.stack           # uses SCED EOCs to calculate the heat rates of units and make a gen stack
#   get.gen.hsls            # finds the hsl and lsls for units
#
#
# Pull data specific to our portfolio:
#   get.our.zonal.loads     # pulls Rayburn, Stream, and CNE FP loads.     1/1/08 - 11/30/10
#   get.our.zonal.loads.nmkt.earlier  #   Gets load data for Rayburn and Stream before the new QSE inception  5/1/08 - 11/30/10
#   
#
#
# Variables
#   region                  ERCOT congestion managmenet zone (does NOT include carve-out NOIEs)
#   zone                    Nodal load zones (including carve-out NOIEs)
#   dt_he                   e.g. 2010-05-03 03:00 represents the time frame from 2-3AM
#   dt_start                e.g. 2010-05-03 03:00 represents the SCED interval that starts at 3AM and probably lasts until 3:05AM (unless it is a short interval)
#   dt_unknown              don't know which format data is returned (e.g. pulling TSDB)
#   fiveminbegin            e.g. 5 represents the fiveminute interval from 5-10
#   SCED_MINUTE_START       e.g. 7 is probably the SCED interval that starts at 7 and ends at 10                                                                                                                                                              
#   HE                      1-24.  e.g. HE1 is midnight to 1AM
#   OPR_DATE                day beginning e.g. 2010-05-03 is all data for that day (e.g. OPR_DATE for HE24 would be 2/13 while HE would say 2/14 00:00)
#   OPR_HOUR                hour beginning e.g. 23 is all data for that hour (e.g. from 23:00 to 23:59)
#   OPR_MINUTE              15-minute interval beginning e.g. 15 is data from xx:15 to xx:30
#   int                     1:4.  1 is 0:00-0:15; 2 is 0:15-0:30; 3 is 0:30-0:45; 4 is 0:45-0:60
#   ie                      is the hour and minute representing the end of the interval.  e.g. 4:45 is 4:30-4:45
#   
#   ADRIAN'S TO DELETE:
#   bucket.hprices.daily
#   calc_portfolio_pnl       # calc the historical PnL of the positions you chose for tomorrow
#   inc.price.trades
#   find.price.arbitrage
#   fix.exceptions   for WRONG names in eMKT
#   format.next.day.trades   # output for xls upload
#   get.incdec.hprices
#   inc.basic.stats          # retarded little function
#   inc.get.trading.signals  # 
#   max_min_locations        # find the locations with biggest spread
#   next.day.trades          # used by procmon job
#   price_one_incdec         # general function to price inc/decs
#   trigger.Persistence
#   trigger.1DayPanicDA
#   trigger.7DayDomination
#   .lookback1          # actual strategy, calculates PnL, and makes recommendation
#   strategy_hub        # how to play the virts at the hub  (started Jan-2011)
#   strategy_hub_rpt    # how to report it


## rm(list=ls()); source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## All functions for the daily DAM prep file














#####################################################################
# PULL.LMPS
#   returns 5-minute data
#   arrRes=pull.lmps(startdate=as.Date("2011-04-16"), lmtgdate=as.Date("2011-04-17"), arrSPs=c('HB_NORTH','HB_SOUTH'), conn); str(arrRes)
# 
pull.lmps = function (startdate, lmtgdate, arrSPs, conn)
  {

      ## Check that the SPs are valid
        		strQry=paste("select distinct(RT.settlement_point) sp  from ZE_DATA.ercot_nodal_rt_lmp_re_node RT, ZE_DATA.ERCOT_NODAL_DAM_SPPS DA
                  where DA.opr_date=RT.opr_date and DA.opr_hour=(RT.opr_hour+1) and RT.settlement_point=DA.settlement_point 
                    and DA.opr_date=to_date('",startdate,"', 'YYYY-MM-DD') 
                    and RT.settlement_point in ('",paste(arrSPs, collapse="','"),"');",sep="")
        			lmpscheck <- sqlQuery(conn, strQry);
        			if (nrow(lmpscheck)!=length(arrSPs)) {return(paste("You have chosen invalid settlement points to pull RT/DA LMPs.  ", strQry, sep="")); break;}

      ## DA: opr_hour is HE, Hours 1:24 all belong to same opr_date.  RT: opr_hour is HB.  
        		strQry=paste("select RT.opr_date, DA.opr_hour HE, RT.opr_minute SCED_minute_start, RT.settlement_point sp, rt.lmp rtspp, DA.settlement_point_price daspp
                  from ZE_DATA.ercot_nodal_rt_lmp_re_node RT, ZE_DATA.ERCOT_NODAL_DAM_SPPS DA
                  where DA.opr_date=RT.opr_date and DA.opr_hour=(RT.opr_hour+1) and RT.settlement_point=DA.settlement_point
                  and DA.opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and DA.opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD') 
                  and RT.settlement_point in ('",paste(arrSPs, collapse="','"),"');",sep="")
        			lmpsraw <- sqlQuery(conn, strQry);
    
            spps=lmpsraw
            spps$dt_start=spps$OPR_DATE+(spps$HE-1)*60*60+spps$SCED_MINUTE_START*60
            spps$pk="pk"
            sppsop=subset(spps, HE<7 | HE>22)
            sppsop$pk="op"
            spps=rbind( subset(spps, HE>6 & HE<23), sppsop)
            ## CONVERT OPERATING_MINUTE TO fiveminute_beginning
         		spps$fiveminbegin=floor(spps$SCED_MINUTE_START/5)*5  		
            
            spps=spps[order(spps$dt_start),]
    return(spps)
  }
 






 

#####################################################################
# PULL.SPPS
#   returns 15-minute data
#   arrRes=pull.spps(startdate=as.Date("2011-07-20"), lmtgdate=as.Date("2011-07-20"), arrSPs=c('HB_NORTH','HB_SOUTH'), conn); str(arrRes)
#   startdate=as.Date("2011-07-19"); lmtgdate=as.Date("2011-07-20"); arrSPs=c('HB_NORTH','HB_SOUTH'); conn=channel_zema_prod    
#dlt below        
#start_date='2011-03-13'; end_date='2011-03-14'; connzema=channel_zema_prod
#startdate=STARTDT; lmtgdate=STARTDT+2; conn=channel_zema_prod; arrSPs=c('HB_NORTH','CBEC_CCU1','LZ_SOUTH')
#sp=c('HB_NORTH','CBEC_CCU1','HB_SOUTH','LHSES_UNIT2','HB_HOUSTON','QALSW_CCU1','HB_WEST','WOO_WOODWRD2','FPPYD_FPP_G3','AMISTAD_ALL','QALSW_CCU2','LZ_HOUSTON','LZ_NORTH','LZ_RAYBN','LZ_SOUTH','LZ_WEST','DC_R','FRO_FRONTEG3','KUNI_LGE_NWP','LZ_CPS','NWF_NWF1','SIL_SILAS_9','TXCTY_CTA')
# startdate=as.Date(startdate); lmtgdate=as.Date(enddate); arrSPs=c('HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST'); conn=channel_zema_prod    
#  startdate=STARTDT; lmtgdate=LMTGDT; 

pull.spps = function (startdate, lmtgdate, arrSPs, conn)
  {


      ## DA: opr_hour is HE, Hours 1:24 all belong to same opr_date.  RT: opr_hour is HB.  
        ## Pull DA
        		strQry=paste("select DA.opr_date, DA.opr_hour HE, DA.settlement_point sp, DA.settlement_point_price daspp, count(DA.settlement_point_price)
                  from ZE_DATA.ERCOT_NODAL_DAM_SPPS DA
                  where da.opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and da.opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD') 
                  and DA.settlement_point in ('",paste(arrSPs, collapse="','"),"') 
                  group by DA.opr_date, DA.opr_hour , DA.settlement_point  , DA.settlement_point_price order by 1,2,3",sep="")
        			lmpsrawda <- sqlQuery(conn, strQry);     
        			## Test for duplicate data
        			   lmptest=lmpsrawda; colnames(lmptest)[4]="value"; lmptest$variable="count"; lmptest=cast(lmptest, OPR_DATE+HE+SP~variable, length) 
        			   if(max(lmptest$count)>1) {rLog("Function PULL.SPPS is returning more than one DA SPP for a given date and SP."); return("Function PULL.SPPS is returning more than one DA SPP for a given date and SP.")}
            	## Ensure that data is between 23 and 25 hours
              	lmpstest=lmpsrawda
                if(nrow(lmpstest)>0) if(min(tapply(lmpstest$OPR_DATE, list(lmpstest$OPR_DATE), length))<(length(arrSPs)*23) | max(tapply(lmpstest$OPR_DATE, list(lmpstest$OPR_DATE), length))>(length(arrSPs)*25)) {rLog("Function PULL.SPPS is missing hours when pulling DA SPPs"); return("Function PULL.SPPS is missing hours when pulling DA SPPs.")}

        ## Pull RT
          ## Get before LZ changes
              lmpsrawrtpretest=FALSE
              if(startdate<=as.Date('2012-03-03'))
                {
              		strQry=paste("select RT.opr_date, rt.opr_hour HE, interval, RT.sp sp, avg(rt.value) rtspp, count(rt.value)
                        from ZE_DATA.ERCOT_NODAL_REAL_TIME_SPPS RT
                        where rt.opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and rt.opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD') 
                        and rt.opr_date<'04-mar-12'
                        and RT.sp in ('",paste(arrSPs, collapse="','"),"') 
                        group by RT.opr_date, rt.opr_hour, interval, RT.sp, rt.value order by 1,2,3",sep="")
            			lmpsrawrtpre <- sqlQuery(conn, strQry);
                  lmpsrawrtpretest=TRUE
           			}
                 ## TEST: tapply(lmpsrawrtpre$RTSPP, list(lmpsrawrtpre$OPR_DATE, lmpsrawrtpre$HE, lmpsrawrtpre$SP), length); subset(lmpsrawrtpre, SP=="LZ_NORTH" & HE==1 )
          ## Get before LZ changes
              lmpsrawrtposttest=FALSE
              if(lmtgdate>=as.Date('2012-03-04'))
                {
              		strQry=paste("select RT.opr_date, rt.opr_hour HE, interval, RT.sp sp, avg(rt.value) rtspp, count(rt.value)
                        from ZE_DATA.ERCOT_NODAL_REAL_TIME_SPPS RT
                        where rt.opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and rt.opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD') 
                        and rt.opr_date>='04-mar-12'
                        and RT.sp in ('",paste(arrSPs, collapse="','"),"') and SP_TYPE!='LZ' 
                        group by RT.opr_date, rt.opr_hour, interval, RT.sp, rt.value order by 1,2,3",sep="")
            			lmpsrawrtpost <- sqlQuery(conn, strQry);
            			lmpsrawrtposttest=TRUE
          			}
                 ## TEST: tapply(lmpsrawrtpost$RTSPP, list(lmpsrawrtpost$OPR_DATE, lmpsrawrtpost$HE, lmpsrawrtpost$SP), length); subset(lmpsrawrtpost, SP=="LZ_NORTH" & HE==1 )
          ## Combine as applicable
              if(lmpsrawrtpretest & lmpsrawrtposttest) lmpsrawrt=rbind(lmpsrawrtpre, lmpsrawrtpost)
              if(lmpsrawrtpretest & !lmpsrawrtposttest) lmpsrawrt=lmpsrawrtpre
              if(!lmpsrawrtpretest & lmpsrawrtposttest) lmpsrawrt=lmpsrawrtpost
                 ## TEST: tapply(lmpsrawrt$RTSPP, list(lmpsrawrt$OPR_DATE, lmpsrawrt$HE, lmpsrawrt$SP), length); subset(lmpsrawrt, SP=="LZ_NORTH" & HE==1 )
      			## Remove duplicate data
      			   lmptest=lmpsrawrt; colnames(lmptest)[5]="value"; lmptest$variable="RTSPP"; lmptest=cast(lmptest, OPR_DATE+HE+INTERVAL+SP~variable, mean) 
          	## Ensure that data is between 23 and 25 hours
            	lmpstest=subset(lmpsrawrt, OPR_DATE<=as.POSIXct(Sys.Date()-1)		)
              if(nrow(lmpstest)>0) if(min(tapply(lmpstest$OPR_DATE, list(lmpstest$OPR_DATE, lmpstest$INTERVAL), length))<(length(arrSPs)*23) | max(tapply(lmpstest$OPR_DATE, list(lmpstest$OPR_DATE, lmpstest$INTERVAL), length))>(length(arrSPs)*25)) {rLog("Function PULL.SPPS is missing hours when pulling RT SPPs"); return("Function PULL.SPPS is missing hours when pulling RT SPPs")}
              lmpsrawrt=lmptest
           ## TEST: tapply(lmpsrawrt$RTSPP, list(lmpsrawrt$OPR_DATE, lmpsrawrt$HE, lmpsrawrt$SP), length)
                
      ## Combine DA and RT
            lmpsraw=merge(lmpsrawda[,1:4], lmpsrawrt[,1:5], all=TRUE)                  
            lmpstest=lmpsraw
 
      ## Check that the SPs are valid
                    #      if(nrow(lmpstest)>0)
                #          {
                #              ndays=min(max(lmpstest$OPR_DATE),as.POSIXct(Sys.Date()))-min(lmpstest$OPR_DATE)+1
                #              nexp=ndays*24*4
                #              arrTest=data.frame(tapply(lmpstest$SP, lmpstest$SP, length))
                #              colnames(arrTest)=c("ct"); arrTest$SP=rownames(arrTest)
                #              dfSPs=data.frame(SP=arrSPs)
                #              arrTest=merge(arrTest,dfSPs, all=TRUE)
                #              arrTest[is.na(arrTest)==TRUE]=0
                #              if(min(arrTest$ct)<nexp) {arrTest=subset(arrTest, ct<nexp); rLog(paste("Warning: Not returning all data for the following SPs.  (It may be due to the changing of some CC nodes starting 7/1/11.)  SPs: ",paste(unique(arrTest$SP),collapse=", "),sep=""))}
                #           }
       
      ## Modify data                                                                                                                                                                                                                              
            spps=lmpsraw
            spps$int_end=spps$OPR_DATE+(spps$HE-1)*60*60+spps$INTERVAL*15*60
            spps$pk="pk"
            sppsop=subset(spps, HE<7 | HE>22)
            sppsop$pk="op"
            spps=rbind( subset(spps, HE>6 & HE<23), sppsop)
            
            spps=spps[order(spps$int_end),]
    return(spps)
  }
 




#####################################################################
# PULL.MCPE
#   returns 15-minute zonal data
#   dlt=pull.mcpe(startdate=as.Date("2006-01-01"), lmtgdate=as.Date("2006-05-01"))
#         mcpe=pull.mcpe(startdate=as.Date(startdate), lmtgdate=as.Date(enddate))
#   
pull.mcpe = function (startdate, lmtgdate)
  {

      ## Check for zonal dates
        if(startdate>=as.Date("2010-12-01")) {return("There are no zonal MCPes after 12/1/2010!")}
        if(startdate>=lmtgdate) return("End date must be after start date!")
          
      ## Load MCPes
          load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/mcpesraw.RData")
          arrRes=mcperaw

      ## Get subset of data
          arrRes$OPR_DATE=as.POSIXct(arrRes$OPR_DATE)
          arrRes=subset(mcperaw, OPR_DATE>=as.POSIXct(startdate) & OPR_DATE>=as.POSIXct(lmtgdate))
          arrRes$dt_ie=as.POSIXct(arrRes$dt_ie)
          arrRes$dt_he=as.POSIXct(arrRes$dt_he)
      
      ## Correct for DSF
          arrtest=arrRes[is.na(arrRes$dt_he)==TRUE,]
          arrtest$dt_he=arrtest$OPR_DATE+arrtest$HE*60*60
          arrtest$dt_ie=arrtest$OPR_DATE+arrtest$interval*15*60+(arrtest$HE-1)*60*60
          arrRes=rbind(arrRes[is.na(arrRes$dt_he)==FALSE,],arrtest)


      ## Melt data 
          arrResm=melt(arrRes, id.vars=c('dt_ie','dt_he','OPR_DATE','HE','mon','year','day','interval'), measure.vars=c('HB_HOUSTON','HB_NORTH','HB_SOUTH','HB_WEST'))
          colnames(arrResm)=c('dt_ie','dt_he','OPR_DATE','HE','mon','year','day','interval','SP','RTSPP')

      ## Save results
          ## mcperaw=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/zonal_mcpe.csv",as.is=TRUE)
          ## save(mcperaw, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/mcpesraw.RData")
          
      ## Hourly data?
          ## arrResc=arrResm; colnames(arrResc)[10]="value"; arrResc$variable="RTSPP"
          ## arrResc=cast(arrResc, dt_he+OPR_DATE+HE+mon+year+day+SP~variable, mean); arrResc=as.data.frame(arrResc)

    return(arrResm)
  }



 
 

#####################################################################
# PULL.TSDB
#   arrRes=pull.tsdb(as.Date("2011-04-16"), as.Date("2011-04-17"), symbolst=c('MWDaily_16HrDly_ERCOTHous', 'MWDaily_16HrDly_ERCOTNort'), 2); str(arrRes)
#   hourconvention:  0=HE or Interval-ending (lmps, spps),    1=dt_start (like SCED interval);     2=daily (like Platts data)
#   startdate=as.Date("2006-07-01"); lmtgdate=as.Date("2011-08-13")
#   startdate=STARTDT; lmtgdate=LMTGDT; symbolst=c('prt_ercot_ld_dayahead'); hourconvention=0
#   HOW TO SUPPRESS WARNINGS??
#   startdate=STARTDT; lmtgdate=LMTGDT; symbolst=c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'); hourconvention=0
#   startdate=STARTDT-4; lmtgdate=LMTGDT-4; symbolst=c('ZM_ERCOT_THREE_TIER_FCSTS_POWER'); hourconvention=0
#   startdate=STARTDT-3; lmtgdate=Sys.Date()+2; symbolst=c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'); hourconvention=0


pull.tsdb = function (startdate, lmtgdate, symbolst, hourconvention)
  {
        
        rLog("Starting inside pull.tsdb function")

        ## Get dates
          startdate0=startdate
          lmtgdate0=lmtgdate
          startdate=round(as.POSIXct(startdate, origin="1970-01-01")-1*24*60*60,"days")
          lmtgdate=round(as.POSIXct(lmtgdate, origin="1970-01-01")+1*24*60*60,"days")

        ## Get data
          aggHourlyVector=rep(FALSE, length(symbolst)) 
#          rLog("\tTrying FTR:::FTR.load.tsdb.symbols function")
#            setLogLevel(3)
#          plattsdlyraw=FTR:::FTR.load.tsdb.symbols(symbolst, startdate0-1, lmtgdate0+1, aggHourly=aggHourlyVector)
#            setLogLevel(3); 
#          rLog("\tSucceeded in FTR:::FTR.load.tsdb.symbols function")
#          if(is.na(match("plattsdlyraw", ls()))==TRUE) 
#            {
#                  if(hourconvention==0)
#                    {  
#                      plattsdly=data.frame(dt_he=as.POSIXct(0, origin="1970-01-01"), variable=c(0), value=c(0))
#                      return(plattsdly[0,])
#                    }
#                  if(hourconvention==1)
#                    {  
#                      plattsdly=data.frame(dt_start=as.POSIXct(0, origin="1970-01-01"), variable=c(0), value=c(0), OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=c(0), fiveminbegin=c(0))
#                      return(plattsdly[0,])
#                    }
#                  if(hourconvention==2)
#                    {  
#                      plattsdly=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), variable=c(0), value=c(0))
#                      return(plattsdly[0,])
#                    }
#            }
#            rLog("\tTrying FTR:::FTR.load.tsdb.symbols function")
#          setLogLevel(3)
          plattsdlyraw=FTR:::FTR.load.tsdb.symbols(symbolst, as.Date(startdate), as.Date(lmtgdate), aggHourly=aggHourlyVector)
          setLogLevel(3)
            rLog("\tSucceeded in FTR:::FTR.load.tsdb.symbols function")
            if(ncol(plattsdlyraw)!=length(symbolst)) {rLog(paste("At least one of your Platts DA price TSDB symbol names is invalid.  ", sep="")); break;}
          plattsdly=as.data.frame(plattsdlyraw)
          if(rownames(plattsdly)[1]!="1") plattsdly$dt_unknown=rownames(plattsdly)
          if(rownames(plattsdly)[1]=="1") {d=index(plattsdlyraw); plattsdly$dt_unknown=d}
          plattsdly=melt(plattsdly,id.vars=c(ncol(plattsdly)), measure.vars=(1:(ncol(plattsdly)-1)))
          plattsdly$dt_unknown=as.POSIXct(plattsdly$dt_unknown, origin="1970-01-01")
        
        ## Segregate based on type
            rLog("\tSegregating based on type")
          if(hourconvention==0)
            {  
              plattsdly=subset(plattsdly, dt_unknown>(startdate+1*24*60*60) & dt_unknown<=(lmtgdate))
              colnames(plattsdly)[1]="dt_he"
            }
          if(hourconvention==1)
            {  
              plattsdly=subset(plattsdly, dt_unknown>=(startdate+1*24*60*60) & dt_unknown<(lmtgdate))
              colnames(plattsdly)[1]="dt_start"
            	plattsdly$OPR_DATE=as.POSIXct(paste(as.POSIXlt(plattsdly$dt_start)$year+1900,sprintf("%02.0f",as.POSIXlt(plattsdly$dt_start)$mon+1),
                sprintf("%02.0f", as.POSIXlt(plattsdly$dt_start)$mday),sep="-"))
            	plattsdly$HE=as.POSIXlt(plattsdly$dt_start)$hour+1
            	plattsdly$fiveminbegin=floor(as.POSIXlt(plattsdly$dt_start)$min/5)*5
            }
          if(hourconvention==2)
            {  
              plattsdly=subset(plattsdly, dt_unknown>=round(startdate+1*24*60*60, "days") & dt_unknown<=round(lmtgdate-1*24*60*60, "days"))
              colnames(plattsdly)[1]="OPR_DATE"
            }

            rLog("\tFinishing pull.tsdb")
        return(plattsdly)
  }





#####################################################################
# PULL.WIND.REG
#   arrRes=pull.wind.reg(startdate=as.Date("2011-04-10"), lmtgdate=as.Date("2011-04-16")); str(arrRes)
#   assumes that 96.5% of north_west wind gen is in the west and 3.5% is in the north
pull.wind.reg = function (startdate, lmtgdate, conn)
  {
  
      windtest1=0; windtest2=0
  
        ## Regional WIND actuals,   hours 1:23 on respective date and HE0 on next day's date  
           	## Wind Actuals - There is no data before 12/11 so return empty dataframe
             if (startdate<="2010-12-11")
              	{
                  windact0=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, dt_he=as.POSIXct(0, origin="1970-01-01"), region=0, value=0)
          			}
           	## Wind Actuals - Uses data up to 4/14   (this data ends 4/15 11AM)
             if ((lmtgdate<"2011-04-15" | startdate<"2011-04-15") & startdate>"2010-12-11")
              	{
             		   strQry=paste("select regwind.opr_date, regwind.opr_hour HE, regwind.region variable, avg(regwind.value) value
                      from ze_data.ercot_nodal_reg_wind_prod regwind where regwind.region in ('ACTUAL_NORTH','ACTUAL_WEST','ACTUAL_SOUTH')
                      and regwind.opr_date>=to_date('",min(startdate,as.Date("2011-04-14")),"', 'YYYY-MM-DD') 
                      and (regwind.opr_date<=to_date('",min(lmtgdate+1,as.Date("2011-04-15")),"', 'YYYY-MM-DD')) and value>0 
                      group by regwind.opr_date, regwind.opr_hour, regwind.region order by 1 desc, 2 desc",sep="")
                  			windactraw <- sqlQuery(conn, strQry);
                  			colnames(windactraw)[match("VALUE",colnames(windactraw))]="value"
             			  ## Format
 			                windact=cast(windactraw, OPR_DATE+HE~VARIABLE, mean)
                      windact$dt_he=windact$OPR_DATE + windact$HE*60*60
                    	windact$OPR_DATE =as.POSIXct(paste(as.POSIXlt(windact$dt_he-15)$year+1900,sprintf("%02.0f",as.POSIXlt(windact$dt_he-15)$mon+1),
                                      sprintf("%02.0f", as.POSIXlt(windact$dt_he-15)$mday),sep="-"))
                      windact[which(windact$HE==0), "HE"]=24
                      windact=subset(windact, OPR_DATE>=as.POSIXct(startdate) & OPR_DATE<=min(as.POSIXct(lmtgdate+1),as.POSIXct("2011-04-14")))
                      windact=as.data.frame(windact)
                      windact=melt(windact, id.vars=c("OPR_DATE","HE","dt_he"), measure.vars=c("ACTUAL_WEST","ACTUAL_NORTH","ACTUAL_SOUTH"))
                      windmap=cbind(c("ACTUAL_WEST","ACTUAL_NORTH","ACTUAL_SOUTH"),c("WEST","NORTH","SOUTH")); colnames(windmap)=c("variable", "region")
                      windact=merge(windact, windmap)
                      windact=windact[,c("OPR_DATE","HE", "dt_he", "region", "value")]
                     windact1=windact
                     windtest1=1
          			}
           	## Wind Actuals -  Uses data >=4/15  (this data starts 4/14 ) 
             if (lmtgdate>="2011-04-15" | startdate>="2011-04-15")
              	{
             		   strQry=paste("select regwind.opr_date, regwind.opr_hour HE, regwind.region variable, avg(regwind.value) value
                      from ze_data.ercot_nodal_reg_wind_prod regwind where regwind.region in ('ACTUAL_WEST_NORTH','ACTUAL_SOUTH_HOUSTON')
                      and regwind.opr_date>=to_date('",max(startdate,as.Date("2011-04-15")),"', 'YYYY-MM-DD') 
                      and (regwind.opr_date<=to_date('",max(lmtgdate+1,as.Date("2011-04-16")),"', 'YYYY-MM-DD')) and value>0 
                      group by regwind.opr_date, regwind.opr_hour, regwind.region order by 1 desc, 2 desc",sep="")
                  			windactraw2 <- sqlQuery(conn, strQry);
                  			colnames(windactraw2)[match("VALUE",colnames(windactraw2))]="value"
            			## Format data
                    windact=cast(windactraw2, OPR_DATE+HE~VARIABLE, mean)
                    windact$dt_he=as.POSIXct(windact$OPR_DATE + windact$HE*60*60, origin="1970-01-01")
                  	windact$OPR_DATE =as.POSIXct(paste(as.POSIXlt(windact$dt_he-15)$year+1900,sprintf("%02.0f",as.POSIXlt(windact$dt_he-15)$mon+1),
                                    sprintf("%02.0f", as.POSIXlt(windact$dt_he-15)$mday),sep="-"), origin="1970-01-01")
                    windact[which(windact$HE==0), "HE"]=24
                    windact=subset(windact, OPR_DATE>=max(as.POSIXct(startdate),as.POSIXct('2011-04-15')) & OPR_DATE<=as.POSIXct(lmtgdate+1))
                  ## convert to north and west
                    windact$WEST=windact$ACTUAL_WEST_NORTH*0.965
                    windact$NORTH=windact$ACTUAL_WEST_NORTH*0.035
                    windact$SOUTH=windact$ACTUAL_SOUTH_HOUSTON
                  ## Melt data
                    windact=as.data.frame(windact)
                    windact=melt(windact, id.vars=c("OPR_DATE","HE", "dt_he"), measure.vars=c("WEST","NORTH","SOUTH"))
                    colnames(windact)[4]="region"
                  windact2=windact
                     windtest2=1
         			} else print("no")
         ## Merge
             if(windtest1==1 & windtest2==1)  windact=rbind(windact1, windact2) 
             if(windtest1==1 & windtest2==0)  windact=windact1
             if(windtest1==0 & windtest2==1)  windact=windact2
             if(windtest1==0 & windtest2==0)  windact=windact0[0,]
             return(windact)
                                       
}













#####################################################################
# PULL.WIND.REG.FCST
#   assumes that 96.5% of north_west wind gen is in the west and 3.5% is in the north; 100% of south-houston is in the south
#   Won't return any data if you choose dates <=12/11/10 as have no data
#   arrRes=pull.wind.reg.fcst(startdate=as.Date("2011-04-10"), lmtgdate=as.Date("2011-04-16"), DAct=1, regional=1); str(arrRes)
#   DAct gives the number of days ahead that you want to see the forecast
#   regional=1 returns forecast by zone while regional=0 returns ERCOT-wide forecast
#   startdate=STARTDT;  lmtgdate=LMTGDT; DAct=1; regional=1
pull.wind.reg.fcst = function (startdate, lmtgdate, DAct=1, regional=1, conn)
  {

      windtest1=0; windtest2=0
      
      ## Test number of symbols
     		   strQry=paste("select unique(regwind.region) variable from ze_data.ercot_nodal_reg_wind_prod regwind where regwind.opr_date=to_date('",max(lmtgdate,as.Date('2011-04-19')),"', 'YYYY-MM-DD')",sep="")
   			   windtest <- sqlQuery(conn, strQry);
           if(nrow(windtest)!=6 & nrow(windtest)!=12 & nrow(windtest)!=9) {return("ze_data.ercot_nodal_reg_wind_prod regwind is returning more than the expected number of symbols (6 after 4/19/11)--check that the regions it the table."); break}

  
        ## Regional WIND actuals,   hours 1:23 on respective date and HE0 on next day's date  
           	## Wind Actuals - Uses data up to 4/14   (this data ends 4/15 11AM)
             if (startdate<="2010-12-11" & regional==1)
              	{
                  windact0=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, dt_he=as.POSIXct(0, origin="1970-01-01"), region=0, value=0, fcst_type=0)
          			}
             if (startdate<="2010-12-11" & regional==0)
              	{
                  windact0=data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, dt_he=as.POSIXct(0, origin="1970-01-01"), variable=0, value=0)
          			}
             if ((lmtgdate<"2011-04-15" | startdate<"2011-04-15") & startdate>"2010-12-11")
              	{
             		   rLog("Running query in pull.wind.reg.fcst")
                   strQry=paste("select regwind.opr_date, regwind.opr_hour HE, date_inserted publish_date, regwind.region variable, regwind.value
                      from ze_data.ercot_nodal_reg_wind_prod regwind where regwind.region in ('WGRPP_NORTH','WGRPP_WEST','WGRPP_SOUTH','STWPF_NORTH','STWPF_WEST','STWPF_SOUTH')
                      and regwind.opr_date>=to_date('",min(startdate,as.Date("2011-04-14")),"', 'YYYY-MM-DD') 
                      and (regwind.opr_date<=to_date('",min(lmtgdate+1,as.Date("2011-04-15")),"', 'YYYY-MM-DD'))  
                      order by 1 desc, 2 desc",sep="")
                  			windactraw <- sqlQuery(channel_zema_prod, strQry);
                  			colnames(windactraw)[match("VALUE",colnames(windactraw))]="value"
             		   rLog(strQry)
             			  ## Format
 			                windact=cast(windactraw, OPR_DATE+HE+PUBLISH_DATE~VARIABLE, mean)
                      windact$dt_he=windact$OPR_DATE + windact$HE*60*60
                    	windact$OPR_DATE =as.POSIXct(paste(as.POSIXlt(windact$dt_he-15)$year+1900,sprintf("%02.0f",as.POSIXlt(windact$dt_he-15)$mon+1),
                                      sprintf("%02.0f", as.POSIXlt(windact$dt_he-15)$mday),sep="-"))
                      windact[which(windact$HE==0), "HE"]=24
                      windact=subset(windact, OPR_DATE>=as.POSIXct(startdate) & OPR_DATE<=min(as.POSIXct(lmtgdate+1),as.POSIXct("2011-04-14")))
                      windact=as.data.frame(windact)
                    ## Get the x DA forecast
                      windact$dtdesired=as.POSIXct(paste(as.POSIXlt(windact$OPR_DATE)$year+1900,"-",sprintf("%02.0f",as.POSIXlt(windact$OPR_DATE)$mon+1),"-",
                                      sprintf("%02.0f", as.POSIXlt(windact$OPR_DATE)$mday)," 08:30",sep=""))-DAct*24*60*60
                      windact$value=abs(windact$dtdesired-windact$PUBLISH_DATE)
                      windactmap=windact; windactmap$variable="value"
                      windactmap=as.data.frame(cast(windactmap, dt_he~variable, min))
                      windact=as.data.frame(windact)
                      windact=merge(windact, windactmap)
                    ## Melt wind totals
                      windtot=windact; 
                      windtot$STWPF=windtot$STWPF_WEST+windtot$STWPF_SOUTH+windtot$STWPF_NORTH; 
                      windtot$WGRPP=windtot$WGRPP_WEST+windtot$WGRPP_SOUTH+windtot$STWPF_NORTH; 
                      windtot=melt(windtot, id.vars=c("OPR_DATE","HE","dt_he"), measure.vars=c("STWPF","WGRPP"))
                    ## Melt regional wind forecasts
                      windact=melt(windact, id.vars=c("OPR_DATE","HE","dt_he"), measure.vars=c('WGRPP_NORTH','WGRPP_WEST','WGRPP_SOUTH','STWPF_NORTH','STWPF_WEST','STWPF_SOUTH'))
                      windmap=cbind(c('WGRPP_NORTH','WGRPP_WEST','WGRPP_SOUTH','STWPF_NORTH','STWPF_WEST','STWPF_SOUTH'),c("NORTH","WEST","SOUTH","NORTH","WEST","SOUTH"), c('WGRPP','WGRPP','WGRPP','STWPF','STWPF','STWPF')); colnames(windmap)=c("variable", "region","fcst_type")
                      windact=merge(windact, windmap)
                      windact=windact[,c("OPR_DATE","HE","dt_he", "region", "value", "fcst_type")]
                    ## Take correct data
                      if(regional==1) windact1=windact
                      if(regional==0) windact1=windtot
                     windtest1=1
          			}
           	## Wind Forecasts -  Uses data >=4/15  (this data starts 4/14 ) 
             if (lmtgdate>="2011-04-15" | startdate>="2011-04-15")
              	{
             		   strQry=paste("select regwind.opr_date, regwind.opr_hour HE, date_inserted publish_date, regwind.region variable, regwind.value
                      from ze_data.ercot_nodal_reg_wind_prod regwind where regwind.region in ('WGRPP_WEST_NORTH','WGRPP_SOUTH_HOUSTON','STWPF_WEST_NORTH','STWPF_SOUTH_HOUSTON')
                      and regwind.opr_date>=to_date('",max(startdate,as.Date("2011-04-15")),"', 'YYYY-MM-DD') 
                      and (regwind.opr_date<=to_date('",max(lmtgdate+1,as.Date("2011-04-16")),"', 'YYYY-MM-DD'))",sep="")
                  			windactraw2 <- sqlQuery(channel_zema_prod, strQry);
                  			colnames(windactraw2)[match("VALUE",colnames(windactraw2))]="value"
             			  ## Format
 			                windact=cast(windactraw2, OPR_DATE+HE+PUBLISH_DATE~VARIABLE, mean)
                      windact$dt_he=windact$OPR_DATE + windact$HE*60*60
                    	windact$OPR_DATE =as.POSIXct(paste(as.POSIXlt(windact$dt_he-15)$year+1900,sprintf("%02.0f",as.POSIXlt(windact$dt_he-15)$mon+1),
                                      sprintf("%02.0f", as.POSIXlt(windact$dt_he-15)$mday),sep="-"))
                      windact[which(windact$HE==0), "HE"]=24
                      windact=subset(windact, OPR_DATE>=max(as.POSIXct(startdate),as.POSIXct('2011-04-15')) & OPR_DATE<=as.POSIXct(lmtgdate+1))
                      windact=as.data.frame(windact)
                    ## Get the x DA forecast
                      windact$dtdesired=as.POSIXct(paste(as.POSIXlt(windact$OPR_DATE)$year+1900,"-",sprintf("%02.0f",as.POSIXlt(windact$OPR_DATE)$mon+1),"-",
                                      sprintf("%02.0f", as.POSIXlt(windact$OPR_DATE)$mday)," 08:30",sep=""))-DAct*24*60*60
                      windact$value=abs(windact$dtdesired-windact$PUBLISH_DATE)
                      windactmap=windact; windactmap$variable="value"
                      windactmap=as.data.frame(cast(windactmap, dt_he~variable, min))
                      windact=as.data.frame(windact)
                      windact=merge(windact, windactmap)
                    ## Melt wind totals
                      windtot=windact; 
                      windtot$STWPF=windtot$STWPF_WEST_NORTH+windtot$STWPF_SOUTH_HOUSTON
                      windtot$WGRPP=windtot$WGRPP_WEST_NORTH+windtot$WGRPP_SOUTH_HOUSTON; 
                      windtot=melt(windtot, id.vars=c("OPR_DATE","HE","dt_he"), measure.vars=c("STWPF","WGRPP"))
                    ## Melt regional wind forecasts
                      windact$WGRPP_WEST=windact$WGRPP_WEST_NORTH*0.965
                      windact$WGRPP_NORTH=windact$WGRPP_WEST_NORTH*0.035
                      windact$WGRPP_SOUTH=windact$WGRPP_SOUTH_HOUSTON
                      windact$STWPF_WEST=windact$STWPF_WEST_NORTH*0.965
                      windact$STWPF_NORTH=windact$STWPF_WEST_NORTH*0.035
                      windact$STWPF_SOUTH=windact$STWPF_SOUTH_HOUSTON
                      windact=melt(windact, id.vars=c("OPR_DATE","HE","dt_he"), measure.vars=c('WGRPP_NORTH','WGRPP_WEST','WGRPP_SOUTH','STWPF_NORTH','STWPF_WEST','STWPF_SOUTH'))
                      windmap=cbind(c('WGRPP_NORTH','WGRPP_WEST','WGRPP_SOUTH','STWPF_NORTH','STWPF_WEST','STWPF_SOUTH'),c("NORTH","WEST","SOUTH","NORTH","WEST","SOUTH"), c('WGRPP','WGRPP','WGRPP','STWPF','STWPF','STWPF')); colnames(windmap)=c("variable", "region","fcst_type")
                      windact=merge(windact, windmap)
                      windact=windact[,c("OPR_DATE","HE","dt_he", "region", "value", "fcst_type")]
                    ## Take correct data
                      if(regional==1) windact2=windact
                      if(regional==0) windact2=windtot
                     windtest2=1
          			}

         ## Merge
             if(windtest1==1 & windtest2==1)  windact=rbind(windact1, windact2) 
             if(windtest1==1 & windtest2==0)  windact=windact1
             if(windtest1==0 & windtest2==1)  windact=windact2
             if(windtest1==0 & windtest2==0)  windact=windact0[0,]
             return(windact)
 
 }





#####################################################################
# PULL.LOAD.REG
#   arrRes3=pull.load.reg(startdate, lmtgdate); str(arrRes3)
#   startdate=as.Date("2011-04-14"); lmtgdate=as.Date("2011-04-16")
#   startdate=as.Date("2011-08-01"); lmtgdate=as.Date("2011-08-02")
#   startdate=STARTDT; lmtgdate=LMTGDT
#
pull.load.reg = function (startdate, lmtgdate)
  {

        ## Get dates
          startdate=round(as.POSIXct(startdate, origin="1970-01-01"),"days")
          lmtgdate=round(as.POSIXct(lmtgdate, origin="1970-01-01"),"days")

        ## Get data
            loadz=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/uploadzonalload.csv", as.is=TRUE)
            if(is.na(match("loadz", ls()))) {return("Cannot find load zone loads file at S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/uploadzonalload.csv"); break;}
            loadz=melt(loadz, id.vars=c(1:7), measure.vars=c(8:ncol(loadz)))
            colnames(loadz)[match("variable",colnames(loadz))]="zone"      
            colnames(loadz)[match("value",colnames(loadz))]="load_reg_act" 
            colnames(loadz)[match("ercot",colnames(loadz))]="load_reg_act_sum" 
            loadz$OPR_DATE=as.POSIXct(paste(loadz$year,sprintf("%02.0f", loadz$month), sprintf("%02.0f", loadz$day), sep="-"), origin="1970-01-01")

        ## Get requested data
            loadz=subset(loadz, OPR_DATE>=startdate & OPR_DATE<=lmtgdate)
            return(loadz)
}

              









#####################################################################
# PULL.LOAD.REG.FCST
#   DAct gives the number of days ahead that you want to see the forecast
#   Won't return any data if you choose dates <=12/20/10 as have no data
#   arrRes=pull.load.reg.fcst(startdate=as.Date("2011-04-10"), lmtgdate=as.Date("2011-04-11"), DAct=1, agg=2); str(arrRes)
#   agg is aggregation.  agg=0 is load zone, agg=1 is ercot total, agg=2 is weather zone
#   startdate=as.Date("2011-04-01"); lmtgdate=as.Date("2011-04-16")
#   startdate=as.Date("2010-12-01"); lmtgdate=as.Date("2010-12-05")
#   startdate=STARTDT; lmtgdate=LMTGDT; DAct=as.numeric(STARTDT-Sys.Date()); agg=0; conn
#

pull.load.reg.fcst = function (startdate, lmtgdate, DAct=1, agg=0, conn)
  {
     
      ## Break if dates are before we have data
      if (as.Date(startdate)<=as.Date("2010-12-20"))
        {
           if(agg==0) {return(data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, ZONE=0, loadfc=0)[0,])}
           if(agg==1) {return(data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, loadfc=0)[0,])}
           if(agg==2) {return(data.frame(OPR_DATE=as.POSIXct(0, origin="1970-01-01"), HE=0, ZONE=0, loadfc=0)[0,])}
        }


      if (as.Date(startdate)>as.Date("2010-12-20"))
        {
          ## Choose source table
            if (agg==0 | agg==1) sourcetable="ze_data.ercot_nodal_fc_load_load"
            ## if (agg==1) sourcetable="ze_data.ercot_nodal_fc_load_system"  This table doesn't have publish date/time
            if (agg==2) sourcetable="ze_data.ercot_nodal_fc_load_weather"
    
            ## LOAD DA forecasts     
              ## Query databse            
                strQry=paste(
                            "select publish_date, publish_time, OPR_DATE, OPR_HOUR, zone, value loadfc
                              from ",sourcetable,"  
                              where opr_hour>0 AND opr_date-PUBLISH_DATE=",DAct,"
                              and opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD')",sep="")
                 ldrawda1 <- sqlQuery(conn, strQry);
                strQry=paste(
                            "select publish_date, publish_time, OPR_DATE, OPR_HOUR, zone, value loadfc
                              from ",sourcetable,"
                              where opr_hour=0 AND opr_date-PUBLISH_DATE=",DAct+1,"
                              and opr_date>=to_date('",startdate+1,"', 'YYYY-MM-DD') and opr_date<=to_date('",lmtgdate+1,"', 'YYYY-MM-DD')",sep="")
                 ldrawda2 <- sqlQuery(conn, strQry);
    
             ## Combine data
                  lda=rbind(ldrawda1, ldrawda2)    
                  if(nrow(lda)==0) {rLog("pull.load.reg.fcst returning no data."); return(rLog("pull.load.reg.fcst returning no data."))}
                  colnames(lda)[c(3:5)]=c("OPR_DATE","HE","ZONE")
                  lda[lda$HE==0,"OPR_DATE"]=lda[lda$HE==0,"OPR_DATE"]-1*24*60*60
                  lda[lda$HE==0,"HE"]=24
    
             ## Get the forecast available that is closest to 830AM
                  lda$pubtime=apply(lda, 1, function(x)  { unlist(strsplit(x[2],":"))[1]})
                  lda$pubtime=as.numeric(lda$pubtime)
                  lda$value=abs(lda$pubtime-8)
                  lda$variable="value"
                  minpubdiff=cast(lda, OPR_DATE+HE+ZONE~variable, min)
                  ldamin=merge(lda, minpubdiff)
                  lda=ldamin[,c('OPR_DATE','HE','ZONE','LOADFC')]
                  colnames(lda)[match("LOADFC",colnames(lda))]="value"

             ## Check data
                if (length(unique(lda$OPR_DATE))<=(min(Sys.Date(),lmtgdate)-max(startdate, as.Date('2010-12-20')))) {return("The load forecast did not pull data for all day's chosen.  Is it because the publish date of 8:30 is not valid for all days?"); break;}
             
             ## Return data
                if(agg==1) 
                  {
                  lda=cast(lda, OPR_DATE+HE~ZONE, mean)
                  lda$loadfc=lda$Houston + lda$North + lda$South + lda$West
                  lda=lda[,c('OPR_DATE','HE','loadfc')]
                  }   else  colnames(lda)[match("value",colnames(lda))]="loadfc"       
                return(lda)
        }

 }







#####################################################################
# PULL.LOAD
#   startdate=as.Date("2011-04-01"); lmtgdate=as.Date("2011-04-16")
#   
pull.load = function (startdate, lmtgdate, conn)
{

      ## NODAL SYS LOAD hour/min is the start of the interval
            enddate=min(as.Date('2011-02-11'), lmtgdate+1)
         		strQry=paste(
                      "select opr_date od,opr_hour HB,OPr_minute MB,value ld from ze_data.ERCOT_NODAL_SYSTEM_LOAD ld
                        where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') AND opr_date<=to_date('",enddate,"', 'YYYY-MM-DD');",sep="")
             		ldraw1 <- sqlQuery(conn, strQry);
         		ld1test=0
         		if(nrow(ldraw1)>0)
         		{
            	ldraw1$dt_he=ldraw1$OD + ldraw1$HB*60*60 + ldraw1$MB*60 + 15*60
            	ldraw1=ldraw1[,c(4,5)]
            	ld1test=1
         		}
    
      ## SYS-WIDE DEMAND hour/min is the end of the interval
            strtdate=max(as.Date('2011-01-11'), startdate)
           		strQry=paste(
                      "select opr_date OD,opr_hour HB,OPr_minute ME,demand ld from ze_data.ercot_nodal_sys_wd_dmnd ld
                        where opr_date>=to_date('",strtdate,"', 'YYYY-MM-DD') AND opr_date<=to_date('",lmtgdate+1,"', 'YYYY-MM-DD');",sep="")
             		ldraw2 <- sqlQuery(conn, strQry);
         		ld2test=0
         		if(nrow(ldraw2)>0)
         		{
            	ldraw2$dt_he=ldraw2$OD + ldraw2$HB*60*60 + ldraw2$ME*60
            	ldraw2=ldraw2[,c(4,5)]      	
            	ld2test=1
         		}


      ##  CONVERT FORMATS
            if(ld1test==ld2test & ld1test==1) {ld=rbind(ldraw1, ldraw2)}
            if(ld1test!=ld2test & ld1test==1) {ld=ldraw1}
            if(ld1test!=ld2test & ld1test==0) {ld=ldraw2}
            if(ld1test==ld2test & ld1test==0) {return("No data returned for actual system load."); break}
            colnames(ld)[1]="value"; ld$variable='ld'; ld=cast(ld, dt_he~variable, mean)
          	ld$OPR_DATE =as.POSIXct(paste(as.POSIXlt(ld$dt_he-15*60)$year+1900,sprintf("%02.0f",as.POSIXlt(ld$dt_he-15*60)$mon+1),
                              sprintf("%02.0f", as.POSIXlt(ld$dt_he-15*60)$mday),sep="-"))
          	ld$IE=as.POSIXlt(ld$dt_he)$min       
            ld$HE=as.POSIXlt(ld$dt_he-15*60)$hour+1   
            intmap=cbind(c(15,30,45,0),c(1,2,3,4)); colnames(intmap)=c("IE","int")                     
            ld=merge(ld, intmap)
            ld=ld[,c("OPR_DATE","HE", "int", "ld")]

      ## Subset data
          startdate=round(as.POSIXct(startdate, origin="1970-01-01"),"days")
          lmtgdate=round(as.POSIXct(lmtgdate, origin="1970-01-01"),"days")
          ld=subset(ld, ld<100000 & OPR_DATE>=startdate & OPR_DATE<=lmtgdate)
      
      return(ld)

}








#####################################################################
# PULL.60D.SCED.DATA
#   startdate=Sys.Date()-101; lmtgdate=Sys.Date()-101
#   
pull.60d.sced.data = function (startdate, lmtgdate, conn)
{

    ## Get units with an OUT status
      strQry=paste("select *
                    from ze_data.ercot_nodal_60dscedgrd
                    where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') AND opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD')", sep="")
  		scedraw <- sqlQuery(conn, strQry);

    ## Return empty if cannot find data
      if(nrow(scedraw)==0) {
                          rLog(paste("Did not find data for start date of ",startdate," and end date of ",lmtgdate,sep="")); 
                          return(paste("Did not find data for start date of ",startdate," and end date of ",lmtgdate,sep=""))
                          }

    ## Correct opr_minute and add hour convention
      scedraw$OPR_MINUTE=round(scedraw$OPR_MINUTE/15,0)*15
      scedraw$ie = scedraw$OPR_DATE + scedraw$OPR_HOUR*60*60 + scedraw$OPR_MINUTE*60 + 15*60

    return(scedraw)

}






#####################################################################
# PULL.60D.NME.DATA
#   startdate=Sys.Date()-101; lmtgdate=Sys.Date()-101
#   
pull.60d.mne.data = function (startdate, lmtgdate, conn)
{

    ## Get units with an OUT status
      strQry=paste("select *
                    from ze_data.ercot_nodal_60dscedsmnegr
                    where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') AND opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD')", sep="")
  		scedraw <- sqlQuery(conn, strQry);

    ## Return empty if cannot find data
      if(nrow(scedraw)==0) {
                          rLog(paste("Did not find data for start date of ",startdate," and end date of ",lmtgdate,sep="")); 
                          return(paste("Did not find data for start date of ",startdate," and end date of ",lmtgdate,sep=""))
                          }

    ## Correct opr_minute and add hour convention
      scedraw$OPR_MINUTE=round(scedraw$OPR_MINUTE/15,0)*15
      scedraw$ie = scedraw$OPR_DATE + scedraw$OPR_HOUR*60*60 + scedraw$OPR_MINUTE*60 + 15*60

    return(scedraw)

}







#####################################################################
# GET.OUR.ZONAL.LOADS
#   startdate=as.Date('2010-11-01'); lmtgdate=as.Date('2010-11-10')
#   startdate=as.POSIXct('2010-11-01'); lmtgdate=as.POSIXct('2010-11-10')
#   
get.our.zonal.loads = function (startdate, lmtgdate)
{

    ## Error out for nodal data
      startdatetest=as.POSIXct(startdate)
      if(startdatetest>=as.POSIXct("2010-12-01")) 
        { rLog("Cannot pull data after nodal inception (12/1/10"); return(FALSE)
        }

    ## Get units with an OUT status
        zonalc_old=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_our_loads_zonal.csv",as.is=TRUE); 
        zonalc_old$day=as.POSIXct(zonalc_old$day); zonalc_old$hour=as.numeric(zonalc_old$hour); zonalc_old=zonalc_old[,2:(ncol(zonalc_old))]
    
    ## Subset of data
        zonalc_old=subset(zonalc_old, day>=startdate & day<=lmtgdate)

    return(zonalc_old)

}









#####################################################################
# GET.OUR.ZONAL.LOADS.NMKT.MORE.RECENT
#   startdate=as.Date('2010-12-11'); lmtgdate=as.Date('2010-11-10')
#   startdate=as.POSIXct('2010-12-11'); lmtgdate=as.POSIXct('2010-11-10')
#   DSN_PATH = paste("FileDSN=",  "S:/All/Risk/Software/R/prod/Utilities/DSN/nMarket_tx_zl.dsn;",  "UID=dat_user;", "PWD=datusertx;", sep=""); channel_nmkt_zl = odbcDriverConnect(DSN_PATH)
#   conn_nmkt_zl=channel_nmkt_zl
#   
get.our.zonal.loads.nmkt.more.recent = function (startdate, lmtgdate, conn_nmkt_zl)
{

    ## Error out for nodal data
      startdatetest=as.POSIXct(startdate)
      if(startdatetest>=as.POSIXct("2010-12-01")) 
        { rLog("Cannot pull data after nodal inception (12/1/10"); return("Cannot pull data after nodal inception (12/1/10")
        }

    ## Error out for data before 5/1/09
      lmtgdatetest=as.POSIXct(lmtgdate)
      if(lmtgdatetest>=as.POSIXct('2009-05-01')) 
        { rLog("Cannot pull data before QSE formation (5/1/09)--use get.our.zonal.loads.nmkt.earlier function"); return("Cannot pull data before QSE formation (5/1/09)--use get.our.zonal.loads.nmkt.earlier function")
        }

        
    ## Pull data from N-Market zonal
        		strQry=paste("  select m.ptcpt_cd, nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', m.end_dt_gmt, 'CPT', 1) HE, xp.xp_nme, m.quantity
                        from all_mgr.alloc_meter_data m, nmarket.nm_xp xp
                            where m.xp_id=xp.xp_id and 
                            (  (meter_data_source_cd = 'LEXTR'    and   ptcpt_cd in ('CNE', 'RAYBRN'))
                            or (meter_data_source_cd = '3PARTY'   and   ptcpt_cd in ('STRMA', 'STRM')) 
                            or (                                        ptcpt_cd = 'EXPLR'           )   )
                            and m.end_dt_gmt >= to_date('",startdate-1,"', 'YYYY-MM-DD')
                            and m.end_dt_gmt < to_date('",lmtgdate+1,"', 'YYYY-MM-DD') 
                           order by 4 desc;",sep="")
        			zonalraw <- sqlQuery(conn_nmkt_zl, strQry);
    

    return(zonalraw)

}



#####################################################################
# GET.OUR.ZONAL.LOADS.NMKT.EARLIER
#   Gets load data for Rayburn and Stream before the new QSE inception
#   startdate=as.Date('2010-12-11'); lmtgdate=as.Date('2010-11-10')
#   startdate=as.POSIXct('2010-12-11'); lmtgdate=as.POSIXct('2010-11-10')
#   DSN_PATH = paste("FileDSN=",  "S:/All/Risk/Software/R/prod/Utilities/DSN/nMarket_tx_zl.dsn;",  "UID=dat_user;", "PWD=datusertx;", sep=""); channel_nmkt_zl = odbcDriverConnect(DSN_PATH)
#   conn_nmkt_zl=channel_nmkt_zl
#   
get.our.zonal.loads.nmkt.earlier = function (startdate, lmtgdate, conn_nmkt_zl)
{

    ## Error out for data before 5/1/09
      startdatetest=as.POSIXct(startdate)
      if(startdatetest>=as.POSIXct('2009-05-01')) 
        { rLog("Cannot pull data after QSE formation (5/1/09)--use get.our.zonal.loads.nmkt.more.recent function"); return("Cannot pull data before QSE formation (5/1/09)--use get.our.zonal.loads.nmkt.more.recent function")
        }

        
    ## Pull data from N-Market zonal
          		strQry=paste("  select m.ptcpt_cd, nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', m.end_dt_gmt, 'CPT', 1) HE, xp.xp_nme, m.quantity
                          from all_mgr.alloc_meter_data m, nmarket.nm_xp xp
                              where m.xp_id=xp.xp_id and 
                              (  (meter_data_source_cd = 'LEXTR'    and   ptcpt_cd in ('RAYBRN'))
                              or (meter_data_source_cd = '3PARTY'   and   ptcpt_cd in ('STRMA', 'STRM')) 
                              or (                                        ptcpt_cd = 'EXPLR'           )   )
                              and m.end_dt_gmt >= to_date('",startdate-1,"', 'YYYY-MM-DD')
                              and m.end_dt_gmt < to_date('",lmtgdate+1,"', 'YYYY-MM-DD') 
                             order by 4 desc;",sep="")
          			zonalraw <- sqlQuery(conn_nmkt_zl, strQry);

    return(zonalraw)

}










#####################################################################
# ALERT.SPS
#   Sends me an email when my plant_data file is missing locations
#   plantdata=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv", as.is=TRUE)
# 
alert.SPs = function (arrSPs, conn)
  {

      ## Check that the SPs are valid
        		strQry=paste("select resource_name, median(hsl)
                          from ze_data.ercot_nodal_60dscedgrd sced where resource_name in ('",paste(arrSPs,collapse="','"),"') 
                          and opr_date>=to_date('",Sys.Date()-200,"', 'YYYY-MM-DD')
                          group by resource_name",sep="")
        			gendata <- sqlQuery(conn, strQry);
        			if (nrow(gendata)==0) 
                {  ## Send out results
                        setLogLevel(3)    
                        sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                            subject=paste("Need to update upload_plant_datafinal.csv",sep=""),
                            body=paste("Need to update \nS:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv\n with\n\n",
                            paste(arrSPs, collapse=", "), sep=""))
                        setLogLevel(3)
                }
        			if (nrow(gendata)!=0) 
                {  
                  gendata$email = apply(gendata, 1,  function(x)  {paste(x[1], ", ", x[2], "\n")}  )         
                  ## Send out results
                        setLogLevel(3)    
                        sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                            subject=paste("Need to update upload_plant_datafinal.csv",sep=""),
                            body=paste("Need to update \nS:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv\n with\n\n",
                            paste(arrSPs, collapse=", "), "\n\n SP, median(HSL)\n", paste(gendata$email, collapse=""), sep=""))
                        setLogLevel(3)
                }


  }
 












#####################################################################
# GET.NODAL.ZONAL.DATA
#   Combines hourly LMPs, wind, load, and load forecast from nodal and zonal
# startyear=Startyear; endyear=Endyear
#

get.nodal.zonal.data = function(startyear=2006, endyear=2011)
    {

      rLog("Starting pulling data")
      if(endyear<startyear) return("Must choose an endyear greater than or equal to the start year!")

        ##  GET NODAL DATA
            nodal_years=seq(2010,as.POSIXlt(Sys.Date())$year+1900)
            if(endyear>=2010) for (i in 1:length(nodal_years))
              {
                file1=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_", nodal_years[i], ".RData", sep="")
                load(file=file1);
                if(i==1) rthourlyc=rthourly else rthourlyc=rbind(rthourlyc, rthourly)
              }
              ## Add buckets
                 load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData");
                 rthourlyc$HE=as.integer(rthourlyc$HE)
                 rthourlyb=merge(rthourlyc, bkts[,c("OPR_DATE","HE","bucket","dayname")])
              rthourlyb$month=as.POSIXlt(rthourlyb$OPR_DATE)$mon+1
              rthourlyb=rthourlyb[,c("OPR_DATE","HE","SP","DASPP","RTSPP","dt_he","yr","moyr","month","bucket","dayname","platts_hh","platts_hsc","loadrt","windrt","load_sys_fcst","plattspwr")]
              

        ##  GET ZONAL DATA
         load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/mcpe.RData");
         colnames(mcpe)[match("plattshenry",colnames(mcpe))]="platts_hh"; colnames(mcpe)[match("plattsgas",colnames(mcpe))]="platts_hsc";
              mcpe=mcpe[,c("OPR_DATE","HE","SP","DASPP","RTSPP","dt_he","yr","moyr","month","bucket","dayname","platts_hh","platts_hsc","loadrt","windrt","load_sys_fcst","plattspwr")]

        ##  MERGE NODAL AND ZONAL
          historical=rbind(mcpe, rthourlyb)
          
  return(historical)


    }









#####################################################################
# GET.GEN.STACK
#   Uses the SCED EOCs to determine the heat rates and compiles into a gen stack
# startyear=Startyear; endyear=Endyear
#

get.gen.stack = function(startdate=Sys.Date(), conn)
    {


      ## Check that the SPs are valid
        		strQry=paste("select resource_type, OPR_DATE, OPR_HOUR, OPR_MINUTE, RESOURCE_NAME, HSL, LSL, HASL, LASL, TELEMETERED_RESOURCE_STATUS, QSE_SUBMITTED_CURVE_MW1, sced.qse_submitted_curve_price1, QSE_SUBMITTED_CURVE_MW2, sced.qse_submitted_curve_price2, QSE_SUBMITTED_CURVE_MW3, sced.qse_submitted_curve_price3, QSE_SUBMITTED_CURVE_MW4, sced.qse_submitted_curve_price4, QSE_SUBMITTED_CURVE_MW5, sced.qse_submitted_curve_price5
                          from ze_data.ercot_nodal_60dscedgrd sced where  
                          opr_date>=to_date('",startdate-120,"', 'YYYY-MM-DD') and opr_hour=17",sep="")
       			gendata <- sqlQuery(conn, strQry);
       			str(gendata)

      ## Merge with gas data to get heat rates
             gas=pull.tsdb(startdate-120, startdate-60, c('gasdailymean_ship'), hourconvention=2)
             colnames(gas)[3]="plattsgas"
             gen=merge(gendata, gas[,c("OPR_DATE","plattsgas")])
             

      
      ## Merge with plant stats
            plantdata=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv", as.is=TRUE)
            ## Combine ercot_unit_code
              plantdatauc=plantdata[,c("ercot_unit_code", "ercot_unit_name", "fuel","mw","my_type")]
              colnames(plantdatauc)[1]="RESOURCE_NAME"
              genuc=merge(gen, plantdatauc)
              genuc$test=1
              gentest=merge(genuc, gen, all=TRUE)
              gentest=gentest[is.na(gentest$test)==TRUE,]
              tapply(gentest$HSL, gentest$RESOURCE_NAME, max)
             #write.csv(gen, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")

              
              
              
            
            #gen=merge(gen, )

       			

  


         
  return(gen)


    }









#####################################################################
# GET.GEN.HSLS
#   Uses the 60d SCED EOCs to determine the HSLs and LSLs - throws out zeroes and gets 5%ile of LSL and 95%ile of HSL 
#   get.gen.hsls (startdate=Sys.Date()-72, conn=channel_zema_prod)
# 

get.gen.hsls = function(startdate=Sys.Date()-72, enddate=Sys.Date()-60, conn)
    {
      
      ## Check that the SPs are valid
        		strQry=paste("select opr_date, opr_hour, resource_name, resource_type, lsl, hsl
                          from ze_data.ercot_nodal_60dscedgrd where 
                          opr_date>=to_date('",startdate,"', 'YYYY-MM-DD') and opr_date<=to_date('",enddate,"', 'YYYY-MM-DD') 
                          and hsl>0 ",sep="")
       			gendata <- sqlQuery(conn, strQry);

 			## Get unique names
 			  arrNames = data.frame(NAME=unique(gendata$RESOURCE_NAME), LSL=0, HSL=0)
 			  for (i in 1:nrow(arrNames))  
 			    {
            gendatai = subset(gendata, RESOURCE_NAME==arrNames$NAME[i])
            ## Get HSL
              HSL=tapply(gendatai$HSL, gendatai$HSL, length)
              if(length(HSL)==1) 
                {arrNames$HSL[i]=gendatai$HSL[1]} else 
                {HSL=sort(HSL, decreasing=TRUE); arrNames$HSL[i] = as.numeric(rownames(HSL)[1])}            
            ## Get LSL
              LSL=tapply(gendatai$LSL, gendatai$LSL, length)
              if(length(LSL)==1) 
                {arrNames$LSL[i]=gendatai$LSL[1]} else 
                {LSL=sort(LSL, decreasing=TRUE); arrNames$LSL[i] = as.numeric(rownames(LSL)[1])}            
           }
      
      colnames(arrNames)[1]="ercot_unit_code"
      
      ## Check for the expired names
        #maxdate = as.data.frame(tapply(gendata$OPR_DATE, gendata$RESOURCE_NAME, max))
        #mindate = as.data.frame(tapply(gendata$OPR_DATE, gendata$RESOURCE_NAME, min))
        #maxdate$max_date=as.POSIXct(maxdate[,1], origin="1970-01-01")
        #mindate$min_date=as.POSIXct(mindate[,1], origin="1970-01-01")
        #maxdate$RESOURCE_NAME=rownames(maxdate)
        #mindate$RESOURCE_NAME=rownames(mindate)
        #maxdate=maxdate[,2:3]
        #mindate=mindate[,2:3]
        #arrComb=merge(maxdate, arrNames, all=TRUE); arrComb=merge(mindate, arrComb, all=TRUE)
          ## write.csv(arrComb, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/hsls_temp.csv")

      return(arrNames)


    }











#####################################################################
# PULL.THREE.TIER
#   Do DAct=0 for the latest forecast
#   startdate=STARTDT+1;  lmtgdate=startDate+1; # DAct=1; DAtime="16:30"
pull.three.tier = function (startdate, lmtgdate, DAct, DAtime="16:30", connn)
  {

          if(lmtgdate<=as.Date('2012-06-14'))  {rLog("  Cannot use pull.three.tier before 6/14/12 as the new symbols weren't created."); return()}


        ## Use new three tier symbols for data after 6/14 -  Uses data >=6/14/12  (as of 6/20/12, waiting for a backfill)
          if(lmtgdate>=as.Date('2012-06-14'))  
            {
                     rLog("  Three-tier wind forecasts--attempting to pull new data directly from Zema DB")
               		   strQry=paste("select opr_date, opr_hour he, forecast_time publish_date, power value
                        from ze_data.ercot_three_tier_forecasts
                        where opr_date>=to_date('",startdate,"', 'YYYY-MM-DD')
                              and opr_date<=to_date('",lmtgdate,"', 'YYYY-MM-DD')",sep="")
                    			windactraw <- sqlQuery(connn, strQry);
                    			colnames(windactraw)[match("VALUE",colnames(windactraw))]="value"
                      ## Get the x DA forecast
                        rLog("  Three-tier wind forecasts--Getting the desired forecast date")
                        windact=windactraw
                          if(DAct>0)
                            { 
                              rLog("  Three-tier wind forecasts--Getting data from x days ahead")
                              windact$dtdesired=as.POSIXct(paste(as.POSIXlt(windact$OPR_DATE)$year+1900,"-",sprintf("%02.0f",as.POSIXlt(windact$OPR_DATE)$mon+1),"-",
                                            sprintf("%02.0f", as.POSIXlt(windact$OPR_DATE)$mday)," ",DAtime,sep=""))-DAct*24*60*60
                              windact$value=abs(windact$dtdesired-windact$PUBLISH_DATE)
                              windactmap=windact; windactmap$variable="value"
                              windactmap=as.data.frame(cast(windactmap, dt_he~variable, min))
                              windact=as.data.frame(windact)
                              windact=merge(windact, windactmap)
                            }
                          if(DAct==0)
                            { 
                              rLog("  Three-tier wind forecasts--Getting most recent wind forecast")
                              windactmap=windact; windactmap$variable="PUBLISH_DATE"; windactmap$value=windactmap$PUBLISH_DATE
                              windactmap=cast(windactmap, OPR_DATE+HE~variable, max)
                              windact$PUBLISH_DATE=as.numeric(windact$PUBLISH_DATE)
                              windact=merge(windact, windactmap)
                            }
            }

        ## Use new three tier symbols for data after 6/14 -  Uses data >=6/14/12  (as of 6/20/12, waiting for a backfill)
          if(startdate<as.Date('2012-06-14'))  
            {
                          rLog("  Three-tier wind forecasts--attempting to pull old data using pull.tsdb()")
                          if(DAct>0)
                            { 
                                threetier=pull.tsdb(startdate, lmtgdate, symbolst=c('THREETIER_daFcast_WIND_Mw'), hourconvention=0)
                            }
                          if(DAct==0)
                            { 
                                threetier=pull.tsdb(startdate, lmtgdate, symbolst=c('THREETIER_rtFcast_WIND_Mw'), hourconvention=0)
                            }
                threetier$OPR_DATE = paste(as.POSIXlt(threetier$dt_he)$year+1900,
                                          sprintf("%02.0f",as.POSIXlt(threetier$dt_he)$mon+1),
                                          sprintf("%02.0f", as.POSIXlt(threetier$dt_he)$mday),sep="-")
                threetier$HE = as.POSIXlt(threetier$dt_he)$hour
                threetier=threetier[,c("OPR_DATE", "HE", "value")]
            }
            


        ## COMBINE DATA
        if(!is.na(match("windact",ls())) & !is.na(match("threetier",ls())))
          {
            threetier=subset(threetier, OPR_DATE<=min(windact$OPR_DATE))
            windact=rbind(windact[,c("OPR_DATE", "HE", "value")], threetier[,c("OPR_DATE", "HE", "value")])
          }
        if(!is.na(match("windact",ls())) & is.na(match("threetier",ls())))
          {
            windact=windact[,c("OPR_DATE", "HE", "value")]
          }
        if(is.na(match("windact",ls())) & !is.na(match("threetier",ls())))
          {
            threetier=subset(threetier, OPR_DATE<=min(windact$OPR_DATE))
            windact=threetier[,c("OPR_DATE", "HE", "value")]
          }

        return(windact)
 
 }



















#
#
######################################################################
## bucket hPP for a given set of buckets.  Should support SecDB ones
## sometime in the future.
##
#bucket.hprices.daily <- function(hPP, bucket, do.split=TRUE)
#{
#  bPP <- vector("list", length=length(bucket))
#  names(bPP) <- bucket
#  
#  dtime <- as.POSIXlt(index(hPP))
#  
#  if (bucket=="1X16"){
#    ind.hour <- which(dtime$hour %in% 8:23)
#    bPP <- aggregate(hPP[ind.hour,],
#      list(as.Date(format(index(hPP)[ind.hour], "%Y-%m-%d"))),
#                     mean, na.rm=T)
#  } else 
#  if (bucket=="1X8"){
#    ind.hour <- which(dtime$hour %in% 0:7)
#    bPP <- aggregate(hPP[ind.hour,],    # substract one second for HE
#      list(as.Date(format(index(hPP)[ind.hour]-1, "%Y-%m-%d"))),
#                     mean, na.rm=T) 
#  } else {
#    browser()
#    ind.b <- secdb.getHoursInBucket(region, bucket,
#      index(hPP)[1]+1, index(hPP)[ncol(hPP)])
#    hPP <- window(hPP, index=ind.b$time)
#    bPP <- aggregate(hPP,               # substract one second for HE
#       list(as.Date(format(index(hPP)-1, "%Y-%m-%d"))), mean) 
#  }  
#
#  if (do.split){  # should you group the RT and DA from the same node
#    ind <- split(colnames(bPP), gsub("RT_|DA_", "", colnames(bPP)))
#    bPP <- lapply(ind, function(ind.x, bPP){bPP[,ind.x]}, bPP)
#    names(bPP) <- paste(bucket, "_", names(bPP), sep="")
#  }
#  
#  return(bPP)  # bucket prices
#}
#
######################################################################
## trd is a list as constructed in the demo.inc-dec.R file
##
#calc_portfolio_pnl <- function(trd,
#  days=seq(Sys.Date()-10, Sys.Date(), by="1 day"))
#{
#  PnL <- vector("list", length=length(trd))
#
#  # loop over all trades
#  for (i in 1:length(trd)) {
#    info <- trd[[i]]
#    if (!is.null(info$bucket)){
#      info$hours <- switch(info$bucket,
#                           `1X24` = 0:23,
#                           `1X8`  = 0:7,        # HourEnding
#                           `1X16` = 8:23)       # HourEnding 
#    }
#    info$is.INC <- ifelse(info$inc.dec=="INCREMENT", TRUE, FALSE)
#    price_rtda<- FTR:::.inc.dec.tsdbprices(info$ptid, "NEPOOL",
#                                           min(days), max(days))
#    PnL[[i]] <- info
#    PnL[[i]]$pnl <- price_one_incdec(price_rtda, hrs=info$hours,
#                                     qty=info$mw, is.INC=info$is.INC)
#  }
#  
#  PnL
#}
#
#
#
#
######################################################################
## Pick distinct pairs for nodes.  Used in find.price.arbitrage
#.pickNodes <- function(x, y, pick=3)
#{
#  if (pick > unique(length(x))){
#    rLog("You want to pick more elements than you have!")
#    return(NULL)
#  }
#  
#  X <- rep(NA, pick)
#  Y <- rep(NA, pick)
#  X[1] <- x[1]
#  Y[1] <- y[1]
#  IND <- c(1, rep(NA, pick-1))  # interested in this one only
#  
#  ind.pk  <- 1
#  ind.row <- 2    
#  while (ind.row <= length(x) & ind.pk < pick){
#    if ((x[ind.row] %in% X)|(y[ind.row] %in% Y)){
#      ind.row <- ind.row + 1
#      next
#    }
#    ind.pk <- ind.pk + 1
#    X[ind.pk]   <- x[ind.row]
#    Y[ind.pk]   <- y[ind.row]
#    IND[ind.pk] <- ind.row
#    ind.row <- ind.row + 1
#  }
#
#  return(na.omit(IND))
#}
#
######################################################################
## Occasionally prices among nodes close by are different in DA and
## identical in RT.  Check for such occurences.  These make for nice 
## arbitrage ideas.
##
## This is rare, so don't panick if you don't get them...
## trigger - the value that will trigger highligting. 
##
#find.price.arbitrage <- function(MAP, threshold=0.5, pick=2, hDays=10)
#{
#  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.DA.RData")  # hPP.DA
#  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.RT.RData")  # hPP
#  hPP.RT <- hPP
#  rm(hPP)
#  if (ncol(hPP.DA) < 900 | ncol(hPP.RT) < 900){
#    rLog("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
#    rLog("Not all nodes have been read from tsdb.  Please fix.")
#    return(NULL)
#  }
#  
#  hPP <- cbind(hPP.DA, hPP.RT)  
#  colnames(hPP) <- gsub("NEPOOL_SMD_|_LMP","", colnames(hPP))
#  
#  MAP <- subset(MAP, in.pnode.table)
#  MAP <- subset(MAP, !is.na(dispatch.zone))
#  groups <- split(MAP, MAP$dispatch.zone)
#
#  # look for arbitrage inside a dispatch.zone only
#  res <- NULL
#  for (g in seq_along(groups)){
#    rLog("Working on:", names(groups)[g])
#    cnames <- c(paste("DA_", groups[[g]]$ptid, sep=""),
#                paste("RT_", groups[[g]]$ptid, sep=""))
#
#    ind   <- which(colnames(hPP) %in% cnames)
#    if (length(ind) == 0)
#      next
#    
#    ptids <- table(gsub("RT_|DA_", "", colnames(hPP[,ind])))
#    ptids <- sort(as.numeric(names(ptids[ptids==2])))
#    hPP.g <- hPP[,ind]
#    hPP.g[is.na(hPP.g)] <- 0
#
#    DEC <- zoo(matrix(0, nrow=nrow(hPP), ncol=length(ptids)), index(hPP))
#    colnames(DEC) <- ptids
#    for (p in 1:length(ptids)){
#      DEC[,p] <- hPP.g[,paste("RT_", ptids[p], sep="")]-
#                 hPP.g[,paste("DA_", ptids[p], sep="")]
#    }
#    
#    x  <- apply(tail(DEC,hDays*24),2, mean)  # last hDays
#    DM <- as.matrix(dist(as.matrix(x), method="manhattan"))
#    DM[lower.tri(DM)] <- NA
#    ind.picks <- which(DM > threshold, arr.ind=TRUE)
#    if (nrow(ind.picks)==0){next}  # nothing
#    picks <- data.frame(zone  = names(groups)[g], 
#                        dec   = rownames(DM)[ind.picks[,"row"]],
#                        inc   = colnames(DM)[ind.picks[,"col"]])
#    picks$value <- x[picks$dec]-x[picks$inc]
#
#    ind <- which(picks$value < 0)
#    if (length(ind)>0){   # need to reverse these guys
#      aux <- picks$inc[ind]
#      picks$inc[ind] <- picks$dec[ind]
#      picks$dec[ind] <- aux
#      picks$value[ind] <- -picks$value[ind]
#    }
#    picks <- picks[order(-picks$value), ]
#    picks <- picks[.pickNodes(picks$dec, picks$inc, pick=3),]
#    
#    res[[g]] <- picks
#  }
#
#  res <- do.call(rbind, res)
#  rownames(res) <- NULL
#  colnames(res)[4] <- "value"
#  
#  return(res)
#}
#
#
#
######################################################################
## Select trades for next day based on the long list of recommendations. 
##
#fix.exceptions <- function(x)
#{
#  x <- gsub("LD.BOGGY_BK46  ",     "LD.BOGGY_BK46 ",     x)
#  x <- gsub("LD.CANDLE  13.2",     "LD.CANDLE 13.2",     x)
#  x <- gsub("LD.CANTON  23",       "LD.CANTON 23",       x)
#  x <- gsub("UN.BPT_ENER16  BHCC", "UN.BPT_ENER16 BHCC", x)
#  x <- gsub("UN.JAY     13.8VCG1", "UN.JAY  13.8VCG1",   x)
#  
#  
#  x
#} 
#
#
######################################################################
## Select trades for next day based on the long list of recommendations. 
##
#format.next.day.trades <- function( NXT, ptids=NULL, quantity=15, book="VIRTSTAT", 
#  maxPrice=300, minPrice=10, save=TRUE, asOfDate=Sys.Date()+1)
#{
#  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
#
#  format1 <- function(x, quantity){
#    if ("bucket" %in% names(x)){
#      X <- data.frame(x[c("inc.dec", "ptid", "bucket")], first.HE="",
#                        last.HE="")
#    } 
#    if ("hrs" %in% names(x)){
#      aux <- strsplit(x$hrs, "-")
#      if (any(sapply(aux, length)!=2))
#        stop("Misspecified hrs element in the input list!")
#      X <- data.frame(inc.dec=x$inc.dec, ptid=x$ptid, bucket="",
#        first.HE=sapply(aux, "[", 1), last.HE=sapply(aux, "[", 2))
#    }
#    if ("mw" %in% names(x)){ X$mw <- x$mw 
#    } else { X$mw <- quantity}
#    if ("book" %in% names(x)){ X$book <- x$book 
#    } else { X$book <- book}
#    
#    return(X)
#  }
#
#  
#  if (class(NXT)=="data.frame"){
#    if (!is.null(ptids))
#      NXT <- subset(NXT, ptid %in% ptids)
#  } else {  # if it's a list, one trade per element
#      NXT <- do.call(rbind, lapply(NXT, format1, quantity)) 
#  }
#
#  res <- merge(NXT, MAP[,c("ptid", "name")], all.x=TRUE, sort=FALSE)
#  if (!("inc.dec" %in% names(res)))
#    res$inc.dec <- ifelse(res$dec, "DECREMENT", "INCREMENT")
#  res$block    <- tolower(res$bucket)
#  res$price    <- ifelse(res$inc.dec=="DECREMENT", maxPrice, minPrice)
#  res$shape    <- 2
##  res$book     <- "NESTAT"
#  
#  res.csv <- res[,c("name", "inc.dec", "block", "first.HE", "last.HE",
#                    "mw", "price", "shape", "book")]
#
#  res.csv <- apply(res.csv, 1, function(x){
#    paste(x, sep="", collapse=",")})
#  res.csv <- paste(paste(res.csv, sep="", collapse="\n"), "\n", 
#                   sep="")
#
#  res.csv <- fix.exceptions(res.csv)
#  cat(res.csv)
#
#  # save the trades to file for reconciliation later!
#  if (save){
#    fileOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/AAD_incdec_bids.RData"
#    if (file.exists(fileOut)){
#      load(fileOut)
#    } else {
#      incdec <- NULL
#    }
#    incdec[[as.character(asOfDate)]] = res
#    
#    save(incdec, file=fileOut)
#    rLog("\nSaved bids for", as.character(asOfDate), "to", fileOut)
#  }
#  
#  
#  invisible(res)
#}
#
#
#
######################################################################
##
#get.incdec.hprices <- function(buckets, ptids=NULL,
#  start.dt=as.POSIXct(paste(min(Sys.Date()-31), "01:00:00")),
#  end.dt=as.POSIXct(paste(Sys.Date()+1, "00:00:00")), do.split=TRUE,
#  do.melt=FALSE, add.RT5M=TRUE, ...)
#{
#  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
#  if (is.null(ptids)){
#    load(paste("//ceg/cershare/All/Risk/Data/FTRs/",
#               "NEPOOL/ISO/DB/RData/map.RData", sep=""))
#    ptids <- subset(MAP, in.pnode.table)$ptid
#  }
#  info <- file.info(paste(dir, "nepool.RT.RData", sep=""))
#  if (is.na(info$size)){
#    # get the RT prices
#    tsdb.symbols <- paste("NEPOOL_SMD_RT_", ptids, "_LMP", sep="")
#    end.dt.RT <- as.POSIXct(paste(Sys.Date(), "00:00:00", sep=""))
#    hPP <- FTR.load.tsdb.symbols( tsdb.symbols, start.dt, end.dt.RT)
#    cat("Saving the RT prices to nepool.RT.Data\n", sep="")
#    save(hPP, file=paste(dir, "nepool.RT.RData", sep=""))
#  } else {
#    cat("Loading the RT prices from nepool.RT.Data\n")    
#    load(paste(dir, "nepool.RT.RData", sep=""))
#  }
#  
#  info <- file.info(paste(dir, "nepool.RT5M.RData", sep=""))
#  if (is.na(info$size)){
#    if (add.RT5M){
#      # get the 5min RT prices ...
#      tsdb.symbols <- paste("NEPOOL_SMD_RT5M_", ptids, "_LMP", sep="")
#      hPP.5M <- FTR.load.tsdb.symbols( tsdb.symbols,
#        as.POSIXct(paste(Sys.Date(), "00:00:01")), min(Sys.time(), end.dt))
#      hPP.5M <- toHourly(hPP.5M, HE=TRUE)
#      colnames(hPP.5M) <- gsub("5M","", colnames(hPP.5M))
#      
#      cat("Saving the RT5M prices to nepool.RT5M.Data\n")
#      save(hPP.5M, file=paste(dir, "nepool.RT5M.RData", sep=""))
#    }
#  } else {
#    cat("Loading the RT5M prices from nepool.RT5M.Data\n")    
#    load(paste(dir, "nepool.RT5M.RData", sep=""))
#  }
#
#  # put them together ...
#  hPP <- hPP[,colnames(hPP.5M)]  # some 5M symbols are missing
#  hPP <- rbind(hPP, hPP.5M)
#
#  info <- file.info(paste(dir, "nepool.DA.RData", sep=""))
#  if (is.na(info$size)){
#    # get the DA prices 
#    tsdb.symbols <- paste("NEPOOL_SMD_DA_", ptids, "_LMP", sep="")
#    hPP.DA <- FTR.load.tsdb.symbols( tsdb.symbols, start.dt, end.dt)
#    
#    cat("Saving the DA prices nepool.DA.Data\n")
#    save(hPP.DA, file=paste(dir, "nepool.DA.RData", sep=""))
#  } else {
#    cat("Loading the DA prices nepool.DA.Data\n")    
#    load(paste(dir, "nepool.DA.RData", sep=""))
#  }
###  cnames <- gsub("_RT_", "_DA_", colnames(hPP))
###  hPP.DA <- hPP.DA[, cnames]
#    
#  # put RT and DA prices together ... 
#  hPP <- cbind(hPP, hPP.DA)  
#  colnames(hPP) <- gsub("NEPOOL_SMD_|_LMP","", colnames(hPP))
#   
#  bPP <- NULL
#  if (!is.null(buckets)){
#    rLog("Bucket prices:")
#    for (b in buckets)
#      if (do.split)
#         bPP <- c(bPP, bucket.hprices.daily(hPP, bucket=b, ...))
#      else 
#        bPP[[b]] <- bucket.hprices.daily(hPP, bucket=b, ...)
#    rLog("Done")
#  }
#  if (do.melt){
#    aux <- lapply(bPP, as.data.frame)
#    aux <- lapply(aux, melt)
#    # needs work ... 
#  }
#  
#  return(list(hPP, bPP))
#}
#
#
######################################################################
## Calculate some basic statistics to use in filters ...  
##
#inc.basic.stats <- function(X)
#{
#  X <- na.omit(X)
#  spread <- X[,1]-X[,2]
#  res <- NULL
#  res["avg.spread.all"]   <- mean(spread)
#  res["avg.spread.last3"] <- mean(tail(spread,3))
#  res["avg.spread.last1"] <- tail(spread,1)
#  res["sd.spread.all"]    <- sd(spread)
#  res["sd.spread.last3"]  <- sd(tail(spread,3))
#
#  return(res)
#}
#
#
######################################################################
## Get the trading signals and historical PnL on a given strategy.
##
#inc.get.trading.signals <- function(bPP, methods)
#{
#  TRD.PnL <- NULL
#  for (method in methods){
#    cat(paste("Working on method ", method, "\n", sep=""))
#    TRD <- lapply(bPP, method)
#    TRD <- do.call(rbind, TRD)
#
#    if (!is.null(TRD)){
#      TRD$bucket.ptid <- sapply(strsplit(rownames(TRD), "\\."),
#                              function(x){x[1]}) 
#      rownames(TRD) <- NULL
# 
#      TRD.PnL[[method]] <- inc.price.trades(TRD, bPP)
#      TRD.PnL[[method]]$method <- method
#    }
#  }
#  TRD.PnL <- do.call(rbind, TRD.PnL)
#  rownames(TRD.PnL) <- NULL
#  
#  return( TRD.PnL )
#}
#
######################################################################
## Price the trades ... 
## TRD - the data.frame with trading signals 
## bPP - list of historical prices: (RT|DA) pairs for bucket.ptid
##
#inc.price.trades <- function(TRD, bPP)
#{
#  DEC <- sapply(bPP, function(X){
#    X <- X[,1]-X[,2]})   # RT - DA !     # no need for drop=TRUE in 2.10.1
#  
#  # subset DEC by only the dates you're interested ...
#  ind <- which(as.Date(rownames(DEC)) %in% unique(TRD$start.dt))
#  if (length(ind)==0){
#    TRD$value <- TRD$PnL <- NA
#  } else {
#    DEC <- DEC[which(as.Date(rownames(DEC)) %in% unique(TRD$start.dt)),,drop=F]
#    DEC <- melt(DEC)
#    names(DEC)[1:2] <- c("start.dt", "bucket.ptid")
#    DEC$start.dt <- as.Date(DEC$start.dt)
#    DEC$bucket.ptid <- as.character(DEC$bucket.ptid)
#
#    hours <- as.numeric(gsub(".*X|_.*","",TRD$bucket.ptid))
#    TRD <- merge(TRD, DEC, all.x=TRUE)
#    TRD$value <- TRD$value*(2*TRD$dec - 1)
#    TRD$PnL   <- TRD$value*hours
#  }
#  
#  return(TRD)
#}
#
######################################################################
## pick - the number of nodes to look at
##
#max_min_locations <- function(files, pick=6)
#{
#  load(files[1])  # RT prices
#  rt <- tail(hPP, 24)
#  
#  aux <- melt(apply(rt[8:23,], 2, mean))      # RT ONPEAK from yesterday
#  aux <- data.frame(ptid=gsub("NEPOOL_SMD_RT_(.*)_LMP", "\\1", rownames(aux)),
#                    price=aux$value)
#  aux <- aux[order(aux$price),]
#  res <- rbind(cbind(bucket="Onpeak", type="RT.Min", head(aux, pick)),
#               cbind(bucket="Onpeak", type="RT.Max", tail(aux, pick)))
#  
#  aux <- melt(apply(rt[c(1:7,24)], 2, mean))  # RT OFFPEAK from yesterday 
#  aux <- data.frame(ptid=gsub("NEPOOL_SMD_RT_(.*)_LMP", "\\1", rownames(aux)),
#                    price=aux$value)
#  aux <- aux[order(aux$price),]
#  res <- rbind(res,
#               rbind(cbind(bucket="Offpeak", type="RT.Min", head(aux, pick)),
#               cbind(bucket="Offpeak", type="RT.Max", tail(aux, pick))))
#
#  load(files[3])  # DA prices
#  da <- tail(hPP.DA, 24)
#  if (as.Date(index(da)[1]) != Sys.Date()+1)
#    warning("Not using tomorrow's DA data!")
#  
#  aux <- melt(apply(da[8:23,], 2, mean))      # DA ONPEAK for tomorrow
#  aux <- data.frame(ptid=gsub("NEPOOL_SMD_DA_(.*)_LMP", "\\1", rownames(aux)),
#                    price=aux$value)
#  aux <- aux[order(aux$price),]
#  res <- rbind(res,
#               cbind(bucket="Onpeak", type="DA.Min", head(aux, pick)),
#               cbind(bucket="Onpeak", type="DA.Max", tail(aux, pick)))
#  
#  aux <- melt(apply(da[c(1:7,24)], 2, mean))  # DA ONPEAK for tomorrow 
#  aux <- data.frame(ptid=gsub("NEPOOL_SMD_DA_(.*)_LMP", "\\1", rownames(aux)),
#                    price=aux$value)
#  aux <- aux[order(aux$price),]
#  res <- rbind(res,
#               rbind(cbind(bucket="Offpeak", type="DA.Min", head(aux, pick)),
#               cbind(bucket="Offpeak", type="DA.Max", tail(aux, pick))))
#
#  res <- merge(res, MAP[, c("ptid", "name")], by="ptid")
#  res <- res[, c("bucket", "type", "ptid", "name", "price")]
#  res <- res[order(res$bucket, res$type, res$price),]
#  
#
#  aux <- split(res, res$bucket)
#  aux <- lapply(aux, function(x, pick){
#    y <- split(x, rep(c("DA", "RT"), each=2*pick))
#    y1 <- capture.output(print(y[[1]], row.names=FALSE))
#    y2 <- capture.output(print(y[[2]][,-1], row.names=FALSE))
#    paste(y1, y2, sep="  |  ")
#  }, pick)
#  
#  out <- c("Min/Max nodes in the pool:\n", aux[[2]], "\n\n", aux[[1]])
#  
#  out  
#}
#
#
######################################################################
## See if there are next day trades, and analyze them better by pulling
## the 5m RT symbols
#next.day.trades <- function( TRD.PnL, options )
#{
#  NXT <- subset(TRD.PnL, is.na(PnL))
#  if (nrow(NXT)==0){
#    rLog("No proposed trades for tomorrow")
#    return(NXT)
#  }
#  aux <- strsplit(NXT$bucket.ptid, "_")
#  NXT$bucket <- sapply(aux, function(x){x[1]})
#  NXT$ptid   <- sapply(aux, function(x){x[2]})
#  NXT$bucket.ptid <- NULL
#  NXT$asOfDate <- options$asOfDate
#  NXT$start.dt <- NXT$start.dt + 1  # for next day
#  NXT <- NXT[,c("start.dt", "dec", "bucket", "ptid", "method")]
#  
##  all.NXT <- NXT; save(all.NXT, file=options$trades.file)
#  if (nrow(NXT)>0){
#    cat("Saving the proposed trades for tomorrow into archive...")
#    load(options$trades.file)
#    all.NXT <- subset(all.NXT, start.dt != NXT$start.dt[1])
#    all.NXT <- rbind(all.NXT, NXT)
#    rownames(all.NXT) <- NULL
#    save(all.NXT, file=options$trades.file)
#    cat("Done.\n")
#  }
#
#  return(NXT)
#}
#
#
######################################################################
## Called by calc_portfolio_pnl.
## price_rtda is a zoo objects with hourly data, two columns: rt, da
## qty is in MW, hrs are in HourEnding convention
##
#price_one_incdec <- function(price_rtda, hrs=8:23, qty=1, is.INC=TRUE)
#{
##  browser()
#  N <- length(hrs)
#  pnl <- subset(price_rtda, as.numeric(format(index(price_rtda), "%H")) %in% hrs)
#  index(pnl) <- index(pnl) - 60  # go back one minute 
#  pnl <- aggregate(pnl, as.Date(format(index(pnl))), mean)
#  pnl$pnl <- N*qty*(pnl[,2]-pnl[,1])*ifelse(is.INC, 1, -1)
#   
#  pnl  # pnl by day in the 3rd column
#}
#
#
######################################################################
## PnL is a list with all the info
##
#report_portfolio_pnl <- function(pnl, MAP)
#{
#  res <- lapply(pnl, function(x){x$pnl[,"pnl"]})
#  res <- do.call(cbind, res)
#  colnames(res) <- 1:ncol(res)
#  res <- t(res)
#  res <- round(res)
#
#  node.names <- structure(MAP$location, names=MAP$ptid)
#  
#  rownames(res) <- lapply(pnl, function(x, node.names){
#    paste(substr(x$inc.dec, 1, 3), x$mw, "MW",
#          node.names[as.character(x$ptid)], x$bucket)}, node.names)
#  colnames(res) <- format(as.Date(colnames(res)), "%m/%d")
#
#  print(res)
#}
#
#
#
######################################################################
## select dates that you want to place your inc or dec based on 
## lookback only!
## RULE: look if previous day > a large treshold is good indicative for next
##       day
##
#trigger.Persistence <- function(X, limit=10)
#{
#  TRD <- NULL
#  X <- na.omit(X)
#  spread <- X[,1] - X[,2]    # RT - DA
#  ind <- which(spread > limit)
#  if (length(ind) > 0){
#    TRD <- data.frame(dec=TRUE, start.dt=index(spread)[ind]+1)
#  }
#  ind <- which(spread < -limit)
#  if (length(ind) > 0){
#    TRD <- rbind(TRD, data.frame(dec=FALSE, start.dt=index(spread)[ind]+1))
#  }
##  TRD$ptid <- as.numeric(gsub("RT_|DA_", "",colnames(X)[1]))
#  return(TRD)
#}
#
######################################################################
## search for when RT > DA by $40.  Is there reversion? 
## lookback only!
## RULE: look if previous day > a treshold is good indicative for next
##       day
##
#trigger.1DayPanicDA <- function(X, limit=40)
#{
#  TRD <- NULL
#  X <- na.omit(X)
#  spread <- X[,1] - X[,2]   # RT - DA
#  ind <- which(spread > limit)
#  if (length(ind) > 0){     # inc the next day 
#    TRD <- data.frame(dec=FALSE, start.dt=index(spread)[ind]+1)
#  }
#  return(TRD)
#}
#
######################################################################
## search for when RT > DA consistently over the past 7 days.  
## 
##
#trigger.7DayDomination <- function(X, limit=5, last=7)
#{
#  TRD <- NULL
#  X <- na.omit(X)
#  spread <- X[,1] - X[,2]    # RT - DA
#  spread <- tail(spread, last)
#  if ((mean(spread) > limit) && (all(spread > 0))){
#    TRD <- data.frame(dec=TRUE, start.dt=index(spread)[last]+1)
#  }
#  if ((mean(spread) < -limit) && (all(spread < 0))){
#    TRD <- rbind(TRD, data.frame(dec=FALSE, start.dt=index(spread)[last]+1))
#  }
#  return(TRD)
#  
#}
#
#
#
######################################################################
## simple trend following strategy.  If the inc made money in the
## previous day, stay an inc, if it lost money in the prev day, switch
## to a dec.
##
## DD is a list with one element per day
##
#.lookback1 <- function(DD)
#{
#  N <- length(DD)
#
#  DD[[1]]$strategy <- "INC"    
#  DD[[1]]$qty <- 1
#  DD[[1]]$PnL <- DD[[1]]$`da/rt` * DD[[1]]$qty
#  
#  # loop over all the time steps (days)
#  for (d in 2:N) {
#    # get the strategy for day d
#    if (DD[[d-1]]$PnL > 0) {    # made money in the previous period, 
#      DD[[d]]$strategy <- DD[[d-1]]$strategy   # stay the same
#      DD[[d]]$qty <- 1.5        # increase the qty a bit
#    } else {                    # do the opposite
#      if (DD[[d-1]]$strategy=="INC") {
#        DD[[d]]$strategy <- "DEC"
#      } else {
#        DD[[d]]$strategy <- "INC"
#      }
#      DD[[d]]$qty <- 1           
#    }
#    
#    # calculate PnL in this time step
#    DD[[d]]$PnL <- if (d != N){
#      if (DD[[d]]$strategy == "INC") {
#          DD[[d]]$qty * DD[[d]]$`da/rt`
#        } else {
#          -DD[[d]]$qty * DD[[d]]$`da/rt`
#        }
#    } else { NA }
#  }
# 
#  DD 
#}
#
#
#
#
######################################################################
## New trading strategy
##
#strategy_hub <- function(hrs, startDt=as.POSIXct("2010-01-01"),
#  endDt=Sys.time())
#{
#  nextDate <- Sys.Date()+1
#  
#  symbs <- c("nepool_smd_da_4000_lmp", "nepool_smd_rt_4000_lmp",
#             "nepool_smd_rt5m_4000_lmp")
#
#  res <- FTR.load.tsdb.symbols(symbs, startDt, endDt,
#    aggHourly=c(FALSE, FALSE, TRUE))  
#  colnames(res) <- c("lmp.da", "lmp.rt", "lmp.rt5m")
#  suppressWarnings(res <- data.frame(time=index(res), as.matrix(res)))
#
#  res <- res[!is.na(res$lmp.da),]  # rm some missing data
#  ind <- which(is.na(res$lmp.rt) & !is.na(res$lmp.rt5m))
#  if (length(ind) > 0)
#    res[ind, "lmp.rt"] <- res[ind, "lmp.rt5m"]
#  
#  res$`da/rt` <- res$lmp.da  - res$lmp.rt
#  res <- subset(res, !is.na(res$`da/rt`))
#  res$hour <- as.numeric(format(res$time, "%H"))
#  res$day  <- as.Date(format(res$time, "%Y-%m-%d")) 
#
#
#  # look at onpeak hours
#  aux <- subset(res, hour %in% hrs)
#  DD  <- aggregate(aux[,c(2:3,5)], list(day=aux$day), mean)
#  DD  <- split(DD, DD$day)
#  DD[[format(nextDate)]] <- data.frame(day=nextDate)
#  
#  on  <- .lookback1(DD)
#  #PnL <- do.call(rbind, lapply(on, function(x)x[,c("day", "PnL")]))
#  #plot(PnL$day, cumsum(PnL$PnL), col="blue", type="l")
#  
#  on
#}
#
######################################################################
## prepare the string output
##
#strategy_hub_rpt <- function(X, txt)
#{
#  aux <- do.call(rbind, tail(X[-length(X)], 10))
#  aux[,c(2:4,7)] <- sapply(aux[,c(2:4,7)], round, 2)
#  totalPnL <- sum(aux$PnL)
#  fcst <- tail(X,1)[[1]]
#
#  out <- paste(txt, " ",format(fcst$day), " to ",
#    fcst$strategy, " ", fcst$qty, "MW", sep="")    
#  out <- c(out, "\nLast 10 days of PnL for this strategy:")
#  out <- c(out, capture.output(print(aux, row.names=FALSE)))
#  out <- c(out, paste("           Total PnL:", totalPnL))
#
#  out
#}
#
#
#
#
### #####################################################################
### #
### get.latest.st.outage <- function()
### {
###   dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages"
###   files <- list.files(dir, full.names=T)
#
###   file <- files[length(files)]
###   res  <- read.csv(file)
#  
###   return(res)
### }
#
### #####################################################################
### #
### compare.last2.outage.rpts <- function(asOfDate=Sys.Date())
### {
###   dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages"
###   files <- list.files(dir, full.names=T)
#
###   time <- sapply(strsplit(files, "\\."), function(x){x[3]})
#  
###   files <- files[(length(files)-1):length(files)]
#  
###   res.1  <- read.csv(files[1])
###   names(res.1)[1] <- "time.1"
###   res.2  <- read.csv(files[2])
###   names(res.2)[1] <- "time.2"
#
###   aux <- merge(unique(res.1[,c("time.1", "application.number")]),
###     unique(res.2[,c("time.2", "application.number")]), all=T)
#
###   res.2$actual.start.date <- as.POSIXct(res.2$actual.start.date)
### #  res.2$actual.start.dt <- as.POSIXct(res.2$actual.start.dt)
#  
###   res.2 <- subset(res.2, )asOfDate
### }
#
#
######################################################################
## Check which nodes have not been mapped and email Michelle
## Some nodes are not in SecDb when booking.  Stupid!
##
## NOT USED ANYMORE.
## HAVE A PROCMON JOB CONDOR/CHECK_NEPOOL_INCDEC_NODE_MAPPING.R
### check.node.mapping <- function(incdec, email=FALSE)
### {
###   require(xlsReadWrite)
###   require(CEGxls)
#
###   fname <- paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/",
###     "Nepool Nodes Status.xls", sep="")
###   NN <- read.xls(fname, sheet=1)
###   NN$nameWoSpaces <- gsub(" ", "", NN$MappedNodes)
#
###   incdec$nameWoSpaces <- gsub(" ", "", incdec$name)
###   incdec <- unique(incdec[,c("name","nameWoSpaces")])
#
###   miss <- merge(incdec[,c("name", "nameWoSpaces")], NN, all=TRUE)
#
###   out <- ""
###   ind <- which(is.na(miss$MappedNodes))
#
###   if (length(ind)>0){
###     out <- c("Following nodes need to be mapped:\n")
###     ind <- is.na(miss$MappedNodes)
###     out <- c(out, paste(miss$name[ind], sep="", collapse="\n"))
###     # add the missing nodes to the MappedNodes column ...
###     miss$MappedNodes[ind] <- miss$name[ind]
###   }
###   if (out[1] == "") out <- "OK.  No nodes need mapping."
#
#    
###   if (email){
###     to <- paste(c(
###       "michelle.bestehorn@constellation.com",
###       "eric.riopko@constellation.com",
###       "adrian.dragulescu@constellation.com"), sep="", collapse=",")
###     from <- "PM_AUTOMATIC_CHECK@constellation.com"
###     if (out[1] != "OK.  No nodes need mapping")
###       sendEmail(from, to, "Nepool inc/dec nodes to map", out)
#
###     rLog("Updating spreadsheet...")
###     write.xls(miss[,"MappedNodes", drop=FALSE], file=fname)
###     rLog("Done.")
###   } else {
###     print(out)
###   }
#  
### }
#
#
### #####################################################################
### # Plot the PnL from the inc/dec trades suggested by the given strategy.
### #
### plot.trd.pnl <- function( TRD.PnL)
### {
###   PnL.day <- aggregate(TRD.PnL$PnL, list(start.dt=TRD.PnL$start.dt), sum)
###   windows()
###   plot(PnL.day$start.dt, PnL.day$x/1000, type="o", col="blue",
###        ylab="PnL, K$")
###   abline(h=0, col="gray")
### }
#
### #####################################################################
### # Select trades for next day based on historical performance
### #
### select.historical <- function( NXT, TRD.PnL)
### {
###   NXT$bucket.ptid <- paste(NXT$bucket, NXT$ptid, sep="_")
#
###   hPnL <- merge(TRD.PnL, NXT[,c("bucket.ptid", "dec")])
###   hPnL <- hPnL[order(hPnL$bucket.ptid, hPnL$dec, hPnL$start.dt), ]
#
###   aux <- cast(hPnL, bucket.ptid + dec ~ ., mean, na.rm=TRUE)
###   names(aux)[3] <- "avg.PnL"
###   aux <- aux[order(-aux$avg.PnL), ]
#  
### }
#
#

