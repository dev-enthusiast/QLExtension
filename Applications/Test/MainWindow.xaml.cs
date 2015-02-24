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

            DataTable aprimeTable = CEGLib.APrimeDB.GetRAFTForLiquidityStress(new DateTime(2015, 1, 30));

            foreach (var row in aprimeTable.AsEnumerable())
            {
                var a = row[0];
                var b = row[1];
            }
        }
    }
}
