################################################################################
# Send email from R.
# Where body is as.character.
#  from  <- adrian.dragulescu@constellation.com
#  to <- "frank.hubbard@constellation.com, wang.yu@constellation.com"
#  subject <- "Curves that are missing from Frank's list"
#  body <- "test message" or c("text", "text", "text")
#  cc <- same as to: field
#  attachments <- "file.txt" or c("file1", "file2", "file3")
#  links <- "Link Location" or c("Link 1", "Link 2", "Link 3")
#
sendEmail <- function(from, to, subject, body, cc=NULL,
  attachments=NULL, links=NULL, html=FALSE)
{
#  BLAT <- "\\\\DFS-CNDLR-01\\cerapps\\util\\blat.exe"
  BLAT <- "I:/util/blat.exe"   # cannot find blat anymore AAD 30Jan09
  #MAILSERVER <- "relay1.ceg.corp.net";
  MAILSERVER <- "procmonster.ceg.corp.net"  # AAD 19Nov09
  
  command <- paste(BLAT, "-", "-to", shQuote(to), "-server", 
    MAILSERVER, "-s", shQuote(subject), "-f", shQuote(from))
  
  # if we have a cc field  
  if ( !is.null(cc) )
  {
    command = paste(command, "-cc", shQuote(cc))
  }  

  # send any attachments passed in
  if ( !is.null(attachments) )
  {
    command = .appendAttachments(command, attachments)
  }
  
  # handle any links
  if ( !is.null(links) )
  {
    if (html)
      command = paste( command, "-html")  # HTML seems to screw up text alignment
    body = .handleLinks( body, links, html )
  }
  system(command, input=body)
}


################################################################################
# Add an attach flag for each file the user would like to include
#
.appendAttachments <- function(command, attachments)
{
  attachments = .correctPaths(attachments)
  for( attachment in attachments )
  {
    command = paste( command, "-attach", shQuote(attachment) )
  }
  
  return(command)
}


################################################################################
# Based on if the body was passed as a single string or array of strings, 
# take those links and add them to the bottom of the body of the email
#
.handleLinks <- function(body, links, html)
{
  links = .correctPaths(links)

  if (html){
    linksPart = "<br><br>Included Links:\n"
    for (link in links)
    {
      linksPart = c(linksPart, paste("<br><a href=\"",link, "\">",link,"</a> ",sep="", 
        collapse="\n"))
    }
  } else {
    linksPart = "\n\nIncluded Links:\n"
    linksPart <- paste(linksPart, paste(links, sep="", collapse="\n"), sep="")
  }

  # have to put the body in a text box, otherwise it won't keep spaces
  # doesn't keep them in outlook anyhow... 
##   if (length(body)>0){
## browser()    
##     nRows <- length(body)
##     nCols <- max(nchar(body))
##     fmt <- paste("%-", nCols, "s", sep="")
##     body <- sprintf(fmt, body)
##     body2  <- paste(body, sep="", collapse="\n")
##     body3  <- paste("<textarea rows=", nRows, ", cols=", nCols, ">", body2,
##                   "</textarea>", sep="")
##   }

#  body <- c("<font face=\"courier\">\n", body, linksPart)
  body4 <- c(body, linksPart)
  return(body4)
}

################################################################################
# Function to correct any paths that R may have munged so blat can find the
# files.
#
.correctPaths <- function(paths)
{
  paths = encodeString(paths)
  if (.Platform$OS.type == "windows")
  {
    paths = gsub("/","\\", paths, fixed=TRUE)
  }
  return(paths)
}
################################################################################
getEmailDistro <- function( distroName=NULL, projectName="", on.success=TRUE )
{
    mapping = read.csv("//ceg/cershare/All/Risk/Software/R/prod/Utilities/DistroToEmailMap.csv")
    
    if( is.null(distroName) )
    {
        callingFile = sys.frames()[[1]]$ofile
        rLog("getEmailDistro called from:", callingFile)
        
        if( is.null(callingFile) )
        {
            distroName = "default"
            
        } else
        {
            distroName = basename( callingFile )
        } 
    }

    if (projectName != ""){mapping <- subset(mapping, project==projectName)}
    
    rowData = mapping[ which(mapping$name==distroName), ]
    if( nrow(rowData) != 1 )
    {
        rowData = mapping[ which(mapping$name=="default"), ]
    }

    if (on.success){
      addresses <- rowData$addresses.on.success
    } else {
      addresses <- rowData$addresses.on.fail
    }

    toList = strsplit( as.character(addresses), ";" )[[1]]
    toList = paste( toList, "@constellation.com", sep="", collapse=", ")
    
    return( toList )
}
