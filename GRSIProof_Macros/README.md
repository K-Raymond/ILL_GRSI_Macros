# GRSIPRoof Macros

To compile and run a macro, use the following:

`grsiproof --max-workers=<NUM> <PATH/TO/SELECTOR_MACRO>.C <ANALYSIS_FILE>.root`

### Explanation of each Macro
#### BasicFippsSelector
Creates basic spectra for evaluating various detector systems on FIPPS.
Can also be used to inspect that quality of spectra and calibration of runs.

#### FippsAngularCorr
Creates a 2D-`THnSparse` for each angular index available for both prompt, background, and event-mixed spectra.
See the [following](https://arxiv.org/abs/1807.07570) paper on how to perform the angular correlations.

### More Details on Proof:

[Root Proof](https://root.cern.ch/proof)

[GRSIProof Macros](https://github.com/GRIFFINCollaboration/GRSISort/wiki/GRSIProof)
