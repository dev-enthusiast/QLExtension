#
#  Main script to drive SOX validation for RightAngle
#

require(RODBC)
require(reshape)

#  Swap Deal
rm(list=ls())

CommonOptions<- NULL
CommonOptions$asOfDate <- as.Date("2007-03-01")

SwapDealOptions <- CommonOptions
SwapDealID <- "ESA10(PS)0001"
SwapDealOptions$DealID <- SwapDealID

source("H:/user/R/RMG/Energy/Systems/RightAngle/Validation/get.RightAngle.data_DealID.R")
SwapDetails <- get.RightAngle.data_DealID(SwapDealOptions)

source("H:/user/R/RMG/Energy/Systems/RightAngle/Valuation/value.SWAP.R")
res <- value.SWAP(SwapDealOptions, SwapDetails)

#  InHouse Swap Deal
rm(list=ls())

CommonOptions<- NULL
CommonOptions$asOfDate <- as.Date("2007-03-01")

InHouseSwapDealOptions <- CommonOptions
InHouseSwapDealID <- "CUK09(IH)0001"
InHouseSwapDealOptions$DealID <- InHouseSwapDealID

source("H:/user/R/RMG/Energy/Systems/RightAngle/Validation/get.RightAngle.data_DealID.R")
InHouseSwapDetails <- get.RightAngle.data_DealID(InHouseSwapDealOptions)

source("H:/user/R/RMG/Energy/Systems/RightAngle/Valuation/value.SWAP.R")
res <- value.SWAP(InHouseSwapDealOptions, InHouseSwapDetails)