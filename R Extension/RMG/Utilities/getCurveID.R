# Get curve id from XCENTRAL given SecDB commodity names
#   source("H:/user/R/RMG/Utilities/futCurve.R")
#   curve.id <- c("Commod NG Exchange", "Commod NG NOVNIT Physical") 
#   res <- getCurveID(curve.id)
#
# Written by Adrian Dragulescu on 20-Oct-2005

getCurveID <- function(curve.id){

  require(RODBC)
  res <- list()
  cat("Reading data from XCENTRAL ... "); flush.console();
  con <- odbcConnect("XCNTRLP", uid="XCENTRAL_R", pwd="XCENTRAL_R")
  
  for (c in 1:length(curve.id)){
    query <- paste("SELECT S_ASSET, S_DELIVERY_POINT, S_QTY_BUCKET_TYPE, S_SEND_FLAG, P_CURVE_TYPE_CD, P_CURVE_NUM, P_CURVE_SHORT_NM, P_IS_BASIS, P_FP_CURVE_NUM, P_CURVE_SOURCE_CD, CREATED_BY_USERID, CREATED_DTM, MODIFIED_BY_USERID, MODIFIED_DTM, P_CURVE_NUM_UNIQUE, P_CURRENCY FROM XCENTRAL.XREF_SEC_PAN_MARKET_DATA WHERE UPPER(S_ASSET)='", toupper(curve.id[c]), "' ORDER BY P_CURVE_NUM, S_DELIVERY_POINT", sep="")
    res[[c]] <- sqlQuery(con, query)
  }
  odbcClose(con)
  cat("Done. \n")
  return(res)
}

