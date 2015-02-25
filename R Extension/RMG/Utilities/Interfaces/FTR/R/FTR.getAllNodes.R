
FTR.getAllNodes <- function(region, asOfDate=Sys.Date())
{
  if (region=="NEPOOL"){
    res <- .getAllNodes.NEPOOL(asOfDate)
  }
  return(res)
}

##############################################################################
# Download most recent list of nodes from the ISO
#
.getAllNodes.NEPOOL <- function(day=Sys.Date())
{
  skip.lines <- 4
  site   <- "http://www.nepool.com/histRpts/da-lmp/"
  filename <- paste(site, "lmp_da_", gsub("-","",day), ".csv", sep="")
  con <- url(filename)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error"){
     rLog(paste("Cannot read url", filename))
     next
  }
  rLog(paste("Reading", filename))
  res <- try(read.csv(con, sep=",", skip=skip.lines))
  if (class(res)[1]=="error"){stop("Cannot download!")}
  res <- res[-nrow(res), ]
  close(con)
  res <- res[, 4:5]
  names(res) <- c("ptid", "iso.name")
  res <- unique(res[order(res$ptid), ])
  res$iso.active <- TRUE
  res$ptid <- as.numeric(res$ptid)

  return(res)  
}


