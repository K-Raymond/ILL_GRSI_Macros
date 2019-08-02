#define FippsAngularCorr_cxx
// The class definition in FippsAngularCorr.h has been generated automatically
#include "FippsAngularCorr.h"

void FippsAngularCorr::CreateHistograms()
{
   // Define Histograms
   fH1["gEM"]  = new TH1D("gEM", "Event-Mixed per angles", fAngleBins, fAngleMin, fAngleMax);

   fHSparse["ggaP"]    = new THnSparseF("ggaP", "Prompt #gamma-#gamma-angle", 3, ggaBins, ggaXMin, ggaXMax);
   fHSparse["ggaEM"]    = new THnSparseF("ggaEM", "EventMixed #gamma-#gamma-angle", 3, ggaBins, ggaXMin, ggaXMax);

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
   return (180/TMath::Pi())*(Hit1->GetPosition().Angle(Hit2->GetPosition()));
}

// ** TIMING FUNCTIONS ** //
bool FippsAngularCorr::IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2)
{
   // Check if hits are less then 500 ns apart.
   return std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt;
}

bool FippsAngularCorr::IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2, TDetectorHit *Hit3)
{
   // Check kif hits are less than 500 ns apart.
   return std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt &&
          std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt &&
          std::fabs(Hit2->GetTime() - Hit3->GetTime()) < fggPrompt;
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
   for(auto i = 0; i < fFipps->GetMultiplicity(); ++i) {
      auto Fipps1 = fFipps->GetFippsHit(i);
      for(auto j = 0; j < fFipps->GetMultiplicity(); ++j) {
         if( i == j) continue;
         auto Fipps2 = fFipps->GetFippsHit(j);
         Double_t RelativeAngle = GetAngle(Fipps1, Fipps2);
         Double_t SparseInput[3] = {RelativeAngle, Fipps1->GetEnergy(), Fipps2->GetEnergy()};
         if(IsCoincidencePrompt(Fipps1, Fipps2)) {
            fHSparse.at("ggaP")->Fill(SparseInput);
         }
         if( IsEventMixed(Fipps1, Fipps2) ) {
            fH1.at("gEM")->Fill(RelativeAngle);
            fHSparse.at("ggaEM")->Fill(SparseInput);
         }
      }
   }

   return;
}

void FippsAngularCorr::EndOfSort()
{
   return;
}

