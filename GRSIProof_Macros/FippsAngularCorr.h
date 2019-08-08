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
#include "TVector3.h"
#include "TVector3.h"

#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"

// Header file for the classes stored in the TTree if any.
#include "TGRSISelector.h"

#include "TFipps.h"
#include "TFippsBgo.h"

#include <algorithm>
#include <vector>


class TAngCorrIndexTranslator {
   // Angles are stored in degrees sorted from least angle to highest angle (0.0 -> 180.0)
   vector<double_t> fIndexToAngleMap;
   // Stores the number of possible combinations with respect to index 
   vector<int> fIndexToCombinationMap;
   TFipps* fDetectorClass = new TFipps();

   void MakeIndexToAngleMap();
   void MakeCombinationMap();
   double_t RadToDegree( double_t radAngle ) { return TMath::RadToDeg()*radAngle; }

   public:
   TAngCorrIndexTranslator();
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

class FippsAngularCorr : public TGRSISelector { // Must be same name as .C and .h

public:
   TFipps *    fFipps     = nullptr; // Pointers to spot that events will be
   TFippsBgo * fFippsBgo  = nullptr;

   TAngCorrIndexTranslator fAngCorrTrans;

   bool fhasFipps     = false;
   bool fhasFippsBgo  = false;

   //*** TIMING PARAMETERS ***//
   Double_t fggPrompt         = 280.;                         // ns
   Double_t fggBackgroundLow  = 1000.;                        // ns
   Double_t fggBackgroundHigh = fggBackgroundLow + fggPrompt; // ns
   Double_t fggEventMixed     = 2000.; // ns

   //*** HISTOGRAM PARAMTERS ***//
   Double_t fXMin             = 0.;
   Double_t fXMax             = 10000.;
   int fEnergyBins            = 20000;
   Int_t fggBins[3] = {fEnergyBins, fEnergyBins};
   Double_t fggXMin[3] = {fXMin, fXMin};
   Double_t fggXMax[3] = {fXMax, fXMax};

   bool IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2);
   bool IsCoincidencePrompt(TDetectorHit *Hit1, TDetectorHit *Hit2, TDetectorHit *Hit3);
   bool IsCoincidenceBackground(TDetectorHit *Hit1, TDetectorHit *Hit2);
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
   }
}

#endif // #ifdef FippsAngularCorr_cxx
