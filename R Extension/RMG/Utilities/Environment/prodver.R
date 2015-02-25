# Copy files from CVS into R prod, or uat area.  Experimental now.
# Where x       - is a file name or directory name.
#       pattern - is a regexp string for selecting files.
#
#   x <- "H:/user/R/RMG/Energy/VaR/rmsys/Overnight/"
#   source("H:/user/R/RMG/Utilities/Environment/prodver.R")
#   prodver(x)
#
# Written by Adrian Dragulescu on 9-Jan-2006


#########################################################################
# dir.create is not recursive, even when recursive=TRUE 2.12.1
# x <- "S:/All/Risk/Software/R/prod/Energy/PM/A/B/C"
#
recursive.dir.create <- function(x)
{
  suppressWarnings(xs <- strsplit(normalizePath(x), "\\\\")[[1]])
  for (i in 2:length(xs)) {
    y <- paste(xs[1:i], sep="", collapse="\\")
    if (!file.exists(y)) dir.create(y)
  }
}


#########################################################################
#
commit.one <- function(x, env, email){
   source("H:/user/R/RMG/Utilities/Environment/CVSUtils.R")
   if( !CVSUtils$isFileUpToDate(x) )
   {
     print("##########################################################")
     cat( "\n\nThe file ", x, " is out of date in CVS.\n\n\n")      
     print("##########################################################")
     stop("\nPlease commit it to CVS first.  Exiting.")
   }
   
   filename <- gsub("H:/user/R/RMG/", paste("S:/All/Risk/Software/R/",
     env, "/", sep=""), x)
   
   recursive.dir.create(dirname(filename))
   rLog("File is", filename)

   aux <- strsplit(x, "\\.")[[1]]
   ext <- aux[length(aux)]        # get the file extension
   if (ext %in% c("R", "r", "bat", "tex", "txt")){
     net.S  <- "//ceg/cershare/"
     net.R  <- "//nas-msw-07/rmgapp/"
     outdir <- paste(net.S, "All/Risk/Software/R/", sep="")
     
     MM <- readLines(x)   # replace all occurences of H:/user/R/RMG/
     # This switches all backslashes, no idea why
     #MM <- gsub("\\\\", "/", MM)  # make everything forward slash
     MM <- gsub("H:/user/R/RMG/", paste(outdir, env, "/", sep=""), MM)
     MM <- gsub("S:/", net.S, MM)
     MM <- gsub("R:/", net.R, MM)

     writeLines(MM, filename)
   } else {   # if it's an xls file or other binary format 
     print(file.copy(x, filename, overwrite=TRUE))
   }
   if (email){
     toList <- "R_devel_CEG@constellation.com"
                                        #getEmailDistro("r_developers")
     from   <- paste(Sys.getenv("USERNAME"), "@constellation.com", sep="")
     subject <- paste("Greetings!  I just moved ", x, " file to R prod.",
       sep="") 
     
     bodyText = CVSUtils$getLastLogEntry(x)
     
     sendEmail(from, toList, subject, body = bodyText)
   }
}

#########################################################################
#
prodver <- function(x, pattern=NULL, env="prod", email=TRUE){
  #source("H:/user/R/RMG/Utilities/sendemail.R")
  require(CEGbase)
  x <- gsub("/$", "", x)   # in case directory name ends with /

  finfo <- file.info(x)
  
  if (finfo$isdir) {
    files <- list.files(x, pattern=pattern)
    ind   <- grep("~", files)
    
    if (length(ind)>0) {
      files <- files[-grep("~", files)]} # remove old cvs version files
    
    for (f in files) {
      filename  <- paste(x, "/", f, sep="")
      finfo <- file.info(filename)
      if (finfo$isdir){next}
      commit.one(filename, env, email)
    }
  } else {
    commit.one(x, env, email)    
  }
  msg <- paste("Commited ", x, " to ", env, ".", sep="")
  cat(msg); flush.console()
}

