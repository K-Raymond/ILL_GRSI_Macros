#include <string>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"

static TH1* gCombinations;
static THnSparse* gAngCorrMat;

void MakeGraph(Double_t Gamma1, Double_t Gamma2)
{
   gAngCorrMat->GetAxis(2)->SetRangeUser(Gamma1 - 6, Gamma2 + 6);
   gAngCorrMat->GetAxis(1)->SetRangeUser(Gamma1 - 6, Gamma2 + 6);
   TH1* Projection = gAngCorrMat->Projection(0);
   Projection->Sumw2();
   Projection->Divide(gCombinations);
   Projection->Draw();
}  

void ProcessAngularCorr(TFile* InFile)
{
   // normalize all bins according to the counts in the first bin (0.0 degrees)
   printf("Loading the combination histogram...");
   gCombinations = (TH1*) InFile->Get("gR");
   if( gCombinations == nullptr ) return;
   printf(" Done!\n");
   gCombinations->Scale( 1/gCombinations->GetBinContent(gCombinations->FindBin(0.0)) );

   printf("Loading the angular correlation matrix...");
   gAngCorrMat = (THnSparse*) InFile->Get("ggaP");
   if( gAngCorrMat == nullptr ) return;
   printf(" Done!");

   // Make graphs here
   MakeGraph(778.9, 344.3);
}

void ProcessAngularCorr( std::string Filename ) 
{
   TFile* InFile = new TFile(Filename.c_str(), "READ");
   if( InFile->IsOpen() ) {
      printf("File Opened: %s\n", Filename.c_str());
      ProcessAngularCorr(InFile);
   }
   return;
}

