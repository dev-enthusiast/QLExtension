set PATH=C:\Program Files\R\R-2.4.0\bin;%PATH%
cd H:/user/R/RMG/Energy/Trading/PriceAnalysis
Rterm -q --no-save --no-restore < overnightPriceAnalysisReport.R > out
