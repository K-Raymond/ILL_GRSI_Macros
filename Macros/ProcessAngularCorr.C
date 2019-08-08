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
static TAngCorrIndexTranslator gAngCorr("AngularCorrelationConfig.cal");

std::tuple<TGraphErrors*, TCanvas*> MakeGraph(TFile* inFile, Double_t Gamma1, Double_t Gamma2, TList* GraphList)
{

   printf("Making graph for E1 = %0.2f and E2 = %0.2f\n", Gamma1, Gamma2);
   //TAngCorrIndexTranslator gAngCorr;
   TGraphErrors* CountsVsAngle = new TGraphErrors();
   CountsVsAngle->SetTitle(Form("Counts vs Angle for E1 = %0.2f keV and E2 = %0.2f keV", Gamma1, Gamma2));
   CountsVsAngle->SetName(Form("gg_%0.0f_%0.0f", Gamma1, Gamma2));
   CountsVsAngle->GetXaxis()->SetTitle("Cos(Theta)");
   CountsVsAngle->GetYaxis()->SetTitle("Normalized Counts");

   TGraphErrors* PromptVsAngle = new TGraphErrors();
   PromptVsAngle->SetTitle(Form("Prompt vs Angle for E1 = %0.2f keV and E2 = %0.2f keV", Gamma1, Gamma2));
   PromptVsAngle->SetName(Form("gg_%0.0f_%0.0f_Prompt", Gamma1, Gamma2));
   PromptVsAngle->GetXaxis()->SetTitle("Cos(Theta)");
   PromptVsAngle->GetYaxis()->SetTitle("Normalized Counts");

   TGraphErrors* EMVsAngle = new TGraphErrors();
   EMVsAngle->SetTitle(Form("Event-Mixed vs Angle for E1 = %0.2f keV and E2 = %0.2f keV", Gamma1, Gamma2));
   EMVsAngle->SetName(Form("gg_%0.0f_%0.0f_EM", Gamma1, Gamma2));
   EMVsAngle->GetXaxis()->SetTitle("Cos(Theta)");
   EMVsAngle->GetYaxis()->SetTitle("Normalized Counts");
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
      
      TH1* Prompt_Spectra = Prompt_Sparse->Projection(1);
      TPeak* PromptPeak = new TPeak(Gamma2, Gamma2 - 5, Gamma2 + 5);
      PromptPeak->Fit(Prompt_Spectra, "MEQ");
      double_t PromptGateWidth = PromptPeak->GetFWHM();
      double_t PromptGateCenter = PromptPeak->GetCentroid();
      delete PromptPeak;

      // Make on energy gate
      Prompt_Sparse->GetAxis(1)->SetRangeUser(PromptGateCenter - PromptGateWidth/2,
           PromptGateCenter + PromptGateWidth/2);
      Prompt_Gate = Prompt_Sparse->Projection(0);
      Prompt_Gate->SetName(Form("Prompt_Gate_%zu", i));
      EM_Sparse->GetAxis(1)->SetRangeUser(PromptGateCenter - PromptGateWidth/2,
           PromptGateCenter + PromptGateWidth/2);
      EM_Gate = EM_Sparse->Projection(0);
      EM_Gate->SetName(Form("EM_Gate_%zu", i));

      // Make off energy gate
      Prompt_Sparse->GetAxis(1)->SetRangeUser(PromptGateCenter + PromptGateWidth,
            PromptGateCenter + 2*PromptGateWidth);
      Prompt_Gate_BG = Prompt_Sparse->Projection(0);
      Prompt_Gate_BG->SetName(Form("Prompt_Gate_BG_%zu",i));
      EM_Sparse->GetAxis(1)->SetRangeUser(PromptGateCenter + PromptGateWidth, PromptGateCenter + 2*PromptGateWidth);
      EM_Gate_BG = EM_Sparse->Projection(0);
      EM_Gate_BG->SetName(Form("EM_Gate_BG_%zu", i));

      // Background Subtract
      Prompt_Gate->Sumw2();
      Prompt_Gate->Add(Prompt_Gate_BG, -1);

      Prompt_Gate->GetXaxis()->SetRangeUser( Gamma1 - 10, Gamma1 + 10 );
      TPeak* RawPromptPeak = new TPeak(Gamma1, Gamma1 - 7, Gamma1 + 7);
      RawPromptPeak->SetLogLikelihoodFlag(false);
      RawPromptPeak->Fit(Prompt_Gate, "MEQ");

      EM_Gate->Sumw2();
      EM_Gate->Add(EM_Gate_BG, -1);

      EM_Gate->GetXaxis()->SetRangeUser( Gamma1 - 10, Gamma1 + 10 );
      TPeak* RawEMPeak = new TPeak(Gamma1, Gamma1 - 7, Gamma1 + 7);
      RawEMPeak->SetLogLikelihoodFlag(false);
      RawEMPeak->Fit(EM_Gate, "MEQ");

      // Extract Measurables
      double_t PromptArea = RawPromptPeak->GetArea()/gAngCorr.GetCombinationOfAngle(i);
      double_t PromptAreaErr = RawPromptPeak->GetAreaErr()/gAngCorr.GetCombinationOfAngle(i);
      double_t EMArea = RawEMPeak->GetArea()/gAngCorr.GetCombinationOfAngle(i);
      double_t EMAreaErr = RawEMPeak->GetAreaErr()/gAngCorr.GetCombinationOfAngle(i);

      double_t FinalArea = PromptArea/EMArea;
      double_t FinalAreaErr = FinalArea * TMath::Sqrt( TMath::Power( PromptAreaErr/PromptArea , 2)
            + TMath::Power( EMAreaErr / EMArea , 2) );
      delete RawEMPeak; delete RawPromptPeak;

      // Add point to graph
      NPoints += 1;
      CountsVsAngle->Set(NPoints);
      CountsVsAngle->SetPoint(NPoints-1, TMath::Cos(TMath::DegToRad()*gAngCorr.IndexToAngle(i)), FinalArea );
      CountsVsAngle->SetPointError( NPoints-1, 0, FinalAreaErr );
      CountsVsAngle->SetMarkerStyle(20);

      PromptVsAngle->Set(NPoints);
      PromptVsAngle->SetPoint(NPoints-1, TMath::Cos(TMath::DegToRad()*gAngCorr.IndexToAngle(i)), PromptArea );
      PromptVsAngle->SetPointError( NPoints-1, 0, PromptAreaErr );
      PromptVsAngle->SetMarkerStyle(24);
      PromptVsAngle->SetMarkerColor(1);

      EMVsAngle->Set(NPoints);
      EMVsAngle->SetPoint(NPoints-1, TMath::Cos(TMath::DegToRad()*gAngCorr.IndexToAngle(i)), EMArea );
      EMVsAngle->SetPointError( NPoints-1, 0, EMAreaErr );
      EMVsAngle->SetMarkerStyle(24);
      EMVsAngle->SetMarkerColor(2);

      Prompt_Gate->Delete(); Prompt_Gate_BG->Delete(); EM_Gate->Delete(); EM_Gate_BG->Delete();
      Prompt_Sparse->Delete(); EM_Sparse->Delete();
      /*
      delete Prompt_Gate; delete Prompt_Gate_BG; delete EM_Gate; delete EM_Gate_BG;
      delete Prompt_Sparse; delete EM_Sparse;
      */
   }
   GraphList->Add(CountsVsAngle);
   GraphList->Add(PromptVsAngle);
   GraphList->Add(EMVsAngle);

   return std::make_tuple(CountsVsAngle, CFits);
}  

void ProcessAngularCorr(TFile* InFile)
{
   TList* PhotoPeakList = new TList();
   TList* GraphList = new TList();

   TList* EMList = new TList();
   TList* CList = new TList();
   TGraphErrors* AngCorrGraph;
   TCanvas* CAngCorrGraphs;

   // Make graphs here

   // 778 keV and 334 keV
   std::tie(AngCorrGraph, CAngCorrGraphs) = MakeGraph(InFile, 778.9, 344.3, GraphList);
   PhotoPeakList->Add( AngCorrGraph );
   CList->Add( CAngCorrGraphs );

   // Reverse of the above
   std::tie(AngCorrGraph, CAngCorrGraphs) = MakeGraph(InFile, 344.3, 778.9, GraphList);
   PhotoPeakList->Add( AngCorrGraph );
   CList->Add( CAngCorrGraphs );

   // Export the lists
   TFile* outFile = new TFile("AngCorrGraphs.root", "RECREATE");
   PhotoPeakList->Write();
   EMList->Write();
//   CList->Write();
   GraphList->Write();

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

