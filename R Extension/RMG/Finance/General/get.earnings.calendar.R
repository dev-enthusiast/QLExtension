# get earnings calendar from Yahoo.
#
#
# written by Adrian Dragulescu on 6-Aug-2008

get.earnings.calendar <- function(date=Sys.Date())
{
  yyyymmdd <- format(date, "%Y%m%d")
  link <- paste("http://biz.yahoo.com/research/earncal/",
                yyyymmdd, ".html", sep="")
  con  <- url(link)
  res <- readLines(con)
  close(con)
  if (class(res)[1]=="error")
    stop(paste("Could not read the web page:", link, sep=""))
  
  ind <- grep("Announcements for", res)
  if (length(ind)==0)
     stop("Page structure has changed.  Please check")

  res <- res[-(1:ind)]
  res <- gsub("align=center","", res)   # remove some yahoo formatting 
  res <- gsub("<small>", "", res)
  res <- gsub("</small>","", res)
  res <- gsub(' href=".*"', "", res)         # remove the url links
  res <- gsub(" bgcolor=eeeeee", "", res)    # remove background color

  ind <- grep("Conference", res)
  res <- res[-(1:(ind-1))]
  ind <- grep("table", res)[1]               # go the the table end
  res <- res[1:ind]

  TT  <- paste(res, sep="", collapse="")     # put all lines together
  TT  <- gsub(" target=calendar", "", TT)
  TT  <- gsub("</*a>",  "", TT)         # get rid of <a> and </a> pairs
  TT  <- gsub("</*tr>", "", TT)        
  TT  <- gsub("<br>", "", TT)        

  aux <- strsplit(TT, "</*td>")[[1]][-1]   # split by <td> and </td>
  aux <- aux[-which(aux=="")]
  ind <- grep("href=", aux)                # there are some links left
  if (length(ind)>1) aux <- aux[-ind]      
  aux <- aux[1:(5*(length(aux)%/%5))]      # cut the few loose ends

  res <- matrix(aux, ncol=5, byrow=TRUE)
  res <- data.frame(res)
  colnames(res) <- c("company", "ticker", "eps", "when", "date")

  op <- options()
  options(warn=-1)
  res$eps  <- as.numeric(res$eps)  # there are some N/A strings
  options(op)
  res$date <- date
  
  return(res)
}






