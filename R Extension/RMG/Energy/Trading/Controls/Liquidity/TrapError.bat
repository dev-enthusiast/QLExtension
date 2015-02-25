I:\R\R-2.6.1\bin\Rscript.exe --vanilla -e "Sys.setenv(commodity='%1')" -e "Sys.setenv(broker='%2')" -e "source('S:/All/Risk/Software/R/prod/energy/trading/controls/Liquidity/ExtractBrokerData.R')"
ECHO.%ERRORLEVEL%
IF ERRORLEVEL   3 EXIT /B 0