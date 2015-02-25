#  This function returns run specification
#
#  options <- NULL
#  options$asOfDate <- as.Date("2007-04-02")
#  options$run.name <- c("Normal")
#  options$source.dir <- "H:/user/R/RMG/Energy/VaR/"
#
#  source("H:/user/R/RMG/Energy/VaR/UKVaR/get.portfolio.book.r")
#  x <-  get.portfolio.book(options)
#
get.portfolio.book <- function(options) {

    require(RODBC)
    file <- paste(options$source.dir,"UKVaR/schedule.overnight.xls", sep="")
    con <- odbcConnectExcel(file)
    synthetic.portfolio <- sqlFetch(con, "RMG_R_VAR_SYNTHETIC_PORTFOLIO")
    run.all <-  sqlFetch(con, "RMG_R_VAR_RUN")
    odbcCloseAll()
    
    synthetic.portfolio <- subset(synthetic.portfolio, toupper(ACTIVE)=="Y")
    ind <- sapply(synthetic.portfolio , class) %in% c("factor", "logical")
    synthetic.portfolio[,ind] <- sapply(synthetic.portfolio[,ind], as.character)
    synthetic.portfolio[,ind] <- sapply(synthetic.portfolio[,ind], toupper)
    
    ind <- sapply(run.all , class) %in% c("factor", "logical")
    run.all[,ind] <- sapply(run.all[,ind], as.character)
    run.all[,ind] <- sapply(run.all[,ind], toupper)
    run.all <- subset(run.all, toupper(ACTIVE)=="Y" & toupper(RUN_NAME)==toupper(options$run.name))
    run.portfolios <- run.all[,c("PORTFOLIO")]
    
    ccg.portfolio.book <- get.portfolio.book.from.cpsprod(options)
    
    run.spec <- NULL
    for (r in 1:length(run.portfolios)) { 
        childen <- subset(synthetic.portfolio, PORTFOLIO == run.portfolios[r] & TYPE == "PORTFOLIO")
        child.names <- childen[,c("CHILD")]
        synportfolio.books <- NULL
        for (i in 1:length(child.names)) {
            leaf.portfolios <- find.child.portfolios(child.names[i], synthetic.portfolio)
            portfolio.books <- find.books(leaf.portfolios, ccg.portfolio.book)
            tmp <- cbind(replicate(nrow(portfolio.books), run.portfolios[r]),
                    replicate(nrow(portfolio.books), child.names[i]), portfolio.books)
            synportfolio.books <- rbind(synportfolio.books, tmp)
        }
        colnames(synportfolio.books) <- c("PORTFOLIO", "CHILD", "LEAF_PORTFOLIO", "BOOK")
        tmp <- merge(synportfolio.books, childen) 
        ind <- sapply(tmp , class) %in% c("factor", "logical")
        tmp[,ind] <- sapply(tmp[,ind], as.character)
        tmp[,ind] <- sapply(tmp[,ind], toupper)
        
        run.spec <- rbind(run.spec, tmp)
        childen.np <- subset(synthetic.portfolio, PORTFOLIO == run.portfolios[r] & TYPE != "PORTFOLIO")
        if (nrow(childen.np)>0) {
            childen.np <- cbind(childen.np[,c("PORTFOLIO")], replicate(nrow(childen.np), ""),
                                replicate(nrow(childen.np), ""), childen.np[,-1])
            colnames(childen.np)[1:4] <- c("PORTFOLIO", "CHILD", "LEAF_PORTFOLIO", "BOOK")
            ind <- sapply(childen.np , class) %in% c("factor", "logical")
            childen.np[,ind] <- sapply(childen.np[,ind], as.character)
            childen.np[,ind] <- sapply(childen.np[,ind], toupper)
            run.spec <- rbind(run.spec, childen.np)
        }
    }    
    
    run.spec <- merge(run.spec, run.all)
    
    return(run.spec)   
}

#
# This function finds leaf level nonsynthetic portfolios
#
#
find.child.portfolios <- function(parent, synthetic.portfolio) {
    child.portfolios <- NULL
    
    childen.recs <-  subset(synthetic.portfolio, PORTFOLIO == parent)
    childen <-  childen.recs[,c("CHILD")]
    if (length(childen) > 0)  {
        for (r in 1:length(childen)) {
            childen.childen.recs <-  subset(synthetic.portfolio, PORTFOLIO == childen[r])
            childen.childen <-  childen.childen.recs[,c("CHILD")]
            
            if (length(childen.childen) > 0) {          
                  tmp <- find.child.portfolios(childen[r], synthetic.portfolio)
                  child.portfolios <-   c(child.portfolios, tmp)
            }                                                                                  
            else {
                child.portfolios <- c(child.portfolios, childen[r])
            }
        }
    }
    else {
        child.portfolios <- c(child.portfolios, parent) 
    }
    return(child.portfolios)
}

#
#  This function finds books
#  Input parameter portfolios is a vector of nonsynthetic portfolio
#
find.books <- function(portfolios, ccg.portfolio.book) {   
    
    parents <- data.frame(portfolios)
    colnames(parents) <- c("PARENT")
    parents$PARENT <- as.character(parents$PARENT)
    portfolio.book <- merge(ccg.portfolio.book[, c("PARENT", "CHILD")], parents, by.x = "PARENT",  by.y = "PARENT")
    
    return(portfolio.book)
    
}

#
#  options <- NULL
#  options$asOfDate <- as.Date("2007-03-28")
#  options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"  
#  options$file.dsn <- "CPSPROD.dsn"   
#
get.portfolio.book.from.cpsprod <- function(options) {
    if (length(options$file.dsn)==0){options$file.dsn <- "CPSPROD.dsn"}
    if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
    
    portfolio.book <- NULL
    query <- paste("Select Pricing_Date, Parent_ID, RP.Name Parent, ",
              "RP.Type Parent_Type, Child_ID, RC.Name Child, ",                     
              "RC.Type Child_Type, RC.SDB_Name Child_SDB_Name, ",      
              "RC.SDB_Type Child_SDB_Type, RC.RA_ID Child_RA_ID, ",              
              "RC.RA_Type Child_RA_Type, RC.RMSYS_ID Child_RMSYS_ID, ",        
              "RC.RMSYS_Type Child_RMSYS_Type  ",    
              "FROM foitsox.CCG_Portfolio_all H, ",         
              "foitsox.CCG_Portfolio_ID_Ref RP, ", 
              "foitsox.CCG_Portfolio_ID_Ref RC  ", 
              "WHERE H.PARENT_ID = RP.ID AND  ",
              "H.Child_ID = RC.ID AND  ",  
              "H.Pricing_Date = to_date('", options$asOfDate, "', 'yyyy-mm-dd') ",
              "and parent_id <> -1  order by Parent, Child ", sep="")
    
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn,
      ";UID=stratdatread;PWD=stratdatread;", sep="")
    con <- odbcDriverConnect(connection.string)  
    ccg.portfolio.book <- sqlQuery(con, query)
    odbcCloseAll()

    ind <- sapply(ccg.portfolio.book , class) %in% c("factor", "logical")
    ccg.portfolio.book[,ind] <- sapply(ccg.portfolio.book[,ind], as.character)
    ccg.portfolio.book[,ind] <- sapply(ccg.portfolio.book[,ind], toupper)
    
    return(ccg.portfolio.book)
}