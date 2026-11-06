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
//
// $Id: ExN02SteppingAction.cc,v 1.9 2006-06-29 17:48:18 gunter Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EMMASteppingAction.hh"
#include "EMMAGlobalField.hh"
#include "EMMAElementField.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4TouchableHandle.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <G4Event.hh>

G4double EMMASteppingAction::PostTargetEnergy;
G4double EMMASteppingAction::PostDegraderEnergy;

G4double EMMASteppingAction::PostTargetXdir;
G4double EMMASteppingAction::PostTargetYdir;
G4double EMMASteppingAction::PostDegraderXdir;
G4double EMMASteppingAction::PostDegraderYdir;

// global variables 
G4double currentCharge = 0.0; // default value is 0

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EMMASteppingAction::EMMASteppingAction()
:dead_hit(0)
{

#ifdef G4ANALYSIS_USE
  G4int nelements=17; //number of elements listed below
  dead_hit = new TH1I("dead_hit","Dead hits",nelements,0,nelements);	//create root histogram
  dead_hit->GetXaxis()->SetTitle("EMMA component number");	//axis labels
  dead_hit->GetYaxis()->SetTitle("Counts");
  
  //label EMMA elements
  G4String* deadname = new G4String[nelements];
  deadname[0]="FocalPlane";
  deadname[1]="Aperture";
  deadname[2]="Q1";
  deadname[3]="Q2";
  deadname[4]="ED1";
  deadname[5]="EDMDdrift";
  deadname[6]="MD";
  //deadname[5]="MDED2drift";
  deadname[7]="ED2";
  deadname[8]="Q3";
  deadname[9]="Q4";
  deadname[10]="PGACbox";
  deadname[11]="hSlits1";
  deadname[12]="hSlits2";
  deadname[13]="hSlits3";
  deadname[14]="vSlits3";
  deadname[15]="MWPCwires"; //if you added elements make sure nelements (number of elements) includes this one
  deadname[16]="Aperture";
  for(Int_t i=0;i<nelements;i++){ //set axis labels to EMMA component names
    dead_hit->GetXaxis()->SetBinLabel(i+1,deadname[i].c_str());
  }

#endif // G4ANALYSIS_USE  

  //useful for debugging by tracking particles through the vacuum regions
  element[0]="Q1Logical";
  element[1]="Q2Logical";
  element[2]="ED1Logical";
  element[3]="MDLogical";
  element[4]="ED2Logical";
  element[5]="Q3Logical";
  element[6]="Q4Logical";
  element[7]="worldLogical";

}


EMMASteppingAction::~EMMASteppingAction()
{
#ifdef G4ANALYSIS_USE
  //This root histogram gets writen to the root file created in the constructor of EventAction.cc
  dead_hit->Write();
#endif // G4ANALYSIS_USE
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EMMASteppingAction::UserSteppingAction(const G4Step* theStep)
{ 

  G4Track* theTrack = theStep->GetTrack();
  const G4DynamicParticle* theParticle = theTrack->GetDynamicParticle();
  G4double theCharge = theParticle->GetCharge();
  G4double theKineticEnergy = theParticle->GetKineticEnergy();
  G4ThreeVector MomentumDirection = theParticle->GetMomentumDirection();
  //  G4cout << "Charge: " << theCharge << G4endl;
  currentCharge = theCharge;
  
  //get event number. useful for debugging
  G4int evnt = G4RunManager::GetRunManager()->GetCurrentEvent() -> GetEventID();

//====================================================================//
/* 
   Writes beam energy, direction and position to file at random 
   depth in foil, so that this information can be used as input
   for a subsequent simulation of reactions taking place in the 
   foil.
*/
  G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
  G4String name = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();

  //if target thickness is smaller than the step length then beam might miss target.
 
  //if that happens a warning output is generated.
  if (prepareBeam){
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
    G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    if(name=="worldLogical" && name2=="Q1Logical"){
      G4cout<<"\nWARNING: target thickness too thin. Particle no. "<<evnt<<
      " run with /mydet/doPrepare "
      "\nmisses target, passes through to focal plane and no information is written to\n"
      <<inTargetFileName<<". \nThis causes /mydet/doReaction to crash."<<G4endl;
    }
  }

 
  if (name=="targetLogical") {
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition = preStepPoint->GetPosition();
    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
    G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4TouchableHandle theTouchable2 = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition2 = postStepPoint->GetPosition();

    //this part finds the beam energy, world position and momentum at the reaction depth in the target
    //and aborts the event so that the beam doesn't go through to the focal plane
    if (prepareBeam) {
      if (name!=name2) {
	// depth declared in PrimaryGeneratorAction
	G4double dz = depth/2.; // correction needed because target placement refers to center of target ...
	std::ofstream beamFile(inTargetFileName, std::ios::app); //Declared in EMMAPrimaryGeneratorAction
	beamFile.precision(17);
	beamFile 
	  << theKineticEnergy/MeV << " " 
	  //	  << worldPosition[0] << " " << worldPosition[1] << " " << worldPosition[2] << " "
	  << worldPosition2[0] << " " << worldPosition2[1] << " " << worldPosition2[2]+dz << " "
	  << MomentumDirection[0] << " " << MomentumDirection[1] << " " << MomentumDirection[2]
	  << G4endl;
	beamFile.close();
	
	G4EventManager::GetEventManager()->AbortCurrentEvent();
      }
    }
//====================================================================//
    
    
    
    
    //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>//
    //   Writes energies and angles just after target to file
    //   (so that electric and magnetic field strengths can be
    //   optimized)
    if (!prepareBeam) {
      if (name2!=name) {
	G4double dirn = sqrt( MomentumDirection[0]*MomentumDirection[0] 
			      + MomentumDirection[1]*MomentumDirection[1]
			      + MomentumDirection[2]*MomentumDirection[2]);
	G4double theta = std::acos( MomentumDirection[2]/dirn );
	G4double theta2 = std::asin( MomentumDirection[0]/dirn );
	G4double phi = std::asin( MomentumDirection[1]/dirn );
	std::ofstream outfile(postTargetFileName, std::ios::app); //Declared in EMMAPrimaryGeneratorAction
	outfile.precision(17);
	outfile << theKineticEnergy/MeV << ", " << theta/deg << ", "
		<< theta2/deg << ", " << phi/deg << G4endl;
		//<< worldPosition2[0] << ", " << worldPosition2[1] << G4endl;

	PostTargetEnergy = theKineticEnergy/MeV;
	PostTargetXdir = MomentumDirection[0];
	PostTargetYdir = MomentumDirection[1];

	outfile.close();
      }
    }
    //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>//
  }

  //......................................................................//
  // Writes energies and angles just after (optional) degrader 1
   
  if (name=="degrader1Logical") {
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
    G4TouchableHandle theTouchable2 = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition2 = postStepPoint->GetPosition();
    G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();

    if (!prepareBeam && name2!=name) {
      G4double dirn = sqrt( MomentumDirection[0]*MomentumDirection[0] 
			    + MomentumDirection[1]*MomentumDirection[1]
			    + MomentumDirection[2]*MomentumDirection[2]);
      G4double theta = std::acos( MomentumDirection[2]/dirn );
      G4double theta2 = std::asin( MomentumDirection[0]/dirn );
      G4double phi = std::asin( MomentumDirection[1]/dirn );
      std::ofstream outfile(postDegrader1FileName, std::ios::app); //Declared in EMMAPrimaryGeneratorAction
      outfile.precision(17);
      outfile << theKineticEnergy/MeV << ", " << theta/deg << ", "
	      << theta2/deg << ", " << phi/deg << G4endl;

		PostDegraderEnergy = theKineticEnergy/MeV;
		PostDegraderXdir = MomentumDirection[0];
		PostDegraderYdir = MomentumDirection[1];

      outfile.close();
    }
  }

  //......................................................................//
  // Writes energies and angles just after (optional) degrader 1
   
  if (name=="degrader2Logical") {
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
    G4TouchableHandle theTouchable2 = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition2 = postStepPoint->GetPosition();
    G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    if (!prepareBeam && name2!=name) {
      G4double dirn = sqrt( MomentumDirection[0]*MomentumDirection[0] 
			    + MomentumDirection[1]*MomentumDirection[1]
			    + MomentumDirection[2]*MomentumDirection[2]);
      G4double theta = std::acos( MomentumDirection[2]/dirn );
      G4double theta2 = std::asin( MomentumDirection[0]/dirn );
      G4double phi = std::asin( MomentumDirection[1]/dirn );
      std::ofstream outfile(postDegrader2FileName, std::ios::app); //Declared in EMMAPrimaryGeneratorAction
      outfile.precision(17);
      outfile << theKineticEnergy/MeV << ", " << theta/deg << ", "
	      << theta2/deg << ", " << phi/deg << G4endl;
	//<< worldPosition2[0] << ", " << worldPosition2[1] << G4endl;

	PostDegraderEnergy = theKineticEnergy/MeV;
	PostDegraderXdir = MomentumDirection[0];
	PostDegraderYdir = MomentumDirection[1];

      outfile.close();
    }
  }

  //......................................................................//
  // Writes X,Y Location at MD mid-plane
 /*  
  //if (name=="degrader2Logical") {
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
    G4TouchableHandle theTouchable2 = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition2 = postStepPoint->GetPosition();
    //G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    if (!prepareBeam && name2!=name) {
      G4double dirn = sqrt( MomentumDirection[0]*MomentumDirection[0] 
			    + MomentumDirection[1]*MomentumDirection[1]
			    + MomentumDirection[2]*MomentumDirection[2]);
      G4double theta = std::acos( MomentumDirection[2]/dirn );
      G4double theta2 = std::asin( MomentumDirection[0]/dirn );
      G4double phi = std::asin( MomentumDirection[1]/dirn );
      std::ofstream outfile(postDegrader1FileName, std::ios::app); //Declared in EMMAPrimaryGeneratorAction
      outfile.precision(17);
      outfile << theKineticEnergy/MeV << ", " << theta/deg << ", "
	      << theta2/deg << ", " << phi/deg << G4endl;
	//<< worldPosition2[0] << ", " << worldPosition2[1] << G4endl;

	PostDegraderEnergy = theKineticEnergy/MeV;
	PostDegraderXdir = MomentumDirection[0];
	PostDegraderYdir = MomentumDirection[1];

      outfile.close();
    }
  //}
*/
  //......................................................................//

  //find out if any walls and slits were hit.
  //terminate event if it does.
  
  //initialize
  dead=false;	//hit focal plane
  deadint=0;	//Focal plane
/*  
  if (name=="hSlits1Logical") {
    NOHslits1 = NOHslits1 + 1; // count number of hits in various places
    deadint=11; //slits1 between ED1 and MD  Should be 12?
  }
  if (name=="hSlits2Logical") {
    NOHslits2 = NOHslits2 + 1;
    deadint=12; //slits2 Between MD and ED1
  }
  if (name=="hSlits3Logical") {
    NOHslits3 = NOHslits3 + 1;
    deadint=13; //slits3 after Q4
  }
  if(name=="hSlits4Logical"){
    NOHslits4 = NOHslits4 + 1;
    deadint=14; //2nd slits after Q4
  }
*/
	if(name=="ApertureLogical") {
		deadint = 1; // Hit Q1 beam pipe
	}

	if(name=="Q1pipeLogical") {
		deadint = 2; // Hit Q1 beam pipe
	}

	if(name=="Q2pipeLogical") {
		deadint = 3; // Hit Q2 beam pipe
	}

	if(name=="ED1Wall1Logical" || name=="ED1Wall2Logical" || name=="ED1Wall3Logical") {
		deadint = 4; // Hit ED1 beam pipe
	}

	if(name=="ED1toMDpipeWallLogical") {
		deadint = 5; // Hit beam pipe between EDs and MD
	}

	if(name=="MDWall1Logical" || name=="MDWall2Logical" || name=="MDWall3Logical" || name=="MDWall4Logical" || name=="MDflangeLogical" || name=="MDflange1Logical" || name=="MDflange2Logical" ) {
		deadint = 6; // Hit MD beam pipe
	}

	if(name=="ED2Wall1Logical" || name=="ED2Wall2Logical" || name=="ED2Wall3Logical") {
		deadint = 7; // Hit ED2 beam pipe
	}

	if(name=="Q3pipeLogical") {
		deadint = 8; // Hit Q3 beam pipe
	}

	if(name=="Q4pipeLogical") {
		deadint = 9; // Hit Q4 beam pipe
	}

	if(name=="PGACboxLogical") {
		deadint = 10; // Hit PGAC box
	}

  // terminate event if trajectory hits a wall or slit
  if(deadint!=0)dead=true;	//hit EMMA wall true
  if (dead==true){
#ifdef G4ANALYSIS_USE
    dead_hit->Fill(deadint);	//fill root histogram with component number
#endif // G4ANALYSIS_USE
    G4EventManager::GetEventManager()->AbortCurrentEvent();
  }
  
//----------------------------------------------------------------------------------------------------//
//debugging

  bool debug=false; //make true to print out debugging statements

  if (name!="targetLogical" && debug==true) {
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle(); //pre-step object
    G4ThreeVector worldPosition = preStepPoint->GetPosition(); //location inside world volume
    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
    	//location relative to logical volume
    G4StepPoint* postStepPoint = theStep->GetPostStepPoint(); //post-step object
    G4String name2 = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    G4TouchableHandle theTouchable2 = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition2 = postStepPoint->GetPosition();
    
    G4double locpos[3],dplane1,dplane2,xplane,dx,dz;
    locpos[0]=worldPosition2[0];
    locpos[1]=worldPosition2[1];
    locpos[2]=worldPosition2[2];


  //print x and y locations at exit of each element
  if(name==element[0] && worldPosition[2]<zQ1ends && worldPosition2[2]>zQ1ends){
    G4cout<<"Q1 exit position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;
  }
  if(name==element[1] && worldPosition[2]<zQ2ends && worldPosition2[2]>zQ2ends){
    G4cout<<"Q2 exit position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;
  }
  if(name==element[2]){
    //calculate x and y location wrt optical axis
    
//THE PLANE EQUATIONS NEED TO BE CALCULATED AGAIN!!!  
    dplane1=-tan(20*deg)*worldPosition[0]+worldPosition[2];
    dplane2=-tan(20*deg)*worldPosition2[0]+worldPosition2[2];
    dx=worldPosition[0]+301.5370253*mm; //did calculations based on measurements in EMMA drawings
    dz=worldPosition[2]-2783.48145*mm;
    xplane=sqrt(dx*dx+dz*dz);
    if(dx<0)xplane*=-1;
    if(dplane1<2893.232*mm && dplane2>2893.232*mm){
      //G4cout<<"ED1 exit position in WorldVolume: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      //  <<G4BestUnit(worldPosition[1],"Length")<<G4endl;      
      G4cout<<"ED1 exit position wrt optical axis: "<<G4BestUnit(xplane,"Length")<<", "
        <<G4BestUnit(worldPosition[1],"Length")<<G4endl;      
    }
  }

  if(name==element[3]){

	if(worldPosition[2]<429.612*cm && worldPosition2[2]>429.612*cm) { 
		G4cout<<"MD center position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
        <<G4BestUnit(worldPosition[1],"Length")<<G4endl;
	}

//THE PLANE EQUATIONS NEED TO BE CALCULATED AGAIN!!!    
    /*dplane1=tan(20*deg)*worldPosition[0]+worldPosition[2];
    dplane2=tan(20*deg)*worldPosition2[0]+worldPosition2[2];
    dx=worldPosition[0]+708.3972367*mm; //did calculations based on measurements in EMMA drawings
    dz=worldPosition[2]-4618.63931*mm;
    xplane=sqrt(dx*dx+dz*dz);
    if(dx<0)xplane*=-1;
    if(dplane1<4356.394632*mm && dplane2>4356.394632*mm){
      //G4cout<<"MD exit position in WorldVolume: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      //  <<G4BestUnit(worldPosition[1],"Length")<<G4endl;      
      G4cout<<"MD exit position wrt optical axis: "<<G4BestUnit(xplane,"Length")<<", "
        <<G4BestUnit(worldPosition[1],"Length")<<G4endl;      
    }*/
  }
  if(name==element[4]){
    if(worldPosition[2]<7479.86*mm && worldPosition2[2]>7479.86*mm){
      G4cout<<"ED2 exit position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
        <<G4BestUnit(worldPosition[1],"Length")<<G4endl;      
    }
  }
  if(name==element[5] && worldPosition[2]<zQ3ends && worldPosition2[2]>zQ3ends){
    G4cout<<"Q3 exit position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;
  }
  if(name==element[6] && worldPosition[2]<zQ4ends && worldPosition2[2]>zQ4ends){
    G4cout<<"Q4 exit position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;
  }

  if(name=="hSlits1Logical"){
    G4cout<<"Slit 1 position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;

	dz = 3374.78*mm - worldPosition[2];
	dx = -509.656 - worldPosition[0];

	xplane = sqrt(dx*dx + dz*dz);

	if(dx<0) xplane*=-1;

      std::ofstream outfile(slit1FileName, std::ofstream::app); //Declared in EMMAPrimaryGeneratorAction
      outfile.precision(5);
      outfile << xplane/mm << " " << worldPosition[1]/mm << G4endl;
      outfile.close();
  }

   if(name=="hSlits2Logical"){
    G4cout<<"Slit 2 position: "<<G4BestUnit(worldPosition[0],"Length")<<", "
      <<G4BestUnit(worldPosition[1],"Length")<<G4endl;

	dz = 5224.98*mm - worldPosition[2];
	dx = -506.92 - worldPosition[0];

	xplane = sqrt(dx*dx + dz*dz);

	if(dx<0) xplane*=-1;

      std::ofstream outfile(slit1FileName, std::ofstream::app); //Declared in EMMAPrimaryGeneratorAction
      outfile.precision(5);
      outfile << xplane/mm << " " << worldPosition[1]/mm << " " << worldPosition[0] << " " << worldPosition[2] << G4endl;
      outfile.close();
  }

  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

