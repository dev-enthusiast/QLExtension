# Library of functions for PNL calculations
# Used by Energy/PM/ERCOT/pnl/main.dam.R
#
#
# Written by Chelsea Yeager & Lori Simpson on 12-Jul-2011




#------------------------------------------------------------
# FUNCTIONS
#   date.function(start_date, end_date, channel_nmktp, channel_teslap)
#   date.function(start_dt='2011-06-01', end_dt='2011-06-02', connnmkt=channel_nmktp, conntesla=channel_teslap)
#   start_dt='2011-06-01'; end_dt='2011-06-02'

get.ptp.mws = function(start_dt, end_dt, connnmkt, conntesla)
  {

    ## Modify dates
      start_dt_1d=as.Date(start_dt)-1
      end_dt_1d=as.Date(end_dt)+2
      
 ## Get the PTPs from NMarket
        strQry = paste("select F.SCHD_TYPE_cd, x1.xp_nme source, x2.xp_nme sink, nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', f.end_dt_gmt, 'CPT', 1) HE,
                        f.client_bid_ref, f.status_cd, f.award_mw, f.award_price, ps_ind, f.hedge_cd
                        from nmarket.nm_schedule_ftr f, nmarket.nm_xp x1, nmarket.nm_xp x2
                        where f.source_xp_id = x1.xp_id and f.sink_xp_id = x2.xp_id and f.schd_type_cd = 'AWD'
                        and f.start_dt_gmt>=to_date('",start_dt_1d,"', 'YYYY-MM-DD') and f.start_dt_gmt<=to_date('",end_dt_1d,"', 'YYYY-MM-DD')
                        and f.clrg_cd='DA' and f.status_cd in ('SUB','SUBC') and award_mw>0;", sep="")
                        ## cat(strQry)  ## Look at the query in a nice way
        arrPtps = sqlQuery(connnmkt, strQry);
      ## Check that any data was returned
                          ## str(arrPtps); head(arrPtps)  ## Check out the data that has returned
        if(nrow(arrPtps)>0) {rLog("Successfully loaded PTP data from NMarket")}
        if(nrow(arrPtps)==0) {rLog("Returned 0 rows of PTP data from NMarket."); return("Returned 0 rows of PTP data from NMarket.")}
      ## Check that correct dates of data returned
                  ## min(arrPtps$HE);   max(arrPtps$HE);
        arrPtpsCut = subset(arrPtps, HE>as.POSIXct(start_dt) & HE<=(as.POSIXct(end_dt)+1*24*60*60) )
                  ## min(arrPtpsCut$HE);  max(arrPtpsCut$HE)

      ## N-Market has duplicate data--we only want unique data
        arrPtpsUnique = aggregate( arrPtpsCut[,c("AWARD_MW","AWARD_PRICE")], by=list(arrPtpsCut$HE, arrPtpsCut$CLIENT_BID_REF, arrPtpsCut$HEDGE_CD, arrPtpsCut$SOURCE, arrPtpsCut$SINK), max)
        colnames(arrPtpsUnique)[1:6]=c("he","BID_ID","type", "source", "sink", "mw")



 ## Get the person who submitted the ptps
        strQry = paste("select bid_id, created_by, count(bid_id)  from  ERCOT.NDL_BID_TYPES b, ERCOT.NDL_BID_SUMMARY a
                          WHERE a.BID_TYPE_ID=b.bid_type_id  AND (b.BID_TYPE_ID=9)
                          AND BID_DATE>=to_date('",start_dt,"', 'YYYY-MM-DD') AND BID_DATE<=to_date('",end_dt,"', 'YYYY-MM-DD')
                          group by bid_id, created_by", sep="")
                        ## cat(strQry)  ## Look at the query in a nice way
        arrPtpPerson = sqlQuery(conntesla, strQry);

 ## Merge Tesla and N-Market data
        arrPtp = merge(arrPtpsUnique, arrPtpPerson[,c(1:2)], all.x=TRUE)
        if(nrow(arrPtp[is.na(arrPtp$CREATED_BY)==TRUE,])>0) {rLog("Could not find a creator for all PTP bids in Tesla"); return("Could not find a creator for all PTP bids in Tesla")}
        if(nrow(arrPtp[is.na(arrPtp$CREATED_BY)==TRUE,])==0)  {rLog("Successfully merged N-Market and Tesla data.")}

      ## Get the total position by source/sink for an hour (e.g. sum if have more than one PTP for a given path for a given hour
        arrPtps = aggregate(arrPtp[,c("mw")], by=list(arrPtp$he, arrPtp$source, arrPtp$sink, arrPtp$CREATED_BY, arrPtp$type), sum)
        colnames(arrPtps)=c("he","source", "sink", "creator","type", "mw")




## DLT BELOW
        str(arrPtpsUnique)

        arrPtpsCut$variable="AWARD_MW"; colnames(arrPtpsCut)[match("AWARD_MW",colnames(arrPtpsCut))]="value"
        arrTest=cast(arrPtpsCut, CLIENT_BID_REF+HE~variable, sd)
        unique(arrTest$AWARD_MW)
        str(arrPtpsCut)

head(arrPtpsCut); arrPtpsCut$variable="count"; arrPtpsCut$value=1
arrTest=cast(arrPtpsCut, CLIENT_BID_REF+HE~variable, sd)

 subset (arrPtpsCut, HE == as.POSIXct('2011-06-02 10:00:00') & CLIENT_BID_REF==27192)
## DELETE ABOVE


      ## Return data
        return(arrPtpsCut)



  }

#
#    --GET INC/DEC AWARDS
#          select xp.xp_nme, s.quantity, s.xp_id, to_char(nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', s.end_dt_gmt, 'CPT', 1),'mm/dd/yyyy hh24') HE, s.schd_type_cd, s.clrg_cd, s.com_cd, s.status_cd from NMARKET.NM_SCHEDULE S, NMARKET.NM_XP XP where S.XP_ID=XP.XP_ID and s.schd_type_cd='AWD'
#            AND s.COM_CD IN ('DEC','INC') and s.start_dt_gmt>='28-mar-11' and s.start_dt_gmt<='02-may-11';
#    --GET PTP AWARDS
#    select F.SCHD_TYPE_cd, x1.xp_nme source, x2.xp_nme sink, to_char(nmarket.xmkt_all_datetime.gmt_to_local('STRM', 'TX', f.end_dt_gmt, 'CPT', 1), 'mm/dd/yyyy hh24') HE,
#            f.client_bid_ref, f.status_cd, f.award_mw, f.award_price, ps_ind, f.hedge_cd
#            from nmarket.nm_schedule_ftr f, nmarket.nm_xp x1, nmarket.nm_xp x2
#            where f.source_xp_id = x1.xp_id and f.sink_xp_id = x2.xp_id and f.schd_type_cd = 'AWD'
#            and f.start_dt_gmt>='28-mar-11' and f.start_dt_gmt<='02-may-11' and f.clrg_cd='DA' and f.status_cd in ('SUB','SUBC') ;
#
#select AWARD_TYPE, CLEARING_CODE, COMMODITY_CODE, SCHEDULE_TYPE, SINK, TRANSACTION_POINT, TRADE_DATE
#from ERCOT.NDL_AWARDS_HEADER  WHERE TRADE_DATE='02-dec-10';
#
#

#
#        strQry=paste("select mt.opr_date, mt.opr_hour, value, mt.SP  FROM
#                      ZE_DATA.ERCOT_NODAL_MARKET_TOTALS mt
#                      where mt.opr_date=to_date('",start_date,"', 'YYYY-MM-DD') and type='S'", sep="")
#         damraw <- sqlQuery(conn, strQry); if(nrow(damraw)==0) {rLog("Returned 0 rows from ZE_DATA.ERCOT_NODAL_MARKET_TOTALS.  Perhaps results haven't been posted?"); return("Returned 0 rows from ZE_DATA.ERCOT_NODAL_MARKET_TOTALS.  Perhaps results haven't been posted?")}
#
