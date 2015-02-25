
# Test RServer and Excel Client Connection
# Written by Wang Yu, June 24 2005

RServerExcelClientTest <- function(username, computer,enumber)
  {
    System <- Sys.info()
    InfoServer <- paste("This is", System["sysname"], System["release"], "runing on", System["nodename"])
    InfoR <- R.version.string
    InfoUser <-  paste("Your name is",username, "and you are authorized as", System["login"])
#    if (enumber != System["login"])
#        {
#          Joke <- paste("Are you pretending to be", enumber)
#        } else
#        { 
#          Joke <- paste("Welcome to R,", System["user"])
#        }
  Message <- c(InfoServer,InfoR,InfoUser)
    
   setwd("s:/All/Risk/Analysis/Zonal.Risk/NEPOOL/CEAR")
   tex.filename <- "s:/All/Risk/Analysis/Zonal.Risk/NEPOOL/CEAR/RServerExcelClientTest"
   system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
   file.remove(paste(tex.filename, ".aux", sep="")) 
   file.remove(paste(tex.filename, ".log", sep=""))
     
  return(Message)   
  }
