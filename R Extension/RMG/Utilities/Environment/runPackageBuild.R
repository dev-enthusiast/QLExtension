# Procmon runs this file everyday at 5pm.
#
# I actually use it for testing code under PROCMON.
# 
#


options(stringsAsFactors=FALSE)
require(methods)  
require(CEGbase)
require(SecDb)
require(RDCOMClient)
Sys.setenv(tz="")

rLog(paste("Start proces at", Sys.time()))

filename <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/NEPOOLFwdPrice_test.xls"
# somehow the com server leaves an Excel zombie on the
# machine.  I get the PID to kill it before exit. 
TL1  <- system('tasklist /fi "IMAGENAME eq excel.exe"',
 show.output.on.console=FALSE, intern=TRUE)
if (length(TL1)>3){
  pid1 <- as.numeric(substr(TL1[4:length(TL1)], 28, 33))
} else {
  pid1 <- NULL
}
rLog("Show existing Excel processes:", pid1)


# create an xls object
xls <- COMCreate("Excel.Application")
print("Start Excel using COM ...")

TL2 <- system('tasklist /fi "IMAGENAME eq excel.exe"',
 show.output.on.console=FALSE, intern=TRUE)
pid2 <- as.numeric(substr(TL2[4:length(TL2)], 28, 33))
pid  <- setdiff(pid2, pid1)
rLog("Show existing Excel processes:", pid2)
rLog("My Excel pid is", pid)


rLog("Modify cell [1,1]")
filename <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/NEPOOLFwdPrice_test.xls"
readpasswd  <- "ne"
writepasswd <- "ne"

xls[["Visible"]] <- FALSE
readOnly <- FALSE

books <- xls[["workbooks"]]
book <- books$open(filename, NA, readOnly, NA, readpasswd, writepasswd)
sheets <- book[["sheets"]]
mySheet <- sheets$Item(as.integer(1))

r <- mySheet[["Cells"]]
v <- r$Item(as.integer(1), as.integer(8))    # add timestamp
rLog("Just before inserting")
v[["Value"]] <- paste("Last Updated:", Sys.time())

rLog("Closing excel")
book$Save()
book$Close()
xls$Quit()
rLog("Closed")

# somehow the com server leaves the process alive!
if (length(pid)>0){
  aux <- system(paste('taskkill /f /fi "pid eq ', pid,'"', sep=""),
         show.output.on.console=FALSE, intern=TRUE)
}
TL3  <- system('tasklist /fi "IMAGENAME eq excel.exe"',
 show.output.on.console=FALSE, intern=TRUE)
if (length(TL3)>3){
  pid3 <- as.numeric(substr(TL3[4:length(TL3)], 28, 33))
} else {
  pid3 <- NULL
}
rLog("Check that my Excel process is no longer there.")
rLog("Exising processes:", pid3)


require(CEGbase)
rLog(paste("Done at ", Sys.time()))







## require(CEGbase)
## # the packageNames below need to exist as a directory
## # in R prod under /R/RMG/Utilities/Interfaces/
## packagesToRun <- c("FTR")

## rCode  <- "H:/user/R/RMG/Utilities/Environment/build.package.R"
## failed <- NULL

## for (pack in packagesToRun){
##   cat("\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n")
##   res <- try(system(paste("Rscript", rCode, pack)))
##   if (class(res)=="try-error")
##     failed <- c(failed, pack)
## }


