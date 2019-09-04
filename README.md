# ILL_GRSI_Macros
Macros for use of analyizing ILL data using GRSISort v4.0.
GRSISort can be found [here](https://github.com/GRIFFINCollaboration/GRSISort).
A general guide on how to analyze data using GRSISort can be found in the GRSISort wiki through the above link, or directly through the useful links section of this README.

## Directory
Below are basic descriptions of each folder.

**CalFiles**: Contains example `.cal` files used to identify different types of detectors.

**RunInfo**: Contains example `.info` files used when sorting `.lst` files with GRSISort.

**SourceData**: Contains transition data from (n,γ) sources, or other nuclei relevent to expierments and wished to be used with the `TNucleous` class.

**Macros**: Special macros for analyizing AnalysisTrees.
These macros are intended to perform corrections to data and analyize final spectra of a experiment.
Examples include energy calibration, gain matching, and analyizing angular correlations.

**GRSIProof_Macros**: Contains macros used with GRSIProof (a variation of Root's regular PROOF and selectors).
These macros usually produce matricies for evauluating data, or analyzing particular aspects of an experiment.

**Src**: Reusable source code is contained here.
In particular, code which is intended to be shared between a macro used with GRSIProof and regular macros

## Useful Links

[Compiling Root Macros](https://root.cern.ch/compiling-macros)

[GRSISort Wiki](https://github.com/GRIFFINCollaboration/GRSISort/wiki)
