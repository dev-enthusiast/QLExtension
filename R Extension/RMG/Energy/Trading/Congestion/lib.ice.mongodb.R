# Put the ICE data into Mongo databases
# Operate on them
#
# .archive_mongo
# .filter_archive          <-- general query 
# .format_ice.gas_trades   <-- helper for speed 
# .get_days_inserted
#
# .get_marks_henry
# .get_marks_nbp
# .get_settlement_prices
#
# .query_ercot_trades
# .query_gas_iferc_trades
# .query_nbp_all
# .query_nepool_nextday
# .query_nepool_zonals
# .query_nextday_gas_trades  <-- nest day gas
# .query_trades              <-- general query for trades
#
# .table_gas_settlements
# .table_power_trades
# .update_ice_archive
#


########################################################################
# @param mongo a connection to a mongo db
# @param ns a string with the namespace of the collection, e.g. "ice.power_trades" 
# @param X a data.frame with one day of data. Keep it only 1 day
#   data!  Easier to understand where it fails ...
# @param check, if I should check not to have double inserts!
# @return nothing
#
.archive_mongo <- function(mongo, ns, X, check=TRUE)
{
  if (length(unique(X$FILENAME)) !=1 )
    stop("Please, only one day at a time!")
  thisFile <- unique(X$FILENAME)

  if (check) {
    # this is slow ... but is better to be sure!
    fnames <- mongo.distinct(mongo, ns, "FILENAME")  
    if (thisFile %in% fnames) {
      rLog("  File ", thisFile, " already archived.  Next!")
      return( TRUE )
    }
  }
  
  rLog("  Inserting ", thisFile)
  Y <- split(X, 1:nrow(X))
  Z <- lapply(Y, mongo.bson.from.list)             # make it a bson 
  mongo.insert.batch(mongo, ns, Z)                 # TRUE if successful insertion
}


########################################################################
# Do a select from the archive
# @param mongo a connection to a mongo db
# @param ns a string with the namespace of the collection, e.g. "ice.power_trades" 
# @param query a mongo.bson object
# @param asDataFrame, if you want results as a data.frame.  It can be really slow,
#   or not work, if your data is not rectangular.
# @return a list or a data.frame
#
.filter_archive <- function(mongo, ns, query,
  sort=mongo.bson.empty(), fields=mongo.bson.empty(),
  asDataFrame=FALSE)
{
  N <- mongo.count(mongo, ns, query)      # how many records
  res <- vector("list", length=N)         # preallocate

  cursor <- mongo.find(mongo, ns, query, sort=sort,
    fields=fields)  # query
  
  i <- 0
  while (mongo.cursor.next(cursor)) {
    #print(mongo.cursor.value(cursor))
    i <- i + 1
    b <- mongo.cursor.value(cursor)
    res[[i]] <- mongo.bson.to.list(b)
  }

  # use a custom formatter for speed, see .format_ice.gas_trades
  if (asDataFrame)
    ldply(res, function(x){as.data.frame(x[-1])})  # slow    
  else 
    res
}


########################################################################
# go from a list of lists to a data.frame
#
.format_ice.gas_trades <- function(trades)
{
  t2 <- matrix(unlist(trades), nrow=length(trades), byrow=TRUE,
               dimnames=list(NULL, names(trades[[1]])))
  t3 <- as.data.frame(t2)

  t3[,c("PRICE", "QUANTITY", "TOTAL_VOLUME")] <-
    sapply(t3[,c("PRICE", "QUANTITY", "TOTAL_VOLUME")], as.numeric)

  t3$TIME <- as.POSIXct(as.numeric(t3$TIME), origin="1969-12-31 19:00:00")
  t3$BEGIN_DATE <- as.POSIXct(as.numeric(t3$BEGIN_DATE), origin="1969-12-31 19:00:00")
  t3$END_DATE <- as.POSIXct(as.numeric(t3$END_DATE), origin="1969-12-31 19:00:00")
  
  
  t3   
}

########################################################################
# go from a list of lists to a data.frame
#
.format_trades <- function(trades)
{
  t2 <- matrix(unlist(trades), nrow=length(trades), byrow=TRUE,
               dimnames=list(NULL, names(trades[[1]])))
  t3 <- as.data.frame(t2)

  t3[,c("PRICE", "QUANTITY", "TOTAL_VOLUME")] <-
    sapply(t3[,c("PRICE", "QUANTITY", "TOTAL_VOLUME")], as.numeric)

  t3$TIME <- as.POSIXct(as.numeric(t3$TIME), origin="1969-12-31 19:00:00")
  t3$BEGIN_DATE <- as.POSIXct(as.numeric(t3$BEGIN_DATE), origin="1969-12-31 19:00:00")
  t3$END_DATE <- as.POSIXct(as.numeric(t3$END_DATE), origin="1969-12-31 19:00:00")
  
  
  t3   
}




########################################################################
# Get the ICE settlement prices
.get_marks_henry <- function(asOfDate=Sys.Date())
{
  product <- if (asOfDate < as.Date("2012-10-15"))
    "NG Fin, FP for LD1" else "NG LD1 Futures"
  .get_settlement_prices(mongo, 
    hub="Henry", product=product,
    asOfDate=asOfDate,
    ns="ice.icecleared_gas")
}


########################################################################
# Get the ICE settlement prices
# @param mongo a mongo connection, somewhere in the search path ...
# @param hub, the hub name
# @param ns, the namespace for the collection
# @result a data.frame 
#
.get_marks_nbp <- function(asOfDate=Sys.Date())
{
  .get_settlement_prices(mongo, 
    hub="NBP", product="UK Natural Gas Futures",
    asOfDate=asOfDate,
    ns="ice.icecleared_gas")
}


########################################################################
# Get the ICE settlement prices
# @param mongo a mongo connection
# @param hub, the hub name
# @param ns, the namespace for the collection
# @result a data.frame 
#
.get_settlement_prices <- function(mongo, 
   hub="NBP", product=NULL,
   asOfDate=as.Date("2013-07-01"),
   ns="ice.icecleared_gas")
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.append(buf, "HUB", hub)
  if (!is.null(product))
    mongo.bson.buffer.append(buf, "PRODUCT", product)
  mongo.bson.buffer.append(buf, "TRADE_DATE",
                           as.POSIXct(paste(asOfDate, "00:00:00")))
  query <- mongo.bson.from.buffer(buf)

  res <- .filter_archive(mongo, ns, query,
    sort=list(STRIP=1L), fields=list(STRIP=1L, SETTLEMENT_PRICE=1L),
    asDataFrame=TRUE)


  res
}



########################################################################
# @param mongo a mongo connection
# @param ns, the namespace for the collection
# @result an R Date
#
.get_days_inserted <- function(mongo, ns="ice.power_trades")
{
  fnames <- sort(mongo.distinct(mongo, ns, "FILENAME"))
    
  aux <- gsub(".*_([[:digit:]]{4}_[[:digit:]]{2}_[[:digit:]]{2})\\..*",
              "\\1", fnames)
  as.Date(aux, format="%Y_%m_%d")
}




########################################################################
# ERCOT
#
.query_ercot_trades <- function(hubs=c("ERCOT North 345KV Hub RT",
  "ERCOT North 345KV Hub", "ERCOT North 345KV Hub DA"))
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.start.object(buf, "HUBS")
    mongo.bson.buffer.append(buf, "$in", hubs)
  mongo.bson.buffer.finish.object(buf)
  
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Construct a query to pull all trades that make the IFERC index
# for a given hub
#
.query_gas_iferc_trades <- function(hub="AGT-CG")
{
  buf <- mongo.bson.buffer.create()
    mongo.bson.buffer.append(buf, "PRODUCT", "NG Firm Phys, BS, LD1")
    mongo.bson.buffer.append(buf, "HUBS", hub)
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Get next day and same day gas trades
#
.query_nextday_gas_trades <- function(hub="AGT-CG", strip=c("Next Day Gas",
      "Same Day", "Custom Daily", "NxIntra"))
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.append(buf, "HUBS", hub)
  mongo.bson.buffer.start.object(buf, "STRIP")
    mongo.bson.buffer.append(buf, "$in", strip)
  mongo.bson.buffer.finish.object(buf)
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Construct a query to pull all NBP trades
# products are c("UK Natural Gas Daily Futures", "UK Natural Gas Futures",
#   "UK Natural Gas TAS", "UK Natural Gas Spr")
#
.query_nbp_all <- function(products=NULL)
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.append(buf, "HUBS", "NBP")
  if (!is.null(products)) {
    mongo.bson.buffer.start.object(buf, "PRODUCT")
      mongo.bson.buffer.append(buf, "$in", products)
    mongo.bson.buffer.finish.object(buf)
  }  
    
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Construct a query to pull Nepool NextDay trades
#
.query_nepool_nextday <- function()
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.start.object(buf, "HUBS")
    mongo.bson.buffer.append(buf, "$in", c("Nepool MH DA (Daily)",
                                           "Nepool MH Day-Ahead"))
  mongo.bson.buffer.finish.object(buf)
  mongo.bson.buffer.append(buf, "STRIP", "Next Day")
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Construct a query to pull Nepool zonal trades
#
.query_nepool_zonals <- function()
{
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.start.object(buf, "HUBS")
    mongo.bson.buffer.append(buf, "$in", c("ISO-NE Con DA",
                                           "ISO-NE Con DA Off-Peak",
                                           "ISO-NE Maine DA",
                                           "ISO-NE Maine DA Off-Peak",
                                           "ISO-NE NE-Mass DA",
                                           "ISO-NE NE-Mass DA Off-Peak",
                                           "ISO-NE New Hampshire DA",
                                           "ISO-NE New Hampshire DA Off-Peak",
                                           "ISO-NE SE-Mass DA",
                                           "ISO-NE SE-Mass DA Off-Peak",
                                           "ISO-NE W Central Mass DA",
                                           "ISO-NE W Central Mass DA Off-Peak")) 
  mongo.bson.buffer.finish.object(buf)
  #mongo.bson.buffer.append(buf, "STRIP", "Next Day")
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Construct a query to pull ICE trades by hubs, and strips
#
.query_trades <- function(hubs=NULL, strips=NULL)
{
  buf <- mongo.bson.buffer.create()
  if (!is.null(hubs)) {
    if (length(hubs)==1) {
      mongo.bson.buffer.append(buf, "HUBS", hubs)
    } else {
      mongo.bson.buffer.start.object(buf, "HUBS")
      mongo.bson.buffer.append(buf, "$in", hubs)
      mongo.bson.buffer.finish.object(buf)
    }
  }
  
  if (!is.null(strips)) {
    if (length(strips)==1) {
      mongo.bson.buffer.append(buf, "STRIP", strips)
    } else {
      mongo.bson.buffer.start.object(buf, "STRIP")
      mongo.bson.buffer.append(buf, "$in", strips)
      mongo.bson.buffer.finish.object(buf)
    }
  }

  
  query <- mongo.bson.from.buffer(buf)
  
  query
}


########################################################################
# Make the table with gas daily marks
# 
.table_gas_settlements <- function()
{
  db  <- "ice"
  tag <- "icecleared_gas"   
  ns  <- paste(db, tag, sep=".")  # the namespace in the database 

  mongo <- mongo.create(host="10.100.101.82")
  mongo.is.connected(mongo)

  ## create some index
  ## mongo.index.create(mongo, ns, list(HUB=1L, TRADE_DATE=1L, PRODUCT=1L))
  #mongo.drop(mongo, ns)    # delete the collection!
  sort(mongo.distinct(mongo, ns, "TRADE_DATE"))

  
  days <- seq(as.Date("2013-08-01"), as.Date("2013-08-31"), by="1 day")
  res <- lapply(days, function(day) {
    X <- .read_file(day, toupper(tag))
    if (length(X) > 0) {
      .archive_mongo(mongo, ns, X, check=FALSE)
    }
  })

  
}



########################################################################
# Make the table with power trades from scratch
# 
.table_power_trades <- function()
{
  db  <- "ice"
  tag <- "power_trades"
  ns  <- "ice.power_trades"

  
  mongo <- mongo.create(host="10.100.101.82")
  mongo.is.connected(mongo)
  #mongo.drop(mongo, ns)    # delete the collection!

  ## create the index
  ## mongo.index.create(mongo, ns, list(HUBS=1L, PRODUCT=1L, STRIP=1L))
  sort(mongo.distinct(mongo, ns, "TRADE_DATE"))

  
  days <- seq(as.Date("2013-01-01"), as.Date("2013-08-19"), by="1 day")
  res <- lapply(days, function(day) {
    X <- .read_file(day, toupper(tag))
    if (length(X) > 0) {
      .archive_mongo(mongo, ns, X, check=FALSE)
    }
  })

  # mongo.bson.to.list(mongo.find.one(mongo, ns))
}



########################################################################
# Stuff to do daily ... 
# @param daysToArchive
#
.update_ice_archive <- function(daysToArchive=NULL,
                                host="10.100.101.82")
{
  require(CEGbase)
  require(reshape)
  require(rmongodb)
  require(RODBC)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.mongodb.R")
 
  #daysToArchive <- seq(as.Date("2013-06-15"), Sys.Date(), by="1 day")

  mongo <- mongo.create(host=host)
  #  mongo <- mongo.create(host="10.100.101.82") # my top PC

  tables <- c(#"ice.power_trades",
              "ice.gas_trades"
              #"ice.icecleared_gas"
              )    
  
  for (ns in tables) {
    rLog("Working on ", ns)
    if ( is.null(daysToArchive) ) {
      daysInArchive <- .get_days_inserted(mongo, ns)
      daysToArchive <- seq(max(daysInArchive), Sys.Date()-1, by="1 day")
    }
    
    tag <- gsub("ice\\.(.*)", "\\1", ns)
    days <- as.Date(setdiff(as.numeric(daysToArchive), as.numeric(daysInArchive)),
                    origin=as.Date("1970-01-01"))
    
    # insert day by day
    res <- lapply(days, function(day) {
      X <- .read_file(day, toupper(tag))
      if (length(X) > 0) {
        .archive_mongo(mongo, ns, X, check=FALSE)
      }
    })
  }
  
  
  
}



########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(rmongodb)
  require(RODBC)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.mongodb.R")

  
  db  <- "ice"
  tag <- "power_trades"   # tag <- "gas_trades"
  ns  <- paste(db, tag, sep=".")  # the namespace in the database 
  
  days <- seq(as.Date("2013-07-01"), as.Date("2013-07-31"), by="1 day")
  res <- lapply(days, function(day) {
    X <- .read_file(day, toupper(tag))
    if (length(X) > 0) {
      .archive_mongo(mongo, ns, X)
    }
  })

  
  res <- res[-which(sapply(res, function(x)length(x)==0))]  # remove empty days

  #mongo <- mongo.create(host="10.100.101.82")
  lapply(res, function(X) {
    .archive_mongo(mongo, ns, X)})

  mongo.count(mongo, ns)
  

  

  mongo <- mongo.create(host="10.100.101.82") # Adrian's top PC
  mongo.is.connected(mongo)
  print(mongo.get.databases(mongo))
  print(mongo.get.database.collections(mongo, db))
  mongo.get.last.err(mongo, "ice")
  #mongo.drop(mongo, ns)    # delete the collection!
  mongo.count(mongo, ns)
  print(mongo.get.databases(mongo))

  mongo.disconnect(mongo)   # close gracefully


  #------------------------------------------------------
  # create indices for HUBS, PRODUCT, STRIP  
  b <- mongo.index.create(mongo, ns,
    list(FILENAME=1L, HUBS=1L, PRODUCT=1L, STRIP=1L))
  


  sort(mongo.distinct(mongo, ns, "HUBS"))
  sort(mongo.distinct(mongo, ns, "PRODUCT"))
  sort(mongo.distinct(mongo, ns, "FILENAME"))

  query <- .query_gas_iferc_trades(hub="AGT-CG")
  mongo.count(mongo, ns, query) 
  res <- .filter_archive(mongo, ns, query, asDataFrame=TRUE)


 

  query <- .query_nepool_zonals()
  mongo.count(mongo, ns, query) 
  res <- .filter_archive(mongo, ns, query, asDataFrame=TRUE)

  buf <- mongo.bson.buffer.create()
  #mongo.bson.buffer.append(buf, "REFERENCE_ID", 90265104)
  mongo.bson.buffer.append(buf, "TIME", as.POSIXct("2013-04-04 15:23:30"))
  query <- mongo.bson.from.buffer(buf)
  mongo.count(mongo, ns, query) 
  res <- .filter_archive(mongo, ns, query, asDataFrame=TRUE)
  
  
  #------------------------------------------------------
  # Example: query all Nepool Next Day trades
  query <- mongo.bson.from.list(list(HUBS="Nepool MH DA (Daily)",
                                     STRIP="Next Day"))
  res <- .filter_archive(mongo, ns, query, asDataFrame=FALSE)


  
  #------------------------------------------------------
  # Example: query some Nepool strips
  # db.power_trades.find( {STRIP: {$in: ["Aug13", "Sep13"]}} )
  # db.power_trades.count( {STRIP: {$in: ["Aug13", "Sep13"]}} )
  ## db.power_trades.find(
  ##   { STRIP: {$in: ["Aug13", "Sep13"]},
  ##     HUBS: "Nepool MH DA"
  ##   } )
  
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.start.object(buf, "STRIP")
    mongo.bson.buffer.append(buf, "$in", c("Aug13", "Sep13"))
  mongo.bson.buffer.finish.object(buf)
  mongo.bson.buffer.append(buf, "HUBS", "Nepool MH DA")
  query <- mongo.bson.from.buffer(buf)
  mongo.count(mongo, ns, query) 
  res <- .filter_archive(mongo, ns, query, asDataFrame=FALSE)




  # to get distinct by HUBS and STRIP, first get the HUBS  
  hubs <- sort(mongo.distinct(mongo, ns, "HUBS"))
  hubsNepool <- hubs[grepl("^nepool", tolower(hubs))]
  buf <- mongo.bson.buffer.create()
  mongo.bson.buffer.start.object(buf, "HUBS")
    mongo.bson.buffer.append(buf, "$in", hubsNepool)
    mongo.bson.buffer.finish.object(buf)
  mongo.bson.buffer.start.object(buf, "FILENAME")
    mongo.bson.buffer.append(buf, "$gt", "powerdeals_2013_06_01.dat.gz")
    mongo.bson.buffer.finish.object(buf) 
  query <- mongo.bson.from.buffer(buf)
  res <- .filter_archive(mongo, ns, query, asDataFrame=FALSE)
  length(res)
    

  
 
  cursor <- mongo.find(mongo, ns, mongo.bson.empty(), limit=10L) 
  cursor <- mongo.find(mongo, ns, mongo.bson.empty(),
                       sort=mongo.bson.from.list(list(HUBS=1L)),
                       fields=mongo.bson.from.list(list(HUBS=1L)),
                       limit=10L)
  while (mongo.cursor.next(cursor)) 
    print(mongo.cursor.value(cursor))

  

  
  query <- mongo.bson.from.list(list(HUBS="Nepool MH DA (Daily)",
                                     PRICE=list('$gt'=65.0)))  # price above $65
  mongo.count(mongo, ns, query)
  
  res <- .filter_archive(mongo, ns, query, asDataFrame=FALSE)
  res[1]
  

  #------------------------------------------------------
  # Example: get GAS settlement prices
  #
  sort(mongo.distinct(mongo, ns, "HUB"))

  
  asOfDate <- as.Date("2013-08-30")
  X <- rbind(
    cbind(hub="nbp",   .get_marks_nbp(asOfDate)),
    cbind(hub="henry", .get_marks_henry(asOfDate)),         
    cbind(hub="algcg_basis",
      .get_settlement_prices(mongo, hub="Algonquin",
         product="NG Basis LD1 for IF Futures", asOfDate=asOfDate)))
  

  X1 <- subset(X, STRIP >= as.POSIXct(nextMonth()))
  X1$price <- X1$adder + (X1$algcg_basis + X1$henry) -0.2
  
  # calculate the distrigas adder
  X1 <- cast(X1, STRIP ~ hub, I, fill=NA, value="SETTLEMENT_PRICE")
  X1$algcg_ferc <- X1$algcg_basis + X1$henry
  X1$adder <- pmax(1.6*1.1*X1$nbp/10 - (X1$algcg_basis + X1$henry),0) + 2
  xyplot(adder ~ STRIP, data= X1, type=c("g", "o"),
    subset=STRIP < as.POSIXct("2018-01-01"),
    xlab="", ylab="Adder, $/mmbtu")
  
  xyplot(price ~ STRIP, data= X1, type=c("g", "o"),
    subset=STRIP < as.POSIXct("2018-01-01"),
    xlab="", ylab="Gas price offer, $/mmbtu")

  

  HH2$rev <- 1400*pmax(HH2$hub.da - 7.1*13.75 - 3.5,0)
  cast(HH2, year ~ ., sum, value="rev")

  ########################################################################
  # Get NBP settlement prices for a list of days
  #
  mongo <- mongo.create(host="10.100.101.82")
  days <- seq(as.Date("2010-01-01"),  Sys.Date()-1, by="1 day")
  res  <- ldply(days, function(day){
    rLog("Working on ", format(day))
    aux <- .get_marks_nbp(as.Date(day))
    if (nrow(aux)==0){
      return(NULL)
    }
    cbind(asOfDate=day, aux)
  })
  write.csv(res, file="c:/temp/nbp_settlement.csv", row.names=FALSE)
  # see NEPOOL/Marks/lib.nbp.R 
  
  
  ########################################################################
  # Compare NBP with Nymex
  asOfDate <- as.Date("2012-08-29")
  X <- rbind(cbind(hub="nbp",   .get_marks_nbp(asOfDate)),
             cbind(hub="henry", .get_marks_henry(asOfDate)))

  xyplot(SETTLEMENT_PRICE ~ STRIP|hub, data=X,  type=c("g", "o"),
         layout=c(1,2),
         scales="free",
         subset=STRIP <= as.POSIXct("2019-12-01 01:00:00"))



  X <- rbind(
    cbind(asOfDate=as.Date("2012-08-29"), .get_marks_nbp(as.Date("2012-08-29"))),
    cbind(asOfDate=as.Date("2013-07-31"), .get_marks_nbp(as.Date("2013-07-31"))))
  xyplot(SETTLEMENT_PRICE ~ STRIP, data=X, groups=asOfDate, 
         type=c("g", "o"),
         scales="free",
         subset=STRIP <= as.POSIXct("2019-12-01 01:00:00"),
         )




  
  .get_settlement_prices(mongo, 
    hub="NBP", product="UK Natural Gas Futures",
    asOfDate=as.Date("2012-08-29"),
    ns="ice.icecleared_gas")

  
 



  
 

  

  
}







