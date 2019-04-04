//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4RunAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
//
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("Eabs","Edep in RHS detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("Egap","Edep in LHS detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("Labs","trackL in absorber", 100, 0., 1*cm);
  analysisManager->CreateH1("Lgap","trackL in LHS detector", 100, 0., 50*cm);
  analysisManager->CreateH1("Ebot","Edep in bottom detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("Lbot","trackL in bottom detector", 100, 0., 50*cm);
  analysisManager->CreateH1("Eback","Edep in back detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("Lback","trackL in back detector", 100, 0., 50*cm);
  analysisManager->CreateH1("Efront","Edep in front detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("Lfront","trackL in front detector", 100, 0., 50*cm);
  analysisManager->CreateH1("EtopR","Edep in top right detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("LtopR","trackL in top right", 100, 0., 50*cm);
  analysisManager->CreateH1("EtopL","Edep in top left detector", 100, 0.000001, 5.*MeV);
  analysisManager->CreateH1("LtopL","trackL in top right", 100, 0., 50*cm);
  analysisManager->CreateH1("Etotal","Total Edep", 100, 0.000001, 5.*MeV);




  // Creating ntuple
  //
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Egap");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Lgap");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B4";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl; 
    }
    
    G4cout << " EAbs : mean = " 
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;
    
    G4cout << " EGap : mean = " 
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
    
    G4cout << " LAbs : mean = " 
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

    G4cout << " LGap : mean = " 
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
    G4cout << " EBot : mean = "
       << G4BestUnit(analysisManager->GetH1(4)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(4)->rms(),  "Energy") << G4endl;
    G4cout << " LBot : mean = " 
      << G4BestUnit(analysisManager->GetH1(5)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(5)->rms(),  "Length") << G4endl;
    G4cout << " EBack : mean = "
       << G4BestUnit(analysisManager->GetH1(6)->mean(), "Energy")
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(6)->rms(),  "Energy") << G4endl;
    G4cout << " LBack : mean = " 
      << G4BestUnit(analysisManager->GetH1(7)->mean(), "Length")
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(7)->rms(),  "Length") << G4endl;

    G4cout << " EFront : mean = "
       << G4BestUnit(analysisManager->GetH1(8)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(8)->rms(),  "Energy") << G4endl;
    G4cout << " LFront : mean = "
      << G4BestUnit(analysisManager->GetH1(9)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(9)->rms(),  "Length") << G4endl;
        G4cout << " ETopR : mean = "
       << G4BestUnit(analysisManager->GetH1(10)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(10)->rms(),  "Energy") << G4endl;
    G4cout << " LTopR : mean = "
      << G4BestUnit(analysisManager->GetH1(11)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(11)->rms(),  "Length") << G4endl;
    G4cout << " ETopL : mean = "
       << G4BestUnit(analysisManager->GetH1(12)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(12)->rms(),  "Energy") << G4endl;
    G4cout << " LTopL : mean = "
      << G4BestUnit(analysisManager->GetH1(13)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(13)->rms(),  "Length") << G4endl;
    G4cout << " ETotal : mean = "
      << G4BestUnit(analysisManager->GetH1(14)->mean(), "Energy")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(14)->rms(),  "Energy") << G4endl;


 }

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
