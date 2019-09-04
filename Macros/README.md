# Macros for Analysis
All macros are intended to be compiled before use.
In an interactive session run:

`.L <MACRO_NAME>.C++`

## Information for each macro
A brief introduction for each macro is below. See specific files for details.
#### LinearGainMatch
Gain match a channel according to two peaks.
This is good for roughly calibrating each spectra, and making sure every peak is agrees within 1-3 keV of literature.
A good first step of any analysis is to linearly calibrate on two peaks below 2 MeV.
Gain matches each channel to eachother.
#### EnergyCalibration
Provides a linear gain match using information stored in the `TNucleous` format.
This allows all peaks to be more accurately placed.
#### AlignRun
Align the spectra from one run to another.
Using a properly calibrated AnalysisTree, use `GenerateCalibrationHistogramFromTree` to generate a "perfect" spectra to align all other runs to.
Then run `AlignFiles` to align a set of files to the calibration histogram.
#### ProcessAngularCorr
Process the data from the angular correlation selector.
Outputs a `TGraph` of counts vs angle, and `TCanvas`s showing each individual fit.
