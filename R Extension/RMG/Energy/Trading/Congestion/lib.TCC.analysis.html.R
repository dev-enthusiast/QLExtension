# Make an html report for easy sharing/investigation
#
# ..add_header
# ..add_coda
# ..add_contents
# .make_report_html
#

RPT <- new.env()

#################################################################
# 
#
..add_header <- function()
{
  RPT$out <- '<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>TCC path analysis</title>
  <link rel="stylesheet" href="external/jquery-ui.css">
  <script src="external/jquery-2.1.1.min.js"></script>
  <script src="external/jquery-ui.min.js"></script>
  <link rel="stylesheet" href="external/my_css.css"> 
  <script>
  $(function() {
    $( "#tabs" ).tabs({
      heightStyle: "auto"
    });
  });
  </script>
</head>
<body>
'
}

#################################################################
# 
#
..add_coda <- function()
{
  RPT$out <- paste(RPT$out, '</body>
</html>
', sep="\n")
  
}


#################################################################
# 
#
..add_contents <- function( paths, cpsp, bids )
{ 
  # construct the body  
  tabs <- paste(
    '<div id="tabs">\n',
    '  <ul>\n',
    paste(sapply(paths, function(path) {
      paste('    <li><a href="#path-', path$path.no, '">Path ',
            path$path.no, '</a></li>', sep="")
    }), sep="", collapse="\n"),
    '\n  </ul>\n',

    paste(mapply(.make_one_tab, paths, cpsp, split(bids, bids$path.no)),
          sep="", collapse="\n"), 
                
    '</div>\n', sep="")

  
  # add to the report
  RPT$out <- paste(RPT$out, tabs)

  invisible()
}


#################################################################
# Prepare the data for the html report
#
.calculate_results <- function( sheetName="NESTAT" )
{
  DIR <- paste("S:/Trading/Nodal FTRs/NYISO/Auctions/",
               auctionName, sep="")

  auction <- parse_auctionName(auctionName, region="NYPP")

  
  # read the bids
  filename <- paste(DIR, "/allBids.xlsx", sep="")
  bids <- .read_bids_xlsx(filename, sheetName=sheetName)

  # construct the paths
  paths <- dlply(bids, "path.no", function(x){
    y <- x[1,]
    Path(source.ptid = y$source.ptid,
         sink.ptid   = y$sink.ptid,
         auction     = auction,
         path.no     = y$path.no)
  })

  # create the directory if not there already
  dir <- paste(DIR, "/resources", sep="")
  if ( !file.exists(dir) )
    dir.create(dir)

  # output the svg
  cpsp <- lapply(paths, function(path) {
    filename <- paste(DIR, "/resources/path_", path$path.no, ".svg", sep="")
    TCC.plot(path, wdim=c(6,6), save_svg=TRUE, filename=filename)
  })
  
  
  list(paths, cpsp, bids)
}


#################################################################
# Export the svg, make the tables, return a string
#
.make_one_tab <- function( path, cpsp, bids )
{  
  # the cpsp table
  x <- cpsp
  x$month <- format(x$month)
  colnames(x)[2] <- "auction"
  y <- print(xtable(tail(x, 14)), type="html", include.rownames=FALSE,
             print.results=FALSE, html.table.attribute='id="cpsp"')

  # the bids
  b  <- bids
  b  <- b[,c("mw", "bid.price")]
  by <-  print(xtable(b), type="html", include.rownames=FALSE,
               print.results=FALSE, html.table.attribute='id="cpsp"')

  
  out <- paste(
    '   <div id="path-', path$path.no, '" style="margin-bottom: 100%;">\n',
    '     <div style="float:left;"><img src="resources/path_', path$path.no,'.svg">\n',
    '     </div>\n',

    # add the cpsp table           
    '     <div style="float:left;">',
            y,  
    '       <p style="font-size: 10pt;">Bids:</p>',
            by,               
    '     </div>\n',
    '   </div>\n',            
    sep="")
  
  out
}

  
#################################################################
# 
#
.make_report_html <- function( auctionName, sheetName="NESTAT" )
{
  library(xtable)

  data  <- .calculate_results(sheetName = sheetName)
  paths <- data[[1]]   # a list of Paths
  cpsp  <- data[[2]]   # a list of data.frames  
  bids  <- data[[3]]   # the bids
  
  RPT$DIR <- paste("S:/Trading/Nodal FTRs/NYISO/Auctions/",
               auctionName, sep="")
  RPT$out <- ""
  ..add_header()
  ..add_contents(paths, cpsp, bids)
  ..add_coda()

  filename <- paste(RPT$DIR, "/report.html", sep="")
  writeLines(RPT$out, con=filename)
  rLog("Wrote file ", filename)
  
}



#################################################################
#################################################################
# 
#
