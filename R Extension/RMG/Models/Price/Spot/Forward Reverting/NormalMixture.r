# Fitting a mixture of two normals
# m0: mean of first normal distribution
# m1: mean of second normal distribution
#	s0: std deviation of first normal distribution
#	s1: std deviation of second normal distribution
#	p: weight on first distribution

# likelihood
llnormix <- function(pars,x) {
	m0 <- pars[2]
	m1 <- pars[3]
	s0 <- pars[4]
	s1 <- pars[5]
	p <- pars[1]
	ll <- -sum(log(p*dnorm(x, m0, s0) + (1-p)*dnorm(x, m1, s1)))
	ll
}

# pdf
dnormix <- function(x,pars) {
	m0 <- pars[2]
	m1 <- pars[3]
	s0 <- pars[4]
	s1 <- pars[5]
	p <- pars[1]
	p*dnorm(x, m0, s0) + (1-p)*dnorm(x, m1, s1)
}

# simulate from a mixture of two normals
rnormix <- function(N,pars) {
	m0 <- pars[2]
	m1 <- pars[3]
	s0 <- pars[4]
	s1 <- pars[5]
	p <- pars[1]
	samp <- rnorm(N, m0, s0)
	samp[sample(1:N, round(N*(1-p)))] <- rnorm(round(N*(1-p)), m1, s1)
	samp
}

