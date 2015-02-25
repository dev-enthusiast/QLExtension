# Get current marks from SecDb using the "Forward Curve" property
#
# curveName <- "Commod PWX 5x16 Physical"
# contractDate <- seq(as.Date("2009-07-01"), by="1 month", length.out=12)
#
#

secdb.getCurrentMarks <- function( curveName, contractDate, expand=FALSE )
{
  if (expand){
    df <- expand.grid( toupper(curveName), contractDate)
    names(df) <- c("curveName", "contractDate")
  } else {
    df <- data.frame(curveName=toupper(curveName), contractDate=contractDate)
  }

  reutersCode <- format.dateMYY(df$contractDate, -1)
  contract <- gsub("COMMOD ", "", df$curveName)

  splitNames <- strsplit(contract, " ")
  prefix <- sapply(splitNames, "[", 1)
  suffix <-  lapply( splitNames, "[", -1 )
  suffixStrings <- sapply( suffix, paste, collapse = " " )
  contract <- paste(prefix, reutersCode, " ", suffixStrings, sep="")
  df$value <- NA

  for (i in 1:nrow(df)){
    aux <- secdb.getValueType( contract[i], "Forward Curve" )
    df$value[i] <- aux$value[length(aux$value)]  # should I use the first element?
  }

  return(df)
}

