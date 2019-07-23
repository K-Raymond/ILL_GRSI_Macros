#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TVirtualFitter.h"

#include "TFipps.h"
#include "TChannel.h"

#include <iostream>
#include <string>

class TAlignHistogram {
  public:
  TAlignHistogram();
  TAlignHistogram( TH1* SeedHistogram, TH1* WorkHistogram, TChannel* Channel )
  {
    LoadSeedHistogram(SeedHistogram);
    LoadWorkHistogram(WorkHistogram);
    LoadChannel(Channel);
  }

  void Align(Double_t XLow, Double_t XHigh);

  void LoadSeedHistogram(TH1* SeedHistogram) {fSeedHistogram = SeedHistogram;}
  void LoadWorkHistogram(TH1* WorkHistogram) {fWorkHistogram = WorkHistogram;}
  void LoadChannel( TChannel* Channel ) { fChannel = Channel; }
  void ExportENGInfoToChannel();
  

  Double_t GetAlignCoeff(int i) { return fAlignCoeff[i]; }
  Double_t GetNewENGCoeff(int i) { return fNewENGCoeff[i]; }

  private:
  TH1* fSeedHistogram = nullptr;
  TH1* fWorkHistogram = nullptr;
  TChannel* fChannel = nullptr;
  Double_t HistCompare(Double_t*x, Double_t* par);

  Double_t fAlignCoeff[2];
  Double_t fNewENGCoeff[2];
};

Double_t TAlignHistogram::HistCompare(Double_t* x, Double_t* par) {
   Double_t xx      = x[0];
   Int_t    bin     = fWorkHistogram->GetXaxis()->FindBin(xx * par[2] + par[1]);
   Double_t content = fWorkHistogram->GetBinContent(bin);

   return par[0] * content;
}

void TAlignHistogram::ExportENGInfoToChannel() {
  fChannel->DestroyENGCal();
  fChannel->AddENGCoefficient(fNewENGCoeff[0]);
  fChannel->AddENGCoefficient(fNewENGCoeff[1]);
}

void TAlignHistogram::Align(Double_t XLow, Double_t XHigh) {
  TF1* AlignFunction = new TF1("AlignFunction", this, &TAlignHistogram::HistCompare, XLow, XHigh, 3);
  AlignFunction->SetNpx(10000);
  AlignFunction->SetParameters(1.0, 1.0, 1.0);

  // Fit
  TVirtualFitter::SetPrecision(1.0e-10);
  TVirtualFitter::SetMaxIterations(10000);
  TFitResultPtr pResult = fSeedHistogram->Fit("AlignFunction", "QRSIL");
  fAlignCoeff[0] = pResult->Parameter(1);
  fAlignCoeff[1] = pResult->Parameter(2);

  if( fChannel != nullptr ) {
    Float_t OldOffset = fChannel->GetENGCoeff()[0];
    Float_t OldSlope = fChannel->GetENGCoeff()[1];
    fNewENGCoeff[0] = OldOffset*fAlignCoeff[1] + fAlignCoeff[0];
    fNewENGCoeff[1] = OldSlope*fAlignCoeff[1];
  }
}

void GenerateCalibrationHistogramFromTree(TTree* AnalysisTree) {
  int NChans = TChannel::ReadCalFromTree(AnalysisTree);
  
   TH2* EngMat = new TH2D("SeedEngMat", "Seed Energy Matrix", NChans, 0, (Double_t)NChans, 12000, 0, 12000);
    AnalysisTree->Project("SeedEngMat", "TFipps.fHits.GetEnergy():TFipps.fHits.GetArrayNumber()");

    TFile* OutFile = new TFile("CalibrationSeedHistogram.root", "RECREATE");
    EngMat->Write();
    OutFile->Close();
    delete EngMat; delete OutFile;
}

void AlignTree(TTree* AnalysisTree) {
  int NChans = TChannel::ReadCalFromTree(AnalysisTree);

  TH2* WorkEngMat = new TH2D("WorkEngMat", "Seed Energy Matrix", NChans, 0, (Double_t)NChans, 12000, 0, 12000);
  AnalysisTree->Project("WorkEngMat", "TFipps.fHits.GetEnergy():TFipps.fHits.GetArrayNumber()");

  TFile* SeedFile = new TFile("CalibrationSeedHistogram.root", "READ");
  TH2* SeedEngMat = (TH2D*)SeedFile->Get("SeedEngMat");
  if( SeedEngMat == nullptr ) {
    printf("Error: Could not find calibrated energy matrix\n");
    return;
  }

  TH1* SeedSlice; TH1* WorkSlice; TChannel* pChannel;
  for( int i = 0; i < NChans; ++i ) {
    pChannel = TChannel::GetChannelByNumber(i);
    SeedSlice = SeedEngMat->ProjectionY(Form("hS_%.2i", i), i+1, i+1);
    WorkSlice = WorkEngMat->ProjectionY(Form("hW_%.2i", i), i+1, i+1);

    if( pChannel == nullptr || SeedSlice->Integral() < 2000 || WorkSlice->Integral() < 2000 )
      continue;

    TAlignHistogram* AlignHistogram = new TAlignHistogram( SeedSlice, WorkSlice, pChannel );
    AlignHistogram->Align(200, 1000);
    AlignHistogram->ExportENGInfoToChannel();

    std::cout << "Alignment Coeff: ";
    std::cout << AlignHistogram->GetAlignCoeff(0) << ", ";
    std::cout << AlignHistogram->GetAlignCoeff(1) << std::endl;
    
    std::cout << "New Energy Coeff: ";
    std::cout << AlignHistogram->GetNewENGCoeff(0) << ", ";
    std::cout << AlignHistogram->GetNewENGCoeff(1) << std::endl;

    delete AlignHistogram;
  }

  delete WorkEngMat; delete SeedEngMat; SeedFile->Close();
}

// Use find *.root > inFile.txt to generate input. Each line should be a path to a root file.
void AlignFiles(std::string InputFileName) {
  std::ifstream InputStream(InputFileName);
  std::string Line;

  while( std::getline(InputStream, Line).good() ) {
    TFile inFile(Line.c_str(), "UPDATE"); // READ?
    if( inFile.IsOpen() ) {
      TTree* AnalysisTree = (TTree*)inFile.Get("AnalysisTree");
      AlignTree(AnalysisTree);
      inFile.Close();
    }
  }
}
