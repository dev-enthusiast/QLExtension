# Email and upload RUC units
#
# Written by Lori Simpson on 25-May-2011




#------------------------------------------------------------
# Functions
# startDate=STARTDT; endDate=ENDDT; conn=channel_zema_prod      ; testing=TRUE
#



pull.ruc.data = function(startDate, endDate, conn, testing=FALSE)
    {
      rLog("Starting pull.ruc.data")
      if(testing==TRUE) emailto=c("lori.simpson@constellation.com") else emailto=c("ccgercot24hrdesk@constellation.com, ptsouthgroup@exeloncorp.com")

    
    ## Get DRUC units
      dructest=1
      strQry=paste("select d.delivery_date, d.hour_ending, d.resource_name ercot_unit_code, d.start_type, d.cleared_commit, d.cleared_decommit, d.reason_for_cmt_or_dcmt, d.reason_for_man_override, d.ruc_timestamp
                    from ze_data.ercot_nodal_druc_cd_res d
                    where delivery_date>=to_date('",startDate,"', 'YYYY-MM-DD') and delivery_date<=to_date('",endDate,"', 'YYYY-MM-DD')", sep="")
  		drucraw <- sqlQuery(conn, strQry); if(nrow(drucraw)>0) drucraw$ruc="druc" else dructest=0
      rLog(paste("Pulled",nrow(drucraw),"rows of DRUC data"))
  		

    ## Get HSL for the units
      hructest=1
      strQry=paste("select d.delivery_date, d.hour_ending, d.resource_name ercot_unit_code, d.start_type, d.cleared_commit, d.cleared_decommit, d.reason_for_cmt_or_dcmt, d.reason_for_man_override, d.ruc_timestamp
                    from ze_data.ercot_nodal_hruc_cu_res d
                    where delivery_date>=to_date('",startDate,"', 'YYYY-MM-DD') and delivery_date<=to_date('",endDate,"', 'YYYY-MM-DD')", sep="")
  		hrucraw <- sqlQuery(conn, strQry); if(nrow(hrucraw)>0) hrucraw$ruc="hruc" else hructest=0
      rLog(paste("Pulled",nrow(hrucraw),"rows of DRUC data"))

    if(hructest==1 & dructest==1) ruc=rbind(hrucraw, drucraw)
    if(hructest==0 & dructest==1) ruc=drucraw
    if(hructest==1 & dructest==0) ruc=hrucraw
    if(hructest==0 & dructest==0) {rLog("Found no RUCs"); return(NULL); break; exit}
    colnames(ruc)[match("ERCOT_UNIT_CODE",colnames(ruc))]="ercot_unit_code"

    ## Get the plant data and merge with rucs
      rLog("Getting the plant data.")
      plantdata=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv", as.is=TRUE)
      options(warn=-1)
      plantdata=plantdata[,c('ercot_unit_code','fuel','zone','mw','phasemaxmw','technology')]; plantdata$plantmw=as.numeric(plantdata$mw); plantdata$mw=as.numeric(plantdata$phasemaxmw)
      plantdata[is.na(plantdata$mw)==TRUE, "mw"]=plantdata[is.na(plantdata$mw)==TRUE, "plantmw"]
      plantdata=plantdata[,c('ercot_unit_code','fuel','zone','mw','technology')]
      options(warn=1)
          ## If needing to modify the save file for changes to plantdat file: colnames(rucisave)[match("PHYSICAL_NAME",colnames(rucisave))]="PHYSICAL_NAME"
      nametestbefore=length(unique(ruc$ercot_unit_code))
      ruc=merge(ruc, plantdata, all.x=TRUE)
      ructest=ruc[is.na(ruc$zone==TRUE),]
      nametestbefore=length(unique(ructest$ercot_unit_code))

    ## Email names that aren't mapped
      if(nametestbefore>0)
        {sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
              subject=paste("ercot_ruc error: Do not have mappings for all plants", sep=""),
              body=paste("Could not find these plants: \n",paste(unique(ructest$ercot_unit_code), collapse=","),
              "\n\n In this file:\n S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv \n Need to update file.",sep=""))
          rLog("Script killed:  Do not have mappings for all RUC plants in the file--see email and update those mappings.")
          return(FALSE)
         } else  rLog("All plant names are mapped.")

    ## Get the saved outages
      ruc$yr=as.POSIXlt(ruc$DELIVERY_DATE)$year+1900
      yrs=unique(ruc$yr)
      rucnew=ruc[0,]
      filestart="S:/All/Portfolio Management/ERCOT/RUC/annual_files/rucunits_"
      fileend=".RData"
      for (i in 1:length(yrs))
        {
            ruci=subset(ruc, yr==yrs[i])
            filesave=paste(filestart, yrs[i], fileend, sep="")
            if(file.exists(filesave))
              {
              load(file=filesave);
                  ## Resave old data:   load(file=filesave); rucisave=subset(rucisave, DELIVERY_DATE<=as.POSIXct('2011-10-01'));  save(rucisave, file=filesave);
              rucisave$test=TRUE
              ructest=merge(ruci, rucisave, all=TRUE)
              ruckeep=ructest[is.na(ructest$test)==TRUE,1:(ncol(ructest)-1)]
              ## Combine new data over the years' loops
                rucnew=rbind(rucnew,ruckeep)
                rLog(paste("Found ",nrow(ruckeep)," new rows of data in ",yrs[i],"."))
              ## Save combined file
                load(file=filesave);
                rucisave=rbind(rucisave, ruckeep)
                save(rucisave, file=filesave);
              }
            if(!file.exists(filesave))
              {
              rucisave=ruci
              rLog(paste("Found ",nrow(rucisave)," new rows of data in ",yrs[i],".  This is the first data found for this year!"))
              save(rucisave, file=filesave);
              rucnew=rbind(rucnew,ruci)
              }
        }

      ##  Email out new RUC listings (for recent days only?)
        # rucemail=subset(new, DELIVERY_DATE>=Sys.Date()-2)
        rucplot=rucnew
        if(nrow(rucnew)>0)
        { 
            ## Get the new RUC units  
              rucplot$dt=rucplot$DELIVERY_DATE+rucplot$HOUR_ENDING*60*60
              rucplot$LOGICAL_NAME=rucplot$ercot_unit_code
              rucplot$dt=rucplot$DELIVERY_DATE
              rtct=length(unique(rucplot$LOGICAL_NAME))+1; if(rtct<=1) rc=1;  if(rtct<=4 & rtct>1) rc=2; if(rtct<=9 & rtct>4) rc=3; if(rtct<=16 & rtct>9) rc=4; if(rtct<=25 & rtct>16) rc=5; if(rtct<=36 & rtct>25) rc=6
              plants=unique(rucplot$LOGICAL_NAME)
              ## par(mfrow=c(rc,rc),mar=c(3,3,3,3))    ;
              committext=paste("Found ", length(plants)," new RUC units from ", min(rucplot$DELIVERY_DATE), " to ",max(rucplot$DELIVERY_DATE), ": \n\n",sep="")
              for(i in 1:length(plants))
                {
                  rucploti=subset(rucplot, LOGICAL_NAME==plants[i])
                  if(rucploti$CLEARED_COMMIT[1]=="Y") testcommit="commit" else testcommit="decommit"
                  ## plot(rucploti$dt, rucploti$mw, type="h" , lwd=50, xlim=c(min(rucplot$dt), max(rucplot$dt)),ylim=c(0, max(rucplot$mw)*1.1), main=paste(plants[i],".   ",rucploti$zone[1],".   ",rucploti$fuel[1],"\nReason for ",testcommit,"ment: ",rucploti$REASON_FOR_CMT_OR_DCMT[1],sep=""),bty="l")
                  committext=paste(committext, plants[i], " (", rucploti$unit_name[1], "a " ,rucploti$mw[1], " mw ", 
                      rucploti$fuel[1]," ",rucploti$technology[1], " plant in the ",
                      rucploti$zone[1], " zone) ",testcommit,"ed from ",
                      min(rucploti$dt)," HE",min(subset(rucploti,dt==min(rucploti$dt))$HOUR_ENDING)," to ", 
                      max(rucploti$dt)," HE",max(subset(rucploti,dt==max(rucploti$dt))$HOUR_ENDING),
                      " for reason: ",rucploti$REASON_FOR_CMT_OR_DCMT[1],", in the RUC ran at ",rucploti$RUC_TIMESTAMP[1],
                      ".  [",rucploti$PUN[1],"]\n\n",  
                      sep="")
                }  
              ### Get the new RUC units  
              #  filestart="S:/All/Portfolio Management/ERCOT/RUC/annual_files/rucunits_"
              #  fileend=".RData"
              #  filesave=paste(filestart, yrs[i], fileend, sep="")
              #  load(file=filesave);
              #  mindt=min(ruc$DELIVERY_DATE)
              #  ructsdb=subset(rucisave, DELIVERY_DATE>=mindt & CLEARED_COMMIT=="Y")
              #  ructsdb$dt=ructsdb$DELIVERY_DATE+ructsdb$HOUR_ENDING*60*60
              
            sendEmail(from="lori.simpson@constellation.com", to=emailto,
                         subject=paste("ERCOT--Units RUC'd for ", paste(unique(rucplot$DELIVERY_DATE),collapse=","), sep=""),
                        body=committext)
               rLog("Emailed new RUCs")
     }       else rLog("Found no new RUCs to email")



      ##  Upload TSDB symbols
        for (i in length(yrs))
        {
            
            filestart="S:/All/Portfolio Management/ERCOT/RUC/annual_files/rucunits_"
            fileend=".RData"
            filesave=paste(filestart, yrs[i], fileend, sep="")
            load(file=filesave);
            mindt=min(ruc$DELIVERY_DATE)
            ructsdb=subset(rucisave, DELIVERY_DATE>=mindt & CLEARED_COMMIT=="Y")
            ructsdb$dt=ructsdb$DELIVERY_DATE+ructsdb$HOUR_ENDING*60*60
            colnames(ructsdb)[match("mw",colnames(ructsdb))]="value"
            ructsdb$value=as.numeric(ructsdb$value)
            if(nrow(ructsdb[is.na(ructsdb$technology)==TRUE,])>0) {rLog("Do not have a technology for all RUCs."); return(ructsdb[is.na(ructsdb$technology)==TRUE,])}
            ructsdb=cast(ructsdb, dt~technology, sum)
            colnames(ructsdb)=tolower(colnames(ructsdb))
              ## Check that do not have new technology types
                techtypes=data.frame(haveya=c('cc','coal','hydro','renew','ct','oil','steam','nuc','wind'),havect=1)
                techtypes2=data.frame(haveya=colnames(ructsdb)[2:ncol(ructsdb)],need=1)
                techcheck=merge(techtypes, techtypes2, all.y=TRUE)
                if(nrow(techcheck[is.na(techcheck$havect)==TRUE,])>0) rLog(paste("Missing technology types: ",paste(techcheck[is.na(techcheck$havect)==TRUE,]$haveya,collapse=", "),sep=""))
                
          ## Upload by fuel type
            for (j in 2:ncol(ructsdb))
              {
                symbol=paste("pm_ercot_ruc_", tolower(colnames(ructsdb)[j]), sep="")
                dfRuc  <- data.frame(time=ructsdb$dt, value=ructsdb[,j])
                tsdbUpdate(symbol, dfRuc)
              }
      }
        
     rLog("Attempted to Upload data to TSDB")


    }





#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(reshape)
# source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
# source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0








#------------------------------------------------------------
# Collect and upload gas data
#

  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)

  ## Get Data -- Function found below
    rLog(paste("Start proces at", Sys.time()))
    STARTDT=Sys.Date() - 6
    ENDDT=Sys.Date() + 2
    res=pull.ruc.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod, testing=FALSE)
    rLog(paste("End proces at", Sys.time()))

  ## Close database connection
  odbcClose(channel_zema_prod)

  # Log finish
  rLog(paste("Done at ", Sys.time()))

















#------------------------------------------------------------
# Force fail so procmon runs script hourly until final hour when needs to succeed
#
    returnCode <- 99
    if (as.numeric(format(Sys.time(), "%H"))==23)   returnCode <- 0   




#------------------------------------------------------------
# Interactive=true if ran by hand
#
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}










## BACKFILL DATA
#for (i in 500:505)
#{
#   STARTDT=Sys.Date()-i
#   ENDDT=Sys.Date()-i+1
#
#    res=store.bentek.data.tx(startDate=STARTDT, endDate=ENDDT, conn=channel_bentek)
#    print(res)
#}



