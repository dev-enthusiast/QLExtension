# Code used for calculating Historical Reserve Margin
#
#
# Written by Chelsea Yeager on 26-Sep-2011




#------------------------------------------------------------
# FUNCTIONS
#


get.reserve = function(start_date, end_date, connnmkt, conntesla, connzema)
  {

  ##################
  ## Modify dates ##
  ##################
      
      ## Modify dates to be larger than the dates provided due to 24th hour issue
        start_dt_1d=as.Date(start_date)-1
        end_dt_1d=as.Date(end_date)-60
          if(end_date>=Sys.Date()) {rLog("You can't run this report for current or future days.  Decrease your end date!")}
          if(end_date<Sys.Date()) {rLog("Dates chosen are appropriate")}
  
  
  
  
  ###############
  ## Wind Data ##
  ###############
   
      ## Get Wind Data (assumes that 96.5% of north_west wind gen is in the west and 3.5% is in the north)
       Wind = pull.wind.reg (startdate=as.Date(start_date), lmtgdate=as.Date(end_date), connzema)
       WindData = aggregate(Wind[,c("value")],by=list(Wind$OPR_DATE, Wind$HE, Wind$dt_he),sum)
        colnames(WindData)[1:4]=c("date", "he","date_he","actual_wind")
        if(nrow(WindData)>0) {rLog("Successfully loaded wind data")}
        if(nrow(WindData)==0) {rLog("Returned 0 rows of wind data.  Check data."); return("Returned 0 rows of wind data.")}
  
     
  
  
  ##############
  ## Gen Data ##
  ##############
  
      ## Get Gen Data    
        strQry=paste("SELECT OPR_DATE,OPR_HOUR,RESOURCE_NAME,AVG(HSL),AVG(LSL),AVG(HASL),AVG(LASL),
          TELEMETERED_RESOURCE_STATUS
          from ze_data.ercot_nodal_60dscedgrd sced
          where TELEMETERED_RESOURCE_STATUS <> 'ONTEST'
          and base_point <> 0 and opr_date>=to_date('",start_dt_1d,"', 'YYYY-MM-DD')
          and opr_date<=to_date('",end_dt_1d,"', 'YYYY-MM-DD')
          group by OPR_DATE, opr_hour, TELEMETERED_RESOURCE_STATUS, resource_name" ,sep="")
        Gen = sqlQuery(connzema, strQry)
      
      ## Map Plant Name to Resource Type    
        RESOURCE_TYPE = data.frame(read.csv(file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/CVS/resource name.csv", header = TRUE, sep = ","))
        GenAll = merge(Gen, RESOURCE_TYPE)
        GenAll$date_he=GenAll$OPR_DATE + GenAll$OPR_HOUR*60*60
        GenUnique = aggregate( GenAll[,c("AVG(HASL)","AVG(LASL)")], by=list(GenAll$OPR_DATE, GenAll$OPR_HOUR, GenAll$date_he,GenAll$RESOURCE_TYPE), sum)
        colnames(GenUnique)[1:6]=c("date","he", "date_he","resource_type", "HASL", "LASL")
        if(nrow(GenUnique)>0) {rLog("Successfully loaded gen data from Zema")}
        if(nrow(GenUnique)==0) {rLog("Returned 0 rows of gen data from Zema.  Check data."); return("Returned 0 rows of gen data from Zema.")}
  
  
  
  ###########################
  ## Merge Wind & Gen Data ##
  ###########################    
      
      ## Pivot (cast) & Merge Data
        GU <- cast(GenUnique, date_he ~ resource_type,sum, value="HASL", fill=0)
        WD <- cast(WindData, date_he ~.,sum, value="actual_wind", fill=0)
        colnames(WD)[2] <- "actual wind"
        DD <- merge(WD, GU)
        colnames(DD)[7] <- "wind plant"

  
  
  ############################
  ## Pull Load & Price Data ##
  ############################
  
      ## Pull Historical Load Data
        curveName <- "ERCOT_load_hist_hrly"
        startDate =  start_dt_1d
        endDate = end_dt_1d
        ERCOTload = tsdb.readCurve(curveName, startDate, endDate)
        ERCOTload$date  <-  as.Date(as.POSIXct(ERCOTload$time[1:2]))
        ERCOTload$he   <- as.numeric(format(ERCOTload$time, "%H"))
        ERCOTload$value   <-    ERCOTload$value * -1
        ERCOTloadSys = aggregate( ERCOTload[,c("value")], by=list(ERCOTload$time), sum)
        colnames(ERCOTloadSys)[1:2]=c("date_he","ercot system load")
        
     ## Pull Historical Load Data 
        tsdb.symbols <- "ZM_ERCOT_RT_SPP_SP_HB_NORTH"
        start.dt =  as.POSIXct(start_dt_1d)
        end.dt = as.POSIXct(end_dt_1d)
        NorthHB = FTR:::FTR.load.tsdb.symbols("ZM_ERCOT_RT_SPP_SP_HB_NORTH", start.dt, end.dt, aggHourly=TRUE)
        NorthHBrt  = NorthHB
        y = cbind(date_he=index(NorthHBrt), data.frame(NorthHBrt))
        head(y)
        Price = aggregate( y[,c("ZM_ERCOT_RT_SPP_SP_HB_NORTH")], by=list(y$date_he), sum)
        colnames(Price)[1:2]=c("date_he","HB_NORTH_RT")
        
        Data1 <- Data1 <-merge(ERCOTloadSys,Price)
        Data <- merge(DD,Data1)

        Data <- merge(DD,Data1)
        colnames(Data)[2] <- "actual_wind"
        colnames(Data)[3] <- "opr_non_quick_start"
        colnames(Data)[4] <- "opr_non_quick_start_private"
        colnames(Data)[5] <- "opr_quick_start"
        colnames(Data)[6] <- "switch_resource"
        colnames(Data)[7] <- "wind"
        colnames(Data)[8] <- "sys_load"
        ReserveNum <- Data$actual_wind +  Data$opr_non_quick_start + Data$opr_non_quick_start_private + Data$opr_quick_start  + Data$switch_resource + Data$wind + Data$sys_load
        ReserveDen <- abs(Data$sys_load)
        Data$reserve_margin <- ReserveNum/ReserveDen
        
 
 
 
 ######################
 ## FIND SIMILAR DAY ##
 ######################

        
 
 
  ## Write detailed data to csv file
      write.csv(Data, file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/Output/Data.csv")

  }
        

