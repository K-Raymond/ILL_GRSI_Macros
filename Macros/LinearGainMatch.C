#include "TH1.h"
#include "TH2.h"
#include "TSpectrum.h"

#include "TPeak.h"
#include "TChannel.h"

#include "TFipps.h"

// 28Al Calibration (Easy)
static double_t gCalPeaks[2][2] = { {511, 70}, {1778.987, 250} };
// 28Al Optional Calibration (Harder, try the above first)
//static double_t gCalPeaks[2][2] = { {1778.987, 250}, {7724.034, 100} };

// Gain Match one file
void LinearGainMatch(TTree* AnalysisTree)
{
	double_t MeasuredPeaks[2];
    
    int NChans = TChannel::ReadCalFromCurrentFile();
    if( NChans == 0 )
       NChans = TChannel::ReadCalFile("CalibrationFile.cal");

    // Keep statistics at 1 keV/bin
    TH2D* mat_en = new TH2D("mat_en", "", NChans, 0, (double)NChans, 10000, 0, 10000);
    TH2D* q_en = new TH2D("q_en", "Charge Matrix", NChans, 0, (double)NChans, 32000, 0, 32000);

    printf("Projecting energy spectra...");
    AnalysisTree->Project("mat_en", "TFipps.fHits.GetEnergy():TFipps.fHits.GetChannel()->GetNumber()");
    printf("Done!\nProjecting charge spectra...");
    AnalysisTree->Project("q_en", "TFipps.fHits.fCharge:TFipps.fHits.GetChannel()->GetNumber()");
    printf("Done!\n");

    TChannel* pChannel = nullptr;
    for (int i = 0; i <= NChans ; i++ ) {
        pChannel = TChannel::GetChannelByNumber(i);
        TH1D *h_en = mat_en->ProjectionY(Form("h_%.2i", i), i + 1, i + 1);
        TH1D *h_q_en = q_en->ProjectionY(Form("h_q_%.2i", i), i + 1, i + 1);

        if( pChannel == nullptr || h_en->Integral() < 1000 )
            continue;

        for ( int k = 0; k < 2 ; k++ ) {
            TSpectrum s;
            h_en->GetXaxis()->SetRangeUser( 
                    gCalPeaks[k][0] - gCalPeaks[k][1],
                    gCalPeaks[k][0] + gCalPeaks[k][1] );
            s.Search(h_en, 2, "", 0.25);
            double_t SpecPeak = s.GetPositionX()[0];
            h_en->GetXaxis()->UnZoom();

            TPeak* TempP = new TPeak(SpecPeak, SpecPeak - 20,
                    SpecPeak + 20 );

            TempP->Fit(h_en,"MQ+");
            MeasuredPeaks[k] = TempP->GetCentroid();
            delete TempP;
        }

        double_t oldOffset = pChannel->GetENGCoeff()[0];
        double_t oldSlope = pChannel->GetENGCoeff()[1];
        double_t ChargePeaks[2];

        for ( int k = 0; k < 2 ; k++ ) {
            ChargePeaks[k] = ( MeasuredPeaks[k] - oldOffset ) / oldSlope;
        }

        double_t MeasuredQPeaks[2];
        for( int k = 0; k < 2; k++ ) {
            TPeak* QPeak = new TPeak( ChargePeaks[k], ChargePeaks[k] - 15/oldSlope, ChargePeaks[k] + 15/oldSlope );
            QPeak->Fit(h_q_en, "MQ+");
            MeasuredQPeaks[k] = QPeak->GetCentroid();
            delete QPeak;
        }
            

        // TODO: Actually fit a charge spectrum

        double_t newSlope = (gCalPeaks[1][0] - gCalPeaks[0][0])
            / (MeasuredQPeaks[1] - MeasuredQPeaks[0]);
        double_t newOffset = gCalPeaks[0][0] - newSlope*MeasuredQPeaks[0];

        pChannel->DestroyENGCal();
        pChannel->AddENGCoefficient( static_cast<Float_t>( newOffset ) );
        pChannel->AddENGCoefficient( static_cast<Float_t>( newSlope ) );
    }

    TChannel::WriteToRoot();
    TChannel::WriteCalFile("./CalibrationFileMatched.cal");
}
