# Source Classes/Namespaces
The classes and namespaces contained in this directory can be `#include`d in other scripts to reduce duplicated code, and to make it easy to test classes.
Below are breif explinations as to what each class/file does. See the individual files for more details.

## Residuals
Used for organizesing multiple `TGraphs` which measure the systematics of a digitizer channel.
Contains three basic functions to read, write, and merge `TGraphs` which are contained in particular `.root` files.
This is useful for using transitions from different calibration sources (or well known transitions from the experimental nucleus).

## TAngCorrIndexTranslator
Translates between an angular index and an angle. Use by creating an instantiation and then calling ether `AngleToIndex` or `IndexToAngle`.
