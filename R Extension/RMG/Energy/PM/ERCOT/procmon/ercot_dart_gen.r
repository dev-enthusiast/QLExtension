# Use the DAM market results to determine which plants will be on
#
#
#
# Written by Lori Simpson on 28-Jun-2011





#------------------------------------------------------------
# MAIN
#

    require(CEGbase)
    options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
    require(SecDb)
    require(reshape)
    require(RODBC)
    require(FTR)
    # source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
    source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
    rLog(paste("Start proces at", Sys.time()))
    returnCode <- 0    # succeed = 0
    
    

 



#------------------------------------------------------------
# FUNCTIONS
# startDate=Sys.Date(); conn=channel_zema_prod; 
#




#
#it_name from the physical_name column
#        if the sum of sold mw for a given my_unit_name is close to the min/max, considered a run of the plant
#        if outside hours are zeroes or numbers, consider inside hours a self-schedule
#        treat as null if value is <50% of LSL--what about high turn down ratios?  50% only for CCs?
#        
#        Conditions:
#        1: Inverse hours--i.e. runs all hours that there is a null value, but have to add in nearby hours
#        
#
#      rLog("Starting ??")
#





## GET DART GEN
#    startDate=STARTDT; startDateHsls=STARTDATEHsls; endDateHsls=ENDDATEHsls; conn=channel_zema_prod
get.dam.gen = function(startDate, startDateHsls, endDateHsls, conn, plantdata)
  {


    ## Get DAM sales
      strQry=paste("select mt.opr_date, mt.opr_hour, value, mt.SP name  FROM
                      ZE_DATA.ERCOT_NODAL_MARKET_TOTALS mt
                      where mt.opr_date=to_date('",startDate,"', 'YYYY-MM-DD') and type='S'", sep="")
  		damraw <- sqlQuery(conn, strQry); if(nrow(damraw)==0) {rLog("Returned 0 rows from ZE_DATA.ERCOT_NODAL_MARKET_TOTALS.  Perhaps results haven't been posted?"); return("Returned 0 rows from ZE_DATA.ERCOT_NODAL_MARKET_TOTALS.  Perhaps results haven't been posted?")}
      rLog(paste("Pulled ",nrow(damraw)," rows of DAM data over",length(unique(damraw$OPR_DATE)),"days."))
      save(damraw, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/dam_gen.rdata")
      


    ## Get the plant data and merge with rucs - make sure that there is a listing for each unique name (need to know if physical or logical!)
      ## Get updated HSLs
        arrHsls = get.gen.hsls (startdate=startDateHsls, enddate=endDateHsls, conn=channel_zema_prod)
          ## write.csv(arrHsls, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/hsls_temp.csv")
        plantdatahsl = merge(plantdata, arrHsls,all=TRUE)
        arrMissingData = plantdatahsl[is.na(plantdatahsl$zone)==TRUE,c("ercot_unit_code","zone","LSL","HSL")]
        if(nrow(arrMissingData)>0)
          {
                  arrMissingData$email = apply(arrMissingData, 1,  function(x)  {paste(x[1], ", ", x[3], ", ", x[4],"\n")}  )         
                  ## Send out results
                        setLogLevel(1)    
                        sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                            subject=paste("Need to update HSLs/LSLs in upload_plant_datafinal.csv",sep=""),
                            body=paste("Need to update HSLs/LSLs in \nS:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv\n with\n\n"
                            , paste(arrMissingData$email, collapse=""), sep=""))
                        setLogLevel(3)
                  rLog(paste("Not finding all ercot_unit_codes!.  Need to update HSLs/LSLs in \nS:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv\n with\n\n"
                            , paste(arrMissingData$email, collapse=""), sep=""))
          }  else rLog("All plants we found HSL data for can be found in the upload_plant_datafinal.csv sheet.")
        ## Use phaseminmw and phasemaxmw for missing updated LSL/HSL data
          plantdatahsl[is.na(plantdatahsl$LSL)==TRUE,"LSL"] = plantdatahsl[is.na(plantdatahsl$LSL)==TRUE,"phaseminmw"]
          plantdatahsl[is.na(plantdatahsl$HSL)==TRUE,"HSL"] = plantdatahsl[is.na(plantdatahsl$HSL)==TRUE,"phasemaxmw"]


    ## Merge Plant data with DAM awards
        dam=merge(damraw, plantdatahsl, all.x=TRUE)

      ## Check if have mapped all the SPs
        damtest=dam[is.na(dam$mw)==TRUE,]; 
        damtest=subset(damtest, VALUE>5)
        if(length(unique(damtest$NAME))>0) 
          { rLog(paste(length(unique(damtest$NAME)),"SPs require mapping in Second Simpson/R/written docs/prod/upload_plant_datafinal.csv."))
            alert.SPs(arrSPs=unique(damtest$NAME), conn)
            return("See email: SPs require mapping in Second Simpson/R/written docs/prod/upload_plant_datafinal.csv.")
          }  else rLog("All plants we found DAM award data for can be found in the upload_plant_datafinal.csv sheet.")
      
    return(dam)


    }






###########################################33
## Evaluate the CCs
## arrDAM=arrDam; arrNAMES=names
## How to handle Forney on 11/4 that had phys and fin awards that look like gen

find.cc.run = function(arrDAM, arrNAMES)
    {
        ## Get only CCs
          namescc=subset(arrNAMES, my_type=="ccle90" | my_type=="ccgt90" )[,1:2]
          colnames(namescc)=c("my_unit_name")
          damcc=merge(arrDAM, namescc)
          load(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/dam_gen.rdata")


        ## Output file
          damccres=data.frame(OPR_HOUR=0, OPR_DATE=damraw$OPR_DATE[1], dammw=0, phaserun=0, phasemw=0, techtype="cc", schdltype=0)[0,]

        ## Individually look at each plant
            plants=unique(damcc$my_unit_name)
            for (planti in 1:length(plants))
            for (planti in (planti+1):(planti+6))
              {
                #planti=planti+1
                damccplanti=subset(damcc, my_unit_name==plants[planti])
                damccplanti$value=as.numeric(damccplanti$VALUE)
                # Get one set of data 
                  damccplanti$variable="nada"; damcci_names=cast(damccplanti, NAME+sp_type~variable, length)
                  damcci=merge(damcci_names[,1:2], damraw)
                ## cast(damcci, OPR_DATE + OPR_HOUR ~ sp_type + NAME, sum)
                damcci$value=as.numeric(damcci$VALUE)
                damccic = cast(damcci, OPR_DATE + OPR_HOUR ~ sp_type, sum)
                arrHours=data.frame(OPR_DATE=damccic$OPR_DATE[1], OPR_HOUR=0:23)
                damccic=merge(damccic, arrHours, all=TRUE)
                damccic$run=0; damccic$shift=0
                damccic=damccic[order(damccic$OPR_HOUR),]

              ## Calculate DAM-awarded TPOs for logical configurations
                if (!is.na(match("logical",colnames(damccic))))  #if(length(which(is.na(damcci$logical)))>0 & length(which(is.na(damcci$logical)==FALSE))>0)
                      {
#!is.na(match("logical",colnames(damccic)))
                            damccic$phaserun=""
                            damccic$phasemw=""
                            damccic$techtype=""
                            damccic$schdltype=""
                            plant_phases = subset(plantdata, my_unit_name==plants[planti] & sp_type=='logical')
                          ## All NULLs for logical node hours are run hours OR OUTAGES!
                            damccic$schdltype[which(is.na(damccic$logical))]="selfschdl"
                            damccic$run[which(is.na(damccic$logical))]=floor(max(plant_phases$phasemaxmw)); damccic$shift=damccic$run
                            ## If only morning or evening hours, assume that the lack of data is an outage
                              zeroes=subset(damccic, logical==0); nas=damccic[is.na(damccic$logical)==TRUE,]
                              if(nrow(nas)>0) if(min(nas$OPR_HOUR)==0 & mean(zeroes$OPR_HOUR)>10 & mean(zeroes$OPR_HOUR)<20 ) 
                                  {
                                    damccic$schdltype[which(is.na(damccic$logical))]="outage?"
                                    damccic$run[which(is.na(damccic$logical))]=0; damccic$shift=damccic$run
                                  }
                          ## All logical awards are DAM awards IF ABOVE LSL!
                            damccic$schdltype[which(damccic$logical>min(plant_phases$phaseminmw[is.na(plant_phases$phaseminmw)==FALSE]))]="awd"
                            damccic$run[which(damccic$logical>min(plant_phases$phaseminmw[is.na(plant_phases$phaseminmw)==FALSE]))]=damccic$logical[which(damccic$logical>min(plant_phases$phaseminmw[is.na(plant_phases$phaseminmw)==FALSE]))]; damccic$shift=damccic$run
                          ## Find out which configuration was procured
                            plant_phases=plant_phases[order(plant_phases$phaseminmw),]
                          ## Find the correct phase and MWs procured
                            ## Match LSL and HSL to phasemin/max
                              LSLHSL=cbind(data.frame(tapply(damccplanti$LSL,damccplanti$ercot_unit_code, mean)),data.frame(tapply(damccplanti$HSL,damccplanti$ercot_unit_code, mean)))
                              colnames(LSLHSL)=c("LSL","HSL"); LSLHSL$ercot_unit_code=rownames(LSLHSL)
                              plant_phases=merge(plant_phases, LSLHSL)
                            ## Get plant info  
                            for(rowi in 1:nrow(plant_phases))
                              {
                                  plant_phases$phaseminmw[rowi]=min(plant_phases$phaseminmw[rowi], plant_phases$LSL[rowi])
                                  plant_phases$phasemaxmw[rowi]=max(plant_phases$phasemaxmw[rowi], plant_phases$HSL[rowi])
                                  damccic$phaserun[which(damccic$run>=(plant_phases$phaseminmw[rowi]*0.9))]=plant_phases$NAME_CC[rowi]                              
                                  damccic$phasemw[which(damccic$run>(plant_phases$phaseminmw[rowi]*0.9))]=round(plant_phases$phasemaxmw[rowi],0)                              
                              }
                            for(rowi in 1:nrow(damccic))                              
                              {if(damccic$phasemw[rowi]<damccic$run[rowi]) damccic$phasemw[rowi]=damccic$run[rowi]}
                            damccic$techtype="cc"; colnames(damccic)[match("run",colnames(damccic))]="dammw"
                          ## Look at data to check
                            dlt=damraw[grep(unlist(strsplit(plants[planti],"_"))[1],damraw$NAME),]; dlt=dlt[order(dlt$OPR_HOUR),]; print(dlt[order(dlt$NAME),])
                            #print(subset(damraw, NAME==plants[planti])[1:24,]); print("Will only return logical results.")
                            print(plant_phases)
                            print(damccic)
                            readline(prompt = "Pause. Press <Enter> to continue...") 
                          ## ERROR OUT IF AWARD NOT WITH LSL/HSL
                            arrTest=subset(damccic, dammw!=0 & phaserun=="")
                            if(nrow(arrTest)>0) {return(paste("Award MW of ",arrTest$logical[1]," for ",plants[planti]," not within range of phaseminmw (",round(min(plant_phases$phaseminmw),0),") and phasemaxmw (",round(max(plant_phases$phaseminmw),0),") (LSL and HSL) in the upload_plant_datafinal table",sep=""))}  
                          ## Merge with old data
                            damccres=rbind(damccres, damccic[(damccic$dammw)!=0,c("OPR_HOUR","OPR_DATE","dammw","phaserun","phasemw","techtype","schdltype")])                        
                            # print(damccres)
                      }
                     
              ## Calculate ?? for only physical results
                if (is.na(match("logical",colnames(damccic))) & !is.na(match("physical",colnames(damccic))))  
                {
                  ##  WHAT DO I DO HERE????
                  print("Only physical")
                            subset(damraw, NAME==plants[planti])[1:2,]; print("Will only return logical results.")
                            plant_phases
                            damccic
                  
                }

            }
      }
      




###########################################33
## Evaluate which units are expected to run in a certain zone?  Combine with heat rate, or shift factor data?







      
    




#------------------------------------------------------------
# GET THE DART GEN!
#
  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)

  ## Set variables
    rLog(paste("Start proces at", Sys.time()))
    STARTDT=Sys.Date()
    STARTDATEHsls=Sys.Date() - 72
    ENDDATEHsls=Sys.Date() - 60

  ## Get the data
    ## GET Plant data:  NAME is NOT the common name for a phase!
      rLog("Getting the plant data.")
      plantdata=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv", as.is=TRUE)
      plantdata=plantdata[,c('NAME','my_unit_name_c','my_unit_name','ercot_unit_code','NAME_CC','fuel','my_type','sp_type','mw','unitminmw','unitmaxmw','phaseminmw','phasemaxmw','zone')]
    arrDam=get.dam.gen(startDate=STARTDT, startDateHsls=STARTDATEHsls, endDateHsls=ENDDATEHsls, conn=channel_zema_prod, plantdata)
    ## GET UNIQUE PLANTS - the length is the number of unique names per common name
      names=data.frame(NAME=unique(dam$my_unit_name_c))
      names=merge(names, plantdata[,c("NAME","my_type")])
      names=aggregate(names$NAME, by=list(names$NAME, names$my_type), length) 
      colnames(names)=c("NAME","my_type")
      
  ## Get DART gen for CCs
    find.cc.run(arrDAM=arrDam, arrNAMES=names)
            
      

  ## GET GEN STACK
      get.gen.stack(startDate=Sys.Date(), conn)



  ## Close database connection
  odbcClose(channel_zema_prod)
  odbcCloseAll()

  # Log finish
  rLog(paste("Done at ", Sys.time()))




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

#      write.csv(da, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/da.csv")






