# Fold a long string to a fixed width format, similar to what
# 'fill-paragraph does in Emacs
#
#
# Written by Adrian Dragulescu on 15-Feb-2010

## x <- "Want key facts about the power system in your state? Check out ISO-NE's updated state profiles and new regional profile. You'll find info on demand growth, fuel mix, DR participation, RPS, transmission development, wholesale prices, and FCA results."
## cat(text_wrap(x))

text_wrap <- function(x, WIDTH=70)
{
  out      <- NULL
  lineNo   <- 1
  rest     <- x

  while(TRUE){
    indSpace <- grep(" ", strsplit(rest, NULL)[[1]])
    if (length(indSpace)==0){
      out[length(out)] <- paste(out[length(out)], rest, sep="", collapse="")
      break
    }
    lenLine  <- max(indSpace[indSpace < WIDTH])

    out  <- c(out, substring(rest, 1, lenLine))
    rest <- substring(rest, lenLine+1, nchar(rest))
  }
  out <- paste(out, sep="", collapse="\n")
  out <- paste(out, "\n", sep="", collapse="")

  out
}
