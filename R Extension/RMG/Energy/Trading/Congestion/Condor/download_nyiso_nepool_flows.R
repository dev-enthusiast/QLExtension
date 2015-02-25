# To get the flows between NY and NEPOOL, go to:
# Market Data > Market & Operational Data > Reports & Info tab > Reports> Daily Energy
# it's a csv file named yyyymmddDAM_energy_rep.csv published at 10:00 AM every day. 
# column BK "NPX Proxy DNI NPX-AC" is what you want + number means flow is going into NY
# column AE is the total of AC, 1385 and CSC, BH is Roseton only
#
## Daily Energy Report - Day Ahead Market (DAM) 
## NYISO Load Forecast: The weather-adjusted forecast of the expected
## energy load or consumption for the day as determined by the NYISO.
## Customer Load Forecast: The forecast energy consumption as submitted
## by NYISO Customers.  For informational purposes, the percentage
## difference between the Customer and NYISO load forecasts is provided.
## Load Bids: The amount of load Customers will procure in the DAM at the
## location based marginal price (LBMP). Customers may choose to bid more
## or less than their load forecast with any excess or shortage being
## settled in the real-time market.  All fixed load bids offered within
## New York are automatically scheduled by NYISO.
## Bilateral Bids: These transactions are scheduled directly between an
## internal generator and an internal load, that is, bilateral - between
## two parties.  All firm bilateral bids offered are also automatically
## scheduled by NYISO.
## Price Capped Load Bids: This allows customers to set limits on the
## amount they are willing to pay for their energy.  Customers can also
## choose to buy more energy if the price is lower.  These load bids
## consist of price capped physical loads at internal NYCA bus specific
## locations.
## Virtual Load Bids: Day-Ahead purchase of energy by a non-LSE that is
## sold back in Real-Time.  This is the virtual energy purchased in the
## DAM and sold in the RT Market.  These are financial transactions only
## and have no effect on real time physical energy consumption.  The
## amount scheduled is determined by price, thus not all that is bid gets
## scheduled.  Actual energy consumed equals 0 MWHrs.
## Generation Bids: All generating units in New York must submit bids in
## order to be considered by NYISO.  Units are scheduled based on
## economics and the need to maintain reliable system operation.
## Virtual Supply Bids: Day-Ahead sale of energy by a non-Generator that
## is bought back in Real-Time.  This is the virtual energy sold in the
## DAM and purchased in the RT Market.  These are financial transactions
## only and have no effect on real time physical energy consumption.  The
## amount scheduled is determined by price, thus not all that is bid gets
## scheduled.  Actual energy consumed equals 0 MWHrs.
## Imports: This is energy brought into New York from neighboring control
## areas, which include PJM Interconnection, ISO New England, Hydro
## Quebec and IMO (Ontario). The amount scheduled is determined by price,
## thus not all that is bid gets scheduled.  Does not include Wheel
## Throughs.
## Exports: Energy flowing out of New York to the neighboring control
## areas.  Again price determines how much energy is actually scheduled
## for export.  Does not include Wheel Throughs.
## Net Imports/(Exports): This is the sum of energy imports less exports.
## Wheel throughs have no impact on this calculation as their flow in and
## out nets to zero.
## Wheel Throughs: The amount of energy purchased in one control area and
## delivered to another control area, that is, just wheeling through New
## York.


# Written by Adrian Dragulescu on 2010-10-28


############################################################################
# the flows
.download_report_flows <- function(reportDate, destfile)
{  
  rootURL <- "http://mis.nyiso.com/public/csv/damenergy/"
  fullURL <- paste(rootURL, format(reportDate, "%Y%m%d"),
                   "DAM_energy_rep.csv", sep="")
  aux <- try(download.file(fullURL, destfile))
  if (aux != 0){
    return(99)             # FAILED, file not there?!
  } 

  return(0)       # SUCCESS
}

############################################################################
# 
.read_DA_binding_constraints <- function(reportDate)
{  
  rootURL <- "http://mis.nyiso.com/public/csv/DAMLimitingConstraints/"
  URL <- paste(rootURL, format(reportDate, "%Y%m%d"),
               "DAMLimitingConstraints.csv", sep="")
  aux <- try(read.csv( URL ))
  if (class(aux) == "try-error"){
    return(NULL)             # FAILED, file not there, 
  } 
  aux$Time.Zone <- NULL
  aux$Facility.PTID <- NULL
  colnames(aux)[4] <- "Constraint.Cost"
  
  out <- capture.output( print(aux, row.names=FALSE) )
  
  return(out)    
}


############################################################################
# 
.output_email <- function(reportDate, destfile)
{
  out <- c(
   .read_DA_binding_constraints(reportDate),   
   "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/download_nyiso_nepool_flows", 
   "Contact: Adrian Dragulescu")
  
  out
}  


############################################################################
############################################################################
# MAIN 

options(width=2400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
returnCode <- 0    # succeed = 0


asOfDate   <- Sys.Date()
reportDate <- asOfDate + 1    # for next day

rLog("Download NYISO DA energy report for", as.character(reportDate))
rLog("Start at ", as.character(Sys.time()))

outDir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/",
  "NYPP/OpAnnouncements/", sep="")
destfile <- paste(outDir, "dam_energy_report.csv", sep="")


returnCode <- .download_report_flows(reportDate, destfile)

if (returnCode == 0) {
  to <- paste("NECash@constellation.com", "vikram.krishnaswamy@constellation.com",
      "dibyendu.das@constellation.com", "andrew.culham@constellation.com",     
      "MidAtlCash@constellation.com", "andrew.hlasko@constellation.com", sep=", ")
  #to <- "adrian.dragulescu@constellation.com"
  rLog("Email results")
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    to,
    paste("NYISO DA report for", reportDate),
    .output_email(reportDate, destfile), attachments=destfile)
}

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

