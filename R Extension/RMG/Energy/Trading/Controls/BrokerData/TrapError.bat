Call //ceg/cershare/All/Risk/Software/R/prod/Energy/Trading/Controls/BrokerData/CleanUpDB.bat %1
ECHO %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 EXIT /B 1
\\ceg\cershare\All\Risk\Software\R\R-2.15.1\bin\Rscript.exe --vanilla -e "Sys.setenv(commodity='%1')" -e "Sys.setenv(broker='%2')" -e "source('//ceg/cershare/All/Risk/Software/R/prod/energy/trading/controls/BrokerData/ExtractBrokerData.R')"
ECHO %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 EXIT /B 1
EXIT /B 0