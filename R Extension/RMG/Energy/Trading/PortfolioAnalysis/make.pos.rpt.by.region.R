#
#  source("//nas-omf-01/cpsshare/All/Risk/Software/R/prod/Energy/Trading/PortfolioAnalysis/make.pos.rpt.by.region.R")
#  x <- make.pos.rpt.by.region(dataByRegion, fileNamePos)  

make.pos.rpt.by.region <- function(dataByRegion, fileNamePos)  {
  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/write.xls.R")
  
  region <- unique(dataByRegion$REGION)
  
  for(rg in region) {
     res <- subset(dataByRegion, REGION==rg)
     write.xls(res, fileNamePos, rg, visible=FALSE)
  }
  
  return(0)

}
