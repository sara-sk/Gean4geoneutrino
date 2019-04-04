// $Id: B4cDetectorConstruction.cc 101905 2016-12-07 11:34:39Z gunter $
// 
/// \file B4cDetectorConstruction.cc
/// \brief Implementation of the B4cDetectorConstruction class

#include "B4cDetectorConstruction.hh"
#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4cDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::B4cDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true),
   fNofLayers(-1)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{ 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::DefineMaterials()
{ 
  // Silicon material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Si");
  
  // Liquid argon material - used previously for gap filling, omitted for this
  // simulation
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  fNofLayers = 1;
  G4double absoThickness = 2.5*mm;
  G4double gapThickness = 50.*mm;
  G4double calorSizeXY  = 50.*cm;

  auto layerThickness = absoThickness + gapThickness;
  auto calorThickness = fNofLayers * layerThickness;
  auto worldSizeXY = 50*mm;
  auto worldSizeZ  = 50*mm; 
  
  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_Si");
  auto gapMaterial = G4Material::GetMaterial("liquidArgon");
  
  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  
   
  //     
  // World
  //
  auto worldS 
    = new G4Box("World",           // its name
                 worldSizeXY, worldSizeXY, worldSizeZ); // its size
                         
  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
                                   
  auto worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0.,0.,0.),  // at (0,0,0*mm)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  

  //                               
  //                               
  // Absorber RHS
  //
  auto absorberS 
    = new G4Box("Abso",            // its name
                 3*cm, 3*cm, absoThickness); // its size
                         
  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "AbsoLV");        // its name
                                   
   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., 32.5), // its position
                 absorberLV,       // its logical volume                         
                 "Abso",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  //
  //
  // Absorber LHS
  //
  auto absorber2S
    = new G4Box("Abso2",            // its name
                 3*cm, 3*cm, absoThickness); // its size

  auto absorber2LV
    = new G4LogicalVolume(
                 absorber2S,        // its solid
                 absorberMaterial, // its material
                 "Abso2LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., -32.5), // its position
                 absorber2LV,       // its logical volume
                 "Abso2",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

 //
  //
  // Absorber Bottom
  //
  auto absorber3S
    = new G4Box("Abso3",            // its name
                 3*cm, absoThickness, 3*cm); // its size

  auto absorber3LV
    = new G4LogicalVolume(
                 absorber3S,        // its solid
                 absorberMaterial, // its material
                 "Abso3LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., -32.5, 0.), // its position
                 absorber3LV,       // its logical volume
                 "Abso3",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps


  //
  // Absorber Front
  //
  auto absorber4S
    = new G4Box("Abso4",            // its name
                 absoThickness, 3.*cm, 3*cm); // its size

  auto absorber4LV
    = new G4LogicalVolume(
                 absorber4S,        // its solid
                 absorberMaterial, // its material
                 "Abso4LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(32.5, 0., 0.), // its position
                 absorber4LV,       // its logical volume
                 "Abso4",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps
 

  //
  // Absorber Back
  //
  auto absorber5S
    = new G4Box("Abso5",            // its name
                 absoThickness, 3.*cm, 3*cm); // its size

  auto absorber5LV
    = new G4LogicalVolume(
                 absorber5S,        // its solid
                 absorberMaterial, // its material
                 "Abso5LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(-32.5, 0., 0.), // its position
                 absorber5LV,       // its logical volume
                 "Abso5",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

  //
  // Absorber Top Right
  //
  auto absorber6S
    = new G4Box("Abso6",            // its name
                 3.*cm, absoThickness, 1.45*cm); // its size

  auto absorber6LV
    = new G4LogicalVolume(
                 absorber6S,        // its solid
                 absorberMaterial, // its material
                 "Abso6LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 32.5, 15.5), // its position
                 absorber6LV,       // its logical volume
                 "Abso6",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps
  //
  // Absorber Top Left
  //
  auto absorber7S
    = new G4Box("Abso7",            // its name
                 3.*cm, absoThickness, 1.45*cm); // its size

  auto absorber7LV
    = new G4LogicalVolume(
                 absorber7S,        // its solid
                 absorberMaterial, // its material
                 "Abso7LV");        // its name

   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 32.5,  -15.5), // its position
                 absorber7LV,       // its logical volume
                 "Abso7",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  worldLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // 
  // Sensitive detectors
  //
  auto absoSD 
    = new B4cCalorimeterSD("AbsorberSD", "AbsorberHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
  SetSensitiveDetector("AbsoLV",absoSD);

auto absoSD2
    = new B4cCalorimeterSD("AbsorberSD2", "AbsorberHitsCollection2", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD2);
  SetSensitiveDetector("Abso2LV",absoSD2);

auto absoSD3
    = new B4cCalorimeterSD("AbsorberSD3", "AbsorberHitsCollection3", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD3);
  SetSensitiveDetector("Abso3LV",absoSD3);

auto absoSD4
    = new B4cCalorimeterSD("AbsorberSD4", "AbsorberHitsCollection4", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD4);
  SetSensitiveDetector("Abso4LV",absoSD4);

auto absoSD5
    = new B4cCalorimeterSD("AbsorberSD5", "AbsorberHitsCollection5", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD5);
  SetSensitiveDetector("Abso5LV",absoSD5);
auto absoSD6
    = new B4cCalorimeterSD("AbsorberSD6", "AbsorberHitsCollection6", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD6);
  SetSensitiveDetector("Abso6LV",absoSD6);
auto absoSD7
    = new B4cCalorimeterSD("AbsorberSD7", "AbsorberHitsCollection7", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD7);
  SetSensitiveDetector("Abso7LV",absoSD7);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
