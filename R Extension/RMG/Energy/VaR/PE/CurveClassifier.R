################################################################################
# This library will provide an interface for classifying curves used in the PE
# calculation. If we change how we will classify curves in the future, this
# interface will allow us to hide the underlying logic.
#

################################################################################
# External Libraries
#
library(RODBC)
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
CurveClassifier = new.env(hash=TRUE)


################################################################################
# Exported Variables

#Curve classification types
CurveClassifier$CT_FUEL_REF = "FUEL_REF"
CurveClassifier$CT_NG_REF = "NG_REF"
CurveClassifier$CT_ELEC_REF = "ELEC_REF"
CurveClassifier$CT_MARKET = "MARKET"


################################################################################
# Returns the universal name prefix to use when labeling data according to
# classification type.
#
CurveClassifier$getNamePrefix <- function( type )
{
  if( type == CurveClassifier$CT_FUEL_REF )
  {
    return("AllFuelReferenceCurves")
  }else if( type == CurveClassifier$CT_NG_REF )
  {
    return("NGReferenceCurves")
  }else if( type == CurveClassifier$CT_ELEC_REF )
  {
    return("ElecMarketReferenceCurves")
  }else if( type==CurveClassifier$CT_MARKET )
  {
    return("%MKT%Curves")
  }else
  {
    rError("Invalid curve type specified. Can not generate name prefix.")
    return(NULL)
  }
}


################################################################################
# Returns the classification of a given curve based on it's name
#
CurveClassifier$getCurveType <- function( curveName )
{
  curveName = toupper(curveName)
  
  curveMapData = CurveClassifier$loadCurveClassificationList()

  curveInfo = subset(curveMapData, curve.name == curveName)[1,]
  
  if( nrow(curveInfo) > 0 )
  {

    if( CurveClassifier$.isFuelReferenceCurve(curveInfo) )
    {
      return( CurveClassifier$CT_FUEL_REF )
    } else if( CurveClassifier$.isNGReferenceCurve(curveInfo) )
    {
      return( CurveClassifier$CT_NG_REF )

    } else if( CurveClassifier$.isPowerReferenceCurve(curveInfo) )
    {
      return( CurveClassifier$CT_ELEC_REF )
    } else
    {
      return( CurveClassifier$CT_MARKET )
    }
  } else
  {
    rError( "Unable to classify curve:", curveName, "\nCurve information not",
      "found in map file!" )
    return(NULL)
  }
}


################################################################################
# Loads the map of curve names to classification types
#
CurveClassifier$loadCurveClassificationList <- function()
{
  if( is.null(CurveClassifier$.curveInfoMap) )
  {
    CURVE_MAP_PATH =
      "H:/user/R/RMG/Models/Price/ForwardCurve/Network/allcurves.xls"
    WORKSHEET_NAME = "allcurves"
  
    channel = odbcConnectExcel(CURVE_MAP_PATH)
    data = sqlFetch(channel, WORKSHEET_NAME)
    names(data)[1:5] = c("curve.name", "reference", "parent1", "parent2", "parent3")
    odbcClose(channel)
  
    data$curve.name = toupper(data$curve.name)
    data <- data.frame(as.matrix(data), stringsAsFactors=FALSE)
    
    CurveClassifier$.curveInfoMap = data
  }
  
  invisible(CurveClassifier$.curveInfoMap)
}



########################### INTERNAL OBJECTS ###################################


################################################################################
# Private variables

#Where we store the curve info map loaded in .loadCurveClassificationList
CurveClassifier$.curveInfoMap = NULL


################################################################################
# Is it one of the fuel reference curves?
#
CurveClassifier$.isFuelReferenceCurve <- function(curveInfo)
{
  if( is.na(curveInfo$curve.name) )
  {
    return( FALSE )
  }
  
  if(nrow(curveInfo) > 1) browser()

  return( curveInfo$curve.name %in% c("COMMOD NG EXCHANGE",
    "COMMOD WTI EXCHANGE", "COMMOD FCO API2 WEEKLY", "COMMOD COL NYMEX PHYSICAL") )
}


################################################################################
# Is it one of the NG reference curves?
#
CurveClassifier$.isNGReferenceCurve <- function(curveInfo)
{
  if( is.na(curveInfo$curve.name) || is.na(curveInfo$reference) )
  {
    return( FALSE )
  }

  return( regexpr("COMMOD NG ", curveInfo$curve.name) != -1 && 
          curveInfo$reference == "REGIONAL REF" )
}


################################################################################
# Is it one of the power market reference curves?
#
CurveClassifier$.isPowerReferenceCurve <- function(curveInfo)
{
  if( is.na(curveInfo$curve.name) || is.na(curveInfo$reference) )
  {
    return( FALSE )
  }

  return( regexpr("COMMOD PW", curveInfo$curve.name) != -1 && 
          curveInfo$reference=="COMMOD REF" )
}




