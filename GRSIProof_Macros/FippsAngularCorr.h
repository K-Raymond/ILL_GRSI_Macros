//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct 25 13:18:27 2016 by ROOT version 5.34/24
// from TTree FragmentTree/FragmentTree
// found on file: fragment07844_000.root
//////////////////////////////////////////////////////////

#ifndef FippsAngularCorr_h
#define FippsAngularCorr_h

#include "TChain.h"
#include "TFile.h"

#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"

// Header file for the classes stored in the TTree if any.
#include "TGRSISelector.h"

#include "TFipps.h"
#include "TFippsBgo.h"
#include "TFippsLaBr.h"
#include "TFippsTAC.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class FippsAngularCorr : public TGRSISelector { // Must be same name as .C and .h

public:
   TFipps *    fFipps     = nullptr; // Pointers to spot that events will be
   TFippsBgo * fFippsBgo  = nullptr;
   TFippsLaBr *fFippsLaBr = nullptr;
   TFippsTAC * fFippsTAC  = nullptr;

   bool fhasFipps     = false;
   bool fhasFippsBgo  = false;
   bool fhasFippsLaBr = false;
   bool fhasFippsTAC  = false;

   //*** TIMING PARAMETERS ***//
   Double_t fggPrompt         = 500.;                         // ns
   Double_t fggBackgroundLow  = 1000.;                        // ns
   Double_t fggBackgroundHigh = fggBackgroundLow + fggPrompt; // ns
   Double_t fggEventMixed     = 2000.; // ns

   //*** HISTOGRAM PARAMTERS ***//
   Double_t fXMin             = 0.;
   Double_t fXMax             = 10000.;
   int fEnergyBins            = 30000;
   Double_t fAngleMin         = 0.0;
   Double_t fAngleMax         = 180.0;
   int fAngleBins             = 360;
   Int_t ggaBins[3] = {fAngleBins, fEnergyBins, fEnergyBins};
   Double_t ggaXMin[3] = {fAngleMin, fXMin, fXMin};
   Double_t ggaXMax[3] = {fAngleMax, fXMax, fXMax};

   bool IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2);
   bool IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2, TDetectorHit *Hit3);
   bool IsCoincidenceBackground(TDetectorHit *Hit1, TDetectorHit *Hit2);
   bool IsCoincidenceBackground(TDetectorHit *Hit1, TDetectorHit *Hit2, TDetectorHit *Hit3) { return false; };
   bool IsEventMixed(TDetectorHit* Hit1, TDetectorHit* Hit2);

   Double_t GetAngle(TDetectorHit* Hit1, TDetectorHit* Hit2);

   FippsAngularCorr(TTree * /*tree*/ = 0) : TGRSISelector()
   {
      SetOutputPrefix("FippsAngluarCorr_"); // Changes prefix of output file
   }
   // These functions are expected to exist
   virtual ~FippsAngularCorr() {}
   virtual Int_t Version() const { return 2; }
   void          CreateHistograms();
   void          FillHistograms();
   void InitializeBranches(TTree *tree);
   void EndOfSort();

   //** HISTOGRAM FILLING FUNCTIONS **//
   void FillGammaHistograms();
   void FillAddbackHistograms();
   void FillSuppressedHistograms();
   void FillSuppressedAddbackHistograms();

   ClassDef(FippsAngularCorr, 2); // Makes ROOT happier
};

#endif

#ifdef FippsAngularCorr_cxx
void FippsAngularCorr::InitializeBranches(TTree *tree)
{
   if (!tree) return;
   if (tree->SetBranchAddress("TFipps", &fFipps) == TTree::kMissingBranch) {
      fFipps = new TFipps;
   } else {
      fhasFipps = true;
   }
   if (tree->SetBranchAddress("TFippsBgo", &fFippsBgo) == TTree::kMissingBranch) {
      fFippsBgo = new TFippsBgo;
   } else {
      fhasFippsBgo = true;
   }
   if (tree->SetBranchAddress("TFippsLaBr", &fFippsLaBr) == TTree::kMissingBranch) {
      fFippsLaBr = new TFippsLaBr;
   } else {
      fhasFippsLaBr = true;
   }
   if (tree->SetBranchAddress("TFippsTAC", &fFippsLaBr) == TTree::kMissingBranch) {
      fFippsTAC = new TFippsTAC;
   } else {
      fhasFippsTAC = true;
   }
}

#endif // #ifdef FippsAngularCorr_cxx
