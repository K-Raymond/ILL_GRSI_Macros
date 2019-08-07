#define TAngCorrIndexTranslator_cxx
// The class definition in FippsAngularCorr.h has been generated automatically
#include "TAngCorrIndexTranslator.h"

TAngCorrIndexTranslator::TAngCorrIndexTranslator()
{
   MakeIndexToAngleMap();
   MakeCombinationMap();
}

TAngCorrIndexTranslator::~TAngCorrIndexTranslator()
{
   delete fDetectorClass;
}

void TAngCorrIndexTranslator::MakeIndexToAngleMap()
{
   bool isNewAngle = true;
   // Loop over clover 1
   for( size_t DetectorIndex1 = 1 ; DetectorIndex1 <= 16; DetectorIndex1++ ) {
      for( size_t CrystalIndex1 = 0; CrystalIndex1 < 4; CrystalIndex1++ ) {
         for( size_t DetectorIndex2 = 1 ; DetectorIndex2 <= 16; DetectorIndex2++ ) {
            // Loop over clover 2
            for( size_t CrystalIndex2 = 0; CrystalIndex2 < 4; CrystalIndex2++ ) {
               isNewAngle = true; // assume angle combination is new
               TVector3 Pos1 = TFipps::GetPosition(DetectorIndex1, CrystalIndex1, 90.0);
               TVector3 Pos2 = TFipps::GetPosition(DetectorIndex2, CrystalIndex2, 90.0);
               double_t RelAngle = TMath::RadToDeg()*Pos1.Angle(Pos2);
               for( size_t i = 0; i < fIndexToAngleMap.size(); i++) {
                  if( isNewAngle == false ) continue;
                  if( abs( RelAngle - fIndexToAngleMap[i] ) < 0.0001 )
                     isNewAngle = false;
               }
               if( isNewAngle == true )
                  fIndexToAngleMap.push_back(RelAngle);
            }
         }
      }
   }
   // Sort order from least to most
   std::sort(fIndexToAngleMap.begin(), fIndexToAngleMap.end());
   std::cout << fIndexToAngleMap.size() << " unique angles found" << std::endl;
}

void TAngCorrIndexTranslator::MakeCombinationMap()
{
   // Make the combination and angle maps the same size
   fIndexToCombinationMap.resize(fIndexToAngleMap.size());
   for( size_t DetectorIndex1 = 1 ; DetectorIndex1 <= 16; DetectorIndex1++ )
      for( size_t DetectorIndex2 = 1 ; DetectorIndex2 <= 16; DetectorIndex2++ )
         for( size_t CrystalIndex1 = 0; CrystalIndex1 < 4; CrystalIndex1++ )
            for( size_t CrystalIndex2 = 0; CrystalIndex2 < 4; CrystalIndex2++ ) {
               bool isNewAngle = true; // assume angle combination is new
               TVector3 Pos1 = TFipps::GetPosition(DetectorIndex1, CrystalIndex1, 90); // d to target 9cm
               TVector3 Pos2 = TFipps::GetPosition(DetectorIndex2, CrystalIndex2, 90);
               double_t RelAngle = TMath::RadToDeg()*Pos1.Angle(Pos2);
               fIndexToCombinationMap[AngleToIndex(RelAngle)] += 1; // incriment combination of angle
            }
   for( size_t i = 0; i < fIndexToCombinationMap.size(); i++ ) {
      std::cout << "Angle: " << fIndexToAngleMap[i] << ", ";
      std::cout << "Counts: " << fIndexToCombinationMap[i] << std::endl;
   }
}

int TAngCorrIndexTranslator::AngleToIndex(double_t angle)
{
   auto it = std::find_if(fIndexToAngleMap.begin(), fIndexToAngleMap.end(),
         [angle](double_t b) { return abs(angle - b) < 0.0001; } );
   if( it == fIndexToAngleMap.end() )
      return 0;
   return std::distance(fIndexToAngleMap.begin(), it);
}

double_t TAngCorrIndexTranslator::IndexToAngle( int i )
{
   if( i >= 0 && i < static_cast<int>(fIndexToAngleMap.size()) )
      return fIndexToAngleMap[i];
   return -1.0; // not within bounds
}

int TAngCorrIndexTranslator::GetCombinationOfIndex( int i )
{
   if( i >= 0 && i < static_cast<int>(fIndexToCombinationMap.size()) )
      return fIndexToCombinationMap[i];
   return 0;
}

int TAngCorrIndexTranslator::GetCombinationOfAngle( double_t angle )
{
   return fIndexToCombinationMap[AngleToIndex(angle)];
}
