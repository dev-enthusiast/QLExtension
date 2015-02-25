#  This function gets aggregated positions for a run. It applies weight
#    and date range to the positions.
#
#  options <- NULL
#  options$asOfDate <- as.Date("2007-04-04")
#  run  # - data.frame from calling get.portfolio.book.R,
#       # run contains data for only ONE run
#  options$data.dir <- "S:/All/Risk/Data/VaR/prod/Positions/" 
#  options$source.dir <- "H:/user/R/RMG/"
#
#  source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.positions.r")
#  res <- aggregate.run.positions(options)

aggregate.run.positions <- function(run, options) 
{    
    cat("Aggregating run positions ... ")
    
    deltas = .getDeltas( run, options )
    
    vegas = .getVegas( run, options )
    
    if ( nrow(vegas) == 0 && nrow(deltas) == 0) 
    {
        cat("No Vegas or Deltas. Done.\n")
        return( NULL )
    } 
    
    
    positions <- rbind(deltas, vegas)
    
    positions$contract.dt <- as.Date(positions$contract.dt)
    positions$curve.name  <- as.character(positions$curve.name)
    positions$vol.type    <- as.character(positions$vol.type)
    
    # remove WTH positions from the VaR calculation.
    ind <- grep("COMMOD WTH", positions$curve.name)
    if (length(ind)>0)
    {
        positions <- positions[-ind, ]   
    }
    
    cat("Done.\n")
    return(positions)
}


.getDeltas <- function( run, options )
{
    filename <- paste(options$save$datastore.dir, "Positions/vcv.positions.",
            options$asOfDate, ".RData", sep="")                          
    if (file.exists(filename) == FALSE) {
        QQ.all <- get.positions.from.vcv.vcentral(options)
    } else 
    {
        backupLoad(filename)
    }
    
    spec <- run$specification[,c("BOOK", "WEIGHT", "START_DATE", "END_DATE")]
    pos  <- merge(QQ.all, spec)
    pos$DELTA <- pos$DELTA * pos$WEIGHT
    pos$VEGA  <- pos$VEGA * pos$WEIGHT
    pos <- subset(pos, is.na(START_DATE) | (CONTRACT_MONTH >= START_DATE &
                        CONTRACT_MONTH <= END_DATE))
    positions <- NULL
    deltas <- pos[,c("CURVE_NAME", "CONTRACT_MONTH", "DELTA")]
    colnames(deltas) <- c("CURVE_NAME", "CONTRACT_MONTH", "value")
    deltas$CONTRACT_MONTH <- as.Date(format(deltas$CONTRACT_MONTH, "%Y-%m-01"))
    deltas <- cast(deltas, CURVE_NAME + CONTRACT_MONTH ~ ., sum) 
    colnames(deltas) <- c("CURVE_NAME", "CONTRACT_MONTH", "value")
    deltas <- subset(deltas, value!=0)
    if (nrow(deltas) != 0) {
        deltas <- cbind(deltas, vol.type="NA")
        names(deltas) <- c("curve.name", "contract.dt", "position", "vol.type")
    }
    
    return(deltas)
}


.getVegas <- function( run, options )
{
    vegaFile = paste(options$save$datastore.dir, "Positions/vega.positions.",
            options$asOfDate, ".RData", sep="")   
    backupLoad(vegaFile)
    
    spec <- run$specification[,c("BOOK", "WEIGHT", "START_DATE", "END_DATE")]
    vpos  <- merge(vegaPositions, spec, by.x="book", by.y="BOOK")
    vpos$vega  <- vpos$vega * vpos$WEIGHT
    vpos <- subset(vpos, is.na(START_DATE) | (contract.dt >= START_DATE &
                        contract.dt <= END_DATE))
    
    vegas <- vpos[,c("curve.name", "contract.dt", "vol.type","vega")]
    colnames(vegas) <- c("curve.name", "contract.dt", "vol.type", "value")
    vegas$contract.dt <- as.Date(format(vegas$contract.dt, "%Y-%m-01"))
    vegas <- cast(vegas, curve.name + contract.dt + vol.type ~ ., sum)
    
    colnames(vegas) <- c("curve.name", "contract.dt", "vol.type", "position")
    vegas <- subset(vegas, position!=0)
    vegas = vegas[, c("curve.name", "contract.dt", "position", "vol.type")]
    
    return(vegas)
}
