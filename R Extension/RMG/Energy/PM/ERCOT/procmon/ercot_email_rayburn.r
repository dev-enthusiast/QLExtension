# Email Rayburn load forecast to Rayburn
#  Takes the xDA lock of Rayburn forecast and emails to Rayburn
#
#
# Written by Lori Simpson on 31-Oct-2011




#------------------------------------------------------------
# Functions
# startdate=Sys.Date()+0; enddate=startdate+3; testing=TRUE
#

email.ray.load = function(startdate, enddate, testing)
    {
      if((startdate-Sys.Date())>5) {sendEmail(from="lori.simpson@constellation.com", to="lori.simpson@constellation.com", subject="NEED TO UPDATE Rayburn Forecast Lock!", body=paste("Can't return data for more than 6 days forward.  Missing data for dates ",startdate, " to ",enddate,".\n\nLori Simpson\n410-470-3217\nlori.simpson@constellation.com",sep="")); return("Looking too far out for data!")}

      rayld=pull.tsdb(startdate-5, enddate, symbolst=c('ercot_ray_ld_lock'), hourconvention=0)
      rayld$value=rayld$value/4
      ## Get opr_day
        rayld$HE=as.POSIXlt(rayld$dt_he)$hour
        rayld$OPR_DATE=rayld$dt_he - rayld$HE*60*60
        rayld[rayld$HE==0,"OPR_DATE"]=rayld[rayld$HE==0,"OPR_DATE"]-1*60*60*24
        rayld[rayld$HE==0,"HE"]=24
        dates_returned=data.frame(dt=unique(rayld$OPR_DATE), returned=1)
        dates_desired=data.frame(dt=seq(startdate, enddate, by="day")); ## dates_desired$dt=round(as.POSIXct(dates_desired$dt), "day")
        dates_desired$dt=paste(as.POSIXlt(dates_desired[,1])$year+1900,sprintf("%02.0f", as.POSIXlt(dates_desired[,1])$mon+1),sprintf("%02.0f", as.POSIXlt(dates_desired[,1])$mday),sep="-"); dates_desired$dt=as.POSIXct(dates_desired$dt)
      
      ## Check that we've retrieved all dates
        datescheck=merge(dates_returned, dates_desired, all.y=TRUE)
        failedct=nrow(datescheck[is.na(datescheck$returned)==TRUE, ])
      
      if(failedct>0)
        {
        sendEmail(from="lori.simpson@constellation.com", to="lori.simpson@constellation.com",
              subject="NEED TO UPDATE Rayburn Forecast Lock!",
              body=paste("Missing some data for dates ",startdate, " to ",enddate,".\n\nLori Simpson\n410-470-3217\nlori.simpson@constellation.com",sep=""))
        }
       
      if(failedct==0)
        {
          ## Who do i email
            if(testing==TRUE) emailto=c("lori.simpson@constellation.com")
            if(testing==FALSE) emailto=c("akirkland@rayburnelectric.com, jkirkland@rayburnelectric.com, dnaylor@rayburnelectric.com, jdavis@rayburnelectric.com, #wtercot@constellation.com, ercotpm2@constellation.com, lori.simpson@constellation.com, Suzanne.Eagles@constellation.com, Laura.Castle@constellation.com, Matt.Swartz@constellation.com, Jason.Miller@constellation.com, Adam.Boyd@constellation.com, Gary.Gibson@constellation.com, craig.dolan@constellation.com, walter.kuhn@constellation.com")
    
          ## Write file to send out
            datafile="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Load Bids/rayloadlock.csv"
            colnames(dates_desired)="OPR_DATE"
            rayld=merge(rayld, dates_desired)
            write.csv(rayld[,c("dt_he","value")], file=datafile)

          ## Check for reasonableness of results
            if( min(rayld$value)<(140/4) | max(rayld$value)>(775/4)) 
                {
                rLog(paste("The data does not seem reasonable.  It should be within 35 and 194 MW, but is returning a min value of ", round(min(rayld$value),0)," and a max value of ", round(max(rayld$value),0),".",sep=""))
                sendEmail(from="lori.simpson@constellation.com", to="lori.simpson@constellation.com",
                      subject="Rayburn Forecast Lock data unreasonable!",
                      body=paste("The data does not seem reasonable.  It should be within 35 and 194 MW, but is returning a min value of ", round(min(rayld$value),0)," and a max value of ", round(max(rayld$value),0)," for dates ",startdate, " to ",enddate,".",sep=""))
                return(paste("The data does not seem reasonable.  It should be within 35 and 194 MW, but is returning a min value of ", round(min(rayld$value),0)," and a max value of ", round(max(rayld$value),0),".",sep=""))
                }

          ## Email results    
            if( min(rayld$value)>(140/4) & max(rayld$value)<(775/4)) 
                {
                  setLogLevel(1)
                  sendEmail(from="lori.simpson@constellation.com", to=emailto,
                          subject="Rayburn Forecast Lock",
                          body=paste("Please see attached for the Rayburn lock for ",startdate, " to ",enddate,".\n\nLori Simpson\n410-470-3217\nlori.simpson@constellation.com",sep=""),
                          attachments=c(datafile))
                      setLogLevel(3)
                }
        }


    }





#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(RODBC)
require(Tsdb)
require(FTR)
# source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0





#------------------------------------------------------------
# Collect and upload gas data
#   email.ray.load(STARTDT+3, STARTDT+3, testing==FALSE)

  ## Set variables
    STARTDT=Sys.Date()
    STARTDTdayofweek=as.POSIXlt(STARTDT)$wday

    ## Send Data
     email.ray.load(STARTDT+1, STARTDT+1, testing=FALSE)

    ## TESTING
      # email.ray.load(STARTDT-0, STARTDT+1, testing=TRUE)
      # email.ray.load(STARTDT+0, STARTDT+1, testing=FALSE)
      # HOLIDAYS:  email.ray.load(STARTDT+1, STARTDT+5, testing=FALSE)

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




#------------------------------------------------------------
# Upload data
#

BACKFILL=FALSE

  if(BACKFILL==TRUE)
    {
       raydata = read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/uploadraydafcst.csv");
        #write.csv(dlt, file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/dlt.csv")

      ## Upload data
        symbol <- "ercot_ray_ld_lock"
        dfRay  <- data.frame(time=as.POSIXct(raydata[1000:nrow(raydata),4]),
                              value=raydata[1000:nrow(raydata),3])
        rLog("Made dataframe")
        tsdbUpdate(symbol, dfRay)
        ## rLog(paste("Tried to upload I2 data to TSDB for date ",round(dfNoms[1,1],"day")," --success uncertain",sep=""))
     }









#                       sendEmail(from="lori.simpson@constellation.com", to=c("ercotpm2@constellation.com"),
#                          subject="Testing automated emails--please respond",
#                          body=paste("Please please reply if you receive this email so i know this is working.  THanks!",sep=""),
#                          attachments=c(datafile))
