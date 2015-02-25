# Examples of how to use the various attrition scripts
#
#
#


require(CEGbase)
require(reshape)
require(xlsx)

source("H:/user/R/RMG/Energy/PM/NEPOOL/Attrition/lib.attrition.nstar.R")

month <- as.Date("2011-12-01")
NSTAR$download_customer_info(month)


NSTAR$update_customer_info_archive()


NSTAR$download_ICAP_tags()



res <- NSTAR$read_customer_info_xls(as.Date("2011-06-01"))




## months <- seq(as.Date("2010-04-01"), as.Date("2011-12-01"), by="1 month")
## for (month in months)
##   NSTAR$download_customer_info(as.Date(month, origin="1970-01-01"))

