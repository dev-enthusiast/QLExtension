# Look at auction statistics  
#
# paste results into:
#  S:\All\Structured Risk\NEPOOL\Ancillary Services\Reserves\LFRM/FwdRes Auction Analysis.pptx
#
# .analyze_auctions
# .analyze_boston
# .plot_offer_curve
# .plot_tmor_offers
#  ..export_offerdata_csv    for dart application





############################################################################
#
.analyze_auctions <- function()
{
  lastAuction <- "S14"
  
  # loads unmaskedParticipants data.frame
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/unmaskedParticipants.RData")
  
  DD <- .read_lfrm_offers()  # read all the offer data

  CP <- .read_lfrm_results() # read the clearing prices, MW

  # add the clearing prices to the offers 
  DD <- merge(DD, CP[,c("auctionName", "zone",
    "TMNSR.Clearing.Price.MW.Month", "TMOR.Clearing.Price.MW.Month",
    "TMNSR.Supply.Cleared.MW", "TMOR.Supply.Cleared.MW")],
    by=c("auctionName", "zone"))

  # add the unmasked curtomer name
  unmaskedParticipants[,1] <- as.numeric( unmaskedParticipants[,1] )
  names(unmaskedParticipants)[1] <- "Masked.Customer.ID"
  DD <- merge(DD, unmaskedParticipants, all.x=TRUE, by="Masked.Customer.ID")
  DD$name <- ifelse(is.na(DD$name), "", DD$name)

  # add the capacity prices
  CapPrice <- list("S10" = 4500, "W10-11" = 4500,
                   "S11" = 3600, "W11-12" = 3600,
                   "S12" = 2951, "W12-13" = 2951,
                   "S13" = 2951, "W13-14" = 2951,
                   "S14" = 3209, "W14-15" = 3209,
                   "S15" = 3434, "W15-16" = 3434,
                   "S16" = 3150, "W16-17" = 3150)  # what happens with Boston, etc.?
  CapPrice <- data.frame(auctionName=names(CapPrice),  capacityPrice=as.numeric(CapPrice))
  DD <- merge(DD, CapPrice, all.x=TRUE)
  DD$Segment.Price.Relative <- DD$Segment.Price - DD$capacityPrice

  
  # save this to json for dart
  ## fname <- "C:/dart/fwdres_viewer/resources/fwdres_offers.json"
  ## require(df2json);
  ## EE <- DD;  EE$startDate <- format(EE$startDate); writeLines(df2json(EE), con=fname)
  
  
  # number of customers
  ddply(DD, .(startDate), function(x){
    data.frame(count=length(unique(x$Masked.Customer.ID)))
  })

  
  # total MW offered by auction -- pretty stable
  aux <- ddply(DD, .(auctionName), function(x){
    data.frame(totalMW=round(sum(x$Segment.Quantity)))
  })
  aux <- aux[order.FRA(aux$auctionName),] 
  print(aux, row.names=FALSE)
  
  # look at max MW by customer, by auction
  # good to see who came in the market, who left
  aux <- ddply(DD, .(auctionName, Masked.Customer.ID, name), function(x){
    data.frame(totalMW=sum(x$Segment.Quantity))
  })
  print(cast(aux, name + Masked.Customer.ID ~ auctionName, I,
    fill=0, value="totalMW"), row.names=FALSE)

  # order participants by total offered mw, one auction only
  aux <- cast(DD, auctionName + Masked.Customer.ID + name ~ ., sum, fill=0,
              value="Segment.Quantity", subset=auctionName==lastAuction)
  colnames(aux)[4] <- "total.MW.offered"
  print(aux[order(-aux[,4]),], row.names=FALSE)

  
  
  # look at max MW by customer, by auction, by zone
  # good to see who came in the market, who left, and who's got market power
  aux <- ddply(DD, .(auctionName, zone, Masked.Customer.ID), function(x){
    data.frame(totalMW=sum(x$Segment.Quantity))
  })
  print(cast(aux,  Masked.Customer.ID + auctionName ~ zone, I,
   fill=0, value="totalMW"), row.names=FALSE)

  
  .plot_offer_curve(DD, auction="S14")


  X <- subset(DD, auctionName == "S14")

  
    
}



############################################################################
#
.analyze_boston <- function(DD)
{
  X <- subset(DD, zone=="NEMA")

  # if there is a requirement, we'll be setting the price.  

  ex <- subset(DD, Masked.Customer.ID == "140603")


  
}

############################################################################
# Plot the offer curve across different auctions to see how the curve
# shifted.  Can look at one participant only.
#
..plot_offer_curve_auctions <- function(DD,
                                        auctions=c("S12", "S13", "W13-14", "S14"),
                                        participantId=NULL)
{
  if (is.null(participantId)) {
    X <- DD
    title <- "All participants"
  } else {
    X <- subset(DD, Masked.Customer.ID == participantId)
    title <- paste("Participant:", X$name[1])
  }
  
  X <- subset(X, auctionName %in% auctions)
  X <- X[order(X$auctionName, X$Segment.Price.Relative), ]
  XX <- ddply(X, c("auctionName"), function(x) {
    data.frame(x = c(0, cumsum(x$Segment.Quantity)),
               y = c(x$Segment.Price.Relative[1], x$Segment.Price.Relative))
  })

  
  print(xyplot(y ~ x, data = XX, groups=auctionName,
         type=c("g", "S"),
         xlab="Total offered, MW",
         ylab="Offer price (net of capacity), $/MW-month",
         main=title,
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=length(unique(XX$auctionName)))
  ))
  
}

############################################################################
#
.plot_offer_curve <- function(DD, auction="S12")
{
  require(lattice)
  ###############################################
  # look at the shape of the Pool aggregate offers accross auctions 
  
  ###############################################
  # look at the offers who set the clearing price
  #X <- subset(DD, auctionName==auction & zone %in% c("CT", "SWCT"))  # for CT TMOR requirement
  X <- subset(DD, auctionName==auction & Reserve.Product.Type == "TMNSR") # for Pool TMNSR
  #X <- subset(DD, auctionName==auction & Reserve.Product.Type == "TMOR")  # for Pool TMOR
  X <- X[order(X$auctionName, X$Segment.Price), ]
  X$cumMW <- cumsum(X$Segment.Quantity)
  print(X, row.names=FALSE)



  
  
  ###############################################
  # first look at TMNSR and TMOR Pool aggregate offers
  auctions <- c("S12", "S13", "W13-14", "S14")
  ..plot_offer_curve_auctions(DD, auctions=auctions))
  ..plot_offer_curve_auctions(DD, auctions=auctions,
                              participantId=441991)  # GdF Suez
  ..plot_offer_curve_auctions(DD, auctions=auctions,
                              participantId=529988)  # NRG
  ..plot_offer_curve_auctions(DD, auctions=auctions,
                              participantId=698953)  # 



  
  w <- 14
  h <- 9
  windows(w,h)
  
  layout(matrix(1:4, ncol=2))
  par(mar=c(4,4,2,1))
  #layout.show()
  #mtext(auction, side=3, line=1)
  
  ###############################################
  # look at CT first 
  X <- subset(DD, zone=="CT" & auctionName==auction)
  X <- X[order(X$Segment.Price), ]

  RR <- subset(X, Reserve.Product.Type == "TMOR")
  x <- c(0, cumsum(RR$Segment.Quantity))
  y <- c(RR$Segment.Price[1], RR$Segment.Price)
  #windows(w,h)
  plot(x,y, type="S", col="blue",
    xlab="CT TMOR, MW",
    ylab="CT TMOR, $/kW-month")
  points(x, y, col="blue")
  abline(v=RR$TMOR.Supply.Cleared.MW, col="gray")
  abline(h=RR$TMOR.Clearing.Price.MW.Month, col="gray")
  
  RR <- subset(X, Reserve.Product.Type == "TMNSR")
  x <- c(0, cumsum(RR$Segment.Quantity))
  y <- c(RR$Segment.Price[1], RR$Segment.Price)
  #windows(w,h)
  plot(x,y, type="S", col="blue",
    xlab="CT TMNSR, MW",
    ylab="CT TMNSR, $/kW-month")
  points(x, y, col="blue")
  abline(v=RR$TMNSR.Supply.Cleared.MW, col="gray")
  abline(h=RR$TMNSR.Clearing.Price.MW.Month, col="gray")


  ###############################################
  # now the rest of the system ... 
  X <- subset(DD, zone=="ROS" & auctionName==auction)
  X <- X[order(X$Segment.Price), ]

  RR <- subset(X, Reserve.Product.Type == "TMOR")
  x <- c(0, cumsum(RR$Segment.Quantity))
  y <- c(RR$Segment.Price[1], RR$Segment.Price)
  #windows(w,h)
  plot(x,y, type="S", col="blue",
    xlab="ROS TMOR, MW",
    ylab="ROS TMOR, $/kW-month")
  points(x, y, col="blue")
  abline(v=RR$TMOR.Supply.Cleared.MW, col="gray")
  abline(h=RR$TMOR.Clearing.Price.MW.Month, col="gray")

  RR <- subset(X, Reserve.Product.Type == "TMNSR")
  x <- c(0, cumsum(RR$Segment.Quantity))
  y <- c(RR$Segment.Price[1], RR$Segment.Price)
  #windows(w,h)
  plot(x,y, type="S", col="blue",
    xlab="ROS TMNSR, MW",
    ylab="ROS TMNSR, $/kW-month")
  points(x, y, col="blue")
  abline(v=RR$TMNSR.Supply.Cleared.MW, col="gray")
  abline(h=RR$TMNSR.Clearing.Price.MW.Month, col="gray")

  ###############################################
  # now SWCT 
  X <- subset(DD, zone=="SWCT" & auctionName==auction)
  X <- X[order(X$Segment.Price), ]

  aux <- subset(DD, auctionName==auction)
  aux <- aux[,c("zone", "Reserve.Product.Type", "Segment.Price",
    "Segment.Quantity", "TMOR.Clearing.Price.MW.Month",
    "TMNSR.Clearing.Price.MW.Month", "name")]
  names(aux)[c(2:6)] <- c("Product", "Price", "MW",
     "TMOR.Clearing.Price", "TMNSR.Clearing.Price")
  aux <- aux[order(aux$Price),]
  aux$cumMW <- cumsum(aux$MW)
  print(aux, row.names=FALSE)

  title(paste("auction", auction), outer=TRUE, line=-1)
}


############################################################################
# Determine if a requirement is satisfied or not.
# @param requirements the list of requirements
# @param X a data.frame of offers, ordered decreasingly
#
is_satisfied <- function(requirements, X)
{
  
  mw.pool_tmnsr <- sum(subset(X, Reserve.Product.Type == "TMNSR")$Segment.Quantity)
  # you can apply extra tmnsr (ct or ros) to satisfy the pool tmor if you have them 
  extra_tmnsr_for_tmor <- max(sum(subset(X, Reserve.Product.Type == "TMNSR")$Segment.Quantity) -
                              requirement$pool_tmnsr, 0)
  
  mw.pool_tmor  <- sum(subset(X, Reserve.Product.Type == "TMOR")$Segment.Quantity) + extra_tmnsr_for_tmor
  mw.ct_tmor    <- sum(subset(X, zone == "CT")$Segment.Quantity)  # both tmor and tmnsr from ct qualify
  
  structure(c((mw.pool_tmnsr >= requirements$pool_tmnsr),
              (mw.pool_tmor  >= requirements$pool_tmor),
              (mw.ct_tmor    >= requirements$ct_tmor)),
     names=c("pool_tmnsr", "pool_tmor", "ct_tmor"))

}




############################################################################
# Clear the auction given a set of offers
# @param X is a data.frame with offers for a given auction
# @param requirements is a list of MW 
#  requirements_s13 <- list(pool_tmor  = 723,
#                           pool_tmnsr = 1349,
#                           swct_tmor  = 0,
#                           ct_tmor    = 747,
#                           nema_tmor  = 0)
#
#
.clear_auction <- function(X, requirements)
{

   X <- subset(DD, auctionName == "S13")
   X <- X[order(X$Segment.Price),]   # sort them first
   X$id <- 1:nrow(X)                 # give every offer an ID 


   pool_tmor_cleared <- ros_tmor_cleared <- NULL
   swct_tmor_cleared <- ct_tmor_cleared <- NULL
   isSatisfied <- structure(rep(FALSE, 5),
     names=c("pool_tmnsr", "pool_tmor", "ct_tmor", "swct_tmor", "nema_tmor"))
   CP <- structure(rep(NA,5),
     names=c("pool_tmnsr", "pool_tmor", "ct_tmor", "swct_tmor", "nema_tmor"))


   cleared <- NULL
   
   
  for (r in 1:nrow(X)) {
     rLog("Working on offer", r)
     x <- X[r,] 
     #browser()

     satisfied <- is_satisfied(requirements, X[1:r,])
     if (satisfied["ct_tmor"] && x$zone == "CT" && x$Reserve.Product.Type == "TMOR")
       next   # stop!

     if (satisfied["pool_tmor"] && x$Reserve.Product.Type == "TMOR") {
       # if (!satisfied["ct_tmor"])  that means CT tmor will have higher clearing price!
       next   # stop!
     }
       

     if (satisfied["pool_tmnsr"]) {
       rLog("we are done at offer", r)
       break
     }
  }  


   
     if (all(.is_satisfied(requirements, X[1:r,]))) {
       rLog("Auction satisfied.  Exiting.")
       break
     }

     if ( !isSatisfied["pool_tmnsr"] && x$Reserve.Product.Type == "TMNSR")
     {
       cumMW <- sum(pool_tmnsr_cleared$Segment.Quantity)
       if ((cumMW + x$Segment.Quantity) <= requirements$pool_tmnsr) {
         pool_tmnsr_cleared <- rbind(pool_tmnsr_cleared, x)
         x$Segment.Quantity <- 0
       } else {
         rLog("pool_tmnsr requirement satisfied")
         isSatisfied["pool_tmnsr"] <- TRUE
         CP["pool_tmnsr"] <- x$Segment.Price             # clearing price
         # the rest can be applied to pool_tmor
         x$Segment.Quantity <- cumMW + x$Segment.Quantity - requirements$pool_tmnsr
         browser()
       }
     } 
     

     if ( !isSatisfied["ct_tmor"] && x$Reserve.Zone != "Rest Of System"
          && x$Segment.Quantity > 0 )
     {
       cumMW <- sum(ct_tmor_cleared$Segment.Quantity)
       if ((cumMW + x$Segment.Quantity) <= requirements$ct_tmor) {
         if (x$Reserve.Product.Type == "TMNSR") {
           rLog("using some ct_tmnsr for ct_tmor")
           browser()
         } 
         ct_tmor_cleared <- rbind(ct_tmor_cleared, x)
       } else {
         rLog("ct_tmor requirement satisfied")
         isSatisfied["ct_tmor"] <- TRUE
         CP["ct_tmor"] <- x$Segment.Price             # clearing price
         # the rest can be applied to pool_tmor
         x$Segment.Quantity <- cumMW + x$Segment.Quantity - requirements$ct_tmor
         browser()
       }
     }

     
     if ( !isSatisfied["pool_tmor"] && x$Segment.Quantity > 0 )
     {
       cumMW <- sum(pool_tmor_cleared$Segment.Quantity)
       if ((cumMW + x$Segment.Quantity) <= requirements$pool_tmor) {
         if (x$Reserve.Product.Type == "TMNSR") {
           rLog("using some ct_tmnsr for pool_tmor")
           browser()
         }
         pool_tmor_cleared <- rbind( pool_tmor_cleared, x)
       } else {
         rLog("pool_tmor requirement satisfied")
         isSatisfied["pool_tmor"] <- TRUE
         CP["pool_tmor"] <- x$Segment.Price             # clearing price
         x$Segment.Quantity <- requirements$pool_tmor - cumMW
         pool_tmor_cleared  <- rbind( pool_tmor_cleared, x)
         if ( !isSatisfied["ct_tmor"] )
         
         browser()
       }
     }
   


   

   
 
   
   pool_tmor_cleared
   ct_tmor_cleared
   pool_tmnsr_cleared
   
}

   
############################################################################
# save the data to csv for dart
#
..export_offerdata_csv <- function()
{
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/unmaskedParticipants.RData")
  
  DD <- .read_lfrm_offers()  # read all the offer data
  
  # add the unmasked curtomer name
  unmaskedParticipants[,1] <- as.numeric( unmaskedParticipants[,1] )
  names(unmaskedParticipants)[1] <- "Masked.Customer.ID"
  DD <- merge(DD, unmaskedParticipants, all.x=TRUE, by="Masked.Customer.ID")
  DD$name <- ifelse(is.na(DD$name), "", DD$name)

  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FwdRes/FRM_Offers/csv/fwdres_offers.csv"
  write.csv(DD, file=fname, row.names=FALSE)


}






############################################################################
############################################################################
#
# First you will need to update the most recent auction data by using 
# Congestion/lib.NEPOOL.LFRM.R
#
.main <- function()
{
  require(CEGbase)
  require(plyr)
  require(reshape)
  require(DivideAndConquer)
  require(SecDb)
  require(zoo)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.LFRM.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.fwd.reserves.R")

  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/",
    "ISO_Data/ForwardReserveAuction/main_frm_auctions.R", sep=""))

  .analyze_auctions()
  
  requirements_s13 <- list(pool_tmor  = 723,
                           pool_tmnsr = 1349,
                           swct_tmor  = 0,
                           ct_tmor    = 747,
                           nema_tmor  = 0)
  


  
  requirements <- requirements_s13
  X <- subset(DD, auctionName == "S13")
  

  requirements_w1314 <- list(pool_tmor  = 915,
                           pool_tmnsr = 1532,
                           swct_tmor  = 155,
                           ct_tmor    = 578,
                           nema_tmor  = 0)
  requirements <- requirements_w1314
  X <- subset(DD, auctionName == "W13-14")

  
  
  
}





























   ## #X <- subset(DD, auctionName==auction)
   ## X <- X[order(X$Segment.Price),]
   ## X$id <- 1:nrow(X)   # give every offer an ID 

   ## # first the pool tmnsr requirement is filled
   ## pool_tmnsr_offers <- subset(X, Reserve.Product.Type == "TMNSR")
   ## pool_tmnsr_offers$cumMW <- cumsum(pool_tmnsr_offers$Segment.Quantity)
   ## ind <- min(pool_tmnsr_offers$id[pool_tmnsr_offers$cumMW >= requirements$pool_tmnsr])
   ## pool_tmnsr_cleared <- subset(pool_tmnsr_offers, id <= ind)
   ## CP <- list(pool_tmnsr = tail(pool_tmnsr_cleared,1)$Segment.Price)

   ## # remove the offers used for the pool tmnsr
   ## X1 <- X[which(!(X$id %in% pool_tmnsr_cleared$id)),]
   ## aux <- tail(pool_tmnsr_cleared, 1)  # last tmnsr offer is partial, add the rest
   ## aux$Segment.Quantity <- aux$cumMW - requirements$pool_tmnsr
   ## aux$cumMW <- NULL
   ## X1 <- rbind(X1, aux)
   ## X1 <- X1[order(X1$Segment.Price),]

   
   ## # second the swct tmor requirement is filled from the remaining offers
   ## if (requirements$swct_tmor > 0) {
   ##   swct_tmor_offers <- subset(X1, Reserve.Zone == "Southwest Connecticut")
   ##   stop("FIX SWCT req")
   ##   X2 <- X1
   ## } else {
   ##   X2 <- X1
   ## }

   ## # third the ct tmor requirement is filled
   ## ct_tmor_offers <- subset(X, Reserve.Zone %in% c("Southwest Connecticut",
   ##                                                  "Connecticut"))
   ## ct_tmor_offers$cumMW <- cumsum(ct_tmor_offers$Segment.Quantity)
   ## ind <- min(ct_tmor_offers$id[ct_tmor_offers$cumMW >= requirements$ct_tmor])
   ## ct_tmor_cleared <- subset(ct_tmor_offers, id <= ind)
   ## CP <- list(ct_tmor = tail(pool_tmnsr_cleared,1)$Segment.Price)


   
   


   ## total_req <- requirements$pool_tmor + requirements$pool_tmnsr
   ## cumMW <- cumsum(X$Segment.Quantity)
   ## X.cleared <- X[1:min(which(cumMW > total_req)),]  # auction ends here
   ## # marginal offer
   ## x <- tail(X.cleared,1)
   ## x$Segment.Quantity <- total_req - sum(X.cleared$Segment.Quantity[1:(nrow(X.cleared)-1)])
   ## X.cleared <- X.cleared[-nrow(X.cleared),]
   ## X.cleared <- rbind(X.cleared, x)  # add back the marginal offer

   ## stopifnot(sum(X.cleared$Segment.Quantity) == total_req)

   ## # the amounts that cleared ...
   ## mw <- list(
   ##   swct_tmor  = sum(subset(X.cleared, Reserve.Product.Type == "TMOR" & zone == "SWCT")$Segment.Quantity),
   ##   ct_tmor    = sum(subset(X.cleared, Reserve.Product.Type == "TMOR" & zone == "CT")$Segment.Quantity),
   ##   ros_tmor   = sum(subset(X.cleared, Reserve.Product.Type == "TMOR" & zone == "ROS")$Segment.Quantity),
   ##   ros_tmnsr  = sum(subset(X.cleared, Reserve.Product.Type == "TMNSR" & zone == "ROS")$Segment.Quantity),
   ##   ct_tmnsr   = sum(subset(X.cleared, Reserve.Product.Type == "TMNSR" & zone == "CT")$Segment.Quantity),
   ##   swct_tmnsr = sum(subset(X.cleared, Reserve.Product.Type == "TMNSR" & zone == "SWCT")$Segment.Quantity),
   ##   nema_tmnsr = sum(subset(X.cleared, Reserve.Product.Type == "TMNSR" & zone == "NEMA")$Segment.Quantity)   
   ## )
   
   
   ## # you may need to allocate ct tmnsr for pool tmor
   ## tmnsr_for_pool_tmor <- sum(subset(X.cleared,
   ##   Reserve.Product.Type == "TMNSR")$Segment.Quantity) - requirements$pool_tmnsr
   
   ## if (tmnsr_for_pool_tmor == 0) {
   ##   rLog("no price separation between tmor and tmnsr products!")
     
   ## }

   ##   subset(X.cleared, Reserve.Product.Type == "TMNSR" &&
   ##                                  Reserve.Zone != "Rest Of System")$

     
   ## pool_tmnsr_cleared <- subset(X.cleared, Reserve.Product.Type == "TMNSR")
 
