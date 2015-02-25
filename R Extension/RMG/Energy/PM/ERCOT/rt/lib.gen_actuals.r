# Convert gen actuals to an hourly quantity we actually get paid on
#     Price weight gen actuals
#     Discounts quantity for any basepoint deviation charges
#
# Written by Lori Simpson on 29-Jan-2013






#------------------------------------------------------------
# Functions
# 
#


do_gen_actuals = function(strDtStart=Sys.Date(), strDtEnd=Sys.Date()+1, ignoreLmpErrors=FALSE, conn)
      ##strDtStart=DtStart; strDtEnd=DtEnd; ignoreLmpErrors=FALSE; conn=channel_zema_prod
  {

    # Check if data exists and pass highest hour
    rLog(paste("Start getData function at", Sys.time()))
    dfRes = getData(ignoreLmpErrors=FALSE, startDate=strDtStart, lmtgDate=strDtEnd, strMethod=2, conn=conn)
      # min(dfRes[is.na(dfRes$OPR_DATE)==FALSE,"OPR_DATE"]); max(dfRes[is.na(dfRes$OPR_DATE)==FALSE,"OPR_DATE"])
    rLog(paste("End getData function at", Sys.time()))
    dfRes2 = modifyData(dfRes)
    rLog(paste("End modifyData function at", Sys.time()))
    

    # Save to csv file
        saveCsv(dfRes2)
        rLog(paste("Saved csv files from",min(dfRes2$dt_he), "to", max(dfRes2$dt_he)))
        
        
    # Save to TSDB
        # pm_ercot_gen_lmpwt_cbec1,pm_ercot_gen_lmpwt_cbec2,pm_ercot_gen_lmpwt_ftr,pm_ercot_gen_lmpwt_han3,pm_ercot_gen_lmpwt_han4,pm_ercot_gen_lmpwt_han5,pm_ercot_gen_lmpwt_lp1,pm_ercot_gen_lmpwt_lp2,pm_ercot_gen_lmpwt_lp3,pm_ercot_gen_lmpwt_lp4,pm_ercot_gen_lmpwt_mc6,pm_ercot_gen_lmpwt_mc7,pm_ercot_gen_lmpwt_mc8,pm_ercot_gen_lmpwt_mv,pm_ercot_gen_lmpwt_qrec1,pm_ercot_gen_lmpwt_qrec2,pm_ercot_gen_lmpwt_wh
        uploadData(dfRes2)

  
  }



getData = function (ignoreLmpErrors=FALSE, startDate=Sys.Date(), lmtgDate=Sys.Date()+1, strMethod=2, conn)
  # startDate=strDtStart; lmtgDate=strDtEnd; ignoreLmpErrors=FALSE; strMethod=2
  {

   
    ## Data to pull

      dfGenMap=data.frame(
                      nameGen=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8', 'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv1', 'mv2'),
                      gen=c('ftr_net_mw_1M', 'wh_net_mw_1M', 'han3_net_mw_1M', 'han4_net_mw_1M', 'han5_net_mw_1M', 'lp1_net_mw_1M', 'lp2_net_mw_1M', 'lp3_net_mw_1M', 'lp4_net_mw_1M', 'mc6_net_mw_1M', 'mc7_net_mw_1M', 'mc8_net_mw_1M', 'cbec_phase1_mw_1M', 'cbec_phase2_mw_1M', 'qrec_phase1_mw_1M', 'qrec_phase2_mw_1M','MV_rttp_unita','MV_rttp_unitb')
                      )
            ## Have to convert 2 Magic Valley curves into one net output
            dfGenName=data.frame(
                            nameGen=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8', 'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv1', 'mv2'),
                            plant=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8', 'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv', 'mv')
                            )
      dfUdbpMap=data.frame(
                      plant=c('ftr', 'wh', 'han3', 'han4', 'han5',                                         'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8',              'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv'),
                      udbp=c('ftr_udbp_mw', 'wh_udbp_mw', 'han3_udbp_mw', 'han4_udbp_mw', 'han5_udbp_mw', 'lp1_udbp_mw', 'lp2_udbp_mw', 'lp3_udbp_mw', 'lp4_udbp_mw', 'mc6_udbp_mw', 'mc7_udbp_mw', 'mc8_udbp_mw', 'cbec1_udbp_mw', 'cbec2_udbp_mw', 'qrec1_udbp_mw', 'qrec2_udbp_mw', 'mv_udbp')
                      )
      dfRstMap=data.frame(
                      plant=c('ftr',    'wh',     'han3',     'han4',     'han5',     'lp1',     'lp2',     'lp3',     'lp4', 'mc6',         'mc7', 'mc8',         'cbec1',           'cbec2',           'qrec1',            'qrec2'),
                      rst=c('ftr_rst', 'wh_rst', 'han3_rst', 'han4_rst', 'han5_rst', 'lp1_rst', 'lp2_rst', 'lp3_rst', 'lp4_rst', 'mc6_rst', 'mc7_rst', 'mc8_rst', 'cbec1_rst', 'cbec2_rst', 'qrec1_rst', 'qrec2_rst')
                      )
      dfLmpMap=data.frame(
                      nameLmp=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp', 'mc6', 'mc7', 'mc8', 'cbec', 'qrec', 'mv'),
                      #lmp=c('ZM_ERCOT_RT_LMP_sp_ftr_cc1','ZM_ERCOT_RT_LMP_sp_whccs_cc1','ZM_ERCOT_RT_LMP_sp_hlses_unit3','ZM_ERCOT_RT_LMP_sp_hlses_unit4','ZM_ERCOT_RT_LMP_sp_hlses_unit5','ZM_ERCOT_RT_LMP_sp_az_all','ZM_ERCOT_RT_LMP_sp_mcses_unit6','ZM_ERCOT_RT_LMP_sp_mcses_unit7','ZM_ERCOT_RT_LMP_sp_mcses_unit8','ZM_ERCOT_RT_LMP_sp_cbec_all','ZM_ERCOT_RT_LMP_sp_qalsw_all','ZM_ERCOT_RT_LMP_sp_redfish_all')
                      lmp=c('FTR_CC1','WHCCS_CC1','HLSES_UNIT3','HLSES_UNIT4','HLSES_UNIT5','AZ_ALL','MCSES_UNIT6','MCSES_UNIT7','MCSES_UNIT8','CBEC_ALL','QALSW_ALL','REDFISH_ALL')
                      )
            ## Have to convert a single CB/QR curve into 2 curves
            dfLmpName=data.frame(
                          nameLmp=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp',  'lp', 'lp', 'lp','mc6', 'mc7', 'mc8', 'cbec','cbec', 'qrec',  'qrec', 'mv'),
                          plant=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8', 'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv')
                          )
      dfSppMap=data.frame(
                      nameSpp=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp', 'mc6', 'mc7', 'mc8', 'cbec', 'qrec', 'mv'),
                      spp=c('FTR_CC1','WHCCS_CC1','HLSES_UNIT3','HLSES_UNIT4','HLSES_UNIT5','AZ_ALL','MCSES_UNIT6','MCSES_UNIT7','MCSES_UNIT8','CBEC_ALL','QALSW_ALL','REDFISH_ALL')
                      #spp=c('ZM_ERCOT_rt_spp_sp_ftr_cc1','ZM_ERCOT_rt_spp_sp_whccs_cc1','ZM_ERCOT_rt_spp_sp_hlses_unit3','ZM_ERCOT_rt_spp_sp_hlses_unit4','ZM_ERCOT_rt_spp_sp_hlses_unit5','ZM_ERCOT_rt_spp_sp_az_all','ZM_ERCOT_rt_spp_sp_mcses_unit6','ZM_ERCOT_rt_spp_sp_mcses_unit7','ZM_ERCOT_rt_spp_sp_mcses_unit8','ZM_ERCOT_rt_spp_sp_cbec_all','ZM_ERCOT_rt_spp_sp_qalsw_all','ZM_ERCOT_rt_spp_sp_redfish_all')
                      )
            ## Have to convert a single CB/QR curve into 2 curves
            dfSppName=data.frame(
                          nameSpp=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp',  'lp', 'lp', 'lp','mc6', 'mc7', 'mc8', 'cbec','cbec', 'qrec',  'qrec', 'mv'),
                          plant=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp1', 'lp2', 'lp3', 'lp4', 'mc6', 'mc7', 'mc8', 'cbec1', 'cbec2', 'qrec1', 'qrec2', 'mv')
                          )


    ## Pull Data
if(strMethod==1)
{
      rLog(paste("Start tsdb pulls at", Sys.time()))
      dfGenRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfGenMap[,2], hourconvention=1)
      dfUdbpRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfUdbpMap[,2], hourconvention=1)
      dfRstRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfRstMap[,2], hourconvention=1)
        rLog(paste("Start dfLmpRaw at", Sys.time()))
      dfLmpRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfLmpMap[,2], hourconvention=1)
        rLog(paste("end dfLmpRaw at", Sys.time())); flush.console()
      dfSppRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfSppMap[,2], hourconvention=0)
        rLog(paste("end dfSppRaw at", Sys.time())); flush.console()
}
if(strMethod==2)
{
        rLog(paste("Start tsdb at", Sys.time()))   ; flush.console()
      dfGenRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfGenMap[,2], hourconvention=1)
      dfUdbpRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfUdbpMap[,2], hourconvention=1)
      dfRstRaw=pull.tsdb(startdate=startDate, lmtgdate=lmtgDate, symbolst=dfRstMap[,2], hourconvention=1)
        rLog(paste("Start dfLmpRaw at", Sys.time())); flush.console()
        rLog(paste("Querying LMP table for date>=",startDate,as.Date(startDate)))
              		strQry=paste("select RT.opr_date, rt.opr_hour HE, opr_minute, opr_second, RT.settlement_point sp, avg(rt.lmp) rtspp, count(rt.lmp)
                        from ZE_DATA.ercot_nodal_rt_lmp_re_node RT
                        where rt.opr_date>=to_date('",as.Date(startDate),"', 'YYYY-MM-DD')
                        and RT.settlement_point in ('",paste(dfLmpMap[,2], collapse="','"),"') 
                        group by RT.opr_date, rt.opr_hour, opr_minute, opr_second, RT.settlement_point  order by 1,2,3",sep="")
            			lmps <- sqlQuery(channel_zema_prod, strQry);
            		rLog(paste("lmps rows=",nrows(lmps))); 	
            		if(nrows(lmps)=1) rLog(lmps)
        lmps$min=round(lmps$OPR_MINUTE+lmps$OPR_SECOND/100,0)
        rLog(lmps$OPR_DATE[1])
        rLog((lmps$HE)*60*60[1])
        rLog(lmps$min*60[1])
        dfLmpRaw=data.frame(dt_start=(lmps$OPR_DATE+(lmps$HE)*60*60+lmps$min*60), lmp=lmps$SP, value=lmps$RTSPP, min=lmps$min)
        rLog(paste("end dfLmpRaw at", Sys.time())); flush.console()
      # dfSppRaw=FTR:::FTR.load.tsdb.symbols(dfSppMap[,2], startDate, lmtgDate, rep(FALSE, length(dfLmpMap[,2])))
        rLog(paste("end dfLmpRaw at", Sys.time())); flush.console()
              		strQry=paste("select RT.opr_date, rt.opr_hour HE, interval, RT.sp sp, avg(rt.value) rtspp, count(rt.value)
                        from ZE_DATA.ERCOT_NODAL_REAL_TIME_SPPS RT
                        where rt.opr_date>=to_date('",as.Date(startDate),"', 'YYYY-MM-DD')
                        and RT.sp in ('",paste(dfSppMap[,2], collapse="','"),"')  and SP_TYPE!='LZ'
                        group by RT.opr_date, rt.opr_hour, interval, RT.sp, rt.value order by 1,2,3",sep="")
            			spps <- sqlQuery(channel_zema_prod, strQry);
         dfSppRaw=data.frame(dt_he=(spps$OPR_DATE+(spps$HE-1)*60*60+spps$INTERVAL*15*60), spp=spps$SP, value=spps$RTSPP)
        rLog(paste("end dfSppRaw at", Sys.time())); flush.console()

}



    ## Correct and consolidate plant-spescific data
      # Only do data consolidation on data after whichMinuteToStart - 1 hour (as have errors with start of an hour)

      ## Gen table
            dfGen=dfGenRaw
            colnames(dfGen)[match("variable",colnames(dfGen))]="gen"
            dfGen=merge(dfGen, dfGenMap, all=TRUE)
            dfGen=merge(dfGen, dfGenName, all=TRUE)
            dfGen$variable="gen_mw"
            dfGen2=cast(dfGen, plant+dt_start+OPR_DATE+HE+fiveminbegin~variable, sum)
            if(sum(dfGen[is.na(dfGen$value)==FALSE,"value"]) != sum(dfGen2[is.na(dfGen2$gen_mw)==FALSE,"gen_mw"])) rLog("Error: The Gen actual MW is not correctly matching expected.")
            dfGen=dfGen2
      ## Udbp table
            dfUdbp=dfUdbpRaw
            colnames(dfUdbp)[match("variable",colnames(dfUdbp))]="udbp"
            dfUdbp=merge(dfUdbp, dfUdbpMap, all=TRUE)
            dfUdbp$variable="udbp_mw"
            dfUdbp=cast(dfUdbp, plant+dt_start+OPR_DATE+HE+fiveminbegin~variable, sum)
      ## RST table
            dfRst=dfRstRaw
            colnames(dfRst)[match("variable",colnames(dfRst))]="rst"
            dfRst=merge(dfRst, dfRstMap, all=TRUE)
            dfRst$variable="rst_code"
            dfRst=cast(dfRst, plant+dt_start+OPR_DATE+HE+fiveminbegin~variable, mean)
             

      ## Consolidate data tables
            dfConsol = merge(dfGen, dfUdbp, all=TRUE)
            dfConsol = merge(dfConsol, dfRst, all=TRUE)
       ## DF with 1 minute data
            dfTimes  <- data.frame(dt_start=seq(min(dfConsol$dt_start), by="1 min", length.out=(24*60+500)))
            dfTimes=merge(dfTimes, data.frame(plant=unique(dfGenName$plant))); colnames(dfTimes)[2]="plant"
            strMax=max(dfConsol$dt_start); strMin=min(dfConsol$dt_start)
            dfTimes=subset(dfTimes, dt_start>=strMin & dt_start<=strMax)           
      ## Merge with data
            dfConsolt=merge(dfConsol, dfTimes, all=TRUE)
            ##  df=subset(dfConsolt,plant=="wh");  df=df[order(df$dt_end),];  df[1:20,]
            
                                             
             
              #  ## Testing--delete later
              #      i=6
              #      df=dfConsol[is.na(dfConsol[i])==FALSE,]
              #      print(colnames(df[i]))
              #      tapply(df[,5], list(df$plant, df$HE), length)      
              #      i=i+1
              #      print(colnames(df[i]))
              #      df=dfConsol[is.na(dfConsol[i])==FALSE,]
              #      tapply(df[,5], list(df$plant, df$HE), length)      
              #      i=i+1
              #      print(colnames(df[i]))
              #      df=dfConsol[is.na(dfConsol[i])==FALSE,]
              #      tapply(df[,5], list(df$plant, df$HE), length)      
              #




      ## Get and format LMPs and SPPs

           ## LMP table
            dfLmp=dfLmpRaw
            dfLmp=merge(dfLmp, dfLmpMap, all=TRUE)
            dfLmp=merge(dfLmp, dfLmpName, all=TRUE)
            dfLmp=data.frame(plant=dfLmp$plant, dt_start=dfLmp$dt_start, lmp=dfLmp$value, min=dfLmp$min)

          ## Convert from minute beginning to ending
            # dfLmp$dt_end_raw=dfLmp$dt_start+60

          ## Check if there are missing LMPs--e.g. too long between LMPs
#            dfLmpCheck=dfLmp
#            dfLmpCheck=dfLmpCheck[order(dfLmpCheck$dt_start),]
#            dfLmpCheck=dfLmpCheck[order(dfLmpCheck$plant),]
#            dfLmpCheck$check=dfLmpCheck$dt_start[1]
#            dfLmpCheck$check[2:(nrow(dfLmpCheck))]=dfLmpCheck$dt_start[1:(nrow(dfLmpCheck)-1)]
#            dfLmpCheck$value=dfLmpCheck$dt_start-dfLmpCheck$check
#            dfLmpCheckTest=cast(dfLmpCheck, dt_start~plant, max)
#            strMax=max(dfLmpCheckTest[2:nrow(dfLmpCheckTest),2:ncol(dfLmpCheckTest)])
#            ## Send error message if more than six minutes between lmps
#            if((strMax/60)>7 & (strMax/60)<10 & !ignoreLmpErrors)
#              {
#                strDt=subset(dfLmpCheckTest, cbec1==strMax|cbec2==strMax|qrec2==strMax|mv==strMax|ftr==strMax|han4==strMax, select="dt_start")
#                rLog(paste("Finding that lmps that are more than 7 minutes apart.  Are LMPs missing?  Strange date is ", strDt[1,1],sep=""))
#              }
#            if((strMax/60)>=10 & !ignoreLmpErrors)
#              {
#                strDt=subset(dfLmpCheckTest, cbec1==strMax|cbec2==strMax|qrec2==strMax|mv==strMax|ftr==strMax|han4==strMax, select="dt_start")
#                rLog(paste("Finding that lmps that are more than 10 minutes apart.  Are LMPs missing?  Strange date is ", strDt[1,1],sep=""))
#                sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
#                    subject=paste("LMPs in ercot_upload_gen_actuals are more than 10 minutes apart.", sep=""),
#                    body=paste("Finding that lmps that are more than 10 minutes apart.  Are LMPs missing?  Strange date is ", strDt[1,1],sep=""))
#                return(paste("Finding that lmps that are more than 10 minutes apart.  Are LMPs missing?  Strange date is ", strDt[1,1],sep=""))                    
#              }
#
          ## Round to minutes to be able to merge with consolidated table
            # dfLmp$dt_start_raw=dfLmp$dt_start
            # dfLmp$dt_start=round(dfLmp[,"dt_start_raw"],"mins")

          ## Round to fifteen minutes to combine with SPP table
            timeMap=data.frame(min=seq(0,59),fiftnminend=c(rep(15,15),rep(30,15), rep(45,15), rep(60,15)))
            dfLmp=merge(dfLmp, timeMap, all.x=TRUE)
            dfLmp$dt_end=dfLmp$dt_start+(dfLmp$fiftnminend-dfLmp$min)*60
            dfLmp=dfLmp[,c("plant","dt_start","dt_end", "lmp")]
            # dfLmp$dt_he=round(dfLmp$dt_start-(60*30),"hour")+dfLmp$fiftnminend*60


          ## SPP table
            dfSpp=dfSppRaw
            dfSpp=merge(dfSpp, dfSppMap, all=TRUE)
            dfSpp=merge(dfSpp, dfSppName, all=TRUE)
            dfSpp=data.frame(plant=dfSpp$plant, dt_end=dfSpp$dt_he, spp=dfSpp$value)

          ## Combine SPP and LMP tables
            dfP=merge(dfLmp, dfSpp, all=TRUE)
            dfP=merge(dfP, dfTimes, all=TRUE)        


   ########
   ## Merge LMPs with Consolidated data
             dfConsol = merge(dfConsolt, dfP[,c("dt_start","plant","lmp","spp","dt_end")], all=TRUE)
     

          ## Fill minutes with missing LMPs
            dfConsol=dfConsol[order(dfConsol$dt_start),]
            dfConsol=dfConsol[order(dfConsol$plant),]
            # lmp
              dfConsol$lmpC=na.locf(dfConsol$lmp, na.rm=FALSE)
            # gen
              dfConsol$gen1=na.locf(dfConsol$gen_mw, na.rm=FALSE)  
              dfConsol$gen2=na.locf(dfConsol$gen_mw, na.rm=FALSE, fromLast=TRUE)  
              dfConsol$genC=dfConsol$gen1/2 + dfConsol$gen2/2
              dfConsol$gen1=NULL; dfConsol$gen2=NULL
            # udbp
              dfConsol$udbp1=na.locf(dfConsol$udbp_mw, na.rm=FALSE)  
              dfConsol$udbp2=na.locf(dfConsol$udbp_mw, na.rm=FALSE, fromLast=TRUE)  
              dfConsol$udbpC=dfConsol$udbp1/2 + dfConsol$udbp2/2
              dfConsol$udbp1=NULL; dfConsol$udbp2=NULL
            # rst
            dfConsol$rstC=na.locf(dfConsol$rst_code, na.rm=FALSE)      
            # spp
              dfConsol$sppC=na.locf(dfConsol$spp, na.rm=FALSE)
                      
    ## Remove early data that may have incorrectly filled LMPs
        # dfConsol = dfConsol[dfConsol$dt_start>=min(dfConsol$dt_start+60*60)&dfConsol$dt_start<=max(dfConsol$dt_start-60*60), ]


    ## Return dataframe           
                  
       return(dfConsol)     

  }




modifyData = function (dfRes)
  {
    ## Save SPPs
      dfSpp=dfRes
      colnames(dfSpp)[match("sppC",colnames(dfSpp))]="value"
      dfSpp=dfSpp[is.na(dfSpp$value)==FALSE,]
      dfSpp$variable="spp"
      dfSpp$dt_he=dfSpp$OPR_DATE+dfSpp$HE*60*60
      dfSpp=cast(dfSpp, dt_he+plant~variable, mean) 



#    ## MW output is limited to max(udbp*105%, udbp+5)    
#        dfRes$gen_mw_limited=NA
#        dfRes$gen_mw_limited = apply(dfRes, 1, function(x) {min(as.numeric(x["gen_mw"]), max(as.numeric(x["udbp_mw"])*1.05, as.numeric(x["udbp_mw"])+5))})
#        dfRes$bp_dev_mw=dfRes$gen_mw - dfRes$gen_mw_limited
#
#    ## No bd_dev_mw when certain RST codes
#where did MV go???    
#        df1=subset(dfRes, rst_code==)
#
#
#    ## Weight basepoint by LMPs--NO need to sum up total dollars received in an hour 
#        dfRes$value=dfRes$gen_mw_limited*dfRes$lmpCorrected
#        dfRes$variable="prod"
#        dfRes1=cast(dfRes, plant+OPR_DATE+HE~variable, sum)
#
#        dfRes$value=dfRes$lmpCorrected
#        dfRes$variable="lmps"
#        dfRes2=cast(dfRes, plant+OPR_DATE+HE~variable, sum)
#
#      ## What happens if there are NAs???
#        dfRes2=merge(dfRes1, dfRes2)
#        dfRes2$mw=dfRes2$prod/dfRes2$lmps
#        ## Check results:  write.csv(dfRes2, "S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")


    ## LMP-weighted gen output - ignore BPD charges as don't have all data necessary
    for (planti in unique(dfRes$plant))
      {
        # planti=unique(dfRes$plant)[1]
        rLog(paste("/t Hourly average for", planti))
        dfResi=subset(dfRes, plant==planti)
        dfResi=dfResi[is.na(dfResi$genC)==FALSE,]
        dfResi=dfResi[order(dfResi$dt_start),]
        # dfResi$int_start=dfResi$dt_start
        # dfResi$int_start[2:nrow(dfResi)]=dfResi$dt_end[1:(nrow(dfResi)-1)]
        # dfResi$time=dfResi$dt_end-dfResi$int_start                                  
        dfResi$timediff=1;  dfResi$timediff[1:(nrow(dfResi)-1)]=dfResi$dt_start[2:(nrow(dfResi))]-dfResi$dt_start[1:(nrow(dfResi)-1)]
        dfResi$revenue=dfResi$genC*dfResi$lmpC*as.numeric(dfResi$timediff)/60
        if(planti==unique(dfRes$plant)[1]) dfResC=dfResi else dfResC=rbind(dfResC, dfResi)
       
      }

     # Get hourly revenues, avg gen, &
       dfResC$dt=as.POSIXct(round(dfResC$dt_start-60*60*12,"day"))
       dfResC$dt_he=as.POSIXct(round(dfResC$dt_start+60*30,"hour"))
       dfRes1=aggregate( dfResC[,c("genC")], by=list(dfResC$plant, dfResC$dt_he), mean)
       colnames(dfRes1)=c("plant","dt_he","gen")
       dfRes2=aggregate( dfResC[,c("revenue")], by=list(dfResC$plant, dfResC$dt_he), sum)
       colnames(dfRes2)=c("plant","dt_he","rev")
       dfRes3=aggregate( dfResC[,c("timediff")], by=list(dfResC$plant, dfResC$dt_he), sum)
       colnames(dfRes3)=c("plant","dt_he","timediff")
       dfRest=merge(dfRes1, dfRes2)
       dfRest=merge(dfRest, dfRes3)
       dfRest=merge(dfRest, dfSpp)
       dfRest$genLmp=(dfRest$rev/dfRest$spp)*60/dfRest$timediff
        return(dfRest)     
       # dfRest$chk=dfRest$spp*dfRest$gen; dfRest$chk2=round(dfRest$chk-dfRest$rev,0)

    
  }





#store.bentek.data.tx = function(startDate, endDate, conn)
#    {
#     rLog("Inside store.bentek.data.tx function")
#    strQry=paste("select flows.measurement_date, flows.nomination_cycle, sum(flows.scheduled_volume)/1000 flows
#                  from bentek.flowdata flows, bentek.points points
#                  where flows.point_id=points.point_id
#                  and measurement_date>=to_date('",startDate,"', 'YYYY-MM-DD') and measurement_date<=to_date('",endDate,"', 'YYYY-MM-DD')
#                  and points.btk_customer_type=3 and points.state='TX'
#                  group by flows.measurement_date, flows.nomination_cycle
#                  order by nomination_cycle, measurement_date", sep="")
#		nomsraw <- sqlQuery(conn, strQry);
#     rLog("Finished SQL query")
#
#    ## Return empty if cannot find data
#      if(nrow(subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(startDate),"day") & NOMINATION_CYCLE=='I2'))==0) {rLog(paste("Did not find data for desired date of ",startDate,sep="")); return(); break; stop}
#     rLog(paste("Collected ", nrow(subset(nomsraw, MEASUREMENT_DATE>=round(as.POSIXct(startDate),"day"))), " rows of data", sep=""))
#
#    ## Upload pm_ercot_bentek_texas_i2
#      symbol <- "pm_ercot_bentek_texas_i2"
#      dfNoms  <- data.frame(time=seq(round(as.POSIXct(startDate),"day"), by="1 hour", length.out=25),
#                            value=subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(startDate),"day") & NOMINATION_CYCLE=='I2')[,"FLOWS"])[2:25,]
#      rLog("Made dataframe")
#      tsdbUpdate(symbol, dfNoms)
#      rLog(paste("Tried to upload I2 data to TSDB for date",round(dfNoms[1,1],"day")," --success uncertain",sep=""))
#
#    ## Upload pm_ercot_bentek_texas_e
#      symbol <- "pm_ercot_bentek_texas_e"
#      dfNoms  <- data.frame(time=seq(round(as.POSIXct(endDate),"day"), by="1 hour", length.out=25),
#                            value=subset(nomsraw, MEASUREMENT_DATE==round(as.POSIXct(endDate),"day") & NOMINATION_CYCLE=='E')[,"FLOWS"])[2:25,]
#      tsdbUpdate(symbol, dfNoms)
#      rLog(paste("Tried to upload E data to TSDB for date",round(dfNoms[1,1],"day")," --success uncertain",sep=""))
#
#    }





saveCsv = function (dfRes2)
  {
          dfRes2$dt=round(dfRes2$dt_he-60*60*12.1,"day")
          # strFile="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Gen Schedule/actuals_2013.csv"
          filestart="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily Positions/genact/gen_actuals_"
          fileend=".csv"     
          dtsdf=unique(dfRes2$dt)
          for (i in  1:length(dtsdf))
            {
              filesave=paste(filestart, dtsdf[i], fileend, sep="")
                dfRes2i=subset(dfRes2, dt==dtsdf[i])
                dfRes2i$value=round(dfRes2i$genLmp,1)
                dfRes2i=cast(dfRes2i, dt_he~plant, mean)
                write.csv(dfRes2i, file=filesave); 
            }
  }




uploadData = function (dfRes2)
  {
    ## Upload data
  dfRes2=dfRes2[is.na(dfRes2$genLmp)==FALSE,]
      arrUnits=unique(dfRes2$plant)
      for (i in 1:length(arrUnits))
        {
          symbol <- paste("pm_ercot_gen_lmpwt_", arrUnits[i], sep="")
          dfUpload  <- subset(dfRes2, plant==arrUnits[i])
          dfUpload  <- data.frame(time=dfUpload$dt_he,
                                value=dfUpload$genLmp)
          tsdbUpdate(symbol, dfUpload)          
        }
      rLog(paste("Attempted to upload ",nrow(dfUpload)," hours of gen actuals to TSDB COP gen outages.",sep=""))
  }      



