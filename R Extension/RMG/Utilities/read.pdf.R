#
#
#
#

#################################################################
# Is the file really a pfd file?  
#
.isPDF <- function( filename )
{
  aux <- readLines( filename, warn=FALSE )

  if (grepl("^\\%PDF", toupper(aux[1]))) TRUE else FALSE
}




############################################################################
# Read pdf files using the xpdf/pdftotext utilitiy.  See xpdf on the web.  
# Usage:
#
# @filein   - name of the pdf file you want to read
# @fileout  - name of the pdf file you want to output, if = NULL 
#              returns output to R for further processing.              
# @layout   - if you want pdftotext to try and maintain file layout
# @first    - first page to convert, an integer
# @last     - last  page to convert, an integer
# @eol      - end of line specification, can be "unix", "dos", or "mac"
# @opw      - owner password
# @upw      - user  password
#
# Written by Adrian Dragulescu on 23-Jul-2008
#
read.pdf <- function(filein, fileout=NULL, layout=TRUE, first=NULL,
                     last=NULL, eol=NULL, opw=NULL, upw=NULL)
{
  cmd <- "//ceg/cershare/data/pdf/xpdf/pdftotext "

  if (layout) cmd <- paste(cmd, "-layout")
  if (!is.null(first)) cmd <- paste(cmd, "-f", first)
  if (!is.null(last))  cmd <- paste(cmd, "-l", last)
  if (!is.null(eol))   cmd <- paste(cmd, "-eol", eol)
  if (!is.null(opw))   cmd <- paste(cmd, "-opw", opw)
  if (!is.null(upw))   cmd <- paste(cmd, "-upw", upw)
  
  if (!file.exists(filein)){
    stop(paste("Cannot find file: ", filein))
  } else {
    cmd <- paste(cmd, shQuote(filein))
  }

  if (is.null(fileout)){
    cmd <- paste(cmd, "-")
    res <- system(cmd, intern=TRUE)
  } else {
    cmd <- paste(cmd, fileout)
    res <- system(cmd)
    if (res != 0)
      stop("Pdf conversion to txt failed.\n")
  }

  return(res)
}

#########################################################################
# Utility to try hard to adjust horizontal alignment of the fields.
#  - MM is the raw text with the fields, no colnames
#    each element is a different page.
#  - noFields how many fields you know your table should have.
#  noFields <- 14
#  validateList <- list()
#  validateList[["3"]]  <- "[[:alpha:]]"
#  validateList[["4"]]  <- "[[:digit:]]+"
#  validateList[["10"]] <- "Y|N| "
#
# Not perfect, but can do basic things ... 
#
.alignTablePage <- function(MM, noFields, validateList=NULL,
   noSpaces=2)
{
  # split by more than 2 spaces
  aux  <- strsplit(MM, paste(" {", noSpaces,",}", sep=""))
  guessedNoFields <- sapply(aux, length)
  if (max(guessedNoFields) > noFields){
    browser()
    #print(aux)
    #stop("You have more fields than you thought.  Increase spacing?") 
  }
  
  # Validate columns, unfortunately it does not fix anything
  # just finds if there are problems ... 
  for (v in seq_along(validateList)){
    column <- names(validateList)[v]
    fields <- sapply(aux, "[[", as.numeric(column))
    ind <- which(regexpr(validateList[[column]], fields) != 1)
    if (length(ind)>0){
      browser()
      cat("Broken validation for field", v, "\n")
    }
  }

  noFields <- max(max(guessedNoFields), noFields)
  #padd with empty spaces if fields are not equal
  aux1 <- lapply(aux, function(x, noFields){
    if (length(x) < noFields)
      x <- c(x, rep("", noFields-length(x)))
    x}, noFields)
  
  aux2 <- do.call(rbind, aux1)
  
  return(aux2)
}

