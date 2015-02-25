#
#  source("H:/user/R/RMG/Energy/Tools/BasisVaR/BasisStudy.r")
#

basis.var.wrap <- function(file.name) {
  require(xlsReadWrite) 
  require(SecDb) 
  #file <- "//nas-omf-01/cpsshare/All/Risk/Software/R/prod/Energy/Tools/BasisVaR/BasisVaRCalculator.xls"
  #file <- "H:/user/R/RMG/Energy/Tools/BasisVaR/BasisVaRCalculator.xls"
  file <-  file.name
  input.type <-  c(rep("character", 8), rep("isodate", 2) )                 
  inputs <- read.xls(file, colClasses=input.type, sheet="InputToModel")
  inputs <- subset(inputs,  calculator_weight != "0")
  inputs$secdb_bucket <- paste("c(", inputs$secdb_bucket, ")", sep="")
  inputs$hist_start_date <- as.Date(paste(inputs$hist_start_date, " 00:00", sep=""))
  inputs$hist_end_date <- as.Date(paste(inputs$hist_end_date, " 23:00", sep="")) 
  inputs$five_percent <- 0
  inputs$ninetyfive_percent <- 0
  for (i in 1:nrow(inputs)) {
      res <- get.basis.var(inputs$zone_lmp_symbol[i], inputs$hub_lmp_symbol[i],
                           inputs$hist_start_date[i], inputs$hist_end_date[i],
                           inputs$secdb_bucket[i], inputs$secdb_region[i])
      inputs$five_percent[i] <- res[1]
      inputs$ninetyfive_percent[i] <- res[2]
  }

  return(data.frame(inputs)
}                    
get.basis.var <- function(
  zone.lmp.symbol = "PJMLMP_Hr_PEP_ZONE",
  hub.lmp.symbol = "PJMLMP_Hr_Western_HUB",
  hist.date.st= as.Date("2004-01-01 00:00"), # starting date for historic generation analysis
  hist.date.end= as.Date("2007-12-31 23:00"),# end date for historic analysis
  secdb.buckets= c('5x16','2x16H','7x8'),
  secdb.region = 'PJM' ) {
  
  require(SecDb)

  setwd("S:\\Risk\\2008,Q1\\Structured Deals\\Power\\MD Auction Tranch 2\\rfiles_basis")
  
  #source("vlr 1.5.r")
  source("Data Tools.r")
  
  #zone.lmp.symbol = "PJM_DAM_PEP_ZONE"
  #hub.lmp.symbol = "PJM_DAM_Western_HUB"
  

  
  
  gasdaily.index = "Platts_GasDly_IGBEM21u"     #in case we need Heat rate info
  
  
  
  
  
  hourly.data <- get.historic.hourly.data(c(zone.lmp.symbol,hub.lmp.symbol), hist.date.st, hist.date.end,mergeType=TRUE)
  # calc hourly spread as zone-hub.
  hourly.data$Spread = hourly.data[,which(colnames(hourly.data)==zone.lmp.symbol)] - hourly.data[,which(colnames(hourly.data)==hub.lmp.symbol)]
  
  daily.data <- get.historic.daily.data(c(gasdaily.index), hist.date.st, hist.date.end,mergeType=TRUE)
  
  #head(hourly.data)
  #head(daily.data)
  
   
  merged.hourly.data = merge.historic.hourly.daily.data(hourly.data,daily.data)
  #head(merged.hourly.data)  
  
  #design the basis spread data frame
  timeInfo = get.timedate.info(merged.hourly.data)
  head(timeInfo)
  Spread = as.data.frame(merged.hourly.data$Spread)
  
  Spread$yyyymm =  timeInfo$yyyymm
  Spread$Month = timeInfo$Month
  names(Spread)=c("Spread","yyyymm","Month")
  head(Spread)
  
  ## this section is just a check to see if we get different results from a 2-stage regression using individual months (yyyymm) vs a one-stage regression. 
  ## the ar coefficient should be the same.
  ##############
  
  #merge the spread with the monthly averages and compute the difference
  
  monAvgs = aggregate(Spread$Spread,by=list(Month = timeInfo$Month),mean,na.rm=TRUE)
  mon_spread_SD = aggregate(Spread$Spread,by=list(Month = timeInfo$Month),sd,na.rm=TRUE)
  
  colnames(monAvgs)[2] = "SpreadAvg"                                                  
                                                                                      
  Spread = merge(Spread,monAvgs,sort=FALSE)                                                      
  
  Spread$diff = Spread$Spread - Spread$SpreadAvg
  
  row.names(Spread) = merged.hourly.data$time
  lag.plot(Spread$diff)

  lagRegression = ar(Spread$diff,order.max=1)
  lagRegression$ar
  
  ########################################
  # 1-stage regression, combined means and lags
  
  no_obs = length(Spread$diff)
  lead_obs = Spread$Spread[2:(no_obs)]
  
  lag_obs = Spread$Spread[1:(no_obs-1)]
  
  #mon_fact_regr_matrix = create.factor.matrix(timeInfo$yyyymm,no_obs-1,Spread$yyyymm[2:no_obs],row.names(Spread)[2:(no_obs)])
  
  ##########indicator matrix for month = 1...12
  mon_fact_regr_matrix = create.factor.matrix(timeInfo$Month,no_obs-1,Spread$Month[2:no_obs],row.names(Spread)[2:(no_obs)])
  
  no_factors = mon_fact_regr_matrix$no_factors
  labelList = as.numeric(mon_fact_regr_matrix$labelList)
  
  regression_factor_matrix = cbind(lag_obs,mon_fact_regr_matrix$fact_regr_matrix)
  
  colnames(regression_factor_matrix) = c("Lagged Spread",labelList)
  rownames(regression_factor_matrix) = row.names(Spread)[2:(no_obs)]
  
  
  regression = lsfit(regression_factor_matrix,lead_obs,intercept = FALSE)
  
  reg_output = ls.print(regression)
  reg_output_coef = reg_output$coef.table[[1]]
  
  ### this section designs the resdiuals matrix data so we can plug it into the simulation.
  ### the simulation will sample from the empirical cdf of sim_dist for each month in column 1. 
  ### add in the monthly means.
  
  #######put the resduals back by month and name the cols/rows again
  
  ######## actual month averages
  
  mon_avg = cbind(as.numeric(rownames(reg_output_coef)[2:(no_factors+1)]),reg_output_coef[2:(no_factors+1),1])
  colnames(mon_avg) =c("Month","SpreadAvg") 
  alpha = reg_output_coef[1,1] ##########mean reversion coefficient.
  mon_avg[,2] = mon_avg[,2]/(1-alpha)
  
  sim_dist = cbind(as.numeric(Spread$Month[2:(no_obs)]),regression$residuals)
  colnames(sim_dist) =c("Month","Spread Residuals")
  rownames(sim_dist) = rownames(regression_factor_matrix)
  sim_dist = merge(sim_dist,mon_avg)
  ########## make some plots for output
  
  acf(sim_dist[,2])
  windows()
  
  hist(sim_dist[,2],xlim=c(-10,10),freq=FALSE,breaks=2500,main=colnames(sim_dist)[2])
  windows()
  plot.ecdf(sim_dist[,2])
  
  monthSD = aggregate(sim_dist[,2],by=list(Month = sim_dist[,1]),sd,na.rm=TRUE)
  names(monthSD)[2] = "stdev"
  mon_stats = merge(mon_avg,monthSD)
  mon_stats = rbind(mon_stats,c("All",mean(lag_obs),sd(sim_dist[,2])))
  
  print(monthSD)
  
  ### quantiles by month
  probs = c(0,1,2.5,5.0,7.5,10,15,20,30,35,40,45,50,55,60,65,70,75,80,85,90,92.5,95,97.5,99,100)/100
  
  q_dist = quantile(sim_dist[,2],probs)
  
  
  for (i in no_factors:1) {
    q_mon = quantile(sim_dist[sim_dist[,1]==labelList[i],2],probs)
    q_dist =rbind(q_mon,q_dist) 
    rownames(q_dist)[1] = mon_fact_regr_matrix$labelList[i]
  }
  
  rownames(q_dist)[no_factors+1]= "All"
  
  ####################finally we do a simulation by sampling from the empirical cdf
  p=500  ## simulations
  
  hours_per_month = 730
  
  month_dist = matrix(0,nrow=p,ncol=no_factors+2)
  colnames(month_dist) = c(labelList,"All","Wght Avg")
  rownames(month_dist) = 1:p
  ma_coef = (alpha)^((hours_per_month):1)
  ma_facts = (1-ma_coef)/(1-alpha)
  ma_facts_matrix = t(matrix(ma_facts,nrow=hours_per_month,ncol=p))
  
  
  
  for (k in 1:(no_factors+1)) {
    if (k<=no_factors) {
    #########empirical cdf
      X = sim_dist[sim_dist[,1] == labelList[k],2]
    } else {
      #########last column is for whole year
      X = sim_dist[,2]
    }
    simCdf = ecdf(X)
    simCDFX=knots(simCdf)
    simCDFY = simCdf(simCDFX)
    cdfAppxFun = approxfun(simCDFY,simCDFX,rule=2)  
    
    Z=create.uniform.matrix(p,hours_per_month)
    hour_dist = matrix(cdfAppxFun(Z),nrow=p,ncol=hours_per_month)
    hour_dist_ma_weighted = hour_dist * ma_facts_matrix
    month_dist[,k] = apply(hour_dist_ma_weighted,1,mean)
  }
  
  ### should have mean zero and anything else is an artifact of the sampling
  ### so swwep out the means
  month_dist = sweep(month_dist,2,apply(month_dist,2,mean))  
  
  if (exists("risk_weights")) {
    
    idx2 = match(colnames(month_dist),risk_weights[,3])
    month_weights=risk_weights[idx2,2]
    months_undefined = is.na(month_weights) 
    month_weights[months_undefined] =0
    #names(month_weights)=colnames(colnames(month_dist)[1:no_factors])
    month_fact = t(matrix(month_weights,nrow=no_factors+2,ncol=p))
    month_dist[,k+1] = apply(month_dist[,] * month_fact,1,sum)
    
    
  } else {
    month_dist[,k+1] = apply(month_dist[,1:no_factors],1,mean)
  }
  
  
  
  ############## now generate the percentile matrix for the deal
  sim_q_dist = matrix(0,nrow=no_factors+2,ncol=length(probs))
  colnames(sim_q_dist)=probs
  rownames(sim_q_dist) = colnames(month_dist)
  
  
  for (i in 1:(no_factors+2)) {
    sim_q_dist[i,] = quantile(month_dist[,i],probs)
  }
  
  result <- quantile(month_dist[,no_factors+2],c(0.05,.95))

  return(result)
}

 






