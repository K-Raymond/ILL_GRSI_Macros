#include <string>
#include <tuple>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"
#include "TGraph.h"
#include "TPeak.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TRatioPlot.h"
#include "TCanvas.h"

#include "../src/TAngCorrIndexTranslator.cpp"

static TH1* gCombinations;
static THnSparse* gAngCorrMat;
static TAngCorrIndexTranslator gAngCorr;

std::tuple<TGraphErrors*, TCanvas*> MakeGraph(TFile* inFile, Double_t Gamma1, Double_t Gamma2)
{

   printf("Making graph for E1 = %0.2f and E2 = %0.2f\n", Gamma1, Gamma2);
   //TAngCorrIndexTranslator gAngCorr;
   TGraphErrors* CountsVsAngle = new TGraphErrors();
   CountsVsAngle->SetTitle(Form("Counts vs Angle for E1 = %0.2f keV and E2 = %0.2f keV", Gamma1, Gamma2));
   CountsVsAngle->SetName(Form("gg_%0.0f_%0.0f", Gamma1, Gamma2));
   CountsVsAngle->GetXaxis()->SetTitle("Angle (Deg)");
   CountsVsAngle->GetYaxis()->SetTitle("Normalized Counts");
   Int_t NPoints = 0;

   // Make a canvas to show fit plots
   TCanvas* CFits = new TCanvas();
   CFits->SetName(Form("Cgg_%0.0f_%0.0f", Gamma1, Gamma2));
   CFits->Divide(4,13);
   
   // Loop over all crystals, skipping the lowest angle 0.0
   for( size_t i = 1; i < gAngCorr.GetNumberOfUniqueAngles(); i++ )
   {
      // THnSparses for this index
      THnBase* Prompt_Sparse;
      THnBase* EM_Sparse;

      // Gates for Gamma2
      TH1* Prompt_Gate;
      TH1* Prompt_Gate_BG;
      TH1* EM_Gate;
      TH1* EM_Gate_BG;

      Prompt_Sparse = (THnBase*)inFile->Get(Form("ggP_%zu", i));
      EM_Sparse = (THnBase*)inFile->Get(Form("ggEM_%zu", i));

      if( Prompt_Sparse == nullptr || EM_Sparse == nullptr ) {
         printf("Error: Could not find sparses for index %zu\n", i);
         continue;
      }

      // If less than 10,000 events, probably has no events
      // Angle is not populated
      if( Prompt_Sparse->GetEntries() < 10000 ) {
         delete Prompt_Sparse; delete EM_Sparse;
         continue;
      }

      // Make Prompt Gates
      Prompt_Sparse->GetAxis(1)->SetRangeUser(Gamma2 - 5, Gamma2 + 5);
      Prompt_Gate = Prompt_Sparse->Projection(0);
      Prompt_Gate->SetName(Form("Prompt_Gate_%zu", i));
      EM_Sparse->GetAxis(1)->SetRangeUser(Gamma2 - 5, Gamma2 + 5);
      EM_Gate = EM_Sparse->Projection(0);
      EM_Gate->SetName(Form("EM_Gate_%zu", i));

      // Make Background Gates
      Prompt_Sparse->GetAxis(1)->SetRangeUser(Gamma2 + 6 , Gamma2 + 16);
      Prompt_Gate_BG = Prompt_Sparse->Projection(0);
      Prompt_Gate_BG->SetName(Form("Prompt_Gate_BG_%zu",i));
      EM_Sparse->GetAxis(1)->SetRangeUser(Gamma2 + 6, Gamma2 + 16);
      EM_Gate_BG = EM_Sparse->Projection(0);
      EM_Gate_BG->SetName(Form("EM_Gate_BG_%zu", i));

      // Background Subtract
      Prompt_Gate->Sumw2();
      Prompt_Gate_BG->Sumw2();

      Prompt_Gate->Add(Prompt_Gate_BG, -1);

      EM_Gate->Sumw2();
      //EM_Gate->Add(EM_Gate_BG, -1);

      // Produce final histogram for this channel
      Prompt_Gate->Divide(EM_Gate);
      // Measurable event, add point
      NPoints += 1;
      CountsVsAngle->Set(NPoints);
      CFits->cd(i);
      gPad->SetLeftMargin(0.05);
      gPad->SetBottomMargin(0.05);
      gPad->SetRightMargin(0.00);
      gPad->SetTopMargin(0.00);


      // Fit Peaks at Gamma1
      Prompt_Gate->GetXaxis()->SetRangeUser( Gamma1 - 8, Gamma1 + 8 );
      TPeak* TempPeak = new TPeak( Gamma1, Gamma1 - 4, Gamma1 + 4 );
      TempPeak->SetLogLikelihoodFlag(false);
      TempPeak->Fit(Prompt_Gate, "MEQ");
      //TRatioPlot(Prompt_Gate).Draw(); // draw to canvas at subcanvas i 
      Prompt_Gate->DrawCopy("APL");
      CFits->Update();
      CFits->Draw();

      // Add point to graph
      CountsVsAngle->SetPoint(NPoints-1, gAngCorr.IndexToAngle(i), TempPeak->GetArea() );
      CountsVsAngle->SetPointError( NPoints-1, 0, TempPeak->GetAreaErr() );
      CountsVsAngle->SetMarkerStyle(20);

      delete Prompt_Gate; delete Prompt_Gate_BG; delete EM_Gate; delete EM_Gate_BG;
      delete Prompt_Sparse; delete EM_Sparse;
   }

   return std::make_tuple(CountsVsAngle, CFits);
}  

void ProcessAngularCorr(TFile* InFile)
{
   TList* PhotoPeakList = new TList();
   TList* EMList = new TList();
   TList* CList = new TList();
   TGraphErrors* AngCorrGraph;
   TCanvas* CAngCorrGraphs;

   // Make graphs here

   // 778 keV and 334 keV
   std::tie(AngCorrGraph, CAngCorrGraphs) = MakeGraph(InFile, 778.9, 344.3);
   PhotoPeakList->Add( AngCorrGraph );
   CList->Add( CAngCorrGraphs );

   // Reverse of the above
   std::tie(AngCorrGraph, CAngCorrGraphs) = MakeGraph(InFile, 344.3, 778.9);
   PhotoPeakList->Add( AngCorrGraph );
   CList->Add( CAngCorrGraphs );

   // Export the lists
   TFile* outFile = new TFile("AngCorrGraphs.root", "RECREATE");
   PhotoPeakList->Write();
   EMList->Write();
   CList->Write();

   // Garbage Collection
   outFile->Close();
   delete PhotoPeakList; delete EMList;
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

