# Get ERCOT gen outages (telemetered resource status of OUT)
# cllig=coal.  ccgt90=CC >90MW.  ccle90=CC <90MW.  gsnonr=cogen?.  gsreh=steamer (e.g. LHSES).  gssup=cogen?.  scgt90=CT >90MW.  ccle90=CT <90MW.
#  hydro=hydro.  nuc=nuclear
#
# Written by Lori Simpson on 17-May-2011



dev=FALSE
## dev=TRUE


#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(reshape)
## Sys.setenv( SECDB_ORACLE_HOME=paste( "C:\\oracle\\ora90" ) )
## secdb.evaluateSlang( "Try( E ) { ODBCConnect(\"\",\"\"); } : {}" )
if(dev) {source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R"); print("Testing")}  ## For testing
if(!dev) {source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R"); print("Not testing")}  ## For production
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0






#------------------------------------------------------------
# Functions
# desiredDate=DESIREDDT; conn=channel_zema_prod      ; goutsraw=goutsraw_in
#

upload.60d.gen.outages = function(desiredDate, goutsraw, conn)
    {


      rLog(paste("Data available--inside upload.60d.gen.outages",  sep=""))

    ## Correct data
      goutsra=subset(goutsraw, TELEMETERED_RESOURCE_STATUS=="OUT" )[,c("OPR_DATE", "OPR_HOUR", "OPR_MINUTE", "RESOURCE_NAME", "RESOURCE_TYPE", "LSL", "HSL", "ie", "TELEMETERED_RESOURCE_STATUS")]
      colnames(goutsra)[match("RESOURCE_NAME",colnames(goutsra))]="ercot_unit_code"

    ## Get the plant data and merge with OUTAGES
      rLog("Getting the plant data.")
      plantdataraw=read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv", as.is=TRUE)
      # plantdata=plantdata[,c('PHYSICAL_NAME','LOGICAL_NAME','unit_name','unit_code','county','fuel','zone','year_start','mw','outage_mw','type','owner','PUN','cc','fueltype')]
      plantdata=plantdataraw[,c('ercot_unit_code','NAME','fuel','zone','mw','outage_mw','type','cc','technology')]
      colnames(plantdata)[match("technology",colnames(plantdata))]="fueltype"; colnames(plantdata)[match("NAME",colnames(plantdata))]="PHYSICAL_NAME"
      gouts=merge(goutsra, plantdata, all.x=TRUE)
      colnames(gouts)[match("ercot_unit_code",colnames(gouts))]="LOGICAL_NAME"; 
      rLog(paste("Found ",length(unique(gouts$LOGICAL_NAME))," plants on outage.",sep=""))
      goutstest=gouts[is.na(gouts$zone==TRUE),]
      nametest=length(unique(goutstest$LOGICAL_NAME))
      gouts$outage_mw=as.numeric(gouts$outage_mw)
     ## Email names that aren't mapped
        if(nametest>0)
          { 
            goutstestlsl=goutstest; colnames(goutstestlsl)[match("LSL",colnames(goutstestlsl))]="value"
              goutstestlsl=cast(goutstestlsl, LOGICAL_NAME+RESOURCE_TYPE~fuel, mean); colnames(goutstestlsl)[3]="LSL"
            goutstesthsl=goutstest; colnames(goutstesthsl)[match("HSL",colnames(goutstesthsl))]="value"
              goutstesthsl=cast(goutstesthsl, LOGICAL_NAME+RESOURCE_TYPE~fuel, mean); colnames(goutstesthsl)[3]="HSL"
            goutstest=merge(goutstesthsl, goutstestlsl, all=TRUE)  
            emailsend="\nLOGICAL_NAME\tRESOURCE_TYPE\tHSL\tLSL\n"
            for(goutstesti in 1:nrow(goutstest))
              {
                emailsend=paste(emailsend,goutstest[goutstesti,1],"\t",goutstest[goutstesti,2],"\t\t",goutstest[goutstesti,3],"\t",goutstest[goutstesti,4],"\n") 
              }
            setLogLevel(1)
            sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                subject=paste("ercot_upload_gen_outages error: Do not have mappings for all plants", sep=""),
                body=paste("Could not find these plants: \n",paste(unique(goutstest$LOGICAL_NAME), collapse=","),
                "\n\n In this file:\n S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv for ",desiredDate,"\n Need to update file.\n",emailsend,sep=""))
            
            ## Secondary alert system
              alert.SPs(arrSPs=unique(goutstest$LOGICAL_NAME), conn)
            
            setLogLevel(3)                
            rLog("Script killed:  Do not have mappings for all RUC plants in the file--see email and update those mappings.")
            rLog(paste("Could not find these plants: \n",paste(unique(goutstest$LOGICAL_NAME), collapse=","),
                "\n\n In this file:\n S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/prod/upload_plant_datafinal.csv for ",desiredDate,"\n Need to update file.\n",emailsend,sep=""))
            q(status=0)
            return(FALSE)
           } else  rLog("All plant names are mapped.")

    ## Need to find the net capacity out for CC units
      rLog(paste("Getting net capacity for CC units",  sep=""))
      goutscc=subset(gouts, cc==1 & type!="PUN")            
        ## Get the sum of outage_mw by physical resource      
          colnames(goutscc)[match("outage_mw",colnames(goutscc))]="value"; goutscc$variable="value"; 
          goutscc=cast(goutscc, LOGICAL_NAME+ie+PHYSICAL_NAME+fueltype~variable, mean)
          goutscc$variable="summw"
          goutscc=cast(goutscc, ie+PHYSICAL_NAME+fueltype~variable, mean)
        ## Get the min and max MW by physical resource
          plantdataccmin=subset(plantdata, cc==1)
          colnames(plantdataccmin)[match("mw",colnames(plantdataccmin))]="value"; plantdataccmin$variable="minmw"
          plantdataccmin=cast(plantdataccmin, PHYSICAL_NAME~variable, min)
          plantdataccmax=subset(plantdata, cc==1)
          colnames(plantdataccmax)[match("mw",colnames(plantdataccmax))]="value"; plantdataccmax$variable="maxmw"
          plantdataccmax=cast(plantdataccmax, PHYSICAL_NAME~variable, max)
          plantdataccmax=merge(plantdataccmax, plantdataccmin)
        ## Ensure that outage capacity between min and max
          goutsccm=merge(goutscc, plantdataccmax, all.x=TRUE)
          ## goutsccm$mw=as.numeric(apply(goutsccm, 1,  function(x)  { max(min(x["summw"],x["maxmw"]),x["minmw"])  }  )  )
          goutsccm$mw=pmin(pmax(goutsccm$summw, goutsccm$minmw),goutsccm$maxmw)
          goutsccm=goutsccm[,c("PHYSICAL_NAME","ie","fueltype","mw")]
          colnames(goutsccm)[1]='LOGICAL_NAME'
          goutsccm[is.na(goutsccm$mw),"mw"]=0
          

    ## Total outages for non-CCs
      goutso=subset(gouts, cc==0 & type!="PUN")            
      colnames(goutso)[match("outage_mw",colnames(goutso))]="value"; goutso$variable="mw"
      goutso=cast(goutso, LOGICAL_NAME+ie+fueltype~variable, mean)
      goutso[is.na(goutso$mw),"mw"]=0

    ## Combine outages, ensure have all types, and cast
      gouts=rbind(goutso, goutsccm); mindt=min(gouts$ie); maxdt=max(gouts$ie)
      ## Ensure have all types
        fueltypes=data.frame(fueltype=c('renew','wind','hydro','cc','coal','ct','steam','nuc','dctie','dg','oil')) 
        gouts$fueltype=tolower(gouts$fueltype)
        gouts=merge(gouts, fueltypes, all=TRUE)
        gouts[is.na(gouts$mw),"mw"]=0
      ## Cast
        colnames(gouts)[match("mw", colnames(gouts))]="value"; gouts$value=as.numeric(gouts$value)
        gouts=cast(gouts, ie~fueltype, sum)
        rLog("Have merged the outages for CC and non-CC")
    
    ## Ensure there is a time stamp for all fields
      goutsdates=data.frame("ie"=seq(mindt, maxdt, by=60*15))
      gouts=merge(gouts, goutsdates, all=TRUE)
      ## gouts=gouts[is.na(gouts$ie)==FALSE,]
      if(length(gouts[is.na(gouts)==TRUE])>0) 
        {
          rLog(paste("Replacing ",nrow(gouts[is.na(gouts$wind)==TRUE,])," rows of 15-minute data with preceding intervals.",sep=""))
          ## rLog(paste("NAs found in the gouts table when running upload.60d.gen.outages function--missing data for intervals ending ",paste(gouts[is.na(gouts$wind)==TRUE,"ie"],collapse=", "),sep=""))
          for(i in 2:nrow(gouts)) {if(is.na(gouts$wind[i])==TRUE) {gouts[i,c(2:12)]=gouts[i-1,c(2:12)]} }
          for(i in (nrow(gouts)-1):1) {if(is.na(gouts$wind[i])==TRUE) gouts[i,c(2:12)]=gouts[i+1,c(2:12)]}
        }
        
      

    ## Upload data
      gouts=gouts[is.na(gouts$ie)==FALSE,]
      gouts=gouts[,c('ie','renew','wind','hydro','cc','coal','ct','steam','nuc','dg','oil')]
      fueltypes=colnames(gouts[2:ncol(gouts)])
      for (i in 1:length(fueltypes))
        {
          symbol <- paste("pm_ercot_cop_out_", fueltypes[i], sep="")
          dfGouts  <- data.frame(time=gouts$ie,
                                value=gouts[,i+1])
          tsdbUpdate(symbol, dfGouts)          
        }
      rLog(paste("Attempted to upload ",nrow(gouts)," hours of data to TSDB COP gen outages.",sep=""))
      
    return(TRUE)

}





upload.iir.gen.outages = function(desiredDate, conn)
  {
  iirraw = FTR:::FTR.getGenerationOutagesIIR(ctrlarea="ERCO", start.dt=as.Date('2010-01-28'), end.dt=as.Date('2011-09-01'))
  #write.csv(unique(iirraw$unit.name, iirraw$plant.name))  

  iirraw$variable="plant"; iirraw$value=1
  iir=cast(iirraw, unit.name+plant.name+prim.fuel+utype.desc+out.design+heatrate~variable, length)
  
  write.csv(iir, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")  
  
  }
















#------------------------------------------------------------
# Do Something
#


  ## Connect to Zema DB
      DSN_PATH_zema_prod = paste("FileDSN=",fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn"), 
                                  ";UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)


      DESIREDDT=Sys.Date()-62
        rLog(paste("Looking for data for ", DESIREDDT,  sep=""))
            
            #------------------------------------------------------------
            # Collect data
            #
            
            
                ## Get all gen units
                  goutsraw_in=pull.60d.sced.data(DESIREDDT, DESIREDDT, channel_zema_prod); sqlSuccess=TRUE
                  
            
                ## Return empty if cannot find data
                  if(length(goutsraw_in)==1) {
                                      setLogLevel(0)
                                      sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                                          subject=paste("ercot_upload_gen_outages error: didn't find data", sep=""),
                                          body=paste("No data was found in ercot_nodal_60dscedgrd sced for ",  DESIREDDT, sep=""))
                                      setLogLevel(3)
                                      rLog(paste("Did not find data for desired date of ",DESIREDDT,sep=""))
                                      return(paste("Did not find data for desired date of ",DESIREDDT,sep=""))
                                      sqlSuccess=FALSE
                                      q(status=0) # zero status = successful
                                      } else 
            
            
            #------------------------------------------------------------
            # Upload Gen outages
            #
            
            
              ## Get and upload gen outages found in the COPs for the most recent data
                if(sqlSuccess==TRUE) 
                    {
                      rLog(paste("Data available--entering upload.60d.gen.outages",  sep=""))
                      res=upload.60d.gen.outages(goutsraw=goutsraw_in, desiredDate=DESIREDDT, conn=channel_zema_prod)
                    }

                  ## Get Data for old date in case first function didn't work
                    # DESIREDDT=Sys.Date()-71
                    # if(sqlSuccess==TRUE) res=upload.60d.gen.outages(desiredDate=DESIREDDT, conn=channel_zema_prod)
            
              ## Get and upload gen outages found in the COPs for the most recent data
                # DESIREDDT=Sys.Date()-61
                #res=upload.iir.gen.outages(desiredDate=DESIREDDT, conn=channel_zema_prod)
            
            if(!res) rLog(paste("Did not successfully upload outages for ",DESIREDDT, sep=""))
            if(res) rLog(paste("Successfully uploaded outages for ",DESIREDDT, sep=""))
       
    
    
    









#------------------------------------------------------------
# Finalize
#

  ## Close database connection
  odbcClose(channel_zema_prod)

  # Log finish
  rLog(paste("Done at ", Sys.time()))
  q(status=0)




#------------------------------------------------------------
# Interactive=true if ran by hand
#
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



#------------------------------------------------------------
# BACKFILL DATA
#

backfill=FALSE

  if(backfill)
    {
          DESIREDDT=as.Date("2012-05-12")

          for(i in 1:20)
          
          {
          
                         
                  
                  
                  #------------------------------------------------------------
                  # Collect data
                  #
                  
                   print(paste(DESIREDDT))
    
                      ## Get all gen units
                        goutsraw_in=pull.60d.sced.data(DESIREDDT, DESIREDDT, channel_zema_prod); sqlSuccess=TRUE
                  
                      ## Return empty if cannot find data
                        if(length(goutsraw_in)==1) {
                                            setLogLevel(0)
                                            sendEmail(from="lori.simpson@constellation.com", to=c("lori.simpson@constellation.com"),
                                                subject=paste("ercot_upload_gen_outages error: didn't find data", sep=""),
                                                body=paste("No data was found in ercot_nodal_60dscedgrd sced for ",  DESIREDDT, sep=""))
                                            setLogLevel(3)
                                            rLog(paste("Did not find data for desired date of ",DESIREDDT,sep=""))
                                            return(paste("Did not find data for desired date of ",DESIREDDT,sep=""))
                                            sqlSuccess=FALSE
                                            }
                  
                  
                  #------------------------------------------------------------
                  # Upload Gen outages
                  #
                  
                  
                    ## Get and upload gen outages found in the COPs for the most recent data
                      if(sqlSuccess==TRUE) res=upload.60d.gen.outages(goutsraw=goutsraw_in, desiredDate=DESIREDDT, conn=channel_zema_prod)
                        ## Get Data for old date in case first function didn't work
                          # DESIREDDT=Sys.Date()-71
                          # if(sqlSuccess==TRUE) res=upload.60d.gen.outages(desiredDate=DESIREDDT, conn=channel_zema_prod)
                  
                    ## Get and upload gen outages found in the COPs for the most recent data
                      # DESIREDDT=Sys.Date()-61
                      #res=upload.iir.gen.outages(desiredDate=DESIREDDT, conn=channel_zema_prod)
                  
             if(!res) return("Loop killed")
             
             print(paste(DESIREDDT))
             print("\n\n")
             DESIREDDT=DESIREDDT+1    
             flush.console()
       
          }
     }     
    




