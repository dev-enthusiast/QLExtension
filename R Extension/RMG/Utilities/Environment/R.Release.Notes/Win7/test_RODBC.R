# Got a Win7 PC today, and R connection to Oracle does not work anymore
#
#
#

cat("Testing RODBC connection ...\n")

require(RODBC)

if (grepl("XP", Sys.info()["release"])) {
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/DSN/TESLA_ISONE.dsn",
    ";UID=isone_read;PWD=isone_read;", sep="")  
} else {
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/DSN/TESLA_ISONE64.dsn",
    ";UID=isone_read;PWD=isone_read;", sep="")
}

con <- odbcDriverConnect(con.string)


if (con == -1)
  stop("Cannot connect to database!")

query <- paste("SELECT Report_Date, Hour_Ending, ",
  "Constraint_Name, Contingency_Name, Active ",
  "FROM isone.Nepool_Binding_Constraints ",
  "WHERE Report_Type='DA' ",
  "AND report_date >= to_date('2012-08-01', 'yyyy-mm-dd') ",
  "AND report_date <= to_date('2012-09-01', 'yyyy-mm-dd')", sep="")

BC <- sqlQuery(con, query)
odbcCloseAll()



head(BC)

q(save="no", 0)







