FTR.getBindingConstraints <- function(region="NEPOOL", type="DA",
   start.dt=NULL, end.dt=NULL, constraint.names=NULL)
{
  if (!is.null(start.dt))
    start.dt <- as.Date(start.dt)
  if (!is.null(end.dt))
    end.dt <- as.Date(end.dt)
  
  if (region=="NEPOOL"){
    BC <- .getBindingConstraints.NEPOOL(type, start.dt, end.dt, constraint.names)
  }
  if (region=="NYPP"){
    BC <- .getBindingConstraints.NYPP(type, start.dt, end.dt, constraint.names)
  }
  BC <- unique(BC)
  BC <- BC[!is.na(BC$time),]   # there are some NA's in the time column!
  BC <- BC[order(BC$time),]
  
  return(BC)
}


.getBindingConstraints.NEPOOL <- function( type, start.dt, end.dt, constraint.names)
{
  ## con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/DSN/ISOP.dsn",  
  ##      ";UID=geminiops;PWD=gemup05;", sep="")
  
  ## Wu, Fang supports this table
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/TESLA_ISONE.dsn"),  
    ";UID=isone_read;PWD=isone_read;", sep="")  
  con <- odbcDriverConnect(con.string)
  on.exit(odbcCloseAll())

  query <- paste("SELECT Report_Date, Hour_Ending, ",
    "Constraint_Name, Contingency_Name, Active ",
    "FROM isone.Nepool_Binding_Constraints ", sep="")
  
  add <- "where"
  if (!is.null(type)){
    query <- paste(query, add, " Report_Type = '", toupper(type),
                   "'", sep="")
    add <- "and"
  }
  if (!is.null(start.dt)){
    query <- paste(query, add, " report_date >= to_date('", start.dt,
                   "', 'yyyy-mm-dd') ", sep="")
    add   <- "and"
  }
  if (!is.null(end.dt)){
    query <- paste(query, add, " report_date <= to_date('", end.dt,
                   "', 'yyyy-mm-dd') ")
    add   <- "and"
  }
  if (!is.null(constraint.names)){
    aux <- paste(constraint.names, sep="", collapse="', '")
    query <- paste(query, add, " constraint_name in ('", 
      aux, "') ", sep="")
    add   <- "and"
  }

  BC <- sqlQuery(con, query)
  odbcCloseAll()
  names(BC) <- gsub("_", ".", tolower(names(BC)))

  BC$time <- as.POSIXct(paste(as.character(BC$report.date), BC$hour.ending,
                   ":00:00", sep=""))
  BC <- BC[, c("constraint.name", "contingency.name", "time")]
  BC$contingency.name <- as.character(BC$contingency.name)
  BC$constraint.name  <- as.character(BC$constraint.name)

  return(BC)
}


.getBindingConstraints.NYPP <- function(type, start.dt, end.dt, constraint.names )
{
  if (toupper(type) != "DA"){
    stop("Only DA constraints are implemented now.") 
  }

  # this DB ends in 2009-05
##   con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/DSN/ISOP.dsn",
##        ";UID=e13555;PWD=walk2far;", sep="")  
##   con <- odbcDriverConnect(con.string)

  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/DSN/TESLA_NYISO.dsn",
       ";UID=nyiso_read;PWD=nyiso_read;", sep="")  
  con <- odbcDriverConnect(con.string)
  on.exit(odbcCloseAll())
  
  query <- paste("SELECT limiting_facility, facility_ptid, contingency, ", 
       "constraint_cost, day, hour FROM nyiso.day_ahead_constraints ", sep="")

  add <- "where"
  if (!is.null(start.dt)){
    query <- paste(query, add, " day >= to_date('", start.dt,
      "', 'yyyy-mm-dd') ", sep="")
    add   <- "and"
  }
  if (!is.null(end.dt)){
    query <- paste(query, add, " day <= to_date('", end.dt,
      "', 'yyyy-mm-dd') ", sep="")
    add   <- "and"
  }
  if (!is.null(constraint.names)){
    aux <- paste(constraint.names, sep="", collapse="', '")
    query <- paste(query, add, " limiting_facility in ('", 
      aux, "') ", sep="")
    add   <- "and"
  }
  
  BC <- sqlQuery(con, query)
##   # add table day_ahead_constraints_anomaly
##   query2 <- gsub("ahead_constraints", "ahead_constraints_anomaly", query)
##   BC2 <- sqlQuery(con, query2)

##   BC  <- rbind(BC, BC2)  # put them together ...
  names(BC) <- gsub("_", ".", tolower(names(BC)))
  BC$time <- BC$day + BC$hour*3600
  BC <- BC[, c("limiting.facility", "contingency", "time", "constraint.cost",
               "facility.ptid")]

  names(BC)[1:2] <- c("constraint.name", "contingency.name")  
  BC$contingency.name <- as.character(BC$contingency.name)
  BC$constraint.name  <- as.character(BC$constraint.name)
  
  return(BC)
}
