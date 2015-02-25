# Get the TLP unit stack.
#
#
# Written by Adrian Dragulescu on 15-Feb-2008

.demo.get.unit.stack <- function(){
  require(RODBC)
  Sys.setenv(tz="")
  
  source("H:/user/R/RMG/Utilities/Database/TLP/get.unit.stack.R")
  MM <- get.unit.stack("NEPOOL")
  
  write.csv(MM, file="C:/Temp/NEPOOL.stack.csv", row.names=FALSE)
  
}

###########################################################################
#
#
get.unit.stack <- function(iso="NEPOOL"){

  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/TLP.dsn",  
       ";UID=tlpops;PWD=tlpops;", sep="")  
  connection <- odbcDriverConnect(con.string)

  query <- paste("select unit_name, unit_no, iso_ref, ptid, iso, state, ",
    "iso_zone, installation_date, retirement_date, primary_fuel, ",
    "secondary_fuel, winter_rating_to_grid, summer_rating_to_grid, ",
    "full_load_hr, nox_rate_primary, so2_rate_primary, co2_rate_primary, ", 
    "nox_rate_secondary, so2_rate_secondary, co2_rate_secondary ", 
    "from tlp.stack_nypp_0705_t where iso = '", iso, "'   ", sep="")

  MM <- sqlQuery(connection, query)
  names(MM) <- gsub("_", ".", tolower(names(MM)))

  odbcCloseAll()
  return(MM)  
}

###########################################################################
#
#
report.stack <- function(MM)
{
  MM$MW <- (MM$winter.rating.to.grid + MM$summer.rating.to.grid)/2
  res  <- aggregate(MM$MW, list(MM$iso.zone, MM$primary.fuel), sum)
  names(res) <- c("zone", "fuel", "value")

  ind <- which(sapply(res, is.factor))
  res[,ind] <- sapply(res[,ind], as.character)

  
  aux <- cast(res, zone ~ fuel, sum, na.rm=T, margins=TRUE)
  aux[,-1] <- round(aux[,-1])
  (aux)
  
  require(gplots)
  balloonplot(res$zone, res$fuel, round(res$value),
              main="NEPOOL stack composition")

  MM <- MM[order(MM$MW, decreasing=T), ]
  write.csv(MM, file="C:/Temp/junk.csv", row.names=F)

  
}
