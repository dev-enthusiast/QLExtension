% Function that gets 70-day historical prices from TSDB.
function resdata = getHistTSDBPrices(startDate, endDate, curve, fwdStartDate, fwdEndDate)

numFwdMths = months(fwdStartDate,fwdEndDate);

% get yymm date and concatenate with curve name
Months    = [0:numFwdMths];
datSeries = datevec(datemnth(fwdStartDate, Months)); 
yrSeries  = num2str(datSeries(:,1));
yrSeries  = yrSeries(:,3:4);
 
% need to make this code better but ok for now
mthSeries  = datSeries(:,2);
mthSeries2 = [];

for i = 1:(numFwdMths+1)
    if mthSeries(i)<10
        mthChg = ['0' num2str(mthSeries(i))];
    else 
        mthChg = num2str(mthSeries(i));
    end
    mthSeries2 = [mthSeries2; mthChg];
end
      
names = cellstr([repmat(curve,numFwdMths+1,1) repmat('_',numFwdMths+1,1) yrSeries mthSeries2]);
 
resdata = [];
for i = 1:length(names)
   name    = char(names(i));
   z       = tsdbRead(name, startDate, endDate);
   resdata = [resdata z(:,2)];
end