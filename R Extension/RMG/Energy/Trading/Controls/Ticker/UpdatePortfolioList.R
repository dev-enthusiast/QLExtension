###############################################################################
# UpdatePortfolioList.R
# 
# Called from the TickerUpdate.DAG file
#
# Author: John Scillieri
# 
library(RODBC)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/Trading/Controls/Ticker/TickerUtils.R")


################################################################################
# File Namespace
#
UpdatePortfolioList = new.env(hash=TRUE)


################################################################################
# Creates a file listing all child portfolios in the system and how they 
# nest with respect to their parents
#
UpdatePortfolioList$main <- function()
{
    pricingDate = TickerUtils$getAsOfDate()
    
    rLog("Querying for portfolio hierarchy...")
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    con = odbcDriverConnect(conString)
    query <- paste("SELECT * from foitsox.sdb_portfolio_hierarchy ",
            "WHERE PRICING_DATE=TO_DATE('", pricingDate, "','YYYY-MM-DD')", sep="")
    MM    <- sqlQuery(con, query)
    odbcClose(con)
    
    getHierarchy <- function( portfolio, MM )
    {
        result = portfolio
        while( TRUE )
        {
            rowID = which(MM$CHILD == portfolio & MM$PARENT_TYPE=="Portfolio")
            if( length(rowID) == 0 )
            {
                break
            }
            
            parent = as.character(MM$PARENT[rowID])
            
            result = paste(parent, "/", result, sep="")
            portfolio = parent
        }
        
        return(result)
    }
    
    rLog("Creating portfolio path lists...")
    uniqueChildren = as.character(unique(MM$CHILD))
    paths = unlist( lapply( uniqueChildren, getHierarchy, MM ) )
    
    heirarchyFile = paste(DATA_DIR, "/portfolioHeirarchy.phl", sep="")
    rLog("Writing protfolio file:", heirarchyFile)
    writeLines(paths, con=heirarchyFile)
    
    rLog("Done.")
}


getRmsysBookMapping <- function()
{
  cat("Querying for portfolio hierarchy...\n")
  dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn"
  conString = paste( dsnPath, ";UID=rmsys_read;PWD=rmsys_read;", sep="")
  con = odbcDriverConnect(conString)
  
  query ="SELECT * from secdb_portfolio_books "
  MM = sqlQuery(con, query)
  odbcClose(con)
  
  getHierarchy <- function( portfolio, MM )
  {
      result = portfolio
      while( TRUE )
      {
          portfolioRow = which(MM$SECDB_NAME == portfolio)
          parentID = MM$PARENT_ID[portfolioRow]
          rowID = which(MM$PB_ID == parentID)
          if( length(rowID) == 0 )
          {
              break
          }
          
          parent = as.character(MM$SECDB_NAME[rowID])
          
          result = paste(parent, "/", result, sep="")
          portfolio = parent
      }
      
      return(result)
  }
  
  cat("Creating portfolio path lists...\n")
  uniqueChildren = as.character(unique(MM$SECDB_NAME))
  paths = unlist( lapply( uniqueChildren, getHierarchy, MM ) )
  
  heirarchyFile = paste("S:/All/Risk/Data/VaR/live/rmsysPortfolioHeirarchy.phl", sep="")
  cat("Writing protfolio file:", heirarchyFile, "\n")
  writeLines(paths, con=heirarchyFile)
  
  cat("Done.\n")
}


################################################################################
UpdatePortfolioList$main()


