# send an xtable to latex for pdf creation
#
#
require(xtable)

## X <- iris[1:10,]
## X <- xtable(X, caption="Iris data.", align="rllll|r")
## X  <- print(X, include.rownames=FALSE)

## plot.xtable(X, textwidth=5, textheight=3)



plot.xtable <- function(x,  fileout="C:/Temp/test.pdf", textwidth=8.5,
  textheight=11, pointsize=10, ...)
{
  .TEMPLATE.TABLE <- c(
  "\\documentclass[letterpaper,%pointsizept,landscape]{article}", 
  "\\usepackage{longtable}",
  "\\usepackage{color}",
  "%\\usepackage{graphicx} \\usepackage{times}",
  "\\pagestyle{empty}",
  "\\pdfpagewidth       %textwidthin %10.8in %178mm",
  "\\oddsidemargin     0in",
  "  \\advance\\oddsidemargin by -1in",
  "  \\advance\\oddsidemargin by 0.1in",
  "\\evensidemargin    0in",
  "  \\advance\\evensidemargin by -1in",
  "  \\advance\\evensidemargin by 0.1in",
  "\\pdfpageheight        %textheightin % 9.75 is the max you can go",
  "\\topmargin         0in", 
  "  \\advance\\topmargin by -0.5in  % -0.5in the max you can go",
  "\\headheight        0pt",
  "\\headsep           0pt",
  "\\topskip           0pt",
  "",
  "\\begin{document}",
  "%insertTable",
  "\\end{document}"
  )

  tfile <- gsub("%textwidth", as.character(textwidth), .TEMPLATE.TABLE)
  tfile <- gsub("%textheight", as.character(textheight), tfile)
  tfile <- gsub("%pointsize", as.character(pointsize), tfile)

  # add the table
  tfile <- append(tfile, x, after=21)
#  tfile <- gsub("%insertTable", x, tfile)

  # write to a tempfile
  filename <- gsub("\\.pdf$", "\\.tex", fileout)
  writeLines(tfile, con=filename)

  # call Latex
  thisDir <- getwd()
  setwd(dirname(fileout))
  if (file.exists(fileout))
    unlink(fileout)
  cmd <- paste("//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex",
               filename)
  system(cmd, invisible=TRUE)

  cat("Wrote pdf file to", fileout, "\n")

  # come back
  setwd(thisDir)
}
