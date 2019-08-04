#define FippsAngularCorr_cxx
// The class definition in FippsAngularCorr.h has been generated automatically
#include "FippsAngularCorr.h"

void FippsAngularCorr::CreateHistograms()
{
   fH1["gEM"] = new TH1D("gEM", "Event-Mixed", 360, 0, 180);
   fH1["gEM_Index"] = new TH1D("gEM_Index", "Event-Mixed by Index", fAngCorrTrans.GetNumberOfUniqueAngles(),
         0, (double_t) fAngCorrTrans.GetNumberOfUniqueAngles());

   // Make a prompt and event mixed gamma-gamma matrix for each unique angle
   for( size_t i = 0; i < fAngCorrTrans.GetNumberOfUniqueAngles(); i++ ) {
      double_t CurrentAngle = fAngCorrTrans.IndexToAngle(i);
      fHSparse[Form("ggP_%zu", i)] = new THnSparseF(Form("ggP_%zu", i),
            Form("Prompt #gamma-#gamma for angle %0.1f\370", CurrentAngle), 2, fggBins, fggXMin, fggXMax);
      fHSparse[Form("ggEM_%zu", i)] = new THnSparseF(Form("ggEM_%zu", i),
            Form("Prompt #gamma-#gamma for angle %0.1f\370", CurrentAngle), 2, fggBins, fggXMin, fggXMax);
   }

   // Send histograms to Output list to be added and written.
   for (auto it : fH1) {
      GetOutputList()->Add(it.second);
   }
   for (auto it : fH2) {
      GetOutputList()->Add(it.second);
   }
   for (auto it : fHSparse) {
      GetOutputList()->Add(it.second);
   }
}

Double_t FippsAngularCorr::GetAngle(TDetectorHit* Hit1, TDetectorHit* Hit2)
{
   TVector3 Vec1 = TFipps::GetPosition(Hit1->GetDetector(), Hit1->GetCrystal(), 90);
   TVector3 Vec2 = TFipps::GetPosition(Hit2->GetDetector(), Hit2->GetCrystal(), 90);
   return TMath::RadToDeg()*(Vec1.Angle(Vec2));
}

// ** TIMING FUNCTIONS ** //
bool FippsAngularCorr::IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2)
{
   // Check if hits are less then 500 ns apart.
   return std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt;
}

bool FippsAngularCorr::IsCoincidenceBackground(TDetectorHit *Hit1, TDetectorHit *Hit2)
{
   return std::fabs(Hit1->GetTime() - Hit2->GetTime()) > fggBackgroundLow &&
          std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggBackgroundHigh;
}

bool FippsAngularCorr::IsEventMixed(TDetectorHit* Hit1, TDetectorHit* Hit2)
{
   return std::fabs(Hit1->GetTime() - Hit2->GetTime()) > fggEventMixed;
}

// ** END TIMING FUNCTIONS ** //

// Master Histogram making function
void FippsAngularCorr::FillHistograms()
{
   // loop over all events
   for(auto i = 0; i < fFipps->GetMultiplicity(); ++i) {
      auto Fipps1 = fFipps->GetFippsHit(i);
      for(auto j = 0; j < fFipps->GetMultiplicity(); ++j) {
         if( i == j) continue;
         auto Fipps2 = fFipps->GetFippsHit(j);
         Double_t Angle = GetAngle(Fipps1, Fipps2);
         fH1.at("gEM")->Fill(Angle);
         int AngleIndex = fAngCorrTrans.AngleToIndex(Angle);
         fH1.at("gEM_Index")->Fill(AngleIndex);
         Double_t SparseInput[2] = {Fipps1->GetEnergy(), Fipps2->GetEnergy()};

         // Fill the matrix which corrisponds to which angle
         if(IsCoincidencePrompt(Fipps1, Fipps2)) {
            fHSparse.at(Form("ggP_%i", AngleIndex))->Fill(SparseInput);
         }
         if( IsEventMixed(Fipps1, Fipps2) ) {
            fHSparse.at(Form("ggEM_%i", AngleIndex))->Fill(SparseInput);
         }
      }
   }

   return;
}

void FippsAngularCorr::EndOfSort()
{
   return;
}


//** TAngCorrIndexTranslator Functions **//

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
