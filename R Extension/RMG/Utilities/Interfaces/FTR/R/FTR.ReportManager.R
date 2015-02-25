FTR.ReportManager <- function(region="NEPOOL")
{
  res <- NULL
  
  if (region == "NEPOOL") {
    res <- .FTR.ReportManager.NEPOOL()
  } else if (region == "NYPP")
    res <- .FTR.ReportManager.NYPP()
  } else if (region == "PJM") {
    res <- .FTR.ReportManager.PJM()
  } else {
    stop("Unsupported region ", region)
  }
  
  return(res)
}

##############################################################################
`.FTR.ReportManager.NEPOOL` <-
function()
{
  aux <- secdb.getValueType("NEPOOL Report Manager", "location info query")

  MAP <- data.frame(
    location = toupper(sapply(aux, function(x){as.character(x["Location Name"])})),
    ptid = sapply(aux, function(x){as.character(x["ID"])}),
    name = sapply(aux, function(x){as.character(x["Name"])}),
    zone = sapply(aux, function(x){as.character(x["Zone"])}))
  MAP <- subset(MAP, location != "NULL")
  MAP <- MAP[order(as.numeric(MAP$ptid)),]
  
  # add the delivery pt from the elec series.  Some are different from location
  # actual delivery pts can also have MCC prepended and DA/RT appended to the
  # location names.  There are also some exceptions... (how to identify them?!)
  MAP$deliv.pt <- MAP$location
  MAP$deliv.pt[which(MAP$location=="BOSTON")] <- "NEMA"

  # the SECDB parent info is in data("MAP_DP", package="PM") !
  
  MAP$zone <- ifelse(MAP$zone=="0", MAP$ptid, MAP$zone)
  MAP$zone <- ifelse(toupper(MAP$zone)=="HUB", "4000", MAP$zone)  

  MAP$ptid <- as.numeric(MAP$ptid)
  MAP <- MAP[order(MAP$ptid), ]

  MAP <- MAP[!duplicated(MAP), ]  # remove duplicates
  
  MAP$tsdb.symbol <- paste("NEPOOL_SMD_DA_", MAP$ptid, "_CongComp", sep="")

  
  return(MAP)   # return the Report Manager MAP
}

##############################################################################
# What I call location in MAP is also called RelativeFlagCodes in container
# Bal Close PWR NYPP MCC.  The RelativeFlagCodes maps to Locations. 
#
#
`.FTR.ReportManager.NYPP` <-
function()
{
  aux <- secdb.getValueType("NYPP Report Manager", "location info query")

  MAP <- data.frame(
    location = toupper(sapply(aux, function(x){as.character(x["Location Name"])})),
    ptid = sapply(aux, function(x){as.character(x["ID"])}),
    name = sapply(aux, function(x){as.character(x["Name"])}),
    zone = sapply(aux, function(x){as.character(x["Zone"])}),
    type = sapply(aux, function(x){as.character(x["Type"])}))
  MAP <- subset(MAP, location != "NULL")
  MAP$type <- toupper(MAP$type)
  
  # reference node is mapped as load in SecDB
  MAP$type[which(MAP$ptid==24008)] <- "GENERATOR"

  # map the zone of the zones
  ind <- which(MAP$type=="ZONE")
  MAP$zone[ind] <- MAP$name[ind]
  
  MAP$ptid <- as.numeric(MAP$ptid)
  MAP <- MAP[order(MAP$ptid), ]

  MAP <- MAP[!duplicated(MAP), ]  # remove duplicates
  
  MAP$deliv.pt <- MAP$location  # need to investigate if this is OK -- AAD
  
  MAP$tsdb.symbol <- NA_character_
  MAP$tsdb.symbol[which(MAP$type=="ZONE")] <- paste("NYPP_DAM_ZONE_MCC_",
    MAP$ptid[which(MAP$type=="ZONE")], sep="")
  MAP$tsdb.symbol[which(MAP$type=="GENERATOR")] <- paste("NYPP_DAM_GEN_MCC_",
    MAP$ptid[which(MAP$type=="GENERATOR")], sep="")
  # reference node is mapped as load in SecDB
  MAP$tsdb.symbol[which(MAP$ptid==24008)] <- "NYPP_DAM_GEN_MCC_24008"

  return(MAP)   # return the Report Manager MAP
}


##############################################################################
# What I call location in MAP is also called RelativeFlagCodes in container
# Bal Close PWR PJM MCC.  The RelativeFlagCodes maps to Locations. 
#
#
.FTR.ReportManager.PJM <- function()
{
  aux <- secdb.getValueType("PJM Report Manager", "location info query")

  MAP <- data.frame(
    location = toupper(sapply(aux, function(x){as.character(x["Location Name"])})),
    ptid = sapply(aux, function(x){as.numeric(x["ID"])}),
    name = sapply(aux, function(x){as.character(x["Name"])}),
    zone = sapply(aux, function(x){as.character(x["Zone"])}),
    type = sapply(aux, function(x){as.character(x["Type"])}),
    dam.symbol = sapply(aux, function(x){as.character(x["DAM Symbol"])})                
  )
  MAP <- subset(MAP, location != "NULL")
  MAP$type <- toupper(MAP$type)
  

  MAP <- MAP[!duplicated(MAP), ]  # remove duplicates
  
  MAP$deliv.pt <- MAP$location  # need to investigate if this is OK -- AAD
  
  MAP$tsdb.symbol <- NA_character_
  MAP$tsdb.symbol <- gsub("^PJMDAM", "PJMDAC", MAP$dam.symbol)
 

  return(MAP)   # return the Report Manager MAP
}

