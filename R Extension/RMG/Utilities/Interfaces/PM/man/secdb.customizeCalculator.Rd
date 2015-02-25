\name{secdb.customizeCalculator}
\alias{secdb.customizeCalculator}
\title{Customize a calculator in SecDb.}
\description{
  Customize an existing "Elec CFD" or "Fuel CFD" calculator with quantities,
  and fixPrices.  Old customizations are removed.  Also start/end dates
  for the calculator are set to the start/end of the quantity data.frame. 
}
\usage{
secdb.customizeCalculator(fullCalculatorName,
  quantity=NULL, fixPrice=NULL, recalculate=TRUE, comments=NULL)
}
\arguments{
  \item{fullCalculatorName}{A string.  Needs to be the exact name of the
    security that represents the calculator.  See the function 
    \code{\link{calculator.list}} from the SecDb package.  By using the
    full calculator name, I avoid having to pass trader, group,
    calculator name, as all these are value types of the
    fullCalculatorName security.}
  \item{quantity}{A data.frame with prices to update.  If fixPrice is
    not \code{NULL} quantity and fixPrice need to match dimnames.  The
    first column of the data.frame is the month you need to customize
    (as beginning of
    the month date), and the remaining columns are the quantities in
    MW.  The other column names are constructed by pasting the SecDb
    delivery points with the buckets with "\_" as separator, e.g. CT
    DA_2X16H.  See examples.  It is easy to get to this form by
    using cast.}
  \item{fixPrice}{A data.frame with prices to update.  If quantity is
    not \code{NULL} they need to match dimnames.  The fixPrice data
    frame is in exactly the same format as the quantity data.frame. }
  \item{recalculate}{A boolean.  If \code{TRUE} it will update the calculator.}
  \item{comments}{A string.  If \code{NULL} don't update comments.}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# you may need to change the dates below ... 
asOfDate    <- Sys.Date()
startDate   <- as.Date("2010-01-01")
endDate     <- as.Date("2010-12-01")
commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
location    <- "CT DA"
bucket      <- c("5X16", "2X16H", "7X8")
serviceType <- "Energy"
useFutDates <- FALSE
  
# get the prices, and format them right 
PP <- secdb.getElecPrices( asOfDate, startDate, endDate,
  commodity, location, bucket, serviceType, useFutDates)
PP <- cast(PP, month ~ location + bucket, I, fill=NA)

# head(PP)
#       month CT DA_2X16H CT DA_5X16 CT DA_7X8
#1 2010-01-01      64.658     70.938  49.51300
#2 2010-02-01      64.577     70.938  50.78500
#3 2010-03-01      58.346     63.200  44.20965
#4 2010-04-01      57.082     63.600  40.09300
#5 2010-05-01      52.081     59.350  35.69700
#6 2010-06-01      57.624     67.050  37.11300

# create some custom quantities ...
QQ <- PP
QQ[,2:4] <- c(1:36)  # really silly, good for checking

# head(QQ)
#       month CT DA_2X16H CT DA_5X16 CT DA_7X8
#1 2010-01-01           1         13        25
#2 2010-02-01           2         14        26
#3 2010-03-01           3         15        27
#4 2010-04-01           4         16        28
#5 2010-05-01           5         17        29
#6 2010-06-01           6         18        30

# this calc is in my user area
fullCalculatorName <- "Calc ECFD example customize   0"

secdb.customizeCalculator(fullCalculatorName, quantity=QQ, fixPrice=PP)

#Customizing:  ECFD example customize calc 
#Trader:       e47187 
#Group:        CPS Trading Group 
#
#Inserting custom quantities for delivery point: CT DA and bucket: 2X16H
#Inserting custom quantities for delivery point: CT DA and bucket: 5X16
#Inserting custom quantities for delivery point: CT DA and bucket: 7X8
#Inserting custom fixPrice for delivery point:CT DA and bucket: 5X16
#Inserting custom fixPrice for delivery point:CT DA and bucket: 2X16H
#Inserting custom fixPrice for delivery point:CT DA and bucket: 7X8



# For a Fuel CFD calculator, the quantity data frame needs to looks like
# this ...
#       month TETZM3/GAS-DLY MEAN_7X24 TRAZN6 NON/GD MEAN_7X24
#1 2014-03-01                   0.0000               -879851.0
#2 2014-04-01                -598.0489               -416823.6
#3 2014-05-01             -135615.9547              -1105232.0


}
