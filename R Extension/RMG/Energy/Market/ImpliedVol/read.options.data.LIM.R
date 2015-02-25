# Read one month of natural gas options from ICE
#
#
#

read.options.data.LIM <- function(options){

  month.letter <- c("F","G","H","J","K","M","N","Q","U","V","X","Z")
  aux <- month.letter[as.numeric(format(options$contract, "%m"))]
  yyyym <- paste(format(options$contract.dt,"%Y"), aux, sep="")
  strikes <- sprintf("%.1f", options$strikes)  # make them character

  symb.month <- paste("ICE.HENRY.FIN.FP.LD2.SET_", yyyym, ".", sep="")
  symbols1 <- paste("ICE.HENRY.FIN.FP.LD2.SET_", yyyym, ".C", strikes, sep="")
  aux1   <- paste(1:length(symbols), ": Close of front ", symbols1, sep="",
               collapse=" ")
  symbols2 <- paste("ICE.HENRY.FIN.FP.LD2.SET_", yyyym, ".P", strikes, sep="")
  aux2    <- paste(1:length(symbols), ": Close of front ", symbols2, sep="",
               collapse=" ")
  query <- paste("SHOW ", aux1, aux2, "WHEN Date is after 01/01/2006")
  res   <- read.LIM(query)
  colnames(res) <- c("Date", symbols1, symbols2)
  res  <- na.omit(res)
  res  <- cbind(stack(res), Date=rep(res$Date, length(c(symbols1,symbols2))))
  aux  <- unlist(strsplit(as.character(res$ind), symb.month))
  aux  <- matrix(aux, nrow=2)[-1,]
  res  <- cbind(res, type=ifelse(substr(aux, 1, 1)=="C", "CALL", "PUT"))
  res  <- cbind(res, strike=as.numeric(substr(aux, 2, nchar(aux))))

  query <- paste("Show 1: Close of front NG_", yyyym, " when date is after",
    " 01/01/2006", sep="")
  fwd   <- read.LIM(query); fwd <- na.omit(fwd)
  colnames(fwd) <- c("Date", "Price")

  MM <- merge(res, fwd)
  MM$Date <- as.Date(MM$Date)

  return(MM)
}
