#
#  asOfDate <- as.Date("2007-10-01")
#  file <- "c:/temp/attrition.xls"
#
#  source("H:/user/R/RMG/Energy/Tools/PMStressTesting.R")
#  X <-  PMStressTesting(asOfDate, file)
#  RMG_CPS_CURVE

PMStressTesting <- function(asOfDate, file) {
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
  
  options$calc$fill.marks <- TRUE
  # Get a list of the books.
  # file <- paste(options$source.dir,"Tools/attrition.xls", sep="")
  
  con <- odbcConnectExcel(file)
  books <- sqlFetch(con, "SQL Results")
  odbcCloseAll()  
  books <- as.character(books[,"BOOK_NAME"])
  
  con   <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  query <- "select distinct curvename, sttype from foitsox.rmg_cpscurves"
  sttype <- sqlQuery(con, query)
  names(sttype) <- c("curve.name", "sttype")
  sttype$curve.name <-  toupper(sttype$curve.name)
  sttype$sttype <- toupper(sttype$sttype)
  
  # Load position file
  filename <- paste(options$save$datastore.dir, "Positions/forc.positions.",
    options$asOfDate, ".RData", sep="")  
  load(filename)
  
  # Get positions
  positions = subset(QQ.all, BOOK %in% books)
  positions = positions[, c("CURVE_NAME", "BOOK", "CONTRACT_MONTH", "DELTA")]
  colnames(positions)[4] = "value"
  positions = cast(positions, CURVE_NAME + CONTRACT_MONTH + BOOK ~., sum)
  positions <- cbind(positions, vol.type="NA")
  colnames(positions) = c("curve.name", "contract.dt", "book", "position", "vol.type")
  positions$curve.name = as.character(positions$curve.name)
  positions$book = as.character(positions$book)
  positions$contract.dt = as.Date(positions$contract.dt)
  IVaR <- aggregate.run.prices.mkt(positions[,c("curve.name", "contract.dt","position", "vol.type")], options)
  
  IVaR <- cbind(positions$book, IVaR)
  
  names(IVaR)[1] <- "book"
  results <- IVaR[,c("book", "curve.name", "contract.dt", "position", as.character(asOfDate))]
  
  names(results) <- c("book", "curve.name", "contract.dt", "delta", "price")
  results$book <- as.character(results$book)
  
  results <- merge(results, sttype)
  
  results$value.price.down.30.percent <- results$delta * results$price * 0.7
  results$value.price.down.25.percent <- results$delta * results$price * 0.75
  results$value.price.down.20.percent <- results$delta * results$price * 0.8
  results$value.price.down.15.percent <- results$delta * results$price * 0.85
  results$value.price.down.10.percent <- results$delta * results$price * 0.9
  results$value.price.down.05.percent <- results$delta * results$price * 0.95
  results$value                       <- results$delta * results$price 
  results$value.price.up.05.percent   <- results$delta * results$price * 1.05
  results$value.price.up.10.percent   <- results$delta * results$price * 1.1
  results$value.price.up.15.percent   <- results$delta * results$price * 1.15
  results$value.price.up.20.percent   <- results$delta * results$price * 1.2
  results$value.price.up.25.percent   <- results$delta * results$price * 1.25
  results$value.price.up.30.percent   <- results$delta * results$price * 1.3
  
  return(results)

}