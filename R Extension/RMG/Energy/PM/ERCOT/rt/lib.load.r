# Code used for calculating backcast profile load for Stream & STX
#
#
# Written by Chelsea Yeager on 13-Mar-2012




#------------------------------------------------------------
# FUNCTIONS
#   get.load(start_date='2011-08-01', end_date='2011-08-02', connloadwiz=channel_loadwiz_prod)
#   start_date='2011-08-01'; end_date='2011-08-02'; connloadwiz=conlwiz; conncpsercot=concps

get.balance.account = function(start_date, end_date, connloadwiz)
  {

   ## Modify dates
      start_dt_1d=as.Date(start_date)-1
      end_dt_1d=as.Date(end_date)+2
      if(end_date>=Sys.Date()) {rLog("Silly--you can't run this report current or future days.  Decrease your end date!"); returnrLog("Silly--you can't run this report current or future days.  Decrease your end date!")}
   
   
   
   
   ## Get the Gen Data from CPS Ercot
      stream_query <- "select t.name, t.tsdb_reference, eb.month, eb.year, eb.bucket_id,eb.equiv_customer_count 
          from loadwiz.account_t t,loadwiz.equiv_customer_count_bucket_t eb
          where t.name like ('SSPE:%%PRODn')
          and t.account_id = eb.account_id
          and eb.year <= 2012
          and t.supplier_enddate >= SYSDATE order by t.name, eb.year, eb.month"

    data_stream <- sqlQuery(conlwiz, gsub("\n", "", stream_query))
    
    stx_query <- "select t.name, t.tsdb_reference, eb.month, eb.year, eb.bucket_id,eb.equiv_customer_count 
          from loadwiz.account_t t,loadwiz.equiv_customer_count_bucket_t eb
          where t.name like ('STX:%%PRODn')
          and t.account_id = eb.account_id
          and eb.year <= 2012
          and t.supplier_enddate >= SYSDATE order by t.name, eb.year, eb.month"

    data_stx <- sqlQuery(conlwiz, gsub("\n", "", stx_query))
    
    query <- "select t.name, t.tsdb_reference, eb.month, eb.year, eb.bucket_id,eb.equiv_customer_count 
          from loadwiz.account_t t,loadwiz.equiv_customer_count_bucket_t eb
          where t.name like ('SSPE:%%PRODn')
          and t.account_id = eb.account_id
          and eb.year <= 2012
          and t.supplier_enddate >= SYSDATE order by t.name, eb.year, eb.month"

    data_stream <- sqlQuery(con, gsub("\n", "", query))
               
    data = rbind(data_stream, data_stx)
         write.csv(arrAnc, file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/pnl/Output/Anc.csv")
  }
        
