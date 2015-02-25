# Strip the ForeRiver from MIS reports 
#
# put a .bat file on S:\All\Structured Risk\NEPOOL\Temporary\ForeRiver_Reports 
#
# Written by AAD on 10/29/2014
#
# "S:\All\Risk\Software\R\R-2.15.1\bin\i386\Rscript.exe" "S:\All\Risk\Software\R\prod\Energy\Trading\Congestion\Condor\foreriver_goodbye.R" 



library(CEGbase)
library(reshape)
library(argparser)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Projects/GoodByeForeRiver/lib.foreriver.goodbye.R")

rLog("\n\n==================================================================================")
rLog("Forever ForeRiver Goodbye ...\n")
p <- arg.parser("fr")
p <- add.argument(p, "--transferDate", help="date of transfer, format yyyy-mm-dd",
                  default="2014-11-07")
p <- add.argument(p, "--outDir", help="location of output directory",
                  default="S:/All/Structured Risk/NEPOOL/Temporary/ForeRiver_Reports/2014-11-01")
p <- add.argument(p, "--fromDir", help="location of input directory",
                  default="S:/Data/NEPOOL_EXGN/RawData/OldZips/Rpt_201411")

argv <- parse.args(p)
print(argv)

reportsSkipped <- c("SD_HRFTR", "FA_FTRPREAWDREQ",
                    "SD_FTRLT", "SD_FTRMTH", "SD_MTFTR",
                    "WW_FTRLT", "WW_FTRMTH",
                    "SD_FCMRESSTLDTL"                          # Capacity for Kate
                    )
##reportsSkipped <- NULL

rLog("Transfer Date is", format(transferDate))
rLog("Writing files to directory", argv$outDir)
rLog("Gathering the files ...")
fnames <- list.files(argv$fromDir, full.name=TRUE, recursive=TRUE, pattern="\\.CSV$")
reportDay <- .extract_report_day(fnames)
transferDate <- as.Date(argv$transferDate)
fnames <- fnames[reportDay >= transferDate]

reportNames <- .get_reportname(fnames)
fnames <- fnames[!(reportNames %in% reportsSkipped)]
rLog("Found ", length(fnames), "files to process.")

rLog("")
rLog("\n\n")

for (filename in fnames) {
  .process_report_FR( filename, argv$outDir )
}


rLog("\n\n===================================================================================")
rLog("Done!")

