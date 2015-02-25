# Look at similar days based on load and wind
#   rt.price.fcst           # Email similar-day look-back and upload predicted SPP
# Must include start date, wind forecast, and pass a connection
# Should add DA snapshot of load/wind to graphs
# Throw out wierd-price days for both DA and RT
# Increase load forecast and historical load for coal/nuc outages (and for RUC'd on units?)
# Cut-off wind forecast for binding 
# Compare adjusted and non-adjusted for gen outages in backtesting model
# Incorporate Waha gas basis as not all plants are HSC
# Get count of hours of similar-load-days that have prices above 20 HR (PBP spike)
# Add RUC costs 
# Stage 3: Use DAM to get stack build-up
# Stage 4: Gen noms--how many gas-plant equivs
# Stage 5: RUC'd units add to forecasted stack
#
# Written by Lori Simpson on 02-May-2011







#------------------------------------------------------------
# Functions
# startDate=STARTDT; endDate=ENDDT; conn=channel_bentek
#
#   startDate=Sys.Date()+1; RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))); conn=channel_zema_prod; testing=TRUE
#   res=rt.price.fcst(startDate=Sys.Date()+1, ); res
#   startDate=Sys.Date()+2; pkng=4.00; pklmp=90; RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0)))  ; conn=channel_zema_prod
#   startDate=Sys.Date()+1; pkng=2.55; pklmp=160; RTwest=NULL  ; conn=channel_zema_prod; RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1303,17),rep(1868,7))); testing=TRUE

                                                                                                                                                                            
rt.price.fcst = function (startDate=Sys.Date()+1, pkng=NULL, pklmp=NULL, RTwest, conn, testing=FALSE)
  {

 
    if(startDate - Sys.Date()>3 ) {rLog("startDate is more than 3 days greater than today--won't return data."); stop}
    rLog("Starting rt.price.fcst")
        
    ##  GET FORECAST DATA
        STARTDT=startDate
        LMTGDT=STARTDT
        if(testing==FALSE) emailto=c("ccgercot24hrdesk@constellation.com, #wtercot@constellation.com, walter.kuhn@constellation.com") else emailto=c("lori.simpson@constellation.com")
        # if(testing==FALSE) emailto=c("ercotpm2@constellation.com, ptsouthgroup@exeloncorp.com, CCPORTWest@constellation.com, #wtercot@constellation.com, walter.kuhn@constellation.com") else emailto=c("lori.simpson@constellation.com")
        
   			## THREE TIER WIND FORECAST
            rLog("Will pull PRT wind forecast")
            threetier = pull.three.tier(STARTDT, STARTDT, connn=conn, DAct=0)
            if(is.na(match("threetier",ls()))) threetier=data.frame(OPR_DATE=NA, HE=NA, threetierwind=c(NA))[0,] else {colnames(threetier)[3]="threetierwind"; threetier$HE=threetier$HE+1}

        ## DA regional wind forecast
          ## Do not have ERCOT wind forecast more than one day in advance, so have to use PRT.
          ## If using PRT, take PRT * 90% to get the portion of west wind.  90% is a number I made up.
            rLog("Will pull ERCOT wind forecast")
            arrRes=pull.wind.reg.fcst(STARTDT, LMTGDT, DAct=as.numeric(STARTDT-Sys.Date()), regional=1, conn); 
            if(length(arrRes)!=1) 
              {
                arrRes=cast(arrRes, OPR_DATE+HE+dt_he~fcst_type+region, mean); 
                arrRes=merge(arrRes, threetier[,c(1,2,3)], all=TRUE)
                arrRes$wind=(arrRes$STWPF_WEST+arrRes$WGRPP_WEST+arrRes$STWPF_NORTH+arrRes$WGRPP_NORTH+arrRes$STWPF_SOUTH+arrRes$WGRPP_SOUTH)/2
                arrRes$west_wind=(arrRes$STWPF_WEST+arrRes$WGRPP_WEST)/2
                arrRes[is.na(arrRes$wind),"wind"]=arrRes[is.na(arrRes$wind),"threetierwind"]
                arrRes[is.na(arrRes$threetierwind),"threetierwind"]=arrRes[is.na(arrRes$threetierwind),"wind"]
              }
            if(length(arrRes)==1) 
              {
                arrRes=threetier[,c(1,2,3)]
                arrRes$wind=arrRes$threetierwind
                arrRes$west_wind=arrRes$wind*0.9
                arrRes$OPR_DATE=STARTDT
                arrRes=arrRes[order(arrRes$HE),]
                arrRes$dt_he=as.POSIXct(paste(as.POSIXlt(arrRes$OPR_DATE)$year+1900,sprintf("%02.0f",as.POSIXlt(arrRes$OPR_DATE)$mon+1),sprintf("%02.0f", as.POSIXlt(arrRes$OPR_DATE)$mday),sep="-"))+arrRes$HE*60*60
              }
        ## Add opr_date and hour if necessary
            arrRes=arrRes[order(arrRes$dt_he), ]
            arrRes$OPR_DATE=arrRes$OPR_DATE[1]  ## why do i have this code????
            arrRes$HE=seq(1,24)
            


        ## DA forecast by load zone (agg=0)
            rLog("Will pull ERCOT load forecast")
            # ldfc=pull.tsdb(STARTDT, STARTDT, symbolst=c('zm_ercot_nodal_fc_load_load_val_zone_systemtotal'), hourconvention=0)
            # ldfc=data.frame(dt_he=ldfc$dt_he, LOADFC=ldfc$value)
            ldfc=pull.load.reg.fcst(STARTDT, LMTGDT, DAct=as.numeric(STARTDT-Sys.Date()), agg=0, conn)
            colnames(ldfc)[match("loadfc",colnames(ldfc))]="value"
            ldfc=cast(ldfc, OPR_DATE+HE~ZONE, mean); 
            colnames(ldfc)[3:7]=paste("ld", colnames(ldfc)[3:7],sep="_")
            arrRes1=merge(arrRes, ldfc)
            ## Manually merge with wind:  ldfc$dt_he=ldfc$OPR_DATE+ldfc$HE*60*60
                ##   arrRes=merge(arrRes[,c('HE', 'STWPF_NORTH', 'STWPF_SOUTH', 'STWPF_WEST', 'WGRPP_NORTH', 'WGRPP_SOUTH', 'WGRPP_WEST', 'threetierwind', 'wind', 'west_wind')], ldfc) 



        ## PRT Load Forecast
            rLog("Will pull PRT load forecast")
            if(STARTDT<=Sys.Date()+1) {rLog("Trying DA PRT load"); prtld=pull.tsdb(STARTDT, LMTGDT, symbolst=c('prt_ercot_ld_dayahead'), hourconvention=0)}
            daprt_worked=TRUE
            if(is.na(match("prtld", ls()))) {rLog("DA PRT returned NULL"); daprt_worked=FALSE} else
                                            {if(nrow(prtld)==0|length(prtld)==1)  {rLog(paste("\t\t\tDA returned zero rows. Nrow(prtld)=", nrow(prtld), ".  length(prtld)=", length(prtld), sep="")); daprt_worked=FALSE}}
            if(!daprt_worked & STARTDT<=Sys.Date()+1) {prtld=pull.tsdb(STARTDT, LMTGDT, symbolst=c('prt_ercot_ld_0day_fcst'), hourconvention=0); rLog(paste("\t\t\tTried RT PRT.  nrow(prtld)=", nrow(prtld), ".  length(prtld)=", length(prtld), sep=""))}
            if(STARTDT==Sys.Date()+2) {prtld=pull.tsdb(STARTDT, LMTGDT, symbolst=c('prt_ercot_ld_2day_fcst'), hourconvention=0); rLog("\t\t\tTried 2DA PRT")}
            if(STARTDT>=Sys.Date()+3) {prtld=pull.tsdb(STARTDT, LMTGDT, symbolst=c('prt_ercot_ld_3day_fcst'), hourconvention=0); rLog("\t\t\tTried 3DA PRT")}
            rLog(paste(colnames(prtld),collapse=", "))
            colnames(prtld)[3]="prtld"
            arrRes$dt_he=arrRes$OPR_DATE+arrRes$HE*60*60
            # rLog("\t\t\tarrRes$dt_he=arrRes$OPR_DATE+arrRes$HE*60*60")
            # rLog(paste("\t\t\tanrow(arrRes)= ",nrow(arrRes), "   First row: ", paste(arrRes[1,], collapse=", "),   sep=""))
            # rLog(paste("\t\t\tanrow(prtld)= ",nrow(prtld), "   First row: ", paste(prtld[1,], collapse=", "),   sep=""))
           # Convert dates
              prtld$OPR_DATE=arrRes1$OPR_DATE
              prtld$HE=as.POSIXlt(prtld$dt_he)$hour
              prtld[prtld$HE==0, "HE"]=24
           # Merge
            arrRes=merge(arrRes1, prtld[,c("dt_he","prtld","OPR_DATE","HE")], all.x=TRUE)
            rLog(paste("\t\t\tnrow(arrRes)= ",nrow(arrRes), "   First row: ", paste(arrRes[1,], collapse=", "),   sep=""))
            arrRes[is.na(arrRes$prtld)==TRUE,"prtld"]=arrRes[is.na(arrRes$prtld)==TRUE,"ld_SystemTotal"]
            # rLog("\t\t\arrRes[is.na(arrRes$prtld)==TRUE,'prtld']=")



        ## W-N limits DA forecast
          ## Only have limits 1 day ahead of time.  Thus pull prior day's limits if not yet published for desired date.
            # rLog("Will pull ERCOT W-N limits.")
        # Temporary pause as not working: wnda=pull.tsdb(STARTDT-6, Sys.Date()+2, symbolst=c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'), hourconvention=0)
        # wnda=pull.tsdb(STARTDT-60, Sys.Date()+2, symbolst=c('ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north'), hourconvention=0)
            ## Correc for incorrect HE convention
            # wnda$HE=as.POSIXlt(wnda$dt_he)$hour; 
            # if(wnda[nrow(wnda), "HE"]==23) wnda$HE=wnda$HE+1 else wnda[wnda$HE==0,]=24; 
            # wnda$dtcheck=abs(wnda$dt_he-as.POSIXct(STARTDT))
            # wndachk=wnda; colnames(wndachk)[c(3,5)]=c("wn","value"); wndachk$variable="dtcheck"
            # wndachk=cast(wndachk, HE~variable, min)
            # wnda=merge(wnda, wndachk)
            # if(nrow(wnda)==0) {rLog(paste("ERROR-cannot find any DA W-N Limits.  Check 'ZM_ERCOT_NODAL_GEN_TRANS_LIM_LIMit_interface_id_west to north' symbol.")); stop}
            # colnames(wnda)[match("value",colnames(wnda))]="limit_wn_da"; wnda=wnda[order(wnda$dt_he), ]; 
            wnda=data.frame(HE=0,limit_wn_da=0)[0,]
            arrRes2=merge(arrRes, wnda[,c("HE","limit_wn_da")], all.x=TRUE)

        ## W-N limits RT forecast
            # arrRes2=merge(arrRes2, RTwest)

		  	## Add buckets
            rLog("Adding buckets")
            load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/buckets.RData"); 
            rLog("\t\t\t Merging")
    		  	arrRes2=merge(arrRes2, bkts[,c("OPR_DATE","HE","bucket","dayname")], all.x=TRUE)
    		  	da=arrRes2
    		  	colnames(da)[match("HE",colnames(da))]="OPR_HOUR"
        ## Get price forecasts
            rLog("Getting price forecasts")
    		  	if(is.null(pklmp)) 
                {
                    ## secdb.getElecPrices(STARTDT-1, STARTDT, STARTDT,commodity = "COMMOD PWR ERCOT PHYSICAL", location = c("NORTH HUB RT"),bucket = c("PEAK", "OFFPEAK"), serviceType = "Energy", useFutDates = FALSE)
                  strMarks=(Sys.Date()-1)                    
                  if(secdb.isHoliday( strMarks, calendar="CPS-BAL", skipWeekends=FALSE)) strMarks=strMarks-1 else paste(FALSE)
                  if(secdb.isHoliday( strMarks, calendar="CPS-BAL", skipWeekends=FALSE)) strMarks=strMarks-1 else paste(FALSE)
                  if(secdb.isHoliday( strMarks, calendar="CPS-BAL", skipWeekends=FALSE)) strMarks=strMarks-1 else paste(FALSE)                  
                  if(secdb.isHoliday( strMarks, calendar="CPS-BAL", skipWeekends=FALSE)) strMarks=strMarks-1 else paste(FALSE)                  
                  if(secdb.isHoliday( strMarks, calendar="CPS-BAL", skipWeekends=FALSE)) strMarks=strMarks-1 else paste(FALSE)                  
                  res <- secdb.balmoDlyPrices( asOfDate=strMarks, region="ERCOT", location="North Hub RT"); 
                  res=res[res$Date==STARTDT,c("2x16H","5x16")]; pklmp=res[1,which(res>0)]; da$pklmp=pklmp
          		  	if(is.null(res[1,which(res>0)])) {da$pklmp=NA; pkhr=0}
                } else 
                {pkhr=pklmp/pkng; da$pklmp=pklmp; pklmp=round(pklmp,1)}
    		  	if(is.null(pkng)) 
                {
                  arrMarksNG=ntplot.estStrip("PRC_NG_HSCBEA_GDM", startDate=STARTDT, endDate=LMTGDT); pkng=arrMarksNG[nrow(arrMarksNG),"value"]; da$pkng=pkng
          		  	if(is.null(arrMarksNG[nrow(arrMarksNG),"value"])) {da$pkng=NA}
                } else da$pkng=pkng
    		  	pkhr=pklmp/pkng
    		  	da$pkhr=da$pklmp/da$pkng
    		  	dt=da$OPR_DATE[1]
                rLog(paste("pkhr = ",pkhr))
            # aux <- try()
          
            #if (class(aux) != "try-error")
              # res[[i]] <- aux


    		  	


##  GET HISTORICAL DATA
        rLog("Will pull historical data.")
        file1=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_", as.POSIXlt(Sys.Date())$year+1900, ".RData", sep="")
        file2=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_", as.POSIXlt(Sys.Date())$year+1899, ".RData", sep="")
        load(file=file1); 
        rthourly1=subset(rthourly, SP=="HB_NORTH")
        load(file=file2); 
        rthourly2=subset(rthourly, SP=="HB_NORTH")
        rthourly=rbind(rthourly1, rthourly2)
        rthourly=merge(rthourly, bkts[,c("OPR_DATE","HE","bucket","dayname")])
 		  	if(is.na(da$pkng[1])) {gas=rthourly[is.na(rthourly$platts_hsc)==FALSE,];  gas=subset(gas, OPR_DATE==max(gas$OPR_DATE))[1,"platts_hsc"]; gascheck=paste("NOT using updated gas.  Check current gas against gas being used: ",gas,sep="")} else  {gas=da$pkng[1]; gascheck=paste("Using forecasted gas of: ",gas,sep="")}
    ## Get wind data where don't have any
      rthourly$wind=rthourly$windrt
      rthourlyi=rthourly[is.na(rthourly$wind)==TRUE,]; rthourlyi$wind=rthourlyi$wind_act_total; rthourly=rbind(rthourly[is.na(rthourly$wind)==FALSE,], rthourlyi)
      rthourlyi=rthourly[is.na(rthourly$wind)==TRUE,]; rthourlyi$wind=(rthourlyi$wind_reg_fcst_STWPF+rthourlyi$wind_reg_fcst_WGRPP)/2; rthourly=rbind(rthourly[is.na(rthourly$wind)==FALSE,], rthourlyi)
    ## Get load data where don't have any
      rthourly$ld=rthourly$loadrt
      rthourlyi=rthourly[is.na(rthourly$ld)==TRUE,]; rthourlyi$ld=rthourlyi$load_sys_interval; rthourly=rbind(rthourly[is.na(rthourly$ld)==FALSE,], rthourlyi)
      rthourlyi=rthourly[is.na(rthourly$ld)==TRUE,]; rthourlyi$ld=(rthourlyi$load_reg_act_sum); rthourly=rbind(rthourly[is.na(rthourly$ld)==FALSE,], rthourlyi)
      rthourlyi=rthourly[is.na(rthourly$ld)==TRUE,]; rthourlyi$ld=(rthourlyi$load_sys_fcst); rthourly=rbind(rthourly[is.na(rthourly$ld)==FALSE,], rthourlyi)
    ## Merge with buckets
      rthourly=merge(rthourly, bkts, all.x=TRUE)
    ## Get historical gen outages
      ## iirout3=FTR.getGenerationOutagesIIR(ctrlarea="ERCO", start.dt=as.Date('2011-04-28'), end.dt=as.Date('2011-05-01'))
    ## Correct names
      rthourly$plattshr=rthourly$plattspwr/rthourly$platts_hsc
      rthourly$plattsgas=rthourly$platts_hsc
      rLog(paste("pkhr = ",pkhr))

        
            
    

  ## Plot historical days that cleared at the same DA heat rate
                #  ## CONVERT PLATTS PRICE TO HEAT RATES, EQUIVALENT LOAD IS LOAD MINUS WIND, SCALE RTLMP FOR GAS PRICES         			
                #    rtpk=subset(rthourly, plattshr>=(pkhr-0.1)&plattshr<=(pkhr+0.1) & pk=="pk")
                #    rtpk$eld=rtpk$ld-rtpk$WINDACT
                #    rtpk$RTLMPadj=rtpk$RTSPP*gas[1,2]/rtpk$plattsgas
                #    rtpk=merge(rtpk, bkts, all.x=TRUE)
                #    rtpk=subset(rtpk, bucket==bket)
                #  ## PLOT 
                #    windows(12,10);	 
                #    datesunq=unique(rtpk$OPR_DATE)    
                #    par(mfrow=c(1,1)); da=da[order(da$OPR_HOUR), ]
                #    plot(da$OPR_HOUR, da$LOADFC, type="l",lwd=3,bty="l",ylim=c(15000,50000))
                #        lines(da$OPR_HOUR, da$eld,col="black")
                #    for (i in 1:length(datesunq))
                #      {
                #        rti=subset(rtpk, OPR_DATE==datesunq[i]) ; rti=rti[order(rti$HE),]
                #        lines(rti$HE,rti$eld,col="grey")
                #      }
                #    par(new=TRUE)
                #    plot(rti$HE, rti$RTLMPadj, pch=20,bty="l",col="red",axes=FALSE,ylab="",xlab="",ylim=c(0,100),xlim=c(1,24), main=paste(dt,"   HR=",round(pkhr,1),sep="")); axis(4); abline(h=mean(rti$RTLMPadj),col="pink")
                #    for (i in 1:length(datesunq))
                #      {
                #        rti=subset(rtpk, OPR_DATE==datesunq[i])
                #        points(rti$HE,rti$RTLMPadj,pch=20,col="red")
                #        abline(h=mean(rti$RTLMPadj),col="pink")
                #      }
                #    abline(h=pkhr*gas[1,2],col="purple",lwd=4)
                #    legend(21,10,c("Load Forecast","Historical","RT LMP"),fill=c("black","grey","red"))
                #



#################################################
## FIND SIMILAR DAY
## DA is the table with the targeted forecast date
## RT is the table holding historical data
        rLog("Looking for similar days.")
  ## Get DA factors
      ld_morn=subset(da, OPR_HOUR<=10 & OPR_HOUR>2); ld_morn=max(ld_morn$prtld); 
      ld_eve=subset(da, OPR_HOUR>=18); ld_eve=max(ld_eve$prtld); 
      wind_morn=subset(da, OPR_HOUR<=6); wind_morn=min(5500,max(wind_morn$wind)); 
      wind_aft=subset(da, OPR_HOUR>12 & OPR_HOUR<=17); wind_aft_min=min(wind_aft$wind); wind_aft=min(5500,max(wind_aft$wind)); 
      wind_eve=subset(da, OPR_HOUR>20 & OPR_HOUR<=24); wind_eve=min(5500,max(wind_eve$wind)); 
      rLog(paste("Max morn load: ",round(ld_morn,0),". Max eve load: ",round(ld_eve,0),". Max morn wind: ",round(wind_morn,0),". Aftrn wind: ",round(wind_aft,0),":",round(wind_aft_min,0),". Max eve wind: ",round(wind_eve,0),sep=""))
  ## Choose tolerance bandwidths
      ld_morn_tol=5000
      ld_eve_tol=5000
      wind_morn_tol=2000
      wind_aft_tol=2000
      wind_eve_tol=3000          
  ## Match to RT with large tolerances
    rtgood=rthourly
      ## Match morning peak load
        rt=subset(rtgood, HE<=10); rt=rt[is.na(rt$ld)==FALSE,]; colnames(rt)[match("ld",colnames(rt))]="value"; rt$variable="maxld"; rt=cast(rt, OPR_DATE ~ variable, max)  
        rt=subset(rt, maxld>=(ld_morn-ld_morn_tol) & maxld<=(ld_morn+ld_morn_tol))
        rLog(paste("Morning load match: ",nrow(rt)," days",sep=""))
        rtgood=merge(rtgood, rt)     ## unique(rtgood$OPR_DATE)
      ## Match evening peak load
        rt=subset(rtgood, HE>=18); rt=rt[is.na(rt$ld)==FALSE,]; colnames(rt)[match("ld",colnames(rt))]="value"; rt$variable="maxeveld"; rt=cast(rt, OPR_DATE ~ variable, max)  
        rt=subset(rt, maxeveld>=(ld_eve-ld_eve_tol) & maxeveld<=(ld_eve+ld_eve_tol))
        rLog(paste("Evening load match: ",nrow(rt)," days",sep=""))
        rtgood=merge(rtgood, rt)     ## unique(rtgood$OPR_DATE)
        rtsimload=rtgood  ## to be used for outliers later
      ## Match morning wind
        rt=subset(rtgood, HE<=6); rt=rt[is.na(rt$wind)==FALSE,]; colnames(rt)[match("wind",colnames(rt))]="value"; 
        rt$variable="windmorn"; rt=cast(rt, OPR_DATE ~ variable, max)  
        rt=subset(rt, windmorn>=(wind_morn-wind_morn_tol) & windmorn<=(wind_morn+wind_morn_tol))
        rLog(paste("Morning wind match: ",nrow(rt)," days",sep=""))
        rtgood=merge(rtgood, rt)                               ## unique(rtgood$OPR_DATE)
      ## Match afternoon wind
        if(nrow(rt)>6)
          {
            rt=subset(rtgood, HE>12 & HE<=17); rt=rt[is.na(rt$wind)==FALSE,]; colnames(rt)[match("wind",colnames(rt))]="value"; 
            rt$variable="aftmorn"; rt=cast(rt, OPR_DATE ~ variable, max)  
            rt=subset(rt, (aftmorn>=(wind_aft-wind_aft_tol) & aftmorn<=(wind_aft+wind_aft_tol)) | (aftmorn>=(wind_aft_min-wind_aft_tol) & aftmorn<=(wind_aft_min+wind_aft_tol)))
            rLog(paste("Afternoon wind match: ",nrow(rt)," days",sep=""))
            rtgood=merge(rtgood, rt)
          }
      ## Match evening wind
        if(nrow(rt)>6)
          {
            rt=subset(rtgood, HE>20 & HE<=24); rt=rt[is.na(rt$wind)==FALSE,]; colnames(rt)[match("wind",colnames(rt))]="value"; 
            rt$variable="evemorn"; rt=cast(rt, OPR_DATE ~ variable, max)  
            rt=subset(rt, evemorn>=(wind_eve-wind_eve_tol) & evemorn<=(wind_eve+wind_eve_tol))
            rLog(paste("Evening wind match: ",nrow(rt)," days",sep=""))
            rtgood=merge(rtgood, rt)
          }
      ## Cut off matches
        if (length(unique(rtgood$OPR_DATE))>15) {rtdates=unique(rtgood$OPR_DATE); rtgood=subset(rtgood, OPR_DATE>rtdates[length(rtdates)-15])}  
        if (length(unique(rtgood$OPR_DATE))==0) {rLog("Found no similar days."); stop}
        rLog(paste("Total days match: ",length(unique(rtgood$OPR_DATE))," days",sep=""))
  ## Plot matching days
      ##dev.off();dev.off();dev.off();dev.off()
      rLog("Plotting similar days.")
      simdayfile=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/",as.POSIXlt(da$OPR_DATE[1])$year+1900,"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mon+1),"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mday),"_similar_day.pdf",sep="")
      pdf(simdayfile,width=12,height=10)      
      rtct=length(unique(rtgood$OPR_DATE))+1; if(rtct<=1) rc=1;  if(rtct<=4 & rtct>1) rc=2; if(rtct<=9 & rtct>4) rc=3; if(rtct<=16 & rtct>9) rc=4; if(rtct<=25 & rtct>16) rc=5; if(rtct<=36 & rtct>25) rc=6
      par(mfrow=c(rc,rc),mar=c(3,3,3,3))    ; da=da[order(da$OPR_HOUR),]
      plot(da$OPR_HOUR, da$ld_SystemTotal, type='l', lwd=4, bty="l", ylim=c(max(da$ld_SystemTotal)*0.8,max(da$ld_SystemTotal)*1.1), 
          main=paste(dt, "\n",da$dayname[1], sep=""),cex.main=2);      text(10,40000,paste(round(pkhr,1),", ",pklmp,sep=""),cex=2); mtext("Load (PRT in blue)",2); mtext("Wind",4,col="red")
          lines(da$OPR_HOUR, da$prtld, type='l', lwd=2, col="blue") 
          text(5, max(da$ld_SystemTotal)*0.9, paste("Ercot: ", round(max(da$ld_SystemTotal)/1000, 1), sep=""))
          text(5, max(da$ld_SystemTotal)*0.87, paste("PRT: ", round(max(da$prtld)/1000, 1), sep=""))
          par(new=TRUE)
          plot(da$OPR_HOUR, da$wind, type='l', lwd=4, col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2); axis(4)
      ## Prepare text file to send out stats
            textsave="Historical ERCOT data (SSI daily max temp).  Wind is avg wind in HE16-17\n\nHE 7-22 \n"; textsave2="HE 15-18 \n"
            textsaveop="OFFPEAK data (load and temps are the daily min; wind is the offpeak max wind output)\n"
          ## Get the temperatures
              arrTemperatures=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_ssi_dfw_temp_hist'), 0); colnames(arrTemperatures)[match("value",colnames(arrTemperatures))]="dallastemp"
              rtgood=merge(rtgood, arrTemperatures[,c("dt_he","dallastemp")], all.x=TRUE)
              arrTemperatures=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_sSI_IAH_Temp_hist'), 0); colnames(arrTemperatures)[match("value",colnames(arrTemperatures))]="houtemp"
              rtgood=merge(rtgood, arrTemperatures[,c("dt_he","houtemp")], all.x=TRUE)
              arrTemperatures=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_sSI_maf_Temp_hist'), 0); colnames(arrTemperatures)[match("value",colnames(arrTemperatures))]="odessatemp"
              rtgood=merge(rtgood, arrTemperatures[,c("dt_he","odessatemp")], all.x=TRUE)
              arrTemperatures=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_sSI_sat_Temp_hist'), 0); colnames(arrTemperatures)[match("value",colnames(arrTemperatures))]="sanantontemp"
              rtgood=merge(rtgood, arrTemperatures[,c("dt_he","sanantontemp")], all.x=TRUE)
      ## Plot in a loop for the individual days
      datesopr=unique(rtgood$OPR_DATE)
        for (i in 1:length(datesopr))
        {          
           rtgoodnh=subset(rtgood, SP=='HB_NORTH' & OPR_DATE==datesopr[i]); rtgoodnh=rtgoodnh[order(rtgoodnh$HE),]; rtgoodnh=rtgoodnh[order(rtgoodnh$OPR_DATE),]
            plot(da$OPR_HOUR, da$ld_SystemTotal, type='l', lwd=4, bty="l", ylim=c(max(da$ld_SystemTotal)*0.8,max(da$ld_SystemTotal)*1.1), 
                main=paste(datesopr[i],"     ",rtgoodnh$dayname[1],"\n",
                "HR: OTC- ",round(max(rtgoodnh$plattshr),1)," DA-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"])/rtgoodnh$plattsgas[1],1)," RT-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])/rtgoodnh$plattsgas[1],1) ,sep=""), col="black")
            lines(rtgoodnh$HE, rtgoodnh$ld, col="grey", lwd=4)
            mtext("Load",2); mtext("Wind",4,col="red")
            text(5, max(da$ld_SystemTotal)*0.83, paste("Old: ", round(max(rtgoodnh$ld)/1000, 1), sep=""))
            par(new=TRUE)
            plot(da$OPR_HOUR, da$wind, type='l', col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2, lwd=3); axis(4)
            lines(rtgoodnh$HE, rtgoodnh$wind, col="pink", lty=2, lwd=3)  
            points(rtgoodnh$HE, rtgoodnh$RTSPP*10, col="blue", pch=20)
            #par(new=TRUE)
            #plot(rtgoodnh$HE, rtgoodnh$plattshr, type='l', lwd=2, col="lightblue",bty="l", axes=FALSE, ylab="", lty=2,ylim=c(0,15));                 

          ## Prepare text file to send out stats
                    textsave=paste(textsave, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tOTC-",round(max(rtgoodnh$plattspwr),0),"\tDA-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"]),0),"  \tRT-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]),0), 
                              "    \tLoad-",round(max(rtgoodnh[is.na(rtgoodnh$loadrt)==FALSE,"loadrt"]),-2)/1000, 
                              "     \twind-", round(max(subset(rtgoodnh[is.na(rtgoodnh$windrt)==FALSE,],HE>=16&HE<=17)[,"windrt"]),-2),
                              "  \tDallas-",round(max(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"dallastemp"]),0),
                              "\tHou-",round(max(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"houtemp"]),0), 
                              "\t\tSanAn-",round(max(rtgoodnh[is.na(rtgoodnh$sanantontemp)==FALSE,"sanantontemp"]),0), 
                              "\tOdssa-",round(max(rtgoodnh[is.na(rtgoodnh$odessatemp)==FALSE,"odessatemp"]),0), "\n", 
                              sep="")              
                    textsaveop=paste(textsaveop, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1]
                              , ":\tOTC-",round(min(rtgoodnh$plattspwr),1)
                              , "\tDA-",round(mean(subset(rtgoodnh,HE<7|HE>22)[,"DASPP"]),1)
                              , "   \tRT-",round(mean(subset(rtgoodnh,HE<7|HE>22)[,"RTSPP"]),1) 
                              , "  \tLoad-",round(min(rtgoodnh[is.na(rtgoodnh$loadrt)==FALSE,"loadrt"]),-2)
                              , "\twind-", round(max(subset(rtgoodnh[is.na(rtgoodnh$windrt)==FALSE,],HE<7|HE>22)[,"windrt"]),0)
                              , "\tDallas-",round(min(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"dallastemp"]),0)
                              , "\tHou-",round(min(rtgoodnh[is.na(rtgoodnh$houtemp)==FALSE,"houtemp"]),0)
                              , "\t\tSanAn-",round(min(rtgoodnh[is.na(rtgoodnh$sanantontemp)==FALSE,"sanantontemp"]),0)
                              , "\tOdssa-",round(min(rtgoodnh[is.na(rtgoodnh$odessatemp)==FALSE,"odessatemp"]),0)
                              , "\n", sep="")              
                     textsave2=paste(textsave2, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tDA-",round(mean(subset(rtgoodnh,HE>=15&HE<=18)[,"DASPP"]),0),"    \tRT-",round(mean(subset(rtgoodnh,HE>=15&HE<=18)[,"RTSPP"]),0), "\n", sep="")              
          }
 
        par(mfrow=c(1,1))
      dev.off()
      rtgoodnh=subset(rtgood, SP=='HB_NORTH' )  
      emailprices=data.frame(round(tapply(rtgoodnh$RTSPP, rtgoodnh$HE, mean),0))[,1]
      rtgoodnh$RTadj=gas*rtgoodnh$RTSPP/rtgoodnh$plattsgas
      filesavesimday=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/",as.POSIXlt(da$OPR_DATE[1])$year+1900,"/similar_day_data.csv",sep="")
      write.csv(rtgoodnh, file=filesavesimday)

  ## Get RT forecast
        rLog("\tFinding similar days")
      ## Take only data with a similar DA_bilateral platts heat rate, but if missing platts htrt, need DAM heat rate
        rtdaavg=subset(rtgoodnh, bucket=="5x16")
        rtdaavg$value=rtdaavg$DASPP; rtdaavg$variable="daavg"
        rtdaavg=cast(rtdaavg, OPR_DATE~variable, mean)
        rtgoodnh=merge(rtgoodnh, rtdaavg)
        rtgoodnh[is.na(rtgoodnh$plattshr),"plattshr" ]  =rtgoodnh[is.na(rtgoodnh$plattshr),"daavg" ]/rtgoodnh[is.na(rtgoodnh$plattshr),"platts_hh" ]
        rtgoodnh$daavg=NULL
          #if(is.na(pkhr)==FALSE) {pkhrtest=FALSE; pkhr=subset(rtsimload, OPR_DATE==max(rtsimload$OPR_DATE) & HE==16)$plattshr; pkhrdt=subset(rtsimload, OPR_DATE==max(rtsimload$OPR_DATE) & HE==16)$OPR_DATE}
          #if(is.na(pkhr)==TRUE) {pkhrtest=TRUE;  }
          #if(is.na(pkhr)==FALSE) 
            # {

                 hrrange=20
                 if(length(subset(rtgoodnh, plattshr>=(pkhr-1.2) & plattshr<=(pkhr+1.2) & HE==16)[,c("OPR_DATE")])>=4)  hrrange=2
                 if(length(subset(rtgoodnh, plattshr>=(pkhr-1.2) & plattshr<=(pkhr+1.2) & HE==16)[,c("OPR_DATE")])>=4)  hrrange=1.2
                 if(length(subset(rtgoodnh, plattshr>=(pkhr-0.8) & plattshr<=(pkhr+0.8) & HE==16)[,c("OPR_DATE")])>=4)  hrrange=0.8
                 if(length(subset(rtgoodnh, plattshr>=(pkhr-0.5) & plattshr<=(pkhr+0.5) & HE==16)[,c("OPR_DATE")])>=4)    hrrange=0.5
                 if(length(subset(rtgoodnh, plattshr>=(pkhr-0.25) & plattshr<=(pkhr+0.25) & HE==16)[,c("OPR_DATE")])>=4)  hrrange=0.25
                 fcst=subset(rtgoodnh, plattshr>=(pkhr-hrrange) & plattshr<=(pkhr+hrrange) & HE==16)[,c("OPR_DATE","yr")]
                 rLog(paste("Similar day lookback price forecast using a Platts DA heat ranging from ",round(pkhr-hrrange,1)," to ",round(pkhr+hrrange,1),".  Found ",length(subset(rtgoodnh, plattshr>=(pkhr-hrrange) & plattshr<=(pkhr+hrrange) & HE==16)[,c("OPR_DATE")])," days.",sep=""))
             # } else {rLog("No heat rate found for similar day lookback"); break; exit; return(FALSE)}
          fcst=merge(fcst, rtgoodnh)  
            # if(pkhrtest==FALSE) {pkhrtext=paste("No DA heatrate found, thus using the Platts DA HR of ",round(pkhr,1)," from ",pkhrdt,".  Historical data used ranges from ",round(min(subset(fcst, HE==16)$plattshr),1)," to ",round(max(subset(fcst, HE==16)$plattshr),1),".",sep="") }
            # if(pkhrtest==TRUE) {
            pkhrtext=paste("Using a DA heatrate of ",round(pkhr,1),".  Historical data used ranges from ",round(min(subset(fcst, HE==16)$plattshr),1)," to ",round(max(subset(fcst, HE==16)$plattshr),1),".",sep="") 
            rLog(pkhrtext)
          colnames(fcst)[match("RTadj",colnames(fcst))]="value"
          fcstrt=cast(fcst, HE~OPR_DATE, mean)
      ## Verify all columns have data
          for (j in 2:ncol(fcstrt))
          {
            if(is.na(sum(fcstrt[,j]))==TRUE) fcstrt[is.na(fcstrt[,j]),j]=fcstrt[is.na(fcstrt[,j]),j-1]
          }
      ## Find the median of this data
          mdn=apply(fcstrt[1:ncol(fcstrt)], 1,  function(x)  {median(x) }  )      
      ## Get standard deviations all similar load days
            rtsd=subset(rtsimload, OPR_DATE>=as.POSIXct(Sys.Date()-120))
            if(nrow(rtsd)==0) rtsd=rtsimload
            rtsd$value=gas*rtsd$RTSPP/rtsd$plattsgas
            rtsd$variable="sd"
            sdsd=cast(rtsd, HE~variable, sd)
      ## Get counts of load spikes for all similar load days
            rtmean=subset(rtsimload, OPR_DATE>=as.POSIXct(Sys.Date()-360))
            rtmean$value=gas*rtmean$RTSPP/rtmean$plattsgas
            rtmean1=subset(rtmean, value<150)
            rtmean1$rtspike=0
            rtmean2=subset(rtmean, value>=150)
            if(nrow(rtmean2)>0) rtmean2$rtspike=1
            rtmean=rbind(rtmean1, rtmean2)[,c("HE","rtspike")]
            rtmean$variable="ctspikes"
            colnames(rtmean)[match("rtspike",colnames(rtmean))]="value"
            rtmean=cast(rtmean, HE~variable, sum)
  ## Send out results
        rLog("\tAbout to send sim-day lookback email")
          setLogLevel(1)    
    sendEmail(from="lori.simpson@constellation.com", to=emailto,
              subject=paste("DA TOOLS.  Similar Day lookback: ", da$OPR_DATE[1],sep=""),
              body=paste("Similar-day lookback.   \nPRT Peak Hour Load Forecast: ",round(max(da$prtld),-2),
              "\nERCOT Peak Hour Load Forecast: ", round(max(da$ld_SystemTotal),-2),
              "\n\n A) Median RT North Hub LMPs from days with similar load/wind and similar DA heat rates (",pkhrtext,")  (Found ",ncol(fcstrt)-1," days.)",
              "\n B) Standard Deviation of prices from days with similar load only.  (Found ",length(unique(rtsd$OPR_DATE))," days.)  (See PDF for similar/load wind days.)",
              "\n C) Counts of hours with LMP>=$250 from dayswith similar load only.  (Found ",length(unique(rtsd$OPR_DATE))," days.)",
              "\n\nMedian, ", paste(round(mdn,0),collapse=","),
              "\nSD,     ", paste(round(sdsd[,2],0),collapse=","),
              "\nCounts, ", paste(round(rtmean[,2],0),collapse=","),
              "\nAverage Prices from similar load/wind days, \n", paste(data.frame(round(tapply(rtgoodnh$RTadj, rtgoodnh$HE, mean),0))[,1],collapse=","),
              "\n\nAll prices have been adjusted using the last gas-day's data.\n",gascheck,"\nData found here:\n",filesavesimday, 
              "\n\n\n", textsave, "\n\n\n", textsaveop, "\n\n\n", textsave2,
              sep=""),
              attachments=c(simdayfile))
          setLogLevel(3)

  ## Upload price forecasts
        rLog("Will attempt to upload forecast data to TSDB")
        times = seq(round(as.POSIXct(STARTDT), "day"), by="1 hour", length.out=25)[2:25]
        arrSymbols = c('erc_nhrt_dafcst_simday_mdn','erc_nhrt_dafcst_simday_avg','erc_nhrt_dafcst_simday_ct','erc_nhrt_dafcst_simday_sd')
    ## Load individual symbols
        dfUploadData1 <- data.frame(time=times, value=mdn)
        tsdbUpdate(arrSymbols[1], dfUploadData1)
        dfUploadData2 <- data.frame(time=times, value=as.numeric(unlist(strsplit(paste(round(tapply(rtgoodnh$RTadj, rtgoodnh$HE, mean),4), collapse=","),","))))
        tsdbUpdate(arrSymbols[2], dfUploadData2)
        dfUploadData3 <- data.frame(time=times, value=rtmean[,2])
        tsdbUpdate(arrSymbols[3], dfUploadData3)
        dfUploadData4 <- data.frame(time=times, value=sdsd[,2])
        tsdbUpdate(arrSymbols[4], dfUploadData4)




#  ## Find SD for similar-load days
#        rLog("Looking for similar load days for SD.")
#  ## Get DA factors
#      ld_morn=subset(da, OPR_HOUR<=10 & OPR_HOUR>2); ld_morn=max(ld_morn$ld_SystemTotal); 
#      ld_eve=subset(da, OPR_HOUR>=18); ld_eve=max(ld_eve$ld_SystemTotal); 
#      rLog(paste("Max morn load: ",round(ld_morn,0),". Max eve load: ",round(ld_eve,0),sep=""))
#  ## Choose tolerance bandwidths
#      ld_morn_tol=7000
#      ld_eve_tol=7000
#  ## Match to RT with large tolerances
#    rtgood=rthourly
#      ## Match morning peak load
#        rt=subset(rtgood, HE<=10); rt=rt[is.na(rt$ld)==FALSE,]; colnames(rt)[match("ld",colnames(rt))]="value"; rt$variable="maxld"; rt=cast(rt, OPR_DATE ~ variable, max)  
#        rt=subset(rt, maxld>=(ld_morn-ld_morn_tol) & maxld<=(ld_morn+ld_morn_tol))
#        rLog(paste("Morning load match: ",nrow(rt)," days",sep=""))
#        rtgood=merge(rtgood, rt)     ## unique(rtgood$OPR_DATE)
#      ## Match evening peak load
#        rt=subset(rtgood, HE>=18); rt=rt[is.na(rt$ld)==FALSE,]; colnames(rt)[match("ld",colnames(rt))]="value"; rt$variable="maxeveld"; rt=cast(rt, OPR_DATE ~ variable, max)  
#        rt=subset(rt, maxeveld>=(ld_eve-ld_eve_tol) & maxeveld<=(ld_eve+ld_eve_tol))
#        rLog(paste("Evening load match: ",nrow(rt)," days",sep=""))
#        rtgood=merge(rtgood, rt)     ## unique(rtgood$OPR_DATE)
#        rtgoodnh=subset(rtgood, SP=='HB_NORTH' )  
#        rtgoodnh$RTadj=gas*rtgoodnh$RTSPP/rtgoodnh$plattsgas
#  ## Send out results
#          setLogLevel(1)    
#        sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com,alex.werner@constellation.com,kevin.libby@constellation.com,jack.miglioretti@constellation.com,chelsea.yeager@constellation.com"),
#              subject=paste("DA TOOLS.  Similar-Load Day SD lookback: ", da$OPR_DATE[1],sep=""),
#              body=paste("Similar Load-day lookback \nStandard Deviation of Prices:\n", paste(data.frame(round(tapply(rtgoodnh$RTadj, rtgoodnh$HE, sd),0))[,1],collapse=","),"\nUsed ",length(unique(rtgoodnh$OPR_DATE))," days of values.\nRT Prices have been adjusted using the last gas-day's data.\n",gascheck,sep=""), 
#              )
#          setLogLevel(3)
#        
        
          

      
#################
#################
#################
  ## Plot all past of same day
      rLog("Looking for same day of past weeks.")
    rtgood=rthourly
    rtgood=subset(rtgood, dayname==da$dayname[1] & OPR_DATE>=(as.POSIXct(da$OPR_DATE[1])-60*24*60*60))
      ##dev.off();dev.off();dev.off();dev.off()
      samewkdayfile=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/",as.POSIXlt(da$OPR_DATE[1])$year+1900,"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mon+1),"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mday),"_same_weekday.pdf",sep="")
      pdf(samewkdayfile,width=12,height=10)      
      rtct=length(unique(rtgood$OPR_DATE))+1; if(rtct<=1) rc=1;  if(rtct<=4 & rtct>1) rc=2; if(rtct<=9 & rtct>4) rc=3; if(rtct<=16 & rtct>9) rc=4; if(rtct<=25 & rtct>16) rc=5; if(rtct<=36 & rtct>25) rc=6
      par(mfrow=c(rc,rc),mar=c(3,3,3,3))    ; da=da[order(da$OPR_HOUR),]
      plot(da$OPR_HOUR, da$ld_SystemTotal, type='l', lwd=4, bty="l", ylim=c(15000, max(da$ld_SystemTotal)*1.1), 
          main=paste(dt, "\n",da$dayname[1], sep=""),cex.main=2);      text(10,40000,round(pkhr,2),cex=2); mtext("Load",2); mtext("Wind",4,col="red")
          par(new=TRUE)
          plot(da$OPR_HOUR, da$wind, type='l', lwd=3, col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2); axis(4)
      datesopr=unique(rtgood$OPR_DATE)
        for (i in 1:length(datesopr))
        {          
          rtgoodnh=subset(rtgood, SP=='HB_NORTH' & OPR_DATE==datesopr[i]); rtgoodnh=rtgoodnh[order(rtgoodnh$HE),]; rtgoodnh=rtgoodnh[order(rtgoodnh$OPR_DATE),]
          plot(da$OPR_HOUR, da$ld_SystemTotal, type='l', lwd=4, bty="l", ylim=c(15000,max(da$ld_SystemTotal)*1.1), 
              main=paste(datesopr[i],"     ",rtgoodnh$dayname[1],"\n",
              "HR:    OTC-",round(max(rtgoodnh$plattshr),1),"    DA-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"])/rtgoodnh$plattsgas[1],1),"    RT-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])/rtgoodnh$plattsgas[1],1) ,sep=""), col="black")
           #main=paste(datesopr[i],"     ",rtgoodnh$dayname[1],"\nDA MWdly: ",round(max(rtgoodnh$plattshr),1),"   RT: ",round((mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]))/rtgoodnh$plattsgas[1],1),"\nRT:",round((mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])),1),"  RTadj:", round(gas*(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]))/rtgoodnh$plattsgas[1],1)  ,sep=""), col="black")
          lines(rtgoodnh$HE, rtgoodnh$ld, col="grey", lwd=4)
          mtext("Load",2); mtext("Wind",4,col="red")
          par(new=TRUE)
          plot(da$OPR_HOUR, da$wind, type='l', lwd=3, col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2); axis(4)
          lines(rtgoodnh$HE, rtgoodnh$wind, col="pink", lty=2, lwd=3)  
          points(rtgoodnh$HE, rtgoodnh$RTSPP*10, col="blue", pch=20)
          #par(new=TRUE)
          #plot(rtgoodnh$HE, rtgoodnh$plattshr, type='l', lwd=2, col="lightblue",bty="l", axes=FALSE, ylab="", lty=2,ylim=c(0,15));                 
        }
        par(mfrow=c(1,1))
      dev.off()
      rtgoodnh=subset(rtgood, SP=='HB_NORTH' )  
      rtgoodnh$RTadj=gas*rtgoodnh$RTSPP/rtgoodnh$plattsgas
  ## Send out results
          setLogLevel(1)
    #sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com,alex.werner@constellation.com"),
    sendEmail(from="lori.simpson@constellation.com", to=emailto,
              subject=paste("DA TOOLS.  Same day of the week from prior weeks: ", da$OPR_DATE[1],sep=""),
              body=paste("Same day of the week from prior weeks: ",da$dayname[1],"\n Average Prices: \n", paste(data.frame(round(tapply(rtgoodnh$RTadj, rtgoodnh$HE, mean),0))[,1],collapse=","),"\nRT adjusted and average prices are adjusted using the last gas-day's data.\n",gascheck,sep=""), 
              attachments=c(samewkdayfile))
          setLogLevel(3)
    









#################
#################
#################
  ## Plot data for recent days
    rLog("Starting plot section of recent days.")
    rtgood=rthourly
    limitdate=(as.POSIXct(dt)-24*24*60*60)
    rtgood=subset(rtgood, OPR_DATE>=limitdate)
      # rtgood=subset(rtgood, OPR_DATE>=as.POSIXct('2011-08-01') & OPR_DATE<as.POSIXct('2011-08-24')); da$LOADFC=da$ld_SystemTotal
    ## Get the Dallas and Houhigh temperature
      rLog("Pulling temperatures")
      arrTemperaturesDAL=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('wethr_ssi_dfw_temp_hist'), 0); colnames(arrTemperaturesDAL)[match("value",colnames(arrTemperaturesDAL))]="dallastemp"
      rtgood=merge(rtgood, arrTemperaturesDAL[,c("dt_he","dallastemp")], all.x=TRUE)
      arrTemperaturesIAH=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('wethr_ssi_iah_temp_hist'), 0); colnames(arrTemperaturesIAH)[match("value",colnames(arrTemperaturesIAH))]="houtemp"
      rtgood=merge(rtgood, arrTemperaturesIAH[,c("dt_he","houtemp")], all.x=TRUE)
      rLog("Pulled temperatures")
    ## Plot the similar days
      ##dev.off();dev.off();dev.off();dev.off()
      recentdaysfile=paste("S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/",as.POSIXlt(da$OPR_DATE[1])$year+1900,"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mon+1),"/",sprintf("%02.0f", as.POSIXlt(da$OPR_DATE[1])$mday),"_recentdays.pdf",sep="")
      pdf(recentdaysfile,width=12,height=10)      
      rtct=length(unique(rtgood$OPR_DATE))+1; if(rtct<=1) rc=1;  if(rtct<=4 & rtct>1) rc=2; if(rtct<=9 & rtct>4) rc=3; if(rtct<=16 & rtct>9) rc=4; if(rtct<=25 & rtct>16) rc=5; if(rtct<=36 & rtct>25) rc=6
      par(mfrow=c(rc,rc),mar=c(3,3,3,3))    ; da=da[order(da$OPR_HOUR),]
      ymax=max(da$prtld)*1.1
      plot(da$OPR_HOUR, da$LOADFC, type='l', lwd=4, bty="l", ylim=c(15000, ymax), 
          main=paste(dt, "\n",da$dayname[1], sep=""),cex.main=2);      text(10,40000,round(pkhr,2),cex=2); mtext("Load",2); mtext("Wind",4,col="red")
          par(new=TRUE)
          plot(da$OPR_HOUR, da$wind, type='l', lwd=3, col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2); axis(4)
      datesopr=unique(rtgood$OPR_DATE)
      textsave="Historical ERCOT data \n Temps differ from Dave's sheets because it is a different source\n\nHE 7-22 \n"; textsave2="HE 15-18 \n"
      textsaveop="OFFPEAK data (load and temps are the daily min; wind is the offpeak max wind output)\n"
      dfHistDly=data.frame(day=0, OPR_DATE=rtgood$OPR_DATE[1]-90000000, DAL=0, IAH=0, OTC=0, DA=0, RT=0, hsc=0, wind_min_HE16_18=0, holiday=0, ld_hrly_act=0, ld_hrly_fcst=0)[0,]
        for (i in 1:length(datesopr))
        {          
          rtgoodnh=subset(rtgood, SP=='HB_NORTH' & OPR_DATE==datesopr[i]); rtgoodnh=rtgoodnh[order(rtgoodnh$HE),]; rtgoodnh=rtgoodnh[order(rtgoodnh$OPR_DATE),]
          plot(da$OPR_HOUR, da$LOADFC, type='l', lwd=4, bty="l", ylim=c(15000, ymax), 
          # main=paste(datesopr[i],"     ",rtgoodnh$dayname[1],"\nDA MWdly: ",round(max(rtgoodnh$plattshr),1),"   RT: ",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])/rtgoodnh$plattsgas[1],1),"\nRT:",round((mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])),1),"  RTadj:", round(gas*(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]))/rtgoodnh$plattsgas[1],1)  ,sep=""), col="black")
              main=paste(datesopr[i],"     ",rtgoodnh$dayname[1],"\n",
              "HR:  OTC-",round(max(rtgoodnh$plattshr),1),"  DA-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"])/rtgoodnh$plattsgas[1],1),"  RT-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"])/rtgoodnh$plattsgas[1],1) ,sep=""), col="black")
          if(i!=length(datesopr)) 
              {
                textsave=paste(textsave, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tOTC-",round(max(rtgoodnh$plattspwr),0),
                                "\tDA-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"]),0),
                                "  \tRT-",round(mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]),0), 
                                "    \tLoad-",round(max(rtgoodnh[is.na(rtgoodnh$loadrt)==FALSE,"loadrt"]),-2),
                                "\tDallas-",round(max(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"dallastemp"]),0),
                                "\tHou-",round(max(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"houtemp"]),0), "\n", sep="")              
                textsaveop=paste(textsaveop, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tOTC-",round(min(rtgoodnh$plattspwr),1),"\tDA-",round(mean(subset(rtgoodnh,HE<7|HE>22)[,"DASPP"]),1),"  \tRT-",round(mean(subset(rtgoodnh,HE<7|HE>22)[,"RTSPP"]),1) 
                          , "  \tLoad-",round(min(rtgoodnh[is.na(rtgoodnh$loadrt)==FALSE,"loadrt"]),-2)
                          , "\twind-", round(max(subset(rtgoodnh[is.na(rtgoodnh$windrt)==FALSE,],HE<7|HE>22)[,"windrt"]),0)
                          , "\tDallas-",round(min(rtgoodnh[is.na(rtgoodnh$dallastemp)==FALSE,"dallastemp"]),0)
                          , "\tHou-",round(min(rtgoodnh[is.na(rtgoodnh$houtemp)==FALSE,"houtemp"]),0)
                          , "\n", sep="")              
                textsave2=paste(textsave2, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tDA-",round(mean(subset(rtgoodnh,HE>=15&HE<=18)[,"DASPP"]),0),"    \tRT-",round(mean(subset(rtgoodnh,HE>=15&HE<=18)[,"RTSPP"]),0), "\n", sep="")              
              }
                      #          if(i==length(datesopr)) 
                      #              {
                      #                otc=pull.tsdb(datesopr[i], datesopr[i], symbolst=c('MWDaily_16HrDly_ERCOTNort'), hourconvention=1)
                      #                dalmps=pull.tsdb(datesopr[i], datesopr[i]+1*60*60*24, symbolst=c('ZM_ERCOT_DAM_SPP_HB_NORTH'), hourconvention=0); dalmps$hour=as.POSIXlt(dalmps$dt_he)$hour
                      #                textsave=paste(textsave, rtgoodnh$dayname[1]," ", rtgoodnh$OPR_DATE[1], ":\tOTC-",round(otc$value,0),"\tDA-",round(mean(subset(dalmps,hour>=7&hour<=22)[,"value"]),0), "\n", sep="")              
                      #              }
          lines(rtgoodnh$HE, rtgoodnh$ld, col="grey", lwd=4)
          mtext("Load",2); mtext("Wind",4,col="red")
          par(new=TRUE)
          plot(da$OPR_HOUR, da$wind, type='l', lwd=3, col="red",bty="l", axes=FALSE, ylab="", ylim=c(0,7000), lty=2); axis(4)
          lines(rtgoodnh$HE, rtgoodnh$wind, col="pink", lty=2, lwd=3)  
          points(rtgoodnh$HE, rtgoodnh$RTSPP*10, col="blue", pch=20)
          #par(new=TRUE)
          #plot(rtgoodnh$HE, rtgoodnh$plattshr, type='l', lwd=2, col="lightblue",bty="l", axes=FALSE, ylab="", lty=2,ylim=c(0,15));                 
          rtgoodnhsuperpeak=subset(rtgoodnh, HE>=16 & HE<=18)
          # print(paste("i: ",i,".  Nrow(): ", nrow(rtgoodnhsuperpeak), ".  Test: ", nrow(rtgoodnhsuperpeak)!=0, sep="")); flush.console()
          if(nrow(rtgoodnhsuperpeak)!=0) {# print(paste("nrow(rtgoodnhsuperpeak)!=0: ",nrow(rtgoodnhsuperpeak)!=0,sep="")); flush.console()
                                          dfHistDly[nrow(dfHistDly)+1,]= list(rtgoodnhsuperpeak$dayname[1], 
                                                  rtgoodnhsuperpeak$OPR_DATE[1], 
                                                  max(rtgoodnh[!is.na(rtgoodnh$dallastemp),"dallastemp"]), 
                                                  max(rtgoodnh[!is.na(rtgoodnh$houtemp),"houtemp"]), 
                                                  max(rtgoodnh$plattspwr), 
                                                  mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"]), 
                                                  mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]),
                                                  min(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$platts_hsc)==FALSE,"platts_hsc"]),
                                                  min(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$windrt)==FALSE,"windrt"]),
                                                  rtgoodnhsuperpeak$holiday[1],
                                                  max(rtgoodnh[!is.na(rtgoodnh$ld),"ld"]), 
                                                  max(rtgoodnh[!is.na(rtgoodnh$load_sys_fcst),"load_sys_fcst"])
                                                )
                                          }
        }
        par(mfrow=c(1,1))
      dev.off()
      rtgoodnh=subset(rtgood, SP=='HB_NORTH' )  
      rtgoodnh$RTadj=gas*rtgoodnh$RTSPP/rtgoodnh$plattsgas
  ## Send out results
          setLogLevel(1)  
    #sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com,alex.werner@constellation.com"),
    sendEmail(from="lori.simpson@constellation.com", to=emailto,
              subject=paste("DA TOOLS.  Recent days: ", da$OPR_DATE[1],sep=""),
              body=paste("Recent days \n Average Prices: \n", paste(data.frame(round(tapply(rtgoodnh$RTadj, rtgoodnh$HE, mean),0))[,1],collapse=","),"\n RT prices adjusted using the last gas-day's data.  \n",gascheck,sep=""), 
              attachments=c(recentdaysfile))
    sendEmail(from="lori.simpson@constellation.com", to=emailto,
              subject=paste("ERCOT Recent Days' Clearing Prices",sep=""),
              body=paste(textsave, "\n\n\n", textsaveop, "\n\n\n", textsave2,sep=""))
          setLogLevel(3)
    
      rLog("Finished rt.price.fcst.")





#################
#################
#################
  ## Save file of all recent days
    rLog("Saving file of recent days.")

  ## GET NEW DATA
    ## Get the SAT temperature, peak 5-min SCED load, and DA temperature forecasts
      arrTemperaturesSAT=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Temp_ES_SAT_Dly_MAX'), 2); 
          colnames(arrTemperaturesSAT)[match("value",colnames(arrTemperaturesSAT))]="SAT"
          arrTemperaturesSAT=arrTemperaturesSAT[,c("OPR_DATE", "SAT")]
      arrRTload=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('ercot_system_load'), 0);  
          arrRTload$OPR_DATE=as.POSIXct(round(arrRTload$dt_he, "day"))
          arrRTload$variable="ld_SCED_max"
          arrRTload=cast(arrRTload, OPR_DATE~variable, max)
      arrTempF=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am'), 1); 
          arrTempF=cast(arrTempF, OPR_DATE~variable, max)
    ## Get the SAT temperature, peak 5-min SCED load, and DA temperature forecasts
      dfHistDly1=merge(dfHistDly, arrTemperaturesSAT)
      dfHistDly1=merge(dfHistDly1, arrRTload)
      dfHistDly1=merge(dfHistDly1, arrTempF)
   ## Order columns
      dfHistDlyNew=dfHistDly1[,c('OPR_DATE','day','holiday','ld_hrly_act','ld_hrly_fcst','ld_SCED_max','OTC','DA','RT','hsc','wind_min_HE16_18','DAL','IAH','SAT','Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am')]
      

  ## GET OLD DATA
      load(file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/historical_stats.RData"); # Get dfHistDly
      dfHistDlyOld=dfHistDly; 
      
  ## OVERWRITE OLD WITH NEW
      dfHistDlyNew$test=1
      dfTest=merge(dfHistDlyNew[,c("OPR_DATE","test")], dfHistDlyOld, all=TRUE)
      dfHistDlyNew=dfHistDlyNew[            ,c('OPR_DATE','day','holiday','ld_hrly_act','ld_hrly_fcst','ld_SCED_max','OTC','DA','RT','hsc','wind_min_HE16_18','DAL','IAH','SAT','Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am')]
      dfHistDlyOld=dfTest[is.na(dfTest$test),c('OPR_DATE','day','holiday','ld_hrly_act','ld_hrly_fcst','ld_SCED_max','OTC','DA','RT','hsc','wind_min_HE16_18','DAL','IAH','SAT','Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am')]
      dfHistDly=rbind(dfHistDlyNew, dfHistDlyOld)      
      save(dfHistDly, file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/historical_stats.RData"); 
      write.csv(dfHistDly, file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/historical_stats.csv"); 



  ## BACKFILL DATA
          backfill=FALSE
          if(backfill) 
            {
              rtgood=rthourly
              limitdate=(as.POSIXct("2012-08-01"))
              rtgood=subset(rtgood, OPR_DATE>=limitdate)
                  ## Get the Dallas and Houhigh temperature
              rLog("Pulling temperatures")
              arrTemperaturesDAL=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('wethr_ssi_dfw_temp_hist'), 0); colnames(arrTemperaturesDAL)[match("value",colnames(arrTemperaturesDAL))]="dallastemp"
              rtgood=merge(rtgood, arrTemperaturesDAL[,c("dt_he","dallastemp")], all.x=TRUE)
              arrTemperaturesIAH=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('wethr_ssi_iah_temp_hist'), 0); colnames(arrTemperaturesIAH)[match("value",colnames(arrTemperaturesIAH))]="houtemp"
              rtgood=merge(rtgood, arrTemperaturesIAH[,c("dt_he","houtemp")], all.x=TRUE)
              rLog("Pulled temperatures")
            ## Collect data
              datesopr=unique(rtgood$OPR_DATE)
              dfHistDlyBF=data.frame(day=0, OPR_DATE=rtgood$OPR_DATE[1]-90000000, DAL=0, IAH=0, OTC=0, DA=0, RT=0, hsc=0, wind_min_HE16_18=0, holiday=0, ld_hrly_act=0, ld_hrly_fcst=0)[0,]
                for (i in 1:length(datesopr))
                {          
                  rtgoodnh=subset(rtgood, SP=='HB_NORTH' & OPR_DATE==datesopr[i]); rtgoodnh=rtgoodnh[order(rtgoodnh$HE),]; rtgoodnh=rtgoodnh[order(rtgoodnh$OPR_DATE),]
                  rtgoodnhsuperpeak=subset(rtgoodnh, HE>=16 & HE<=18)
                  if(length(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$windrt)==FALSE,"windrt"])>0) strWind=min(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$windrt)==FALSE,"windrt"])
                  if(length(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$windrt)==FALSE,"windrt"])==0) strWind=min(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$wind)==FALSE,"wind"])
                  if(nrow(rtgoodnhsuperpeak)!=0) {
                                                  dfHistDlyBF[nrow(dfHistDlyBF)+1,]= list(rtgoodnhsuperpeak$dayname[1], 
                                                          rtgoodnhsuperpeak$OPR_DATE[1], 
                                                          max(rtgoodnh[!is.na(rtgoodnh$dallastemp),"dallastemp"]), 
                                                          max(rtgoodnh[!is.na(rtgoodnh$houtemp),"houtemp"]), 
                                                          max(rtgoodnh$plattspwr), 
                                                          mean(subset(rtgoodnh,HE>=7&HE<=22)[,"DASPP"]), 
                                                          mean(subset(rtgoodnh,HE>=7&HE<=22)[,"RTSPP"]),
                                                          min(rtgoodnhsuperpeak[is.na(rtgoodnhsuperpeak$platts_hsc)==FALSE,"platts_hsc"]),
                                                          strWind,
                                                          rtgoodnhsuperpeak$holiday[1],
                                                          max(rtgoodnh[!is.na(rtgoodnh$ld),"ld"]), 
                                                          max(rtgoodnh[!is.na(rtgoodnh$load_sys_fcst),"load_sys_fcst"])
                                                        )
                                                  }
                }
                ## Get the SAT temperature, peak 5-min SCED load, and DA temperature forecasts
                  arrTemperaturesSAT=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Temp_ES_SAT_Dly_MAX'), 2); colnames(arrTemperaturesSAT)[match("value",colnames(arrTemperaturesSAT))]="SAT"
                  arrRTload=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('ercot_system_load'), 0);  
                      arrRTload$OPR_DATE=as.POSIXct(round(arrRTload$dt_he, "day"))
                      arrRTload$variable="ld_SCED_max"
                      arrRTload=cast(arrRTload, OPR_DATE~variable, max)
                  arrTempF=pull.tsdb(min(rtgood$OPR_DATE), max(rtgood$OPR_DATE), symbolst=c('Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am'), 1); 
                      arrTempF=cast(arrTempF, OPR_DATE~variable, max)
                ## Get the SAT temperature, peak 5-min SCED load, and DA temperature forecasts
                  dfHistDly1=merge(dfHistDlyBF, arrTemperaturesSAT, all.x=TRUE)
                  dfHistDly1=merge(dfHistDly1, arrRTload, all.x=TRUE)
                  dfHistDly1=merge(dfHistDly1, arrTempF, all.x=TRUE)
               ## Order columns
                  dfHistDly=dfHistDly1[,c('OPR_DATE','day','holiday','ld_hrly_act','ld_hrly_fcst','ld_SCED_max','OTC','DA','RT','hsc','wind_min_HE16_18','DAL','IAH','SAT','Wethr_SSI_dfw_TEMP_Forc1_7am','Wethr_SSI_IAH_TEMP_Forc1_7am','Wethr_SSI_sat_TEMP_Forc1_7am')]
                  
              ## Save data
                  save(dfHistDly, file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/historical_stats.RData"); 
                   
    
            }


   return(TRUE)

  ## End the function      
  }





#######################
## DEPRECATED CODE USED TO SEND WARNING IN CASE DALLAS TEMPS ARE TOO LOW,
## THEN WE'D HAVE TO PAY FUEL OIL TO LAKE HUBBARD
#######################
#check.dallas.temps = function (startDate=Sys.Date()+1, testing=FALSE)
#  {
#  
#
#      if(testing==FALSE) emailto=c("ccgercot24hrdesk@constellation.com") else emailto=c("lori.simpson@constellation.com")
#      
#      ## Get temp forecasts
#          dallas_temps=pull.tsdb(startDate+0, startDate+1, symbolst=c('Wethr_SSI_DFW_TEMP_Forc0','Wethr_SSI_DFW_TEMP_Forc1_7am','Wethr_SSI_DFW_TEMP_Forc2_7am'), hourconvention=0)
#          if(nrow(dallas_temps)==0)  {rLog("Error--not getting forecast data.")}
#          dallas_temps=subset(dallas_temps, dt_he>=round(as.POSIXct(startDate+0),"day") & dt_he<=round(as.POSIXct(startDate+1),"day"))
#          dal_low=min(dallas_temps[is.na(dallas_temps$value)==FALSE,]$value)
#          
#      ##  Email watch  
#        if (dal_low<35)
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("Do NOT tpo Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#
#      ##  Email warning
#        if (dal_low<40 & dal_low>=35)
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("May not want to TPO Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#  }


#######################
## SEND WARNING IF PLANT TEMPS WILL BE BELOW FREEZING 
#######################
#check.plant.temps = function (startDate=Sys.Date(), testing=FALSE)
#  {
#
#      if(testing==FALSE) emailto=c("ccgercot24hrdesk@constellation.com") else emailto=c("lori.simpson@constellation.com")
#      
#       # Get temp forecasts
#          plant_temps=pull.tsdb(startDate+0, startDate+2, symbolst=c('Wethr_SSI_IAH_TEMP_Forc2_7am','Wethr_SSI_maf_TEMP_Forc2_7am'), hourconvention=0)
#          if(nrow(plant_temps)==0)  {rLog("Error--not getting forecast data.")}
#          cbec_temps=subset(plant_temps, variable=='Wethr_SSI_IAH_TEMP_Forc2_7am')
#          cb_low=min(cbec_temps[is.na(cbec_temps$value)==FALSE,]$value)
#          qrec_temps=subset(plant_temps, variable=='Wethr_SSI_maf_TEMP_Forc2_7am')
#          qr_low=min(qrec_temps[is.na(qrec_temps$value)==FALSE,]$value)
#          
#       # Email watch  
#        if (cb_low<35)
#          {
#            cb_log=paste("" ,sep="")
#          } else cblog=""
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("Do NOT tpo Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#
#       # Email warning
#        if (dal_low<40 & dal_low>=35)
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("May not want to TPO Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#  }
#





#######################
## SEND WARNING IF PLANT TEMPS WILL BE BELOW FREEZING 
#######################
#check.plant.temps = function (startDate=Sys.Date(), testing=FALSE)
#  {
#
#      if(testing==FALSE) emailto=c("ccgercot24hrdesk@constellation.com") else emailto=c("lori.simpson@constellation.com")
#      
#       # Get temp forecasts
#          plant_temps=pull.tsdb(startDate+0, startDate+2, symbolst=c('Wethr_SSI_IAH_TEMP_Forc2_7am','Wethr_SSI_maf_TEMP_Forc2_7am'), hourconvention=0)
#          if(nrow(plant_temps)==0)  {rLog("Error--not getting forecast data.")}
#          cbec_temps=subset(plant_temps, variable=='Wethr_SSI_IAH_TEMP_Forc2_7am')
#          cb_low=min(cbec_temps[is.na(cbec_temps$value)==FALSE,]$value)
#          qrec_temps=subset(plant_temps, variable=='Wethr_SSI_maf_TEMP_Forc2_7am')
#          qr_low=min(qrec_temps[is.na(qrec_temps$value)==FALSE,]$value)
#          
#       # Email watch  
#        if (cb_low<35)
#          {
#            cb_log=paste("" ,sep="")
#          } else cblog=""
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("Do NOT tpo Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#
#       # Email warning
#        if (dal_low<40 & dal_low>=35)
#         sendEmail(from="lori.simpson@constellation.com", to=emailto,
#              subject=paste("May not want to TPO Lake Hubbard for ", startDate,sep=""),
#              body=paste("SSI has Dallas's min hourly temp forecast for ",startDate, " at ",dal_low," degrees.\n\n  If NWS has an hourly forecast less than 35d, then we would have to pay fuel oil.  In this case we should NOT TPO!\n\n(http://forecast.weather.gov/MapClick.php?w0=t&w3u=1&w7=thunder&w8=rain&w9=snow&w10=fzg&w11=sleet&w15u=1&w16u=1&AheadHour=0&Submit=Submit&FcstType=graphical&textField1=32.79420&textField2=-96.76520&site=all&unit=0&dd=0&bw=0).", sep=""), 
#              )
#  }

#######################################################################
#######################################################################
# 
#
require(CEGbase)
options(width=150)      
require(SecDb)
require(reshape)
require(RODBC)
require(FTR)
require(PM)
require(Tsdb)
## Sys.setenv( SECDB_ORACLE_HOME=paste( "C:\\oracle\\ora90" ) )
## secdb.evaluateSlang( "Try( E ) { ODBCConnect(\"\",\"\"); } : {}" )

source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0



#------------------------------------------------------------
# Collect and email graphs
# res=rt.price.fcst(startDate=Sys.Date()+2, pkng=2.15, pklmp=25, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(2000,24),rep(2400,0))), conn=channel_zema_prod, testing=TRUE)
# 

  ## Connect to Nodal N-Market
      rLog("Loading DSN")
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      rLog("Connecting to odbc driver")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)


    ## Do Similar-Day Lookback for non-Fridays
      STARTDT=Sys.Date()+1
      rLog(paste("Start date: ",STARTDT,sep=""))
      if(as.POSIXlt(STARTDT)$wday!=6)
        {
          rLog(paste("Doing similar day for ", STARTDT, paste=""))
          rLog("About to start rt.price.fcst()")
          res=rt.price.fcst(startDate=STARTDT, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod, testing=FALSE)
          # rLog(paste("Doing similar day for ", STARTDT+1, paste=""))
          # res=rt.price.fcst(startDate=STARTDT+1, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod, testing=TRUE)
          # rLog(paste("Doing similar day for ", STARTDT-1, paste=""))
          # res=rt.price.fcst(startDate=STARTDT-1, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
        }
        # res=rt.price.fcst(startDate=STARTDT, pkng=2.2, pklmp=25, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
      
    ## Do Similar-Day Lookback for weekend/holidays
      if(as.POSIXlt(STARTDT)$wday==6)
        {
        rLog(paste("Getting data for ",STARTDT,sep=""))
          res=rt.price.fcst(startDate=STARTDT, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
        rLog(paste("Getting data for ",STARTDT+1,sep=""))
          res=rt.price.fcst(startDate=(STARTDT+1), RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
        rLog(paste("Getting data for ",STARTDT+2,sep=""))
          res=rt.price.fcst(startDate=(STARTDT+2), RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
        }



  ## LOOK AT ADDITIONAL DATES
     # res=rt.price.fcst(startDate=STARTDT, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod, testing==TRUE)
     # INPUT DA LMP INTO PKLMP: res=rt.price.fcst(startDate=STARTDT, pkng=4, pklmp=130, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod)
    ADDL_TEST=FALSE
      if(ADDL_TEST==TRUE)
        {
            STARTDT=Sys.Date()+1
            for (i in 1:4)
            {
            STARTDT=STARTDT+1; STARTDT
                cat(paste("\n\n\n\nGetting data for ",STARTDT,sep=""))
                  res=rt.price.fcst(startDate=STARTDT, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1591,24),rep(2400,0))), conn=channel_zema_prod, testing=TRUE)
             }      
        }
  
  ## Close ODBC connection
  odbcCloseAll()
  


    ## Email Lake Hubbard warning if temps are close to <35d 
      # STARTDT=Sys.Date()+1
      # if(STARTDT<as.Date('2011-12-29'))
        # {res=check.dallas.temps(startDate=STARTDT, testing=FALSE)}


  

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
