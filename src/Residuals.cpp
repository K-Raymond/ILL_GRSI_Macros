#include "TFile.h"
#include "TGraph.h"

#include <vector>
#include <iostream>

// The following namespace holds basic functions for operating on TFiles and
// vectors holding TGraphs which contain measured residuals.
//
// 1. It's important to note that channel numbers in TChannels typically start
//    from 1, and the vectors start from 0. All numbering is based from 0, so 
//    all indexes for channels should be incremented by 1 (ie i+1).
//
// 2. To use these functions in scripts, simply include this file. Each function
//    can be called by:
//          Residual::Load( "MyResiduals.root" )
//    or:
//          Residual::Write( "MyResiduals.root", ResidualVector )
//
// 3. Residuals can be built from multiple nuclie, so keep in mind that multiple
//    residual files for the same experiment may exist. The intention is that each
//    of these nuclie will be merged together.
//
// 4. Multiple residuals can be merged by calling Load on multiple files, and loading
//    each resulting residual vector into another vector, and then calling Merge on
//    the vector of vectors.
//
namespace Residual {
   std::vector<TGraph*> Load( std::string &FileName )
   {  
      std::vector<TGraph*> outVec;

      // Read in the TFile, and check if it open
      TFile* inFile = new TFile( FileName.c_str(), "READ" );
      if( !inFile->IsOpen()) {
         std::cout << "Could not open file: " << FileName << std::endl;
         return std::vector<TGraph*>();
      }

      // Open the residual directory, and see if it's openable
      TDirectory* ResidualDirectory = inFile->GetDirectory("Energy_Residuals");
      if( ResidualDirectory == nullptr ) {
         std::cout << "Could not find residuals directory in file: " << FileName << std::endl;
         inFile->Close();
         delete inFile;
         return std::vector<TGraph*>();
      }

      // Read all the residual TGraphs and make copies
      for( size_t i = 0; i < ResidualDirectory.GetNKeys(); i++ ) {
         // Channel numbers usually start from 1
         outVec.push_back( new TGraph(ResidualDirectory->Get<TGraph>(Form("Residual_%zu", i+1))) );
      }

      // Cleanup
      delete ResidualDirectory;
      inFile->Close(); delete inFile;
      return outVec;
   }

   void Write( std::string FileName, std::vector<TGraph*> ResidualVector )
   {
      // Attempt to open output file
      TFile* outFile = new TFile( FileName.c_str(), "RECREATE" );
      if( !outFile->IsOpen()) {
         std::cout << "Could not open file: " << FileName << std::endl;
         return;
      }

      // Create residual directory and write TGraphs
      TDirectory* ResidualDirectory = outFile->mkdir("Energy_Residuals");
      ResidualDirectory->cd();
      for( size_t i = 0; i < ResidualVector.size(); i++ ) {
         ResidualVector[i]->SetName(Form("Residual_%zu", i+1));
         ResidualVector[i]->Write();
      }
   }

   std::vector<TGraph*> Merge( std::vector<std::vector<TGraph*>> ResidualsVec )
   {
      // If empty, no graphs to merge
      if( ResidualsVec.size() == 0 )
         return std::vector<TGraph*>();

      // Check if all residual vectors are the same size
      bool Error = false;
      size_t LastSize = ResidualsVec[0].size();
      for( size_t i = 0; i < ResidualsVec.size(); i++ ) {
         if( LastSize != ResidualsVec[i].size() )
            Error = true;
      }
      if( Error ) {
         std::cout << "Residual vectors are not the same size, could not be added together" << std::endl;
         return std::vector<TGraph*>();
      }

      // Iterate through all vectors in vector, and merge each graph
      std::vector<TGraph*> outVec;
      for( size_t i = 0; i < ResidualsVecs.size(); i++ ) {
         TList* TempList = new TList();
         for( size_t j = 0; k < ResidualsVec[i].size(); i++ )
            TempList->Add(ResidualVec1[i][j]);
         TGraph* newGraph = new TGraph();
         newGraph->Merge( TempList );
         outVec.push_back( newGraph );

         delete TempList;
      }

      return outVec;
   }
} // \namespace Residual
