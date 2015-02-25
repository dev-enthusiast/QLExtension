# Set parameters of a particular run. 
# Still fuzzy how useful will be, but let's try it.
#
# Create the output directory from options$save$reports.dir.run. 
#
# Last modified by Adrian Dragulescu on 27-Mar-2007

set.parms.run <- function(run, options){
  # for backwards compatibility
  if (length(options$calc$fill.marks)==0){options$calc$fill.marks <- FALSE} 
  if (!is.null(run$specification) && nrow(run$specification)==0 ){
    stop(paste("\n run$specification is empty.  R cannot find the portfolio/book to run on.  \n",
      "Check schedule.overnight.xls if your run$run.name is in the list."))
  }
  run$run.name <- gsub(" ", ".", tolower(run$run.name))
  options$save$reports.dir.run <- paste(options$save$reports.dir,
     options$asOfDate,  "/", run$run.name, "/", sep="")
  if (file.access(options$save$reports.dir.run)!=0){
    mkdir(options$save$reports.dir.run)}
  if (options$save$do.IVaR.file){
    options$save$IVaR.filename <- paste(options$save$reports.dir.run,
      "IVaR.", run$run.name, ".xls", sep="")}

  #----------------------------------------------------------------------------
  run$PBT <- class <- NULL
  if (!is.null(run$specification)){
    run$PBT$portfolio <- sort(unique(run$specification$RUN_NAME))
    run$PBT$book <- sort(unique(run$specification$BOOK))

    #------------------------------------------add this run's classifications---- 
    class <- c(as.matrix(run$specification[, grep("CLASS", names(run$specification))]))
    class <- tolower(unique(as.character(class[!is.na(class)])))
  }
  run$classification <- c("year", "type", "market", "market|year",
    "year|type", "market|type|year", class) 
  
  return(list(run, options))
}


##  #-------------------------------------------load nightly options----
##   filename <- paste(options$save$datastore.dir, "options.overnight.",
##     options$asOfDate, ".RData", sep="")
##   load(filename)
 
