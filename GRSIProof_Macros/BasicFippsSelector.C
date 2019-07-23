#define BasicFippsSelector_cxx
// The class definition in BasicFippsSelector.h has been generated automatically
#include "BasicFippsSelector.h"

void BasicFippsSelector::CreateHistograms() {
  // Define Histograms
  fH1["gE"] = new TH1D("gE", "#gamma Singles", (Int_t)4*fXMax, 0, fXMax);
  fH1["aE"] = new TH1D("aE", "Addback Singles", (Int_t)4*fXMax, 0, fXMax);
  fH1["gsE"] = new TH1D("gsE", "suppressed #gamma Singles", (Int_t)4*fXMax, 0, fXMax);
  fH1["asE"] = new TH1D("asE", "suppressed Addback Singles", (Int_t)4*fXMax, 0, fXMax);

  Int_t ggBins[2] = {10000, 10000};
  Double_t ggXMin[2] = {0., 0.};
  Double_t ggXMax[2] = {10000., 10000.};
  fHSparse["ggE"] = new THnSparseF("ggE", "#gamma-#gamma Coincidence", 2,
                                   ggBins, ggXMin, ggXMax);
  fHSparse["aaE"] = new THnSparseF("aaE", "#Addback-Addback Coincidence", 2,
                                   ggBins, ggXMin, ggXMax);
  fHSparse["ggEb"] = new THnSparseF("ggEb", "#gamma-#gamma Background Coincidence", 2,
          ggBins, ggXMin, ggXMax);

  Int_t gggBins[3] = {10000, 10000, 10000};
  Double_t gggXMin[3] = {0., 0., 0.};
  Double_t gggXMax[3] = {10000., 10000., 10000.};
  fHSparse["gggE"] = new THnSparseF("gggE", "#gamma-#gamma-#gamma Coincidence",
                                    3, gggBins, gggXMin, gggXMax);
  fHSparse["aaaE"] = new THnSparseF("aaE", "Addback Coincidence Cube", 3,
                                    gggBins, gggXMin, gggXMax);

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

// ** TIMING FUNCTIONS ** //
bool BasicFippsSelector::IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2) {
  // Check if hits are less then 500 ns apart.
  return std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt;
}

bool BasicFippsSelector::IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2, TDetectorHit *Hit3) {
  // Check kif hits are less than 500 ns apart.
  return std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt &&
         std::fabs(Hit1->GetTime() - Hit2->GetTime()) < fggPrompt &&
         std::fabs(Hit2->GetTime() - Hit3->GetTime()) < fggPrompt;
}

bool BasicFippsSelector::IsCoincidenceBackground(TDetectorHit* Hit1, TDetectorHit* Hit2) {
    return std::fabs(Hit1->GetTime() - Hit2->GetTime()) > fggBackgroundLow &&
        std::fabs(Hit1->GetTime() - Hit2->GetTime() ) < fggBackgroundHigh;
}

// ** END TIMING FUNCTIONS ** //

// ** HISTOGRAM MAKING FUNCTIONS ** //
void BasicFippsSelector::FillGammaHistograms() {
  // Loop over all Fipps Hits
  for (auto i = 0; i < fFipps->GetMultiplicity(); ++i) {
    auto Fipps1 = fFipps->GetFippsHit(i);
    fH1.at("gE")->Fill(Fipps1->GetEnergy());
    // second loop over all Fipps Hits
    for (auto j = 0; j < fFipps->GetMultiplicity(); ++j) {
      if (i == j) continue;
      auto Fipps2 = fFipps->GetFippsHit(j);
      if (IsCoincidencePrompt(Fipps1, Fipps2)) {
        Double_t ggEnergy[2] = {Fipps1->GetEnergy(), Fipps2->GetEnergy()};
        fHSparse.at("ggE")->Fill(ggEnergy);
      }

      if( IsCoincidenceBackground( Fipps1, Fipps2 ) ) {
        Double_t ggEnergy[2] = {Fipps1->GetEnergy(), Fipps2->GetEnergy()};
        fHSparse.at("ggEb")->Fill(ggEnergy);
      }
      // third loop over all Fipps Hits
      for (auto k = 0; k < fFipps->GetMultiplicity(); ++k) {
        if (i == k || j == k) continue;
        auto Fipps3 = fFipps->GetFippsHit(k);
        if (IsCoincidencePrompt(Fipps1, Fipps2, Fipps3)) {
          Double_t gggEnergy[3] = {Fipps1->GetEnergy(), Fipps2->GetEnergy(),
                                   Fipps3->GetEnergy()};
          fHSparse.at("gggE")->Fill(gggEnergy);
        }
      }
    }
  }
}

void BasicFippsSelector::FillAddbackHistograms() {
  // Loop over all addback Fipps Hits
  for (auto i = 0; i < fFipps->GetAddbackMultiplicity(); ++i) {
    auto Fipps1 = fFipps->GetAddbackHit(i);
    fH1.at("aE")->Fill(Fipps1->GetEnergy());
    for( auto j = 0; j < fFipps->GetAddbackMultiplicity(); ++j) {
        if( i == j )
            continue;
        auto Fipps2 = fFipps->GetAddbackHit(j);
        if ( IsCoincidencePrompt(Fipps1, Fipps2) )
        {
            Double_t aaE[2] = {Fipps1->GetEnergy(), Fipps2->GetEnergy()};
            fHSparse.at("aaE")->Fill(aaE);
        }
    }
  }
}

void BasicFippsSelector::FillSuppressedHistograms() {
  // Loop over all suppressed Fipps Hits
  for (auto i = 0; i < fFipps->GetSuppressedMultiplicity(fFippsBgo); ++i) {
    auto Fipps1 = fFipps->GetSuppressedHit(i);
    fH1.at("gsE")->Fill(Fipps1->GetEnergy());
  }
}

void BasicFippsSelector::FillSuppressedAddbackHistograms() {
  // Loop over all suppressed addback Fipps Hits
  for (auto i = 0; i < fFipps->GetSuppressedAddbackMultiplicity(fFippsBgo);
       ++i) {
    auto Fipps1 = fFipps->GetSuppressedAddbackHit(i);
    fH1.at("asE")->Fill(Fipps1->GetEnergy());
  }
}
// ** END HISTOGRAM MAKING FUNCTIONS ** //

// Master Histogram making function
void BasicFippsSelector::FillHistograms() {
  FillGammaHistograms();
  FillAddbackHistograms();
  FillSuppressedHistograms();
  FillSuppressedAddbackHistograms();
}
