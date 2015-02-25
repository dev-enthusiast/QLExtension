# Given a stress test specification, creates the stressed market states. 
#
#
# Written by Adrian Dragulescu on 14-Feb-2007

expand.one.specification <- function(PP, sPP, spec){
  ind.1 <- eval(parse(text=paste("PP$", spec$When, sep="")))
  if (spec$What != "All"){
    ind.2 <- eval(parse(text=paste("PP$", spec$What, sep="")))
  } else {ind.2 <- rep(TRUE, nrow(PP))}
  ind <- ind.1 & ind.2  
  if (length(ind)>0){
    sPP$Price[ind] <- sPP$Price[ind]*(1 + spec$Percent/100)}
  return(sPP)
}

expand.stress.test <- function(PP, scenario){
  sPP <- PP                          # stressed prices
  sPP$Price.0 <- sPP$Price
  for (s in 1:nrow(scenario)){
   spec <- scenario[s,]
   sPP  <- expand.one.specification(PP, sPP, scenario[s,])
  }
  return(sPP)
}

value.change <- function(sPP, QQ, scenario){
  aux <- merge(QQ, sPP, all.x=T)
  aux$value <- aux$delta*(aux$Price - aux$Price.0)

  res <- cast(aux, counterparty ~ ., sum, na.rm=T)   # tapply is faster
  res <- data.frame(name=uScenarios[s], res)
  return(res)
}
