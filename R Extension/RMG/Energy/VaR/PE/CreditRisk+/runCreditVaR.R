# Implement CreditRisk+ methodology for calculating Credit VaR. 
#
#
# First version by Adrian Dragulescu on 13-Jun-2007

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Energy/VaR/PE/CreditRisk+/creditVaR.R")


################################ MAIN EXECUTION ################################
.start = Sys.time()
#to test, uncomment below
#Sys.setenv(asOfDate="2008-02-27")

asOfDate <- Sys.getenv("asOfDate")
if (asOfDate==""){ asOfDate <- as.character(Sys.Date()-1) }

res = try(CVaR$main(asOfDate))

if (class(res)=="try-error"){
  cat("Failed Credit VaR Calculation.\n")
}

Sys.time() - .start
