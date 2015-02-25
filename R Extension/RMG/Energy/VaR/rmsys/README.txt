Packages needed: tcltk2, reshape

Structure of the code:
----------------------
prepare.overnight.VaR.R
|-> get.VCentral.positions.R
|-> aggregate.all.market.states.R
|   |-> get.one.day.prices.R
|   |-> get.corrected.prices.R
|   |-> get.one.day.vols.R
|-> get.price.buddy.curve.R
|-> get.vol.buddy.curve.R

run.overnight.VaR.R
|-> set.parms.overnight.R
|-> main.run.VaR.R
|   |-> aggregate.positions.archive.R
|   |-> utilities.VaR.R
|   |-> make.class.Factors.R
|   |-> make.VaR.report.R
|   |   |-> core.calc.VaR.R
|   |   |-> write.xls.R
|   |-> make.pdf.R
|   |   |-> template.VaR.report.tex

make.VaR.GUI.R
|-> run.interactive.VaR.R
|-> set.parms.interactive.R
|-> main.run.VaR.R
|   |-> aggregate.positions.archive.R
|       aggregate.positons.live.R     
|   |-> utilities.VaR.R
|   |-> make.class.Factors.R
|   |-> make.VaR.report.R
|   |   |-> core.calc.VaR.R
|   |   |-> write.xls.R
|   |-> make.pdf.R
|   |   |-> template.VaR.report.tex
