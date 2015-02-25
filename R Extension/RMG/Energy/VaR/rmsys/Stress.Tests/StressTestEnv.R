############################################################################
# External Libraries
#
require(reshape); require(RODBC)

source("H:/user/R/RMG/Utilities/RLogger.R")


############################################################################
# File Namespace
#
STenv = new.env(hash=TRUE)

############################################################################
# Constants
# The number of curve simulation rows that can be rbound before being
# aggregated together.
STenv$.MAX_SUBSET_SIZE = 2500
STenv$.SOURCE_DIR <- "H:/user/R/RMG/Energy/Stress.Tests/"


############################################################################
# Add the asOfDate prices to the STenv$QQ table
# 
STenv$addPrices <- function()
{
  load(paste("S:/All/Risk/Data/VaR/prod/Prices/sasPrices.", STenv$asOfDate,
     ".RData", sep=""))

  STenv$QQ <- merge(STenv$QQ, hP, all.x=TRUE)
  names(STenv$QQ)[ncol(STenv$QQ)] <- "price"

  aux <- STenv$QQ[is.na(STenv$QQ$price), ]
  aux <- unique(aux$curve.name)
  if (length(aux)>0)
    cat("These curves have no prices:\n", paste(aux, collapse="\n"),
      "\n", sep="")
  
}


############################################################################
# get all the classifications ...
# Where are the ultimate tables?! 
STenv$calcOneStressTest <- function( this.stress.no )
{
  STenv$QQ$value <- NA
  oneST <- subset(STenv$specification, stress.no == this.stress.no)
  for (g in 1:nrow(oneST)){  # loop over the groups for this ST
    ind <- which(with(STenv$QQ,
                eval(parse(text=paste(oneST$select[g], sep="")))))
    if (length(ind)==0)
      cat(paste("No positions found for: ", oneST$select[g],
                "\n", sep=""))
    else {
      if (oneST$shock.type[g]=="additive"){
         dMTM <- as.numeric(oneST$shock.amount[g]) * STenv$QQ$delta[ind]
      } else if (oneST$shock.type[g]=="multiplicative"){
         dMTM <-  as.numeric(oneST$shock.amount[g]) * STenv$QQ$delta[ind] *
           STenv$QQ$price[ind]
      } else
        stop('shock.type not "additive" or "multiplicative".')
      
      STenv$QQ$value[ind] <- dMTM 
    }
  }
  # for debugging, may want to print what curves have not been touched ...

  # aggregate the MTM changes by ETI 
  dMTM <- data.frame(cast(STenv$QQ, cparty + eti ~ ., sum, na.rm=TRUE))
  names(dMTM)[3] <- "dmtm"
#  save(dMTM, file="H:/user/R/RMG/Projects/CashManagement/dMTM.RData")

  dMTM <- subset(dMTM, dmtm != 0)

  return(dMTM) 
}

############################################################################
# get all the classifications ...
# Where are the ultimate tables?! 
STenv$.getCurveClassifications <- function()
{
  vars <- as.character(STenv$specification$select)
  vars <- sort(unique(sapply(strsplit(vars, " "), function(x)x[1])))

  # these come with the QQ
  vars <- vars[-which(vars %in% c("curve.name", "contract.dt", "cparty"))]

  cat("You need to get: ", paste(vars), "!\n", sep="")
  
  # return a table with curve.name class1, class2, class3, etc. 
}

############################################################################
# get the names of external counterparties from somewhere ...
# NEED A BETTER SOURCE 
STenv$.loadExternalCounterparties <- function()
{
  filename <- paste("S:/All/Risk/Projects/PortfolioMargin/2008-10-15/",
    "Exposure by Agreement Mega Rpt COB 2008-10-14 Dev.csv",
    sep="")
  MR <- read.csv(filename)
  STenv$uCparty <- unique(MR[, c("Counterparty", "ShortName")])
  names(STenv$uCparty) <- c("counterparty", "cparty")
}


############################################################################
#
STenv$.loadPositions.VCV <- function() 
{

  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/VCTRLP.dsn;UID=vcentral_read;PWD=vcentral_read;", sep="")  
  con <- odbcDriverConnect(con.string)

  query <- paste("select externaldealid, counterparty, ", 
    "location_nm, valuation_month, delta ", 
    "from vcv.app_positions_detail t where t.valuation_date = ",
    "to_date('", format(STenv$asOfDate, "%m/%d/%Y"),
    "', 'MM/DD/YYYY')", sep="") 

  QQ <- sqlQuery(con, query)
  odbcCloseAll()

  if (nrow(QQ)==0)
    stop("Problems getting positions from VCentral.")
  
  names(QQ) <- gsub("_", ".", tolower(names(QQ)))
  names(QQ) <- c("eti", "cparty", "curve.name", "contract.dt",
                 "delta")

  QQ[,c(1:3)] <- sapply(QQ[,c(1:3)], as.character)

  QQ <- subset(QQ, cparty %in% STenv$uCparty$cparty)
  QQ <- subset(QQ, !(QQ$curve.name %in% c("USD", "EUR", "GBP", "CAD")))
  # look only at the open positions ... 
  QQ <- subset(QQ, contract.dt > as.POSIXct(asOfDate))
  QQ$contract.dt <- as.Date(QQ$contract.dt)

  filename <- paste("S:/All/Risk/Data/VaR/prod/Positions/vcvdetail.",
    "positions.", asOfDate, ".RData", sep="")
  save(QQ, file=filename)

  STenv$QQ <- QQ; rm(QQ)   # put the positions in the STenv
  invisible(STenv$QQ)
}

############################################################################
# load the xls with stress test definitions
STenv$.loadSTdefinitions <- function()
{
  STenv$specification <- NULL
  # load the stress test definition:
  con <- odbcConnectExcel(paste(STenv$.SOURCE_DIR,
    "specify.stress.tests.xls", sep=""))
  STenv$specification$main <- sqlFetch(con, "main")
  STenv$specification$description <- sqlFetch(con, "description")
  odbcCloseAll()

  names(STenv$specification$main) <- gsub("#", ".",
    names(STenv$specification$main))
  STenv$specification$main <- subset(STenv$specification$main, run=="YES")

  names(STenv$specification$description) <- gsub("#", ".",
    names(STenv$specification$description))
  STenv$specification <- merge(STenv$specification$main,
    STenv$specification$description, all.x=TRUE)

} 







