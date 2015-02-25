# Performs 1D numerical integration using a Gaussian 64 point rule
#      int_a^b f(t) dt
# The algorithm does recursive partitioning of the integration interval
# and calculates the integral over the intervals that have not yet showed
# convergence. The intervals are halved until less than options$TolX. 
#			
# function z=quadGaussFun(funcName,a,b,options,...)
#
#  where - funcName is the name of the function f(t) 
#	 - [a,b] are the lower(upper) integration limits  
#	 - options$RelError=[10^-4] the relative error between two iterations
#        - options$TolFun=[10^-10]  integrate only where f(t) > options.TolFun
#        - options$TolX=[10^-5] the length of the smallest interval to integrate 
#	 - options$Display=['off']
#		          ='iter' displays info at each iteration
#			  ='final'displays info only at final interation
# Version 1.0 Written by A.D. on 4-Feb-2002 
#             Works fine if the integrand does not have singularities, or cusps.
# Example:
#
# testFun <- function(x,a){exp(-a*x)}
# (quadGaussFun(testFun,0, Inf, list(), 2))

quadGaussFun <- function(fName,a,b,options, ...){

  if (nargs()==3){options <- list()}
  if (a>b){stop("Invalid range, a>b !")}
  if (length(options$RelError)==0){options$RelError <- 10^(-4)}
  if (length(options$TolFun)==0){options$TolFun <- 10^(-10)}
  if (length(options$Display)==0){options$Display <- "off"}
  if (length(options$TolX)==0){options$TolX <- 10^(-5)}
    
#------------------------------------------------------------------
gSumFun <- function(fName,a,b,bp,wf,nQuad, ...){
  # make the change of variables from [-1,1] to [a,b] 
  baM <- (b-a)/2           # minus
  baP <- (b+a)/2           # plus
  x <- baM*bp + baP        # the real x values          
  fx<- fName(x, ...)       # evaluate the function
  zOut <- baM*sum(wf*fx)   # calculate the sum 
} 
#------------------------------------------------------------------

  #---------------------------------- get the correct integration range ----
  maxLimit <- 1048577  # 2^20 +1 
  sw <- 0
  if (a==-Inf){ 
      sw <- 1
      aTrue <- 1
      f=fName(-aTrue, ...)
      while (abs(f) > options$TolFun){
	aTrue <- aTrue*2
	if (aTrue > maxLimit){
	   stop("The function does not appear to converge at -Inf")}
	f=fName(-aTrue, ...)}
      aTrue <- -aTrue
  } else { aTrue <- a } # if lower limit is finite
  if (b==Inf){
      sw <- 1
      bTrue <- 1
      f=fName(bTrue, ...)
      while (abs(f) > options$TolFun){ 
	bTrue <- bTrue*2
	if (bTrue > maxLimit){ 
	   stop("The function does not appear to converge at +Inf")}
        f=fName(bTrue, ...)}
  } else { bTrue <- b }  # if upper limit is finite

  if (is.na(match(options$Display,"off"))){	 
     aux=paste("The integration is performed between: ",aTrue,"and",bTrue)
     print.noquote(aux)}

#------------------------------Define the points and weights-------
data64<-c(
    0.0243502926634244325,   0.0486909570091397204 ,
    0.0729931217877990394,   0.0485754674415034269 ,
    0.121462819296120554,    0.0483447622348029572 ,
    0.169644420423992818,    0.0479993885964583077 ,   
    0.217423643740007084,    0.0475401657148303087 ,
    0.264687162208767416,    0.0469681828162100173 ,
    0.311322871990210956,    0.0462847965813144173 ,
    0.357220158337668116,    0.0454916279274181445 ,
    0.402270157963991604,    0.0445905581637565631 ,
    0.446366017253464088,    0.0435837245293234534 ,
    0.489403145707052957,    0.0424735151236535890 ,
    0.531279464019894546,    0.0412625632426235286 ,
    0.571895646202634034,    0.0399537411327203414 ,
    0.611155355172393250,    0.0385501531786156291 ,
    0.648965471254657340,    0.0370551285402400460 ,
    0.685236313054233243,    0.0354722132568823838 ,
    0.719881850171610827,    0.0338051618371416094 ,
    0.752819907260531897,    0.0320579283548515536 ,
    0.783972358943341408,    0.0302346570724024789 ,
    0.813265315122797560,    0.0283396726142594832 ,
    0.840629296252580363,    0.0263774697150546587 ,
    0.865999398154092820,    0.0243527025687108733 ,
    0.889315445995114106,    0.0222701738083832542 ,
    0.910522137078502806,    0.0201348231535302094 ,
    0.929569172131939576,    0.0179517157756973431 ,
    0.946411374858402816,    0.0157260304760247193 ,
    0.961008799652035719,    0.0134630478967186426 ,
    0.973326827789910964,    0.0111681394601311288 ,
    0.983336253884625957,    0.00884675982636394772 ,
    0.991013371476744321,    0.00650445796897836286 ,
    0.996340116771955280,    0.00414703326056246764 ,
    0.999305041735772139,    0.00178328072169643295)
dim(data64) <- c(2,32)
data64 <- t(data64)
bp <- array(0, c(64,1)); wf <- bp
bp[33:64] <-  data64[,1]      # ---> the base points
bp[1:32]  <- -data64[32:1,1]
wf[33:64]<-  data64[,2]       # ---> the weight factors
wf[1:32] <-  data64[32:1,2]
nQuad <- 64                   # number of quadrature points
#------------------------------------------------------------------
z <- 0
valInt <- gSumFun(fName,aTrue,bTrue,bp,wf,nQuad, ...)
range  <- array(c(aTrue,bTrue), dim=c(1,2))
h <- bTrue-aTrue              # the length of the integration interval
while ((dim(range)[1]>0) | (h < options$TolX)){
  index <- NULL  
  L <- dim(range)[1]
  rangeNew  <- array(0, c(2*L,2))
  valIntNew <- array(0, c(2*L,1))
  indOdd <- seq(1,2*L-1, by=2);  indEven <- seq(2,2*L, by=2)
  rangeNew[indOdd,]<- cbind(range[,1], (range[,1]+range[,2])/2)
  rangeNew[indEven,]  <- cbind((range[,1]+range[,2])/2,  range[,2])
  for (j in 1:L){
      valIntNew[2*j-1] <-
         gSumFun(fName, rangeNew[2*j-1,1], rangeNew[2*j-1,2], bp, wf, nQuad, ...)
      valIntNew[2*j]   <-
         gSumFun(fName, rangeNew[2*j  ,1], rangeNew[2*j  ,2], bp, wf, nQuad, ...)
      if (abs((valIntNew[2*j-1]+valIntNew[2*j])/valInt[j] - 1)<options$RelError){
           index <- c(index,2*j-1,2*j) # interval j has been integrated succesfully
      }
  }
  if (length(index)!=0){
    z <- z + sum(valIntNew[index])  # add the contributions
    rangeNew  <- rangeNew[-index,]  # remove the intervals which have been integrated 
    valIntNew <- valIntNew[-index]  # and remove the corresponding values
  }
  range <- rangeNew
  valInt <- valIntNew
  if (is.na(match(options$Display,"off"))){	 
    txt <- paste("Iteration:",L,", Value of integral:", z)
    print(txt)
  }
  h <- h/2                       # the length of the integration intervals
  z                              # return the value of the integral
} # end of while loop
return(z)  
} # end of function 


