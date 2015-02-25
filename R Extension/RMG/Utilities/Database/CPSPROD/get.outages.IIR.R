# Get unit outages from the IIR database.
# region = "PJM"
# 
# Written by Adrian Dragulescu on 19-May-2008

get.outages.IIR <- function(region="ISNE", start.dt=NULL,
  end.dt=NULL, fuel=NULL)
{
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
        "DSN/IIR.dsn;UID=ccgtransread;PWD=ccgtransread;", sep = "")
  con <- odbcDriverConnect(con.string)
  query <- paste("select * from ccgtrans.iir_generation_outage ")

  add <- "where"
  if (!is.null(region)){
    query <- paste(query, add, " ctrlareaid='", region, "'", sep="")
    add   <- "and"
  }
  if (!is.null(start.dt)){
    query <- paste(query, add, " TA_END >= to_date('", start.dt,
      "', 'yyyy-mm-dd')", sep="")
    add   <- "and"
  }
  if (!is.null(end.dt)){
    query <- paste(query, add, " TA_START <= to_date('", end.dt,
      "', 'yyyy-mm-dd')", sep="")
    add   <- "and"
  }
  
  data <- sqlQuery(con, query)
  odbcClose(con)

  colnames(data) <- gsub("_", ".", tolower(colnames(data)))
  data <- data[order(data$out.design, decreasing=T), ]

  ind <- c("unit.id", "plant.id", "owner.name", "nerc.reg", "sic.code",
    "nerc.sub", "elect.conn", "elecondesc", "unit.type", "ta.duration")
  data <- data[,-which(colnames(data) %in% ind)]
  
  return(data)
}


