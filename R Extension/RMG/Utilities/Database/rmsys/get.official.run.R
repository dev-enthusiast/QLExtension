# Hook up to rmsys and get the last official run
# options <- NULL
# options$COB <- as.Date("2006-12-01")
# get.last.official.run(options)
#
# Written by Adrian Dragulescu on 17-Mar-2006


get.official.run <- function(options){

  require(RODBC)
  if (!exists("options", mode="list")){options=NULL}
  if (length(options$con)==0 ||  
      !as.logical(grep("RMSYSP", attr(options$con, "connection.string")))){  
    options$con <- odbcConnect("RMSYSP", uid="rmsys_ops", pwd="rmsys_ops")
  }
  query  <- paste("SELECT distinct metacalc_id, cob FROM CALC_KICKOFF WHERE ",
    "OFFICIAL_FLAG=1 and status='COMPLETED' and calc_type=1", sep="")
  if (length(options$COB)>0){
    query <- paste(query, " and cob = to_date('", options$COB,
             "', 'yyyy-mm-dd')", sep="")
  }
  query <- paste(query, "order by metacalc_id desc")
  res   <- sqlQuery(options$con, query)
  return(res[1,])
}
