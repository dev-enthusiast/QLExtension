# Prepare the data for the overnight run. 
#
#
# Written by Adrian Dragulescu on 27-Nov-2006


prepare.overnight.VaR <- function(options){

  #----------------------------------------- get the portfolio hierarchy -----
  cat("Reading in portfolio hierarchy ... ")  
##   query <- paste("SELECT PARENT, CHILD from foitsox.sdb_portfolio_hierarchy ",
##     "WHERE PRICING_DATE=TO_DATE('", options$asOfDate, "','YYYY-MM-DD')", sep="")
##   pHier <- sqlQuery(options$connections$CPSPROD, query)
  query <- paste("select P.short_name portfolio, B.short_name book, B.book_id ",
    "from books B, portfolios P where P.portfolio_id = B.portfolio_id ",
    "order by portfolio, book", sep="")
  pHier <- sqlQuery(options$connections$RMSYS, query)
  colnames(pHier) <- gsub("_", ".", tolower(colnames(pHier)))
  colnames(pHier) <- c("parent", "child", "book.id")
  pHier$parent <- toupper(as.character(pHier$parent))
  pHier$child  <- toupper(as.character(pHier$child))  
  filename <- paste(options$save$datastore.dir, "pHier.", options$asOfDate,
                    ".RData",sep="")
  save(pHier, file=filename)
  cat("Done!\n")

  #----------------------------------------- get curve information ----------
  cat("Reading in curve denomination ... ")  
  query <- paste("select commod_curve, denominated ",
    "from foitsox.sdb_commod_market_location where pricing_date=to_date('",
    options$asOfDate, "','yyyy-mm-dd') and denominated <> 'USD'", sep="")
  den <- sqlQuery(options$connections$CPSPROD, query)
  colnames(den) <- c("curve.name", "denominated")
  den$curve.name <- as.character(den$curve.name)
  den$denominated <- as.character(den$denominated)
  cat("Done!\n")

  cat("Reading in curve region mapping ... ")  
  query <- paste("select t.COMMOD_CURVE, v.REGION ",
    "FROM FOITSOX.SDB_COMMOD_MARKET_LOCATION T, FOITSOX.RMG_CPSCURVES s, ",
    "FOITSOX.RMG_COMMODITIES r, FOITSOX.RMG_SUBREGIONS u, FOITSOX.RMG_REGIONS v ", 
    "where pricing_date=to_date('", options$asOfDate, "', 'yyyy-mm-dd') ", 
    "and t.COMMOD_CURVE=s.CURVENAME and s.COMMID=r.COMM_ID and s.SRID=u.SR_ID ",
    "and u.REGION_ID=v.REGION_ID", sep="")
  curve.info <- sqlQuery(options$connections$CPSPROD, query)
  colnames(curve.info)  <- c("curve.name", "region")
  curve.info$curve.name <- as.character(curve.info$curve.name)
  curve.info$region <- as.character(curve.info$region)
  cat("Done!\n")
  curve.info$hub.name   <- "Nymex"
  curve.info$hub.curve  <- "COMMOD NG EXCHANGE"
  curve.info$vol.type   <- NA 
  filename <- paste(options$save$datastore.dir, "curve.info.",
                    options$asOfDate, ".RData",sep="")
  save(den, curve.info, file=filename)
  cat("Done!\n")

  #------------------------------------------- get overnight positions -------
  source(paste(options$source.dir,"Overnight/get.VCentral.positions.R",sep=""))
  res <- get.VCentral.positions(pHier, options)
##   source(paste(options$source.dir,"Overnight/get.RMSys.positions.R",sep=""))
##   res <- get.RMSys.positions(pHier, options)
  DD  <- res[[1]]; VV  <- res[[2]]


  #--------------------------------------------get asOfDate prices ----------
  get.one.day.prices(options$asOfDate, options)
  source(paste(options$source.dir,"Overnight/get.corrected.marks.R",sep=""))
  cPP <- get.corrected.prices(options)
  correct.hPrices.file(cPP, options)
  
  uCurves <- list(delta = unique(DD[,c("curve.name","contract.dt")]),
                  vega  = unique(VV[,c("curve.name","contract.dt","vol.type")]))
  source(paste(options$source.dir,"Overnight/aggregate.all.market.states.R",sep=""))
  mktData <- aggregate.all.market.states(uCurves, options)

  #---------------------------------------------------------------------------
  cat("Start the buddy curve for prices ... ")
  source(paste(options$source.dir,"Overnight/get.price.buddy.curve.R",sep=""))
  fPP <- get.price.buddy.curve(mktData$prices, den)    # filled prices
  cat("Done!\n")
  cat("Start the buddy curve for volatilities ... ")
  source(paste(options$source.dir,"Overnight/get.vol.buddy.curve.R",sep=""))
  fVV <- get.vol.buddy.curve(mktData$vols, den)        # filled vols
  cat("Done!\n")
  filename <- paste(options$save$datastore.dir, "filled.mktData.", options$asOfDate,
                    ".RData",sep="")
  filled.mktData <- list(prices=fPP, vols=fVV)
  save(filled.mktData, file=filename)
  cat("Exiting prepare.overnight.VaR.R \n") 
}

#  apply(fPP, 2, function(x){any(is.na(x))})
#  apply(fVV, 2, function(x){any(is.na(x))})


