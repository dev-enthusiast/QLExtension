# Read the flt report from secdb archives
#
#

secdb.flatReport <- function(asOfDate=Sys.Date()-1, books=c("NPCLPNPS"),
  variables=c("Quantity", "Today Value", "Active Contracts",
    "Delivery Point", "Service Type", "Settlement Type"), filter=NULL, to.df=FALSE)
{
  getThese <- if (is.null(variables)) {
    "GetThese = Null;"    # you get everything -- A LOT!!!
  } else {
    paste('GetThese = Structure("',
      paste(variables, collapse='", 1; "'), '",1', ');', sep="")
  }

  theBooks <- paste('Structure([',
    paste(' "', books, '", Null', collapse=",", sep=""),
    ']);', sep="")
  
  str <- paste('Link( "_LIB Elec GAAP P&L Fns" );',
    'day = Date("', format(asOfDate, "%d%b%y"), '");', 
    'Books = ', theBooks, '; Results = Structure(); ', getThese,            
    'If( !@GAAP PNL Fns::GetBalReportValues( day, &Books, &GetThese, False, &Results, RptName := "flt" ))
       debug;  Results;', sep="")

  res <- secdb.evaluateSlang(str)

  if (!is.null(filter)) {
    res <- Filter(filter, res)
  }
  
  if (to.df & length(res) != 0) {
    cnames <- names(res[[1]])
    cux <- lapply(cnames, function(x){sapply(res, "[[", x)} )
    df  <- data.frame(lapply(cux, function(x){ unlist(x)} ))
    names(df) <- gsub(" ", "", cnames)

    # add the month info, you need it most of the time!
    if ("ActiveContracts" %in% names(df)) {
      df$month <- format.dateMYY( df$ActiveContracts )
      df <- df[order(df$month), ]
    }
    
    res <- df
  }

  res
}


demo.secdb.flatReport <- function()
{
  require(CEGbase)
  require(SecDb)
  require(reshape)
  
  asOfDate <- as.Date("2012-08-30")
  books    <- "NPCLPNPS" # "CPFGE"
  #variables <- c("Quantity", "Today Value")

  filter <- function(x){ x$`Service Type` == "FwdRes" }
  
  res <- secdb.flatReport(asOfDate, books, filter=filter, to.df=TRUE)
  res[order(res$`Active Contracts`), ]


  books <- secdb.getBooksFromPortfolio("Nepool Load Portfolio")
  res <- secdb.flatReport(asOfDate, books, filter=filter, to.df=TRUE)
  names(res) <- gsub(" ", "", names(res))

  
  aux <- cast(res, ActiveContracts ~ DeliveryPoint, sum, fill=0, value="Quantity")
  print(aux, row.names=FALSE)

  # or in a loop, ...  
  res <- lapply(books, function(book){
    rLog("Working on ", book)
    secdb.flatReport(asOfDate, book, filter=filter, to.df=TRUE)
  })
  res <- do.call(rbind, res)
  names(res) <- gsub(" ", "", names(res))

  aux <- cast(res, ActiveContracts ~ DeliveryPoint, sum, fill=0, value="Quantity")
  print(aux, row.names=FALSE)
  
  

  
  
  
}
