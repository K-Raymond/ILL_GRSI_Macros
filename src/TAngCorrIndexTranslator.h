
#ifndef TAngCorrIndexTranslator_h
#define TAngCorrIndexTranslator_h

#include "TVector3.h"
#include "TVector3.h"

#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"

#include <algorithm>
#include <vector>
#include <string>

#include "TFipps.h"


class TAngCorrIndexTranslator {
   // Angles are stored in degrees sorted from least angle to highest angle (0.0 -> 180.0)
   vector<double_t> fIndexToAngleMap;
   // Stores the number of possible combinations with respect to index 
   vector<int> fIndexToCombinationMap;
   // Stores whether the detector exists in a particular experiment
   // Mapped by DetectorIndex*4+CrystalIndex
   vector<bool> fHasDetectorVec;
   TFipps* fDetectorClass = new TFipps();

   void MakeIndexToAngleMap();
   void MakeCombinationMap();
   double_t RadToDegree( double_t radAngle ) { return TMath::RadToDeg()*radAngle; }

   public:
   TAngCorrIndexTranslator();
   TAngCorrIndexTranslator(std::string InCalFile);
   ~TAngCorrIndexTranslator();

   // Managing translations
   int AngleToIndex(double_t angle);
   double_t IndexToAngle( int i );
   size_t GetNumberOfUniqueAngles() { return fIndexToAngleMap.size(); }

   // Get Raw angle combinations
   int GetCombinationOfIndex( int i );
   int GetCombinationOfAngle( double_t angle);
};
// Fixed size dimensions of array or collections stored in the TTree if any.

#endif // #ifdef TAngCorrIndexTranslator_cxx
