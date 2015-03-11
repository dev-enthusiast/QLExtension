using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Data;

using QuantLib;

namespace Test
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnConnect2APrime_Click(object sender, RoutedEventArgs e)
        {
            // DataTable aprimeTable = CEGLib.APrimeDB.GetAPrimeDataForLiquidityStress(new DateTime(2015, 1, 30), new DateTime(2023, 12, 31), "");

            DataTable aprimeTable = CEGLib.SimCube.GetRAFTForLiquidityStress(new DateTime(2015, 1, 30));

            foreach (var row in aprimeTable.AsEnumerable())
            {
                var a = row[0];
                var b = row[1];
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Date asofDate = new Date((int)DateTime.Today.ToOADate());
            double a = 0.0464041, sigma = 0.00579912;

            YieldTermStructure ts = new FlatForward(asofDate, 0.04875825, new Actual365Fixed());
            QuantLib.Calendar calendar = new UnitedStates();

            Date TDate = calendar.advance(asofDate, 7, TimeUnit.Years);
            Schedule fixedschedule = new Schedule(asofDate, TDate, new Period(Frequency.Semiannual), calendar, BusinessDayConvention.ModifiedFollowing, BusinessDayConvention.ModifiedFollowing,
                DateGeneration.Rule.Forward, true);
            Schedule floatschedule = new Schedule(asofDate, TDate, new Period(Frequency.Quarterly), calendar, BusinessDayConvention.ModifiedFollowing, BusinessDayConvention.ModifiedFollowing,
                DateGeneration.Rule.Forward, true);
            RelinkableYieldTermStructureHandle usdForwardingTSHandle = new RelinkableYieldTermStructureHandle(ts);
            IborIndex usd3mIndex = new IborIndex("USDLIB3M", new Period(Frequency.Quarterly), 0, new USDCurrency(), calendar, BusinessDayConvention.ModifiedFollowing, true, new Actual365Fixed(), usdForwardingTSHandle);
            
            VanillaSwap swap = new VanillaSwap(_VanillaSwap.Type.Payer, 1000, fixedschedule, 0.049, new Actual365Fixed(), floatschedule, usd3mIndex, 0.0, new Actual365Fixed());
            DiscountingSwapEngine pricingEngine = new DiscountingSwapEngine(usdForwardingTSHandle);
            swap.setPricingEngine(pricingEngine);
            double rate = swap.fairRate();

            InstrumentVector portfolio = new InstrumentVector();
            portfolio.Add(swap);
            CEGLib.RiskHullWhiteSimulationEngine engine = new CEGLib.RiskHullWhiteSimulationEngine(DateTime.Today, new YieldTermStructureHandle(ts), a, sigma, portfolio, usd3mIndex, usdForwardingTSHandle);
            engine.SampleSize = 300;
            engine.calculate();
            Dictionary<Date, double> ret = engine.getPFECurve(0.95);
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            // CEGLib.SecDb.Interface.Connect("python", "E053608");
            CEGLib.R.RSecDbInterface.SetDatabase();
        }
    }
}
