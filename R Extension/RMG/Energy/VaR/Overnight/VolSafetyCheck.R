################################################################################
# DESCRIPTION
#

################################################################################
# External Libraries
#
source('H:/user/R/RMG/Utilities/Packages/load.packages.r')
load.packages( c("SecDb", "RODBC") )
source("//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/overnightUtils.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/sendemail.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")


################################################################################
# File Namespace
#
VolSafetyCheck = new.env(hash=TRUE)



################################################################################
#
VolSafetyCheck$main <- function()
{
    #how far back do we want to check CPSPROD
    DAY_RANGE_TO_QUERY = 14
    
    asOfDate <- as.Date(overnightUtils.getAsOfDate())
    
    toDay = asOfDate
    fromDay = toDay - DAY_RANGE_TO_QUERY
    
    VolSafetyCheck$checkRange( fromDay, toDay, sendEmail=TRUE )
    
    print("Done.")
}


################################################################################
#
VolSafetyCheck$checkRange <- function( fromDay, toDay, sendEmail=FALSE )
{
    #Connect to CPSPROD
    rLog("Connecting to CPSPROD...")
    con.string <- paste("FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn",
            ";UID=stratdatread;PWD=stratdatread;", sep="")
    CPSPROD <- odbcDriverConnect(con.string)
    rLog("Connected.")
    
    rLog("Get all vol curves between", fromDay, "and", toDay+1)
    allQuery <- paste("SELECT unique(commod_curve) from foitsox.sdb_bs_volatility where ",
            "pricing_date>=to_date('", fromDay, "','YYYY-MM-DD') ",
            "and pricing_date<to_date('", toDay+1, "','YYYY-MM-DD') ",
            sep="")
    lastWeeksWorth  <- sqlQuery(CPSPROD, allQuery)  # all the vol curves
    rLog("Query Complete.")
    
    print( paste("Get all vol curves for ",toDay, "...", sep="") )
    todayQuery <- paste("SELECT unique(commod_curve) from foitsox.sdb_bs_volatility where ",
            "pricing_date=to_date('", toDay, "','YYYY-MM-DD') ",
            sep="")
    today   <- sqlQuery(CPSPROD, todayQuery)  # todays vol curves
    rLog("Query Complete.")
    
    #for testing, DISABLE BY DEFAULT!!!
    #today = data.frame(today[-c(1,3,4,7,8,10,150),])
    
    rLog("Calculating missing curves...")
    names(today) = "COMMOD_CURVE"
    
    allCurves = as.character(lastWeeksWorth$COMMOD_CURVE)
    todayCurves = as.character(today$COMMOD_CURVE)
    missingCurves = data.frame(setdiff(allCurves,todayCurves))
    names(missingCurves) = "COMMOD_CURVE"
    
    #verify that the curves that are missing actually have vega positions
    curvesWithVega = VolSafetyCheck$.getCurvesWithVega( toDay )
#  if( !is.null(curvesWithVega) )
#  {
#    stillPresent = which( missingCurves$COMMOD_CURVE %in%
#      curvesWithVega$COMMOD_CURVE )
#    badCurves = data.frame(missingCurves[stillPresent,])
#    names(badCurves) = "COMMOD_CURVE"
#    wasVegaVerified = TRUE
#  } else
#  {
#    badCurves = missingCurves
#    wasVegaVerified = FALSE
#  }
    VolSafetyCheck$.handleMissingList( toDay, missingCurves, TRUE,
            sendEmail )
    
    odbcClose(CPSPROD)
    rLog("Done.")
}


################################################################################
# Sends an email if the list of missing curves is non-zero to a larger
# distro than if =0.
VolSafetyCheck$.handleMissingList <- function( toDay, missingCurves,
        wasVegaVerified, sendEmail )
{
    if( nrow(missingCurves)==0 )
    {
        subject = paste("SUCCESS: All volatility curves present for", toDay)
        
        body = paste("Please Contact Jenny Zhang For More Information.",
                "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")
        
        toList ="john.scillieri@constellation.com"
        
        print(subject)
        
    } else
    {
        subject = paste("WARNING: The following", nrow(missingCurves),
                "curves are missing volatility in CPSPROD")
        
        body = paste("As Of Date:", toDay)
        missingList = paste(unlist(missingCurves), collapse="\n")
        body = paste(body, missingList,
                "Please Contact Jenny Zhang For More Information.",
                "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")
        
        cat(subject,"\n")
        cat(body,"\n")
        
        toList = "john.scillieri@constellation.com"
        
        cat(toList,"\n")
    }
    
    if( sendEmail )
    {
        sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
                to=toList, subject, body)
    }
}

################################################################################
# This function extracts all the positions from VCENTRAL that actually have
# a VEGA value. It is used to check that the list of missing positions doesn't
# contain positions that have expired.
#
VolSafetyCheck$.getCurvesWithVega <- function( asOfDate )
{
    # we subtract one here because we don't want to be dependent on todays
    # position pull.  The assumption is that we should be able to get the previous
    # business day's position file because somebody better have fixed it if it broke.
    asOfDate = secdb.dateRuleApply(as.Date(asOfDate),"-1b")
    options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
    filename <- paste( options$save$datastore.dir, "Positions/VCV.positions.",
            options$asOfDate, ".RData", sep="" )
    
    if( file.exists(filename) )
    {
        
        #loads position variable QQ.all
        load(filename)
        
        hasVega = subset(QQ.all, VEGA!=0)
        
        curvesWithVega = data.frame(unique(hasVega$CURVE_NAME))
        names(curvesWithVega) = "COMMOD_CURVE"
        
    } else
    {
        cat("\nUNABLE TO VERIFY VEGA POSITIONS BECAUSE VCV FILE WAS NOT FOUND!\n")
        curvesWithVega = NULL
    }
    
    return(curvesWithVega)
}


################################ MAIN EXECUTION ################################
VolSafetyCheck$main()

