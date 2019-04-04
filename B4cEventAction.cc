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
// $Id: B4cEventAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
// 
/// \file B4cEventAction.cc
/// \brief Implementation of the B4cEventAction class

#include "B4cEventAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction()
 : G4UserEventAction(),
   fAbsHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection* 
B4cEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<B4cCalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
                              G4double absoEdep, G4double absoTrackLength) const
{
  // print event statistics
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl;
//     << "        Gap: total energy: " 
  //   << std::setw(7) << G4BestUnit(gapEdep, "Energy")
    // << "       total track length: " 
   //  << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
 //    << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}











//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  
  // Get hits collections IDs (only once)
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
    fGapHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection2");
    fBotHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection3");
    fBackHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection4");
    fFrontHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection5");
    fTopRHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection6");
    fTopLHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection7");
    }


  // Get hits collections

  auto absoHC = GetHitsCollection(fAbsHCID, event);
  auto gapHC = GetHitsCollection(fGapHCID, event);
  auto botHC = GetHitsCollection(fBotHCID, event);
  auto backHC = GetHitsCollection(fBackHCID, event);
  auto frontHC = GetHitsCollection(fFrontHCID, event);
  auto topRHC = GetHitsCollection(fTopRHCID, event);
  auto topLHC = GetHitsCollection(fTopLHCID, event);

  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries()-1];
  auto gapHit = (*gapHC)[gapHC->entries()-1];
  auto botHit = (*botHC)[botHC->entries()-1];
  auto backHit = (*backHC)[backHC->entries()-1];
  auto frontHit = (*frontHC)[frontHC->entries()-1];
  auto topRHit = (*topRHC)[topRHC->entries()-1];
  auto topLHit = (*topLHC)[topLHC->entries()-1];

  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     
 
  // Define retrieval of deposited energies
//  edepAbso = absoHit->GetEdep();
//  edepGap = gapHit->GetEdep();
//  edepTotal = edepAbso + edepGap;



    PrintEventStatistics(
    //  edepTotal, absoHit->GetTrackLength());
      absoHit->GetEdep(), absoHit->GetTrackLength());
      gapHit->GetEdep(), gapHit->GetTrackLength();
      botHit->GetEdep(), botHit->GetTrackLength(); 
      backHit->GetEdep(), backHit->GetTrackLength();
      frontHit->GetEdep(), frontHit->GetTrackLength();
      topRHit->GetEdep(), topRHit->GetTrackLength();
      topLHit->GetEdep(), topLHit->GetTrackLength(); 


 }
  
  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
 


  // fill histograms
  analysisManager->FillH1(0, absoHit->GetEdep());
  analysisManager->FillH1(1, gapHit->GetEdep());
  analysisManager->FillH1(2, absoHit->GetTrackLength());
  analysisManager->FillH1(3, gapHit->GetTrackLength());
  analysisManager->FillH1(4, botHit->GetEdep());
  analysisManager->FillH1(5, botHit->GetTrackLength());
  analysisManager->FillH1(6, backHit->GetEdep());
  analysisManager->FillH1(7, backHit->GetTrackLength());
  analysisManager->FillH1(8, frontHit->GetEdep());
  analysisManager->FillH1(9, frontHit->GetTrackLength());
  analysisManager->FillH1(10, topRHit->GetEdep());
  analysisManager->FillH1(11, topRHit->GetTrackLength());
  analysisManager->FillH1(12, topLHit->GetEdep());
  analysisManager->FillH1(13, topLHit->GetTrackLength());

// filling total energy into histogram
  analysisManager->FillH1(14, absoHit->GetEdep());
  analysisManager->FillH1(14, gapHit->GetEdep());
  analysisManager->FillH1(14, botHit->GetEdep());
  analysisManager->FillH1(14, topRHit->GetEdep());
  analysisManager->FillH1(14, topLHit->GetEdep());
  analysisManager->FillH1(14, backHit->GetEdep());
  analysisManager->FillH1(14, frontHit->GetEdep());






  // fill ntuple
  analysisManager->FillNtupleDColumn(0, absoHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, gapHit->GetEdep());
  analysisManager->FillNtupleDColumn(2, absoHit->GetTrackLength());
  analysisManager->FillNtupleDColumn(3, gapHit->GetTrackLength());
  analysisManager->AddNtupleRow();  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
