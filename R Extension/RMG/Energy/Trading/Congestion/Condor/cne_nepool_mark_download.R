# Pull from SecDb all the marks CNE needs
#
#
# Written by Adrian Dragulescu on 17-Feb-2010


# check file lib.cne.nepool.marks for HARDCODED constants

#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)  
require(CEGbase)
require(CEGterm)
require(reshape)
require(SecDb)
require(xlsx)

BASEDIR <- "H:/user/R/RMG/Energy/Trading/Congestion/"
source(paste(BASEDIR, "lib.cne.nepool.marks.R", sep=""))
source(paste(BASEDIR, "NEPOOL/CNE/insert_cne_nepool_marks_xls.R", sep=""))

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))   
rLog(paste("Start CNE data pulls for", as.character(asOfDate)))

#DIROUT <- "//CEG/CERShare/SupplyGroup/PUBLIC/East Curves/Archive/"  # MY TEST 
#DIROUT <- "S:/All/JC/WindowsServiceTest/"                           # TESLA TEST
DIROUT <- "//CEG/CERShare/All/data/tesla/pricing/"                   # TESLA PROD
startDate <- as.Date(format(asOfDate, "%Y-%m-01"))
endDate   <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+7, "-12-01", sep=""))
#endDate <- as.Date("2020-12-01")

HC  <- read_hedge_costs(startDate, endDate)
IDM <- read_intraday_move(startDate, endDate)


AA  <- pull_ancillaries_nepool(asOfDate, startDate, endDate, HC, IDM)
ARR <- pull_arr_nepool(asOfDate, startDate, endDate)
ARR$value <- ARR$value * 0.90   # use this because it's simpler!

CC <- pull_capacity_nepool(asOfDate, startDate, endDate, HC, IDM)
OO <- pull_prices_ontario(asOfDate, startDate, endDate, HC, IDM)
NN <- pull_prices_nepool(asOfDate, startDate, endDate, HC, IDM)
RR <- pull_recs_nepool(asOfDate, startDate, endDate, HC, IDM)
try(VV <- pull_vols_nepool(asOfDate, startDate, endDate))
GG  <- pull_gas(asOfDate, startDate, endDate)
AGT <- pull_gas_agt(asOfDate, startDate, endDate)
SC  <- pull_prices_nepool_sc(asOfDate, startDate, endDate, HC, IDM)

# Write for TESLA ... 
try(writecsv_energy_nepool(NN))
try(writecsv_basis_nepool(NN))
try(writecsv_hedgecosts_nepool(NN))

try(writecsv_capacity_nepool(CC))
try(writecsv_hedgecosts_capacity(CC))

try(writecsv_ancillaries_nepool(AA, ARR))
try(writecsv_hedgecosts_ancillaries(AA, ARR))

try(writecsv_energy_ontario(OO))
try(writecsv_hedgecosts_ontario(OO))

try(writecsv_recs_nepool(RR))
try(writecsv_hedgecosts_recs(RR))

try(writecsv_vols_nepool(VV))


filename <- "//ceg/cershare/supplygroup/public/East Curves/aux_nepool.xlsx"
try(writexlsx_buckets_gas_close(filename, NN, GG, AGT, SC))


#try(writecsv_hedgecosts(HC))
#try(writecsv_nesc_file(NN, OO, CC, AA, ARR, RR, VV, HC))


rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

# subset(AA, subtype=="WINTRCAP" & location=="ROP")
# subset(AA, subtype=="OPRES_RT" & location=="ROP")


