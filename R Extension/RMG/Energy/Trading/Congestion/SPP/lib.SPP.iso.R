# Utilities to deal with SPP 
#
#
#
#

##############################################################################
# Make the SPP MAP, read from SecDB the report manager, clean it a bit, 
#
#
.make_MAP_SPP <- function( do.save=TRUE )
{
  aux <- secdb.getValueType("SPP Report Manager", "location info query")
  as.data.frame(aux[[1]])

  bux <- lapply(aux, as.data.frame)
  MAP <- do.call(rbind, bux)
  MAP <- cbind(ptid=MAP$ID, name=MAP$ID, MAP)
  
  if (do.save) {
    filename <- paste(SPP$DIR, "RData/map.RData", sep="")
    save(MAP, file=filename)
    rLog("Saved ", filename)
  }
  
  MAP
}


#############################################################################
#############################################################################
#
.main <- function()
{
  library(CEGbase)
  library(SecDb)
  library(CRR)

  load_ISO_environment(region = "SPP")
  head(SPP$MAP)  
  
  
  .make_MAP_SPP( do.save = TRUE)
  
  
}
