# Quick script to show which counterparties weren't simulated and what their
# current position set looks like

source("H:/user/R/RMG/Energy/VaR/PE/Condor/CheckPEStatus.R")

unfinished = CheckPEStatus$main(as.Date("2007-08-23"))

for( counterparty in unfinished )
{
  print(counterparty);
  print(PEUtils$getPositionsForCounterparty(counterparty));
  cat("\n\n\n")
}

