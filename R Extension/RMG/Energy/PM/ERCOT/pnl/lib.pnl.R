# Library of functions for PNL calculations
# Used by Energy/PM/ERCOT/pnl/main.dam.R
#
#
# Written by Chelsea Yeager & Lori Simpson on 12-Jul-2011




#------------------------------------------------------------
# FUNCTIONS
#   get.dam.pnl(start_date, end_date, channel_nmktp, channel_teslap, channel_zema_prod)
#   get.dam.pnl(start_date='2011-06-01', end_date='2011-06-30', connnmkt=channel_nmktp, conntesla=channel_teslap, connzema=channel_zema_prod)
#   start_date='2011-01-01'; end_date='2011-03-31'; connnmkt=channel_nmktp; conntesla=channel_teslap; connzema=channel_zema_prod; qse='CPSA'; ptpscheck=1
#

get.dam.pnl = function(start_date, end_date, connnmkt, conntesla, connzema, qse, ptpscheck)
  {

    ## Modify dates
      start_dt_1d=as.Date(start_date)-1
      end_dt_1d=as.Date(end_date)+2
      if(end_date>=Sys.Date()) {rLog("Silly--you can't run this report current or future days.  Decrease your end date!"); returnrLog("Silly--you can't run this report current or future days.  Decrease your end date!")}
   
   
   ## Get the PTPs from NMarket
        strQry = paste("select F.SCHD_TYPE_cd, x1.xp_nme source, x2.xp_nme sink, nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', f.end_dt_gmt, 'CPT', 1) HE,
                        f.client_bid_ref, f.status_cd, f.award_mw, f.award_price, ps_ind, f.hedge_cd
                        from nmarket.nm_schedule_ftr f, nmarket.nm_xp x1, nmarket.nm_xp x2
                        where f.source_xp_id = x1.xp_id and f.sink_xp_id = x2.xp_id and f.schd_type_cd = 'AWD'
                        and f.start_dt_gmt>=to_date('",start_dt_1d,"', 'YYYY-MM-DD') and f.start_dt_gmt<=to_date('",end_dt_1d,"', 'YYYY-MM-DD')
                        and f.clrg_cd='DA' and f.status_cd in ('SUB','SUBC') and ptcpt_cd='",qse,"'and 
                        ((award_mw=0 and HEDGE_CD='OPT') or (award_mw>0 and HEDGE_CD='OBL')) ;", sep="")
                        ## cat(strQry)  ## Look at the query in a nice way
        arrPtps = sqlQuery(connnmkt, strQry);  
      ## Check that any data was returned
                  ## str(arrPtps); head(arrPtps)  ## Check out the data that has returned
        if(nrow(arrPtps)>0) {rLog("Successfully loaded PTP data from NMarket")}
        if(ptpscheck==1) if(nrow(arrPtps)==0) {rLog("Returned 0 rows of PTP data from NMarket.  Check there were no PTPs?"); return("Returned 0 rows of PTP data from NMarket.")}
        if(ptpscheck==0) {rLog("You have chosent to ignore that there are no PTP awards being returned."); if(nrow(arrPtps)>0) {rLog("Cannot set ptpscheck==0 if there were PTPs awarded."); return("Cannot set ptpscheck==0 if there were PTPs awarded.")}}
   if(ptpscheck==1) 
    {
      ## Check that correct dates of data returned
                  ## min(arrPtps$HE);   max(arrPtps$HE);
        arrPtpsCut = subset(arrPtps, HE>as.POSIXct(start_date) & HE<=(as.POSIXct(end_date)+1*24*60*60) )
                  ## min(arrPtpsCut$HE);  max(arrPtpsCut$HE)  
                  
      ## N-Market has duplicate data--we only want unique data
        arrPtpsUnique = aggregate( arrPtpsCut[,c("AWARD_MW","AWARD_PRICE")], by=list(arrPtpsCut$HE, arrPtpsCut$CLIENT_BID_REF, arrPtpsCut$HEDGE_CD, arrPtpsCut$SOURCE, arrPtpsCut$SINK), max)
        colnames(arrPtpsUnique)[1:6]=c("he","BID_ID","type", "source", "sink", "mw")
        ##arrPtpsUnique=arrPtpsUnique[,c("he","BID_ID","type", "source", "sink", "mw")]
     }
   
   
  
  
   ## Get the EOBs from NMarket
        strQry = paste("select s.schd_type_cd, xp.xp_nme, nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', s.end_dt_gmt, 'CPT', 1) HE, s.schd_nme, s.status_cd, s.quantity, s.price_amt, s.ps_ind, s.com_cd
                        from NMARKET.nm_schedule S, NMARKET.NM_XP XP where S.XP_ID=XP.XP_ID and s.schd_type_cd='AWD'
                        and s.start_dt_gmt>=to_date('",start_dt_1d,"', 'YYYY-MM-DD') and s.start_dt_gmt<=to_date('",end_dt_1d,"', 'YYYY-MM-DD')
                         and s.com_cd in ('INC','DEC')and abs(s.quantity)>0 and ptcpt_cd='",qse,"';", sep="")
                        ## cat(strQry)  ## Look at the query in a nice way
        arrEOB = sqlQuery(connnmkt, strQry);
        arrEOB$SOURCE = ""
        arrEOB$AWARD_PRICE = arrEOB$PRICE_AMT
        arrEOB$QUANTITY = arrEOB$QUANTITY*-1
        
      ## Check that any data was returned
                          ## str(arrEOB); head(arrEOB)  ## Check out the data that has returned
        if(nrow(arrEOB)>0) {rLog("Successfully loaded EOB data from NMarket")}
        if(nrow(arrEOB)==0) {rLog("Returned 0 rows of EOB data from NMarket."); return("Returned 0 rows of EOB data from NMarket.")}
      ## Check that correct dates of data returned
                  ## min(arrEOB$HE);   max(arrEOB$HE);
        arrEOBCut = subset(arrEOB, HE>as.POSIXct(start_date) & HE<=(as.POSIXct(end_date)+1*24*60*60) )
                  ## min(arrEOBCut$HE);  max(arrEOBCut$HE)
      ## N-Market has duplicate data--we only want unique data                                                                                                                                      
       arrEOBUnique = aggregate(arrEOBCut[,c("QUANTITY","AWARD_PRICE")], by=list(arrEOBCut$HE, arrEOBCut$SCHD_NME, arrEOBCut$COM_CD, arrEOBCut$SOURCE, arrEOBCut$XP_NME), max)                               
       colnames(arrEOBUnique)[1:6]=c("he","BID_ID","type", "source", "sink", "mw")                                                                                                                                         
        ##arrEOBUnique=arrEOBUnique[,c("he","BID_ID","type", "source", "sink", "mw")]                                                                                             

  
  
   ## Aggregate PTPs & EOB data into one table
          if(ptpscheck==1)  arrTotalUniqueSub = rbind(arrPtpsUnique, arrEOBUnique) else arrTotalUniqueSub = arrEOBUnique
     ## Get the Tesla-created bids
        arrTotalUniqueSub$Letter = substring(as.character(arrTotalUniqueSub$BID_ID), 1, 1)
        arrTotalUniqueAuto = arrTotalUniqueSub[grep("[[:digit:]]", arrTotalUniqueSub$Letter), ]
     ## Get the bids directly made in the portal
        CREATED_BY = data.frame(read.table(file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/pnl/CVS/eNumber Table.csv", header = TRUE, sep = ","))
        arrTotalUniqueManual = merge(arrTotalUniqueSub, CREATED_BY, by="Letter")
        if(nrow(arrTotalUniqueManual)>0)
          {
            print("NEED TO COMPLETE THE MANUAL BIDS CODING!!!!!"); return("NEED TO COMPLETE THE MANUAL BIDS PART!!!!!")
            arrTotalUniqueManual$BID_ID = ""
            arrFinalManual = aggregate(arrTotalUniqueManual[,c("mw")], by=list(arrTotalUniqueManual$he, arrTotalUniqueManual$source, arrTotalUniqueManual$sink, arrTotalUniqueManual$CREATED_BY, arrTotalUniqueManual$BID_ID, arrTotalUniqueManual$type), max)
            colnames(arrFinalManual)=c("he","source", "sink", "creator","id","type", "mw") 
          }



  ## Get the person who submitted the ptps
        strQry = paste("select a.bid_id, a.created_by, a.bid_type_id, count(bid_id)  from  ERCOT.NDL_BID_TYPES b, ERCOT.NDL_BID_SUMMARY a
                          WHERE a.BID_TYPE_ID=b.bid_type_id  AND (b.BID_TYPE_ID=9 or b.BID_TYPE_ID=42 or b.BID_TYPE_ID=10) 
                          AND BID_DATE>=to_date('",start_date,"', 'YYYY-MM-DD') AND BID_DATE<=to_date('",end_date,"', 'YYYY-MM-DD')
                          group by a.bid_id, a.created_by, a.bid_type_id", sep="")
                        ## cat(strQry)  ## Look at the query in a nice way
        arrPtpPerson = sqlQuery(conntesla, strQry);
    ## Merge Tesla and N-Market data
        arrTotal = merge(arrTotalUniqueAuto,arrPtpPerson[,c(1:3)], all.x=TRUE)
        if(nrow(arrTotal[is.na(arrTotal$CREATED_BY)==TRUE,])>0 & ptpscheck==1) {rLog("Could not find a creator for all PTP bids in Tesla"); return("Could not find a creator for all PTP bids in Tesla")}
        if(nrow(arrTotal[is.na(arrTotal$CREATED_BY)==TRUE,])==0)  {rLog("Successfully merged N-Market and Tesla data.")}




  ## Get the total position by source/sink for an hour (e.g. sum if have more than one PTP for a given path for a given hour
        if(ptpscheck==0) {arrTotal$CREATED_BY=0; arrTotal$BID_TYPE_ID=0}
        arrFinalAuto = aggregate(arrTotal[,c("mw")], by=list(arrTotal$he, arrTotal$source, arrTotal$sink, arrTotal$CREATED_BY, arrTotal$BID_TYPE_ID, arrTotal$type), sum)
        colnames(arrFinalAuto)=c("he","source", "sink", "creator","id","type", "mw")
        if(is.na(match("arrFinalManual",ls()))) arrFinal = arrFinalAuto else arrFinal = rbind(arrFinalManual, arrFinalAuto)

 
 
  ## Get the Prices
    ## Find the locations that we need prices for
        sp = data.frame(sp = c(arrFinalAuto$source, arrFinalAuto$sink))
        sp = data.frame(unique(sp$sp)); sp=subset(sp, sp!="")
        sp = sp[,1]
    ## Get the Zema data - uses pull.lmps function found in H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R
        arrSPPs = pull.spps(startdate=as.Date(start_date), lmtgdate=as.Date(end_date), arrSPs=sp, connzema)
        arrSPPs$he = arrSPPs$OPR_DATE + arrSPPs$HE*60*60
    ## Average Source RT LMPs over an hour
        arrSPPsMean = aggregate (arrSPPs[,c("RTSPP", "DASPP")], by=list(arrSPPs$he, arrSPPs$SP), mean)
        colnames(arrSPPsMean) = c("he", "source", "sourceRTSPP", "sourceDASPP")
    ## Merge with source prices and check that have data for all sources
        ## Merge for non-null sources
          arrFinal1=subset(arrFinal, source!="")
          arrFinalPrice1 = merge(arrFinal1, arrSPPsMean, all.x=TRUE)
        ## Merge for null sources
          arrFinal2=subset(arrFinal, source=="")
          arrFinalPrice2 = arrFinal2
          arrFinalPrice2$sourceRTSPP=0
          arrFinalPrice2$sourceDASPP=0
        ## Combine null and non-null tables
          arrFinalPrice=rbind(arrFinalPrice1, arrFinalPrice2)
          if(nrow(arrFinalPrice[is.na(arrFinalPrice$sourceRTSPP)==TRUE,])>0 ) {arrTest=arrFinalPrice[is.na(arrFinalPrice$sourceRTSPP)==TRUE,]; rLog(paste("Could not retrieve RT SPPs from for all settlement points and dates from ZE_DATA.Ercot_Nodal_Real_Time_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")); return(rLog(paste("Could not retrieve RT SPPs from for all settlement points and dates from ZE_DATA.Ercot_Nodal_Real_Time_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")))}
          if(nrow(arrFinalPrice[is.na(arrFinalPrice$sourceDASPP)==TRUE,])>0 ) {arrTest=arrFinalPrice[is.na(arrFinalPrice$sourceDASPP)==TRUE,]; rLog(paste("Could not retrieve DA SPPs from for all settlement points and dates from ZE_DATA.ERCOT_NODAL_DAM_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")); return(rLog(paste("Could not retrieve DA SPPs from for all settlement points and dates from ZE_DATA.ERCOT_NODAL_DAM_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")))}
          rLog("Successfully merged PTP MWs with source LMPs.")
    ## Merge with sink prices and check that have data for all sinks
        colnames(arrSPPsMean) = c("he", "sink", "sinkRTSPP", "sinkDASPP")
        arrFinalPrice = merge(arrFinalPrice, arrSPPsMean, all.x=TRUE)
        if(nrow(arrFinalPrice[is.na(arrFinalPrice$sinkRTSPP)==TRUE,])>0 ) {arrTest=arrFinalPrice[is.na(arrFinalPrice$sinkRTSPP)==TRUE,]; rLog(paste("Could not retrieve RT SPPs from for all settlement points and dates from ZE_DATA.Ercot_Nodal_Real_Time_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$sink),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")); return(rLog(paste("Could not retrieve RT SPPs from for all settlement points and dates from ZE_DATA.Ercot_Nodal_Real_Time_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")))}
        if(nrow(arrFinalPrice[is.na(arrFinalPrice$sinkDASPP)==TRUE,])>0 ) {arrTest=arrFinalPrice[is.na(arrFinalPrice$sinkDASPP)==TRUE,]; rLog(paste("Could not retrieve DA SPPs from for all settlement points and dates from ZE_DATA.ERCOT_NODAL_DAM_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$sink),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")); return(rLog(paste("Could not retrieve DA SPPs from for all settlement points and dates from ZE_DATA.ERCOT_NODAL_DAM_SPPS.\n  Missing SPs:\n",paste(unique(arrTest$source),collapse=", "),"Missing dates:\n",paste(unique(arrTest$he),collapse=", "),sep="")))}
        rLog("Successfully merged PTP MWs with sink LMPs.")
        

        
        
    ## Pull forward marks - for valuation date equal to three business days prior to start of month
        
        
 
 
  ## Calculate DA/RT P&L
        arrFinalPrice$RTspread = arrFinalPrice$sinkRTSPP - arrFinalPrice$sourceRTSPP
        arrFinalPrice$DAspread = arrFinalPrice$sinkDASPP - arrFinalPrice$sourceDASPP
        arrFinalPrice$RTaward = (arrFinalPrice$mw) * arrFinalPrice$RTspread
        arrFinalPrice$DAaward = (arrFinalPrice$mw) * arrFinalPrice$DAspread
        arrFinalPrice$DARTpnl = arrFinalPrice$RTaward - arrFinalPrice$DAaward
        write.csv(arrFinalPrice, file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/pnl/Output/PTP_EOBperson.csv")

        return(arrFinalPrice)

  }