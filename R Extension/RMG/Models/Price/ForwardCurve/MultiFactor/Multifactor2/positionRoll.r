generateXSims = function(p,NSims,PCstdev,PC,Xstdev) {
    if (NSims>1) {
      NSimsUp = NSims%/%2
      NSimsBase = NSims-2*NSimsUp
    }  else {
      NSimsUp = NSims
      NSimsBase = 0
    }
    Z = array(rnorm(p * NSimsUp),dim=c(NSimsUp,p))
    Z= rbind(Z,-Z)
    if (NSimsBase>0)  {
      Z = rbind(Z,array(0,dim=c(NSimsBase,p)))
    }
    # replace this with some other distribution if needed.

    generateXSims = Z %*% PCstdev %*% t(PC) %*% Xstdev
      
}

# define vars
PCstdev = PCSimsData$PCstdev
PC = PCSimsData$PC$rotation
Xstdev = PCSimsData$dataStdev
dT = 21
# have to define deltas

rollPds = 72
NSims = 1000

# PC simulation for roll-down of a position
# we have XSim = Z %*% PCstdev %*% t(PC) %*% Xstdev
# XSim = [N1 ... NM] where each row is a simulation.
# Now N1 rolls off and we have [N2 ... NM]
# But now N2 is a function of N1 volatility.
# so we post-multiply by [0;I] where 0 is an 1x(m-1) row of zeros and I is (m-1)x(m-1) identity.

#r = nrow(XSim)

#X0 = marketData$seedRow


MV = matrix(0,nrow=NSims+1,ncol=rollPds)
discDeltasRolled = matrix(0,nrow=1,ncol=rollPds)


X0rolled=X0
XSimRolled = generateXSims(ncol(PC),NSims,PCstdev,PC,diag(Xstdev))
m = ncol(XSimRolled)

XSimApplVol = XSimRolled
deltasRolled = deltas


XSimTemp = (matrix(1,NSims,1) %*% X0) *  exp(XSimRolled*sqrt(dT))
XMeans = apply(XSimTemp,2,mean)
XSimAdj = matrix(1,NSims,1) %*% (X0rolled/XMeans)
XSimTemp = XSimTemp * XSimAdj  #scalar product - element by element

discDeltasRolled[1,1] = sum(deltasRolled[1,] * deltasRolled[2,])
MV[1,1] = X0rolled %*% (deltasRolled[1,] * deltasRolled[2,])


MV[2:(NSims+1),1] = XSimTemp %*% (deltasRolled[1,] * deltasRolled[2,])


for (j in 2:rollPds) {

  
  m = ncol(XSimRolled)
  #r = nrow(XSimRolled)
  
  rotateForwardMatrix = rbind(matrix(0,nrow=1,ncol=m-1),diag(1,m-1))
  # XSim * rotateMatrix drops the first column and rolls all columns backward.
  # we also need to push month 1 volatility to second month by dropping the last column and rolling forward
  matrixCols = rbind(diag(1,nrow=m-1,ncol=m-1),matrix(0,nrow=ncol(PC)-m+1,ncol=m-1))

  applyVolMatrixCols = matrixCols
  #rotateForwardMatrix[m,m]=0
  # XSim * rotateForwardMatrix drops the last column and rolls forward.
  #XSim = [ X1; Nm] X1: r x (m-1)
  #rollback = [0

  XSimsNew = generateXSims(ncol(PC),NSims,PCstdev,PC,diag(Xstdev))

  XSimApplVol = XSimsNew %*% applyVolMatrixCols
  
  XSimRolled =  XSimRolled %*% rotateForwardMatrix
  # XSimRolled is the actual rolled volatility. we keep iterating XSimRolled
  #XSimApplVol= XSimApplVol %*% applyVolMatrixCols

  XSimRolled = XSimRolled + XSimApplVol

  #XSimApplVol is the first m-1 columns of the XSim matrix.
  X0rolled = X0rolled %*% rotateForwardMatrix

  XSimTemp = (matrix(1,NSims,1) %*% X0rolled) *  exp(XSimRolled*sqrt(dT))
  XMeans = apply(XSimTemp,2,mean)
  XSimAdj = matrix(1,NSims,1) %*% (X0rolled/XMeans)
  XSimTemp = XSimTemp * XSimAdj  #scalar product - element by element
  deltasRolled = deltasRolled %*% rotateForwardMatrix
  
  discDeltasRolled[1,j] = sum(deltasRolled[1,] * deltasRolled[2,])
  MV[1,j] = X0rolled  %*% (deltasRolled[1,] * deltasRolled[2,])

  MV[2:(NSims+1),j] = XSimTemp %*% (deltasRolled[1,] * deltasRolled[2,])
}


generateXSims = function(p,NSims,PCstdev,PC,Xstdev) {
    if (NSims>1) {
      NSimsUp = NSims%/%2
      NSimsBase = NSims-2*NSimsUp
    }  else {
      NSimsUp = NSims
      NSimsBase = 0
    }
    Z = array(rnorm(p * NSimsUp),dim=c(NSimsUp,p))
    Z= rbind(Z,-Z)
    if (NSimsBase>0)  {
      Z = rbind(Z,array(0,dim=c(NSimsBase,p)))
    }
    # replace this with some other distribution if needed.

    generateXSims = Z %*% PCstdev %*% t(PC) %*% Xstdev
      
}