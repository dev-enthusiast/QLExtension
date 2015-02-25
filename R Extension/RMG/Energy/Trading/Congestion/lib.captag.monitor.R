# Monitor the daily captags and report issues
#
# .read_peakcontribution_report
#
#

####################################################################
#
.read_peakcontribution_report <- function(days, org="CCG")
{
  res <- vector("list", length=length(days))
  uMonths <- unique(format(days, "%Y-%m-01"))

  i <- 1
  for (month in uMonths) {
    files <- .get_report_csvfilenames(org=org, month=as.Date(month),
      reportName="SP_PEAKCONTRIBUTIONDLY")
    for (file in files) {
      res[[i]] <- .process_report(file)[[1]]
      i <- i+1
    }
  }

  res <- do.call(rbind, res)
  res$Trading.Date <- as.Date(res$Trading.Date, "%m/%d/%Y")
  res <- res[order(res$Asset.ID, res$Trading.Date), ]

  res
}



####################################################################
#
.main <- function()
{

  require(CEGbase)
  require(reshape)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.captag.monitor.R")

  days <- seq(as.Date("2012-01-15"), as.Date("2012-03-07"), by="1 day")
  PC <- .read_peakcontribution_report(days, org="CCG")
  
  uAssets <- unique(PC[,c("Asset.ID", "Asset.Name")])

  aux <- ddply(PC, .(Asset.ID), function(x){
    x <- x[order(x$Trading.Date),]
    pc <- x$Asset.Peak.Contribution
    data.frame(min=min(pc),
               max=max(pc),
               change=pc[length(pc)] - pc[1])
  })
  
  # only look at load > 1MW
  aux <- subset(aux, abs(max) > 1)
  aux <- merge(aux, uAssets, all.x=TRUE)
  aux <- aux[order(-abs(aux$change)),]
  rownames(aux) <- NULL

  PC$Asset.ID <- factor(PC$Asset.ID)
  xyplot(Asset.Peak.Contribution ~ Trading.Date|Asset.ID, data=PC,
    subset=Asset.ID %in% aux$Asset.ID,
    scales="free"
  )

  
  

}
