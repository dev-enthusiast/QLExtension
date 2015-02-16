using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuantLib;

/// Depo is only used for discount term structure.
/// Bootstrap constructs forwarding curve, given discounting curve.
namespace CurveBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            QlArray x = new QlArray(5);
            x.set(0, 0.0);  x.set(1, 1.0); x.set(2, 2.0); x.set(3, 3.0); x.set(4, 4.0);
            QlArray y = new QlArray(5);
            y.set(0, 5.0); y.set(1, 4.0); y.set(2, 3.0); y.set(3, 2.0); y.set(4, 1.0);            

            QuantLib.LinearInterpolation linear = new LinearInterpolation(x, y);
            int s = (int)x.size(); double res = x.get(1);
            res = linear.call(2.0);
            res = linear.call(0.0);
            res = linear.call(3.5);

            // input strikes
            double[] strikes = new double[31];
            strikes[0] = 0.03; strikes[1] = 0.032; strikes[2] = 0.034;
            strikes[3] = 0.036; strikes[4] = 0.038; strikes[5] = 0.04;
            strikes[6] = 0.042; strikes[7] = 0.044; strikes[8] = 0.046;
            strikes[9] = 0.048; strikes[10] = 0.05; strikes[11] = 0.052;
            strikes[12] = 0.054; strikes[13] = 0.056; strikes[14] = 0.058;
            strikes[15] = 0.06; strikes[16] = 0.062; strikes[17] = 0.064;
            strikes[18] = 0.066; strikes[19] = 0.068; strikes[20] = 0.07;
            strikes[21] = 0.072; strikes[22] = 0.074; strikes[23] = 0.076;
            strikes[24] = 0.078; strikes[25] = 0.08; strikes[26] = 0.082;
            strikes[27] = 0.084; strikes[28] = 0.086; strikes[29] = 0.088;
            strikes[30] = 0.09;
            // input volatilities
            double[] volatilities = new double[31];
            volatilities[0] = 1.16725837321531; volatilities[1] = 1.15226075991385; volatilities[2] = 1.13829711098834;
            volatilities[3] = 1.12524190877505; volatilities[4] = 1.11299079244474; volatilities[5] = 1.10145609357162;
            volatilities[6] = 1.09056348513411; volatilities[7] = 1.08024942745106; volatilities[8] = 1.07045919457758;
            volatilities[9] = 1.06114533019077; volatilities[10] = 1.05226642581503; volatilities[11] = 1.04378614411707;
            volatilities[12] = 1.03567243073732; volatilities[13] = 1.0278968727451; volatilities[14] = 1.02043417226345;
            volatilities[15] = 1.01326171139321; volatilities[16] = 1.00635919013311; volatilities[17] = 0.999708323124949;
            volatilities[18] = 0.993292584155381; volatilities[19] = 0.987096989695393; volatilities[20] = 0.98110791455717;
            volatilities[21] = 0.975312934134512; volatilities[22] = 0.969700688771689; volatilities[23] = 0.964260766651027;
            volatilities[24] = 0.958983602256592; volatilities[25] = 0.953860388001395; volatilities[26] = 0.948882997029509;
            volatilities[27] = 0.944043915545469; volatilities[28] = 0.939336183299237; volatilities[29] = 0.934753341079515;
            volatilities[30] = 0.930289384251337;

            QlArray xx = new QlArray(31); QlArray yy = new QlArray(31);
            for (uint i = 0; i < 31; i++)
            {
                xx.set(i, strikes[i]);
                yy.set(i, volatilities[i]);
            }
            // QuantLib.SABRInterpolation sabr = new SABRInterpolation(xx, yy, 1.0, 0.039, Math.Sqrt(0.2), 0.5, Math.Sqrt(0.4), 0.0);
            // double err = sabr.update();
            // res = sabr.call(0.036);
            // res = sabr.call(0.038);
            // res = sabr.call(0.040);
            // res = sabr.call(0.042);
            
            //QuantLib.EuropeanOption option = new EuropeanOption(payoff, exercise);
            //AnalyticEuropeanEngine engine = new AnalyticEuropeanEngine(process);
            //option.delta();

            //QuantLib.ForwardVanillaOption option2 = new ForwardVanillaOption(moneyness, resetDate, payoff, exercise);

            QuantLib.Calendar calendar = new QuantLib.TARGET();
            QuantLib.Date settlementDate = new QuantLib.Date(22, QuantLib.Month.September, 2004);

            // Must be a business day
            settlementDate = calendar.adjust(settlementDate);

            uint fixingDays = 2;
            QuantLib.Date todaysDate = calendar.advance(settlementDate, (int)-fixingDays, QuantLib.TimeUnit.Days);
            QuantLib.Settings.instance().setEvaluationDate(todaysDate);

            todaysDate = QuantLib.Settings.instance().getEvaluationDate();
            Console.WriteLine("Today is " + todaysDate.weekday() + " , " + todaysDate);

            // deposits
            double d1wQuote = 0.0382;
            double d1mQuote = 0.0372;
            double d3mQuote = 0.0363;
            double d6mQuote = 0.0353;
            double d9mQuote = 0.0348;
            double d1yQuote = 0.0345;
            // FRAs
            double fra3x6Quote = 0.037125;
            double fra6x9Quote = 0.037125;
            double fra6x12Quote = 0.037125;
            // futures
            double fut1Quote = 96.2875;
            double fut2Quote = 96.7875;
            double fut3Quote = 96.9875;
            double fut4Quote = 96.6875;
            double fut5Quote = 96.4875;
            double fut6Quote = 96.3875;
            double fut7Quote = 96.2875;
            double fut8Quote = 96.0875;
            // swaps
            double s2yQuote = 0.037125;
            double s3yQuote = 0.0398;
            double s5yQuote = 0.0443;
            double s10yQuote = 0.05165;
            double s15yQuote = 0.055175;

            /********************
             ***    QUOTES    ***
            ********************/

            // SimpleQuote stores a value which can be manually changed;
            // other Quote subclasses could read the value from a database
            // or some kind of data feed.

            // deposits
            QuantLib.QuoteHandle d1wRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(d1wQuote));
            QuantLib.QuoteHandle d3mRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(d1wQuote));
            QuantLib.QuoteHandle d6mRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(d1wQuote));
            QuantLib.QuoteHandle d9mRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(d1wQuote));
            QuantLib.QuoteHandle d1yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(d1wQuote));
            // FRAs
            QuantLib.QuoteHandle fra3x6Rate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fra3x6Quote));
            QuantLib.QuoteHandle fra6x9Rate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fra6x9Quote));
            QuantLib.QuoteHandle fra6x12Rate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fra6x12Quote));
            // futures
            QuantLib.QuoteHandle fut1Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut1Quote));
            QuantLib.QuoteHandle fut2Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut2Quote));
            QuantLib.QuoteHandle fut3Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut3Quote));
            QuantLib.QuoteHandle fut4Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut4Quote));
            QuantLib.QuoteHandle fut5Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut5Quote));
            QuantLib.QuoteHandle fut6Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut6Quote));
            QuantLib.QuoteHandle fut7Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut7Quote));
            QuantLib.QuoteHandle fut8Price = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(fut8Quote));
            // swaps
            QuantLib.QuoteHandle s2yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(s2yQuote));
            QuantLib.QuoteHandle s3yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(s3yQuote));
            QuantLib.QuoteHandle s5yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(s5yQuote));
            QuantLib.QuoteHandle s10yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(s10yQuote));
            QuantLib.QuoteHandle s15yRate = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(s15yQuote));

            /*********************
            ***  RATE HELPERS ***
            *********************/

            // RateHelpers are built from the above quotes together with
            // other instrument dependant infos.  Quotes are passed in
            // relinkable handles which could be relinked to some other
            // data source later.

            // deposits
            QuantLib.DayCounter depositDayCounter = new QuantLib.Actual360();

            QuantLib.RateHelper d1w = new QuantLib.DepositRateHelper(d1wQuote, new QuantLib.Period(1, QuantLib.TimeUnit.Weeks),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper d1m = new QuantLib.DepositRateHelper(d1mQuote, new QuantLib.Period(1, QuantLib.TimeUnit.Months),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper d3m = new QuantLib.DepositRateHelper(d3mQuote, new QuantLib.Period(3, QuantLib.TimeUnit.Months),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper d6m = new QuantLib.DepositRateHelper(d6mQuote, new QuantLib.Period(6, QuantLib.TimeUnit.Months),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper d9m = new QuantLib.DepositRateHelper(d9mQuote, new QuantLib.Period(9, QuantLib.TimeUnit.Months),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper d1y = new QuantLib.DepositRateHelper(d1yQuote, new QuantLib.Period(1, QuantLib.TimeUnit.Years),
                fixingDays, calendar, QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);

            // setup FRAs
            QuantLib.RateHelper fra3x6 = new QuantLib.FraRateHelper(fra3x6Rate, 3, 6, fixingDays, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper fra6x9 = new QuantLib.FraRateHelper(fra6x9Rate, 6, 9, fixingDays, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);
            QuantLib.RateHelper fra6x12 = new QuantLib.FraRateHelper(fra6x12Rate, 6, 12, fixingDays, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter);

            // setup futures
            uint futMonths = 3;
            QuantLib.QuoteHandle conv = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(0.0));
            QuantLib.Date imm = QuantLib.IMM.nextDate(settlementDate);

            QuantLib.RateHelper fut1 = new QuantLib.FuturesRateHelper(fut1Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut2 = new QuantLib.FuturesRateHelper(fut2Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut3 = new QuantLib.FuturesRateHelper(fut3Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut4 = new QuantLib.FuturesRateHelper(fut4Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut5 = new QuantLib.FuturesRateHelper(fut5Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut6 = new QuantLib.FuturesRateHelper(fut6Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut7 = new QuantLib.FuturesRateHelper(fut7Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);
            imm = QuantLib.IMM.nextDate(imm + 1);
            QuantLib.RateHelper fut8 = new QuantLib.FuturesRateHelper(fut8Price, imm, futMonths, calendar,
                QuantLib.BusinessDayConvention.ModifiedFollowing, true, depositDayCounter, conv);

            // setup swaps
            QuantLib.Frequency swFixedLegFrequency = QuantLib.Frequency.Annual;
            QuantLib.BusinessDayConvention swFixedLegConvention = QuantLib.BusinessDayConvention.Unadjusted;
            QuantLib.DayCounter swFixedLegDayCounter = new QuantLib.Thirty360(QuantLib.Thirty360.Convention.European);
            QuantLib.IborIndex swFloatingLegIndex = new QuantLib.Euribor6M();

            QuantLib.RateHelper s2y = new QuantLib.SwapRateHelper(s2yRate, new QuantLib.Period(2, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex);
            QuantLib.RateHelper s3y = new QuantLib.SwapRateHelper(s3yRate, new QuantLib.Period(3, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex);
            QuantLib.RateHelper s5y = new QuantLib.SwapRateHelper(s5yRate, new QuantLib.Period(5, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex);
            QuantLib.RateHelper s10y = new QuantLib.SwapRateHelper(s10yRate, new QuantLib.Period(10, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex);
            QuantLib.RateHelper s15y = new QuantLib.SwapRateHelper(s15yRate, new QuantLib.Period(15, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex);

            /*********************
            **  CURVE BUILDING **
            *********************/

            // Any DayCounter would be fine.
            // ActualActual::ISDA ensures that 30 years is 30.0
            QuantLib.DayCounter termStructureDayCounter = new QuantLib.ActualActual(QuantLib.ActualActual.Convention.ISDA);

            double tolerance = 1.0e-15;

            // A depo-swap curve
            QuantLib.RateHelperVector depoSwapInstruments = new QuantLib.RateHelperVector();
            depoSwapInstruments.Add(d1w);
            depoSwapInstruments.Add(d1m);
            depoSwapInstruments.Add(d3m);
            depoSwapInstruments.Add(d6m);
            depoSwapInstruments.Add(d9m);
            depoSwapInstruments.Add(d1y);
            depoSwapInstruments.Add(s2y);
            depoSwapInstruments.Add(s3y);
            depoSwapInstruments.Add(s5y);
            depoSwapInstruments.Add(s10y);
            depoSwapInstruments.Add(s15y);

            QuantLib.YieldTermStructure depoSwapTermStructure = new QuantLib.PiecewiseFlatForward(settlementDate,
                depoSwapInstruments, termStructureDayCounter);

            double dd1m = depoSwapTermStructure.discount(calendar.advance(settlementDate, new QuantLib.Period(1, QuantLib.TimeUnit.Months)));
            double dd3m = depoSwapTermStructure.discount(calendar.advance(settlementDate, new QuantLib.Period(3, QuantLib.TimeUnit.Months)));
            double dd6m = depoSwapTermStructure.discount(calendar.advance(settlementDate, new QuantLib.Period(6, QuantLib.TimeUnit.Months)));
            double dd1y = depoSwapTermStructure.discount(calendar.advance(settlementDate, new QuantLib.Period(12, QuantLib.TimeUnit.Months)));
            double dd2y = depoSwapTermStructure.discount(calendar.advance(settlementDate, new QuantLib.Period(24, QuantLib.TimeUnit.Months)));

            Console.WriteLine(dd2y);

            // A depo-futures-swap curve
            QuantLib.RateHelperVector depoFutSwapInstruments = new RateHelperVector();
            depoFutSwapInstruments.Add(d1w);
            depoFutSwapInstruments.Add(d1m);
            depoFutSwapInstruments.Add(fut1);
            depoFutSwapInstruments.Add(fut2);
            depoFutSwapInstruments.Add(fut3);
            depoFutSwapInstruments.Add(fut4);
            depoFutSwapInstruments.Add(fut5);
            depoFutSwapInstruments.Add(fut6);
            depoFutSwapInstruments.Add(fut7);
            depoFutSwapInstruments.Add(fut8);
            depoFutSwapInstruments.Add(s3y);
            depoFutSwapInstruments.Add(s5y);
            depoFutSwapInstruments.Add(s10y);
            depoFutSwapInstruments.Add(s15y);
            QuantLib.YieldTermStructure depoFutSwapTermStructure = new QuantLib.PiecewiseFlatForward(settlementDate,
                depoFutSwapInstruments, termStructureDayCounter);

            // A depo-FRA-swap curve
            QuantLib.RateHelperVector depoFRASwapInstruments = new RateHelperVector();
            depoFRASwapInstruments.Add(d1w);
            depoFRASwapInstruments.Add(d1m);
            depoFRASwapInstruments.Add(d3m);
            depoFRASwapInstruments.Add(fra3x6);
            depoFRASwapInstruments.Add(fra6x9);
            depoFRASwapInstruments.Add(fra6x12);
            depoFRASwapInstruments.Add(s2y);
            depoFRASwapInstruments.Add(s3y);
            depoFRASwapInstruments.Add(s5y);
            depoFRASwapInstruments.Add(s10y);
            depoFRASwapInstruments.Add(s15y);
            QuantLib.YieldTermStructure depoFRASwapTermStructure = new QuantLib.PiecewiseFlatForward(settlementDate,
                depoFRASwapInstruments, termStructureDayCounter);

            QuantLib.RelinkableYieldTermStructureHandle discountingTermStructure = new RelinkableYieldTermStructureHandle();
            QuantLib.RelinkableYieldTermStructureHandle forecastingTermStructure = new RelinkableYieldTermStructureHandle();

             /*********************
            * SWAPS TO BE PRICED *
            **********************/

            // constant nominal 1,000,000 Euro
            double nominal = 1000000.0;
            // fixed leg
            QuantLib.Frequency fixedLegFrequency = QuantLib.Frequency.Annual;
            QuantLib.BusinessDayConvention fixedLegConvention = QuantLib.BusinessDayConvention.Unadjusted;
            BusinessDayConvention floatingLegConvention = QuantLib.BusinessDayConvention.ModifiedFollowing;
            DayCounter fixedLegDayCounter = new Thirty360(QuantLib.Thirty360.Convention.European);
            double fixedRate = 0.04;
            DayCounter floatingLegDayCounter = new Actual360();

            Frequency floatingLegFrequency = Frequency.Semiannual;
            IborIndex euriborIndex = new Euribor6M(forecastingTermStructure);
            
            double spread = 0.0;
            QuantLib.QuoteHandle spreadHandle = new QuoteHandle(new SimpleQuote(spread));
            int lenghtInYears = 5;
            QuantLib._VanillaSwap.Type swapType = QuantLib._VanillaSwap.Type.Payer;
            
            Date maturity = calendar.advance(settlementDate, lenghtInYears, TimeUnit.Years);
            
            Schedule fixedSchedule = new Schedule(settlementDate, maturity, new Period(fixedLegFrequency),
                calendar, fixedLegConvention,fixedLegConvention, DateGeneration.Rule.Forward, false);
            Schedule floatSchedule = new Schedule(settlementDate, maturity, new Period(floatingLegFrequency),
                calendar, floatingLegConvention, floatingLegConvention, DateGeneration.Rule.Forward, false);
            VanillaSwap spot5YearSwap = new VanillaSwap(swapType, nominal, fixedSchedule, fixedRate, fixedLegDayCounter,
                floatSchedule, euriborIndex, spread, floatingLegDayCounter);

            Date fwdStart = calendar.advance(settlementDate, 1, TimeUnit.Years);
            Date fwdMaturity = calendar.advance(fwdStart, lenghtInYears, TimeUnit.Years);
            Schedule fwdFixedSchedule = new Schedule(fwdStart, fwdMaturity, new Period(fixedLegFrequency),
                calendar, fixedLegConvention,fixedLegConvention,DateGeneration.Rule.Forward, false);
            Schedule fwdFloatSchedule = new Schedule(fwdStart, fwdMaturity, new Period(floatingLegFrequency),
                calendar, floatingLegConvention, floatingLegConvention, DateGeneration.Rule.Forward, false);
            VanillaSwap oneYearForward5YearSwap = new VanillaSwap(swapType, nominal,fwdFixedSchedule, fixedRate, fixedLegDayCounter,
                fwdFloatSchedule, euriborIndex, spread, floatingLegDayCounter);

            /***************
             * * SWAP PRICING *
             * ****************/
            double NPV;
            double fairRate;
            double fairSpread;

            PricingEngine swapEngine = new DiscountingSwapEngine(discountingTermStructure);

            spot5YearSwap.setPricingEngine(swapEngine);
            oneYearForward5YearSwap.setPricingEngine(swapEngine);

            // Of course, you're not forced to really use different curves
            forecastingTermStructure.linkTo(depoSwapTermStructure);
            discountingTermStructure.linkTo(depoSwapTermStructure);

            NPV = spot5YearSwap.NPV();
            fairSpread = spot5YearSwap.fairSpread();
            fairRate = spot5YearSwap.fairRate();
            Console.WriteLine("Fair rate is " + fairRate + " 5y quote is : " + s5yQuote);

            forecastingTermStructure.linkTo(depoFutSwapTermStructure);
            discountingTermStructure.linkTo(depoFutSwapTermStructure);
            NPV = spot5YearSwap.NPV();
            fairSpread = spot5YearSwap.fairSpread();
            fairRate = spot5YearSwap.fairRate();
            Console.WriteLine("Fair rate is " + fairRate + " 5y quote is : " + s5yQuote);

            forecastingTermStructure.linkTo(depoFRASwapTermStructure);
            discountingTermStructure.linkTo(depoFRASwapTermStructure);
            NPV = spot5YearSwap.NPV();
            fairSpread = spot5YearSwap.fairSpread();
            fairRate = spot5YearSwap.fairRate();
            Console.WriteLine("Fair rate is " + fairRate + " 5y quote is : " + s5yQuote);

            // now let's price the 1Y forward 5Y swap
            forecastingTermStructure.linkTo(depoSwapTermStructure);
            discountingTermStructure.linkTo(depoSwapTermStructure);
            NPV = oneYearForward5YearSwap.NPV();
            fairSpread = oneYearForward5YearSwap.fairSpread();
            fairRate = oneYearForward5YearSwap.fairRate();
            Console.WriteLine("1Yx5Y forward start fair rate is " + fairRate);

            // now let's say that the 5-years swap rate goes up to 4.60%.
            // A smarter market element--say, connected to a data source-- would
            // notice the change itself. Since we're using SimpleQuotes,
            // we'll have to change the value manually--which forces us to
            // downcast the handle and use the SimpleQuote
            // interface. In any case, the point here is that a change in the
            // value contained in the Quote triggers a new bootstrapping
            // of the curve and a repricing of the swap.

            //**************************** OIS **************************************************************************
            QuantLib.RateHelperVector eoniaHelpers = new RateHelperVector();
            QuantLib.RelinkableYieldTermStructureHandle eoniaTermStructure = new RelinkableYieldTermStructureHandle();
            QuantLib.OvernightIndex Eonia = new OvernightIndex("Eonia", 0, new EURCurrency(), new TARGET(), new Actual360(), eoniaTermStructure);

            double depo01dQuote = 0.011;
            double depo11dQuote = 0.011;
            QuantLib.QuoteHandle d01dRate = new QuoteHandle(new QuantLib.SimpleQuote(depo01dQuote));
            QuantLib.QuoteHandle d11dRate = new QuoteHandle(new QuantLib.SimpleQuote(depo11dQuote));
            QuantLib.RateHelper d01d = new QuantLib.DepositRateHelper(d01dRate, new QuantLib.Period(1, QuantLib.TimeUnit.Days),
                0, Eonia.fixingCalendar(), Eonia.businessDayConvention(), Eonia.endOfMonth(), Eonia.dayCounter());
            QuantLib.RateHelper d11d = new QuantLib.DepositRateHelper(d11dRate, new QuantLib.Period(1, QuantLib.TimeUnit.Days),
                1, Eonia.fixingCalendar(), Eonia.businessDayConvention(), Eonia.endOfMonth(), Eonia.dayCounter());

            double ois1wQuote = 0.01245;
            double ois2wQuote = 0.01269;
            double ois3wQuote = 0.01277;
            double ois1mQuote = 0.01281;
            double ois2mQuote = 0.0118;
            double ois3mQuote = 0.01143;
            double ois4mQuote = 0.01125;
            double ois5mQuote = 0.01116;
            double ois6mQuote = 0.0111;
            double ois7mQuote = 0.01109;
            double ois8mQuote = 0.0111;
            double ois9mQuote = 0.01117;
            double ois10mQuote = 0.01129;
            double ois11mQuote = 0.01141;
            double ois12mQuote = 0.01153;
            double ois15mQuote = 0.01218;
            double ois18mQuote = 0.01308;
            double ois21mQuote = 0.01407;
            double ois2yQuote = 0.01510;
            double ois3yQuote = 0.01916;
            double ois4yQuote = 0.02254;
            double ois5yQuote = 0.02523;
            double ois6yQuote = 0.02746;
            double ois7yQuote = 0.02934;
            double ois8yQuote = 0.03092;
            double ois9yQuote = 0.03231;
            double ois10yQuote = 0.03380;
            double ois11yQuote = 0.03457;
            double ois12yQuote = 0.03544;
            double ois15yQuote = 0.03702;
            double ois20yQuote = 0.03703;
            double ois25yQuote = 0.03541;
            double ois30yQuote = 0.03369;

            QuantLib.QuoteHandle ois1wRate = new QuoteHandle(new QuantLib.SimpleQuote(ois1wQuote));
            QuantLib.QuoteHandle ois2wRate = new QuoteHandle(new QuantLib.SimpleQuote(ois2wQuote));
            QuantLib.QuoteHandle ois3wRate = new QuoteHandle(new QuantLib.SimpleQuote(ois3wQuote));
            QuantLib.QuoteHandle ois1mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois1mQuote));
            QuantLib.QuoteHandle ois2mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois2mQuote));
            QuantLib.QuoteHandle ois3mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois3mQuote));
            QuantLib.QuoteHandle ois4mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois4mQuote));
            QuantLib.QuoteHandle ois5mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois5mQuote));
            QuantLib.QuoteHandle ois6mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois6mQuote));
            QuantLib.QuoteHandle ois7mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois7mQuote));
            QuantLib.QuoteHandle ois8mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois8mQuote));
            QuantLib.QuoteHandle ois9mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois9mQuote));
            QuantLib.QuoteHandle ois10mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois10mQuote));
            QuantLib.QuoteHandle ois11mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois11mQuote));
            QuantLib.QuoteHandle ois12mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois12mQuote));
            QuantLib.QuoteHandle ois15mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois15mQuote));
            QuantLib.QuoteHandle ois18mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois18mQuote));
            QuantLib.QuoteHandle ois21mRate = new QuoteHandle(new QuantLib.SimpleQuote(ois21mQuote));
            QuantLib.QuoteHandle ois2yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois2yQuote));
            QuantLib.QuoteHandle ois3yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois3yQuote));
            QuantLib.QuoteHandle ois4yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois4yQuote));
            QuantLib.QuoteHandle ois5yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois5yQuote));
            QuantLib.QuoteHandle ois6yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois6yQuote));
            QuantLib.QuoteHandle ois7yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois7yQuote));
            QuantLib.QuoteHandle ois8yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois8yQuote));
            QuantLib.QuoteHandle ois9yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois9yQuote));
            QuantLib.QuoteHandle ois10yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois10yQuote));
            QuantLib.QuoteHandle ois11yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois11yQuote));
            QuantLib.QuoteHandle ois12yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois12yQuote));
            QuantLib.QuoteHandle ois15yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois15yQuote));
            QuantLib.QuoteHandle ois20yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois20yQuote));
            QuantLib.QuoteHandle ois25yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois25yQuote));
            QuantLib.QuoteHandle ois30yRate = new QuoteHandle(new QuantLib.SimpleQuote(ois30yQuote));

            QuantLib.RateHelper ois1w = new OISRateHelper(2, new Period(1, TimeUnit.Weeks), ois1wRate, Eonia);
            QuantLib.RateHelper ois2w = new OISRateHelper(2, new Period(2, TimeUnit.Weeks), ois2wRate, Eonia);
            QuantLib.RateHelper ois3w = new OISRateHelper(2, new Period(3, TimeUnit.Weeks), ois3wRate, Eonia);
            QuantLib.RateHelper ois1m = new OISRateHelper(2, new Period(1, TimeUnit.Months), ois1mRate, Eonia);
            QuantLib.RateHelper ois2m = new OISRateHelper(2, new Period(2, TimeUnit.Months), ois2mRate, Eonia);
            QuantLib.RateHelper ois3m = new OISRateHelper(2, new Period(3, TimeUnit.Months), ois3mRate, Eonia);
            QuantLib.RateHelper ois4m = new OISRateHelper(2, new Period(4, TimeUnit.Months), ois4mRate, Eonia);
            QuantLib.RateHelper ois5m = new OISRateHelper(2, new Period(5, TimeUnit.Months), ois5mRate, Eonia);
            QuantLib.RateHelper ois6m = new OISRateHelper(2, new Period(6, TimeUnit.Months), ois6mRate, Eonia);
            QuantLib.RateHelper ois7m = new OISRateHelper(2, new Period(7, TimeUnit.Months), ois7mRate, Eonia);
            QuantLib.RateHelper ois8m = new OISRateHelper(2, new Period(8, TimeUnit.Months), ois8mRate, Eonia);
            QuantLib.RateHelper ois9m = new OISRateHelper(2, new Period(9, TimeUnit.Months), ois9mRate, Eonia);
            QuantLib.RateHelper ois10m = new OISRateHelper(2, new Period(10, TimeUnit.Months), ois10mRate, Eonia);
            QuantLib.RateHelper ois11m = new OISRateHelper(2, new Period(11, TimeUnit.Months), ois11mRate, Eonia);
            QuantLib.RateHelper ois12m = new OISRateHelper(2, new Period(12, TimeUnit.Months), ois12mRate, Eonia);
            QuantLib.RateHelper ois15m = new OISRateHelper(2, new Period(15, TimeUnit.Months), ois15mRate, Eonia);
            QuantLib.RateHelper ois18m = new OISRateHelper(2, new Period(18, TimeUnit.Months), ois18mRate, Eonia);
            QuantLib.RateHelper ois2y = new OISRateHelper(2, new Period(2, TimeUnit.Years), ois2yRate, Eonia);
            QuantLib.RateHelper ois3y = new OISRateHelper(2, new Period(3, TimeUnit.Years), ois3yRate, Eonia);
            QuantLib.RateHelper ois4y = new OISRateHelper(2, new Period(4, TimeUnit.Years), ois4yRate, Eonia);
            QuantLib.RateHelper ois5y = new OISRateHelper(2, new Period(5, TimeUnit.Years), ois5yRate, Eonia);
            QuantLib.RateHelper ois6y = new OISRateHelper(2, new Period(6, TimeUnit.Years), ois6yRate, Eonia);
            QuantLib.RateHelper ois7y = new OISRateHelper(2, new Period(7, TimeUnit.Years), ois7yRate, Eonia);
            QuantLib.RateHelper ois8y = new OISRateHelper(2, new Period(8, TimeUnit.Years), ois8yRate, Eonia);
            QuantLib.RateHelper ois9y = new OISRateHelper(2, new Period(9, TimeUnit.Years), ois9yRate, Eonia);
            QuantLib.RateHelper ois10y = new OISRateHelper(2, new Period(10, TimeUnit.Years), ois10yRate, Eonia);
            QuantLib.RateHelper ois11y = new OISRateHelper(2, new Period(11, TimeUnit.Years), ois11yRate, Eonia);
            QuantLib.RateHelper ois12y = new OISRateHelper(2, new Period(12, TimeUnit.Years), ois12yRate, Eonia);
            QuantLib.RateHelper ois15y = new OISRateHelper(2, new Period(15, TimeUnit.Years), ois15yRate, Eonia);
            QuantLib.RateHelper ois20y = new OISRateHelper(2, new Period(20, TimeUnit.Years), ois20yRate, Eonia);
            QuantLib.RateHelper ois25y = new OISRateHelper(2, new Period(25, TimeUnit.Years), ois25yRate, Eonia);
            QuantLib.RateHelper ois30y = new OISRateHelper(2, new Period(30, TimeUnit.Years), ois30yRate, Eonia);

            eoniaHelpers.Add(d01d);
            eoniaHelpers.Add(d11d);
            eoniaHelpers.Add(ois1w);
            eoniaHelpers.Add(ois2w);
            eoniaHelpers.Add(ois3w);
            eoniaHelpers.Add(ois1m);
            eoniaHelpers.Add(ois2m);
            eoniaHelpers.Add(ois3m);
            eoniaHelpers.Add(ois4m);
            eoniaHelpers.Add(ois5m);
            eoniaHelpers.Add(ois6m);
            eoniaHelpers.Add(ois7m);
            eoniaHelpers.Add(ois8m);
            eoniaHelpers.Add(ois9m);
            eoniaHelpers.Add(ois10m);
            eoniaHelpers.Add(ois11m);
            eoniaHelpers.Add(ois12m);
            eoniaHelpers.Add(ois15m);
            eoniaHelpers.Add(ois18m);
            eoniaHelpers.Add(ois2y);
            eoniaHelpers.Add(ois3y);
            eoniaHelpers.Add(ois4y);
            eoniaHelpers.Add(ois5y);
            eoniaHelpers.Add(ois6y);
            eoniaHelpers.Add(ois7y);
            eoniaHelpers.Add(ois8y);
            eoniaHelpers.Add(ois9y);
            eoniaHelpers.Add(ois10y);
            eoniaHelpers.Add(ois11y);
            eoniaHelpers.Add(ois12y);
            eoniaHelpers.Add(ois15y);
            eoniaHelpers.Add(ois20y);
            eoniaHelpers.Add(ois25y);
            eoniaHelpers.Add(ois30y);
            
            QuantLib.YieldTermStructure eoniaTS = new QuantLib.PiecewiseFlatForward(settlementDate, eoniaHelpers, new Actual365Fixed());

            dd1m = eoniaTS.discount(calendar.advance(settlementDate, new QuantLib.Period(1, QuantLib.TimeUnit.Months)));
            dd3m = eoniaTS.discount(calendar.advance(settlementDate, new QuantLib.Period(3, QuantLib.TimeUnit.Months)));

            // OIS Swap back out from curve
            QuantLib.Date tDate = Eonia.fixingCalendar().advance(settlementDate, new Period(2, TimeUnit.Years));
            QuantLib.Schedule sch = new QuantLib.Schedule(settlementDate, tDate, new Period(QuantLib.Frequency.Annual), Eonia.fixingCalendar(),
                QuantLib.BusinessDayConvention.ModifiedFollowing, BusinessDayConvention.ModifiedFollowing, DateGeneration.Rule.Forward, true);
            QuantLib.OvernightIndexedSwap oiswap = new QuantLib.OvernightIndexedSwap(_OvernightIndexedSwap.Type.Payer, 1000.0, sch, 0.01, Eonia.dayCounter(), Eonia);

            eoniaTermStructure.linkTo(eoniaTS);
            QuantLib.PricingEngine engine = new QuantLib.DiscountingSwapEngine(eoniaTermStructure);
            oiswap.setPricingEngine(engine);
            
            double recoveredRate = oiswap.fairRate();
            Console.WriteLine("2Y OIS quote is : " + ois2yQuote + " and fair rate recovered from curve is " + recoveredRate);


            ///*************************************** Libor Curve with OIS discounting ***************************************************//
            QuantLib.RateHelper s2y_ois = new QuantLib.SwapRateHelper(s2yRate, new QuantLib.Period(2, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex, spreadHandle, new Period(0, TimeUnit.Days), eoniaTermStructure);
            QuantLib.RateHelper s3y_ois = new QuantLib.SwapRateHelper(s3yRate, new QuantLib.Period(3, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex, spreadHandle, new Period(0, TimeUnit.Days), eoniaTermStructure);
            QuantLib.RateHelper s5y_ois = new QuantLib.SwapRateHelper(s5yRate, new QuantLib.Period(5, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex, spreadHandle, new Period(0, TimeUnit.Days), eoniaTermStructure);
            QuantLib.RateHelper s10y_ois = new QuantLib.SwapRateHelper(s10yRate, new QuantLib.Period(10, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex, spreadHandle, new Period(0, TimeUnit.Days), eoniaTermStructure);
            QuantLib.RateHelper s15y_ois = new QuantLib.SwapRateHelper(s15yRate, new QuantLib.Period(15, QuantLib.TimeUnit.Years),
                calendar, swFixedLegFrequency, swFixedLegConvention, swFixedLegDayCounter, swFloatingLegIndex, spreadHandle, new Period(0, TimeUnit.Days), eoniaTermStructure);

            // A depo-swap curve
            QuantLib.RateHelperVector depoSwapInstruments_OIS = new QuantLib.RateHelperVector();
            depoSwapInstruments_OIS.Add(d1w);
            depoSwapInstruments_OIS.Add(d1m);
            depoSwapInstruments_OIS.Add(d3m);
            depoSwapInstruments_OIS.Add(d6m);
            depoSwapInstruments_OIS.Add(d9m);
            depoSwapInstruments_OIS.Add(d1y);
            depoSwapInstruments_OIS.Add(s2y_ois);
            depoSwapInstruments_OIS.Add(s3y_ois);
            depoSwapInstruments_OIS.Add(s5y_ois);
            depoSwapInstruments_OIS.Add(s10y_ois);
            depoSwapInstruments_OIS.Add(s15y_ois);

            QuantLib.YieldTermStructure depoSwapTermStructure_OIS = new QuantLib.PiecewiseFlatForward(settlementDate,
                depoSwapInstruments_OIS, termStructureDayCounter);

            double dd1m_ois = depoSwapTermStructure_OIS.discount(calendar.advance(settlementDate, new QuantLib.Period(1, QuantLib.TimeUnit.Months)));
            double dd3m_ois = depoSwapTermStructure_OIS.discount(calendar.advance(settlementDate, new QuantLib.Period(3, QuantLib.TimeUnit.Months)));
            double dd6m_ois = depoSwapTermStructure_OIS.discount(calendar.advance(settlementDate, new QuantLib.Period(6, QuantLib.TimeUnit.Months)));
            double dd1y_ois = depoSwapTermStructure_OIS.discount(calendar.advance(settlementDate, new QuantLib.Period(12, QuantLib.TimeUnit.Months)));
            double dd2y_ois = depoSwapTermStructure_OIS.discount(calendar.advance(settlementDate, new QuantLib.Period(24, QuantLib.TimeUnit.Months)));

            Console.WriteLine("disount 1m under libor is :" + dd1m + " under ois is: " + dd1m_ois);
            Console.WriteLine("disount 3m under libor is :" + dd3m + " under ois is: " + dd3m_ois);
            Console.WriteLine("disount 6m under libor is :" + dd6m + " under ois is: " + dd6m_ois);
            Console.WriteLine("disount 1y under libor is :" + dd1y + " under ois is: " + dd1y_ois);
            Console.WriteLine("disount 2y under libor is :" + dd2y + " under ois is: " + dd2y_ois);

            discountingTermStructure.linkTo(depoSwapTermStructure_OIS);
            NPV = spot5YearSwap.NPV();
            fairSpread = spot5YearSwap.fairSpread();
            fairRate = spot5YearSwap.fairRate();
            Console.WriteLine("Fair rate is " + fairRate + " 5y quote is : " + s5yQuote);


            ///*************************************** OIS discounting with FF Basis Swap ***************************************************//
            QuantLib.RateHelperVector eoniaHelpers2 = new RateHelperVector();

            QuantLib.QuoteHandle basis2y = new QuoteHandle(new SimpleQuote(0.0025));
            QuantLib.QuoteHandle basis3y = new QuoteHandle(new SimpleQuote(0.0025));
            QuantLib.QuoteHandle basis5y = new QuoteHandle(new SimpleQuote(0.0025));
            QuantLib.QuoteHandle basis10y = new QuoteHandle(new SimpleQuote(0.0025));
            QuantLib.QuoteHandle basis15y = new QuoteHandle(new SimpleQuote(0.0025));

            QuantLib.RateHelper ffbs2y = new FedFundBasisSwapRateHelper(2, new Period(3, TimeUnit.Years), s2yRate, basis2y, Eonia);
            QuantLib.RateHelper ffbs3y = new FedFundBasisSwapRateHelper(2, new Period(3, TimeUnit.Years), s3yRate, basis3y, Eonia);
            QuantLib.RateHelper ffbs5y = new FedFundBasisSwapRateHelper(2, new Period(3, TimeUnit.Years), s5yRate, basis5y, Eonia);
            QuantLib.RateHelper ffbs10y = new FedFundBasisSwapRateHelper(2, new Period(3, TimeUnit.Years), s10yRate, basis10y, Eonia);
            QuantLib.RateHelper ffbs15y = new FedFundBasisSwapRateHelper(2, new Period(3, TimeUnit.Years), s15yRate, basis15y, Eonia);

            eoniaHelpers2.Add(d01d);
            eoniaHelpers2.Add(ois1w);
            eoniaHelpers2.Add(ois3w);            
            eoniaHelpers2.Add(ois6m);            
            eoniaHelpers2.Add(ois12m);
            eoniaHelpers2.Add(ois15m);
            eoniaHelpers2.Add(ois18m);
            eoniaHelpers2.Add(ffbs2y);
            eoniaHelpers2.Add(ffbs3y);
            eoniaHelpers2.Add(ffbs5y);
            eoniaHelpers2.Add(ffbs10y);
            eoniaHelpers2.Add(ffbs15y);

            QuantLib.YieldTermStructure eoniaTS2 = new QuantLib.PiecewiseFlatForward(settlementDate, eoniaHelpers2, new Actual365Fixed());

            double dd1m2 = eoniaTS.discount(calendar.advance(settlementDate, new QuantLib.Period(1, QuantLib.TimeUnit.Months)));
            double dd3m2 = eoniaTS.discount(calendar.advance(settlementDate, new QuantLib.Period(3, QuantLib.TimeUnit.Months)));

            eoniaTermStructure.linkTo(eoniaTS);
            recoveredRate = oiswap.fairRate();
            Console.WriteLine("2Y OIS quote is : " + ois2yQuote + " and fair rate recovered from curve is " + recoveredRate);

        }
    }
}
