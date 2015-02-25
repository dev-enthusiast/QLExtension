################################################################################
## Fucntion to clean the directories
## All files except .pdf will be removed if the folder is
## older than 14 (configurable) days
## Written by Hao Wu 2007-06-04
################################################################################

cleanDir <- function(timediff=14, gap=7) {
  cat("Start cleaning old directories ...\n")
  
  today <- Sys.Date() ## today's date
  ## folder lists
  root <- "\\\\ceg\\cershare\\All\\Risk\\Reports\\VaR\\prod\\"
  allfolder <- dir(root)
  alldate <- as.Date(allfolder)
  ## folders older than a certain number of days
  ## if we run this regularly, we should only look at
  ## folders between timediff and timediff+gap days old
  dif <- today - alldate
  old.folders <- allfolder[(dif>timediff) & (dif<timediff+gap) ]
  ## remove NAs
  old.folders <- old.folders[!is.na(old.folders)]
  ## loop on  those folders and clean them up
  for(i in seq(along=old.folders)) {
    subdir <- paste(root, old.folders[i], sep="")
    cat("\t", subdir, " ... \n")
    cleansubdir(subdir)
  }
  cat("Done.\n")
}

#############################################
## utility function to clean one day's folder
## containing many folders for portfolio
#############################################
cleansubdir <- function(folder) {
  tmp <- dir(folder)
  if(length(tmp) == 0) ## empty
    return(NULL)
  dirs <- paste(folder, dir(folder),sep="\\")
  for(i in seq(along=dirs)) {
    if(isdir(dirs[i])) { ## this is a dir
       ## clean it
       cleanit(dirs[i])
    }
  }
}
       
##################################################
## utility function to clean one portfolio folder
##################################################
cleanit <- function(fd) {
  ## all files except pdf
  allfile <- dir(fd)
  if(length(allfile) == 0)
    return
  
  pdffile <- dir(fd, pattern="pdf") # if a file is called pdf.tex, it'll be kept
  if(length(pdffile)==0) ## if there's no pdf file at all
    pdffile <- ""
  ## remove all non-pdf file
  nonpdf.file <- paste(fd, allfile[allfile!=pdffile], sep="\\")
  file.remove(nonpdf.file)
}
  

#############################################
## utility function to check whether an
## entry is a dir
#############################################
isdir <- function(f) {
  file.info(f)$isdir
}

  
## run the function
cleanDir()
   
   
  
