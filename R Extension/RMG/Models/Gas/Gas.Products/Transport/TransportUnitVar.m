% PathApprovalUnitVaR
% Calculates 70-day historical daily VaR
% price data from TSDB
%

clear all

% Input variables
curve1         = 'prc_ng_TRZN6_non_phys'
curve2         = 'prc_ng_TRAZN1_phys'
fwdStartDate   = '1Apr08'
fwdEndDate     = '31Oct10'
mthfixedVolume = 300000
fuelcharge     = 0.0464
commodcharge   = 0.03343+0.0019

startDate = today - 105;
endDate   = today;
 
fwdStartDate = datenum(fwdStartDate);                                       
fwdEndDate   = datenum(fwdEndDate);                                           
numFwdMths   = months(fwdStartDate,fwdEndDate); 
 
% PRICES
resdata1 = getHistTSDBPrices(startDate, endDate, curve1, fwdStartDate, fwdEndDate); % Delivery Point
resdata2 = getHistTSDBPrices(startDate, endDate, curve2, fwdStartDate, fwdEndDate); % Receipt Point

% For USA
resdata2 = resdata2/(1-fuelcharge);
% For CANADA
%resdata2 = resdata2*(1+fuelcharge);

figure; plot(resdata1'); title(curve1); xlabel('months'); ylabel('price'); hold on
figure; plot(resdata2'); title(curve2); xlabel('months'); ylabel('price'); hold on
 
% VOLUMES
totalVolume = mthfixedVolume * (numFwdMths+1)
 
% DISCOUNT FACTOR
dateSeries     = fwdStartDate;
date           = fwdStartDate;
for i = 1:numFwdMths
    date       = eomdate(year(date),month(date))+1;
    dateSeries = [dateSeries; date];
end
 
df = DiscountFactor(dateSeries);
 
% strike = (resdata2/(1-fuelcharge)) - resdata2 + commodcharge;
strike = commodcharge;

mthVal = max(0,resdata1 - resdata2 - strike); 
mthVal = mthVal .* repmat(df,size(resdata1,1),1);
 
% VAR
VaR = totalVolume * 4 * std(diff(mean(mthVal')))
VaR_unitvolume = VaR/totalVolume