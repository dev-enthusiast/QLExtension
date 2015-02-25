#

value.SWAP <- function(SwapDealOptions, SwapDetails)
{
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")

  DealType <- as.character(unique(SwapDetails$DEAL_TYPE))
  if (length(DealType) != 1 ) stop("Multiple DealTypes")

  cat(paste("Working on Deal Type :", as.character(unique(SwapDetails$DEAL_TYPE)), "\n", sep=""))
  flush.console()

  Trade<- NULL
  Trade$StartDate <-  as.Date(unique(SwapDetails$TRADE_START_DATE))
  Trade$EndDate <-  as.Date(format(unique(SwapDetails$TRADE_END_DATE),"%Y-%m-%d"))
  Trade$ValuationDate <-  as.Date(unique(SwapDetails$VALUATION_DATE))

  Commods <- NULL
  Commods$pricing.dt.start  <- Trade$ValuationDate
  Commods$pricing.dt.end    <- Trade$ValuationDate     #optional
  Commods$contract.dt.start <- Trade$StartDate         #optional
  Commods$contract.dt.end   <- Trade$EndDate           #optional
  Commods$curve.names <- as.character(unique(SwapDetails$LOCATION_NM))
  Commods$type <- "corrected"   # or "raw"
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")
  IndexPrice <- read.prices.from.CPSPROD(Commods)

  PhysicalLegs = subset(SwapDetails, PRICE_TYPE == "PHYSICAL")
  # Index Legs are of Zero MTM Value
  IndexLegs =  subset(SwapDetails, PRICE_TYPE == "INDEX")
  
  Disc <- NULL
  Disc$asOfDate  <- Trade$ValuationDate
  Disc$currency  <- "USD"
  Disc$months    <- as.Date(PhysicalLegs$PAYMENT_DATE)
  Disc$months    <- as.Date(PhysicalLegs$DISCOUNT_END_DT)
  # Solarc is upgrading on the Payment Due Date in S.8
  # Solar S.5 is using Discounter_END_DT (NO addition of calcendar dasy)
  DiscounterUSD     <- get.discount.factor(Disc)

  for (Leg in (1:dim(PhysicalLegs)[1]))
  {
      PhysicalLeg <- PhysicalLegs[Leg,]
      Quantity <-  PhysicalLeg$QUANTITY
      Value_Month <- as.Date(PhysicalLeg$VALUATION_MONTH)

      FixedPrice <- PhysicalLeg$PRICE
      FixedPriceUSD <-  PhysicalLeg$PRICE_USD
      FloatPriceUSD <-  IndexPrice$PRICE[which(IndexPrice$START_DATE == Value_Month)]

      MTM_Native <-  PhysicalLeg$MTM_VALUE_NATIVE
      MTM_USD <-  PhysicalLeg$MTM_VALUE_USD

      UnDisc_MTM_Native <-  PhysicalLeg$UNDISC_MTM_VALUE_NATIVE
      UnDisc_MTM_USD <- PhysicalLeg$UNDISC_MTM_VALUE_USD

      PnL_USD <- PhysicalLeg$PNL_VALUE_USD
      Delta <- PhysicalLeg$DELTA
      ImpliedDiscounterUSD <- Delta/Quantity

      PaymentDate <- as.Date(PhysicalLeg$PAYMENT_DATE)
      PaymentDate <- as.Date(PhysicalLeg$DISCOUNT_END_DT)
     # Solarc is upgrading on the Payment Due Date in S.8
     # Solar S.5 is using Discounter_END_DT (NO addition of calcendar dasy)
      
      DiscountUSD <- DiscounterUSD$DF[which(DiscounterUSD$Date == PaymentDate)]

      PhysicalLegs$RMG_UnDisc_MTM_USD[Leg] <-  Quantity * FloatPriceUSD
      PhysicalLegs$RMG_MTM_USD[Leg] <-  Quantity * FloatPriceUSD * DiscountUSD
      PhysicalLegs$RMG_PnL_USD[Leg] <-  Quantity * (FloatPriceUSD - FixedPriceUSD) * DiscountUSD
      PhysicalLegs$RMG_DiscounterUSD[Leg] <-  DiscountUSD
      PhysicalLegs$Implied_DiscounterUSD[Leg] <- ImpliedDiscounterUSD

      PhysicalLegs$Variance_UnDisc_MTM_USD[Leg] <- PhysicalLegs$RMG_UnDisc_MTM_USD[Leg] - UnDisc_MTM_USD
      PhysicalLegs$Variance_MTM_USD[Leg] <-  PhysicalLegs$RMG_MTM_USD[Leg] - MTM_USD
      PhysicalLegs$Variance_PnL_USD[Leg] <-  PhysicalLegs$RMG_PnL_USD[Leg] - PnL_USD
      PhysicalLegs$Variance_DiscounterUSD[Leg] <- PhysicalLegs$RMG_DiscounterUSD[Leg] - ImpliedDiscounterUSD
      
      PhysicalLegs$Variance_UnDisc_MTM_USD_Per[Leg] <- PhysicalLegs$Variance_UnDisc_MTM_USD[Leg]/UnDisc_MTM_USD
      PhysicalLegs$Variance_MTM_USD_Per[Leg] <-  PhysicalLegs$Variance_MTM_USD[Leg]/MTM_USD
      PhysicalLegs$Variance_PnL_USD_Per[Leg] <-  PhysicalLegs$Variance_PnL_USD[Leg]/PnL_USD
      PhysicalLegs$Variance_DiscounterUSD_Per[Leg] <- PhysicalLegs$Variance_DiscounterUSD[Leg]/ImpliedDiscounterUSD
      
  }

  head(PhysicalLegs)
  cat("   Done.\n");
  flush.console()

  return(PhysicalLegs)
}