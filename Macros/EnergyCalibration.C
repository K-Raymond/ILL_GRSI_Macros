#include "TTree.h"
#include "TH2.h"

#include "TCalManager.h"
#include "TEnergyCal.h"
#include "TChannel.h"
#include "TNucleus.h"
#include "TTransition.h"
#include "TFipps.h"
#include "TPeak.h"

#include <vector>
#include <iostream>

// Takes in a roughly energy calibrated GRSI style TTree and computes a more accurate gain and offset
// for the calibration coefficents.
void EnergyCalibration(TTree *AnalysisTree, TNucleus* NucleusForCalibration)
{
   TChannel *pChannel              = nullptr;

   int NChans              = TChannel::ReadCalFromCurrentFile();
   if (NChans == 0) NChans = TChannel::ReadCalFile("CalibrationFile.cal");

   // Create Charge Histogram
   TH2D *mat_q = new TH2D("mat_q", "Charge Matrix", NChans, 0, (double)NChans, 32000, 0, 32000);
   AnalysisTree->Project("mat_q", "TFipps.fHits.fCharge:TFipps.fHits.GetChannel()->GetNumber()");

   for (int i = 0; i < NChans; i++) {
      pChannel  = TChannel::GetChannelByNumber(i);
      TH1D *h_q = mat_q->ProjectionY(Form("hq_%.2i", i), i + 1, i + 1);
      if (pChannel == nullptr || h_q->Integral() < 1000) continue;

      Double_t    OldOffset    = pChannel->GetENGCoeff()[0];
      Double_t    OldSlope     = pChannel->GetENGCoeff()[1];
      Int_t       NTransitions = NucleusForCalibration->NTransitions();
      TEnergyCal *EnergyCal    = new TEnergyCal(Form("ECal_%.2i", i), Form("Energy Calibration for channel %.2i", i));
      EnergyCal->SetHist(h_q);
      EnergyCal->SetNucleus(NucleusForCalibration);

      // Using old energy calibration coefficents to calculate the centroid of literature
      // peaks in the charge spectrum.
      std::vector<Double_t> ChargePeaks;
      for (Int_t j = 0; j < NTransitions; j++) {
         TTransition *Transition = NucleusForCalibration->GetTransition(j);
         ChargePeaks.push_back((Transition->GetEnergy() - OldOffset) / OldSlope);
      }

      // Fit literature transitions in Charge spectra
      for (Int_t j = 0; j < NTransitions; j++) {
         TPeak *TransitionFit = new TPeak(ChargePeaks[j], ChargePeaks[j] - 55, ChargePeaks[j] + 55);
         TransitionFit->Fit(h_q, "LIMQ+");
         EnergyCal->SetPoint(j, TransitionFit);
      }

      // Fit charge vs. literature with a linear line to find the new energy coefficents
      TF1 *EnergyFit = new TF1("energy", "pol1", 0, 35000);
      EnergyFit->SetParameter(0, OldOffset);
      EnergyFit->SetParameter(1, OldSlope);
      EnergyCal->Fit("energy", "MEF");
      EnergyCal->SetFitFunction(EnergyFit);
      EnergyCal->Draw("AL");

      // print status
      std::cout << "Old Coeff: " << OldOffset << " " << OldSlope << std::endl;
      std::cout << "New Coeff: ";
      for (int j = 0; j < 2; j++) {
         std::cout << EnergyFit->GetParameters()[j] << " ";
      }
      std::cout << std::endl;
      pChannel->DestroyENGCal();
      pChannel->AddENGCoefficient(static_cast<Float_t>(EnergyFit->GetParameters()[0]));
      pChannel->AddENGCoefficient(static_cast<Float_t>(EnergyFit->GetParameters()[1]));

      EnergyCal->Draw();
      // getchar();

      delete EnergyCal;
      delete EnergyFit;
   }

   TChannel::WriteToRoot();
   TChannel::WriteCalFile("EnergyCalibrated.cal");

   std::cout << "Making Energy Plot: " << std::endl;
   TH2D *mat_en = new TH2D("mat_en", "", NChans, 0, (double)NChans, 10000, 0, 10000);
   AnalysisTree->Project("mat_en", "TFipps.fHits.GetEnergy():TFipps.fHits.GetChannel()->GetNumber()");
   new TCanvas();
   mat_en->Draw("colz");
}
