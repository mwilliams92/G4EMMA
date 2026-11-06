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
// $Id: EMMAPrimaryGeneratorAction.cc,v 1.5 2006-06-29 16:33:05 gunter Exp $
// --------------------------------------------------------------
//

#include "EMMAPrimaryGeneratorAction.hh"
#include "EMMAPrimaryGeneratorMessenger.hh"
#include "EMMANuclearReactionTwoBody.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
#include "G4NucleiProperties.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"

#include <string> //words and sentences
#include <fstream> //Stream class to both read and write from/to files
#include <sstream>
using namespace std;


// global variables 
G4bool prepareBeam = true;
G4String inTargetFileName;
G4String postTargetFileName;
G4String focalPlaneFileName;
G4double userCharge = 54.; // default value
G4String postDegrader1FileName;
G4String postDegrader2FileName;
G4String slit1FileName;
G4double depth;
G4double d_depth;

G4String particleName;

G4double EMMAPrimaryGeneratorAction::targetEkin;
G4double EMMAPrimaryGeneratorAction::targetX;
G4double EMMAPrimaryGeneratorAction::targetY;
G4double EMMAPrimaryGeneratorAction::targetXdir;
G4double EMMAPrimaryGeneratorAction::targetYdir;

G4double EMMAPrimaryGeneratorAction::Eject_LabTheta;
G4double EMMAPrimaryGeneratorAction::Eject_LabEnergy;

G4double EMMAPrimaryGeneratorAction::Recoil_LabTheta;
G4double EMMAPrimaryGeneratorAction::Recoil_LabEnergy;

EMMAPrimaryGeneratorAction::EMMAPrimaryGeneratorAction()  // constructor
{
	sigmaEnergy = 0.*MeV;
	transEmittance = 0.*mm*mrad;

	G4int n_particle = 1;
	//G4ParticleGun class generates primary particle(s) with a given momentum and position
	particleGun  = new G4ParticleGun(n_particle);

	//create a messenger for this class
	gunMessenger = new EMMAPrimaryGeneratorMessenger(this);

	// default particle kinematics
	particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.*m));
	particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

	G4Step aStep;
	
	energy = aStep.GetTotalEnergyDeposit(); // =0
	Angle = 0;

	// default values
	nEvents = 1e6;
	fZ1=0.,fA1=0., fZ2=0.,fA2=0.;
	fZ3=0.,fA3=0., fZ4=0.,fA4=0.;
	fqmin = 180.*deg;
	fqmax = 0.*deg;
	fCharge3 = 0.;

	inTargetFileName = UserDir + "/BeamSampling/beam.dat";  //Used in EMMASteppingAction

	// alpha-source input file
	useAlphaSource = false;
	G4String text, line;
	ifstream inputfil;
	G4String filename = UserDir + "/UserInput/alphaSource.dat";
	inputfil.open ( filename, ios::in );
	if ( inputfil.is_open() ) {
	  int n=0;
	  while ( inputfil.good() ) {
	    inputfil >> text;
	    if (text=="#") { // skip comments
	      getline (inputfil,line);
	    }
	    else {
	      n = n+1;
	      if (n==1) {if (text=="YES") useAlphaSource = true;}
	      if (n==2) {energyAlphaSource = atof(text.c_str());} 
	      if (n==3) {maxAngleAlphaSource = atof(text.c_str());}
	    }
	  }
	  inputfil.close();
	}
	else G4cout << "Unable to open " << filename << G4endl; 

	std::ofstream outfile;
  	slit1FileName = UserDir;
  	slit1FileName.append("Results/slitpos.dat"); //Used in EMMASteppingAction
  	outfile.open (slit1FileName);
 	outfile.close();
  	G4cout << "Initialized slit position file" << G4endl;

}

EMMAPrimaryGeneratorAction::~EMMAPrimaryGeneratorAction()
{
  delete particleGun;	//must delete G4ParticleGun
  delete gunMessenger;
}

void EMMAPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ // This method is invoked at the beginning of each event

  G4double randNumb;
  if (prepareBeam) randNumb = G4UniformRand(); //used to chose a random reaction depth
  else randNumb = 1.0;
  
  
  // this is to update target thickness
  // <><><><><><><><><><><><><><><><><><><><> //
  depth = targetThickness * randNumb;
  d_depth = 0.001*um; // Add very small bit of target to stop crashes.
  G4VSolid* targetSolid = new G4Box("target",5.*cm,5.*cm,d_depth+depth/2.0);
  G4LogicalVolumeStore* logVolStore = G4LogicalVolumeStore::GetInstance();	
  G4LogicalVolume* target = logVolStore->GetVolume("targetLogical",true); //"targetLogical" declared in DetectorConstruction
  target->SetSolid(targetSolid);
  G4RunManager::GetRunManager()->ReOptimize( target );
  // <><><><><><><><><><><><><><><><><><><><> //
		
  G4double Ekin;
  G4ParticleDefinition* particleDef;


  // to simulate just an isotropic alpha source
  if (useAlphaSource) {	//boolean determined from alphaSource.dat input file
    simulateReaction = false;

    // Ion (values read in from alphaSource.dat in constructor)
    /* 
       NB: The simulation terminates with a bus error if we pick an alpha particle, so instead
       we "cheat" and use a 6Li(3+) ion instead, while scaling the kinetic energy by a factor of 1.5
    */
    particleDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(3,6,0.0); //FindParticle("alpha");
    particleGun->SetParticleDefinition(particleDef);
    // charge
    userCharge = 3; // userCharge is a global variable! (used e.g. in BGFields1-7.cc)
    particleGun->SetParticleCharge(userCharge);    
    // Energy
    Ekin = energyAlphaSource * 1.5;
    particleGun->SetParticleEnergy(Ekin *MeV);
    // PositionAngle
    G4double xBeam=0.*m, yBeam=0.*m;
    G4double xBeam0=-1.5*mm, yBeam0=-1.5*mm;
    G4double SourceDiameter = 3*mm;
    xBeam = xBeam0 + G4UniformRand() * SourceDiameter;
    yBeam = yBeam0 + G4UniformRand() * SourceDiameter;
    
    particleGun->SetParticlePosition(G4ThreeVector(xBeam,yBeam,0.0*m));

    // Sample angle
    G4double x, y, z;
    G4double costh = 1.0 - G4UniformRand() * (1-std::cos(maxAngleAlphaSource*CLHEP::pi/180));
    G4double theta = std::acos(costh);
    G4double phi = G4UniformRand()*CLHEP::twopi;
    x = std::sin(theta) * std::cos(phi);
    y = std::sin(theta) * std::sin(phi);
    z = std::cos(theta);
    particleGun->SetParticleMomentumDirection(G4ThreeVector(x,y,z));

  }


  // BEAM
  if (simulateReaction==false && !useAlphaSource) {
    // Ion
    particleDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(beamZ,beamA,0.0);
    particleGun->SetParticleDefinition(particleDef);
    particleGun->SetParticleCharge(userCharge);    
    // Sample energy
    Ekin = energy;
    if (sigmaEnergy>0.) { 
      G4double mean = energy;
      G4double FWHM = sigmaEnergy/100.*energy;
      G4double std = FWHM/2.35; 
      Ekin = CLHEP::RandGauss::shoot(mean,std);
    }
//---------------------------------------------------------------------------------------//
    //energy including spread
    particleGun->SetParticleEnergy(Ekin *MeV);
    //fixed energy
    //particleGun->SetParticleEnergy(energy *MeV);
//---------------------------------------------------------------------------------------//
    // Sample position
    G4double xBeam=0.*m, yBeam=0.*m;
    G4double xBeam_offset=0.*mm, yBeam_offset=0.*mm;
    G4double r=0.*mm;
    G4double rmax = beamSpotDiameter / 2.0;
    if (beamSpotDiameter>0.) {
      r = G4UniformRand() * rmax;
      G4double phi = G4UniformRand()*CLHEP::twopi;
      xBeam = r*std::cos(phi) + xBeam_offset;
      yBeam = r*std::sin(phi) + yBeam_offset;
    }
    // beam particle z emission location is set to 10 Angstroms, i.e. immediately, in front of the target
    G4double zemit = targetZoffset - targetThickness/2 - 10*angstrom;

//---------------------------------------------------------------------------------------//
    //random emittance off optical axis
    particleGun->SetParticlePosition(G4ThreeVector(xBeam,yBeam,zemit));
    //fix emittance location to optical axis
    //particleGun->SetParticlePosition(G4ThreeVector(0,0,zemit));
//---------------------------------------------------------------------------------------//
    // Determine max angle from normalized transverse emittance
    G4double mass = particleDef->GetPDGMass();
    G4double Etot = Ekin + mass;
    G4double gamma = Etot/mass;
    G4double beta = sqrt(1.0-1.0/(gamma*gamma));
    G4double MaxAngle = 0.0*deg;
    if (rmax>0) {
      MaxAngle = transEmittance/(gamma*beta*rmax);
      MaxAngle = MaxAngle * 180./CLHEP::pi/1000. * deg; // mrad to deg conversion
    }
        G4cout << "rmax = " << rmax << G4endl;
	G4cout << "RADIUS: " << rmax/mm << " mm" << G4endl;
        G4cout << "ANGLE: " << MaxAngle/deg << " deg" << G4endl;

    // Sample angle
    //G4double x=0., y=0., z=1.;
    //G4double theta = 0.0*deg; G4double THETA = 0.0*deg;
    //G4double phi = 0.0*deg;
//---------------------------------------------------------------------------------------//
   
 //fixed angles
/*
    G4double maxAngle = 2*deg;
    //G4double costh2 = 1.0 - G4UniformRand() * (1-std::cos(maxAngle));
    //G4double theta2 = std::acos(costh2);
    G4double theta = maxAngle;
    G4double phi = G4UniformRand()*CLHEP::twopi;
    G4double x = sin(theta) * cos(phi);
    G4double y = sin(theta) * sin(phi);
    G4double z = cos(theta);
*/
/*
	// Random Sample of Horizontal and vertical angles +/- 1.2 deg
	G4double MaxAngleThetaPrime = 6.*pi/180.; // full = 6, other = 2.4
	G4double MaxAnglePhiPrime = 6.*pi/180.;
	G4double thetaprime = (G4UniformRand() * MaxAngleThetaPrime) - 3.0*pi/180;  // Full = -3, Top/centre,bottom=-1.2, right = +0.6, left = -3 
	//G4double thetaprime = 0.*pi/180;
	G4double phiprime = (G4UniformRand() * MaxAnglePhiPrime) - 3.0*pi/180;	// Full =-3, Top=+0.6, right/centre/left = -1.2, bottom=-3
	//G4double phiprime = 2.*pi/180;
	G4double theta = sqrt(pow(thetaprime,2)+pow(phiprime,2));
	G4double phi;

	if (phiprime>0) {	
		phi = acos(thetaprime/theta);
	}
	else {
		phi = acos(thetaprime/theta) + pi; //+ pi and no -1 if top, bottom, center or full. no + pi and add *-1 if left or right.
	}

	//G4cout << "Horizonal Angle = " << thetaprime*180./pi << G4endl;
	//G4cout << "Verical Angle = " << phiprime*180./pi << G4endl;
	//G4cout << "Theta Angle = " << theta*180./pi << G4endl;
	//G4cout << "Phi Angle = " << phi*180./pi << G4endl;

	G4double x = sin(theta) * cos(phi);
    	G4double y = sin(theta) * sin(phi);
    	G4double z = cos(theta);
*/

 //random angles

    G4double theta, phi, x, y, z;

    if (MaxAngle>0.) {
      theta = G4UniformRand() * MaxAngle * sqrt(1.0-(r/rmax)*(r/rmax));
      phi = G4UniformRand()*CLHEP::twopi;

      x = sin(theta); 
      y = sin(theta) * sin(phi);
      z = cos(theta);
    }

    particleGun->SetParticleMomentumDirection(G4ThreeVector(x,y,z));
    
    G4cout<<"Prim.Gen.Action output "<<" Energy(MeV)= "<< energy <<" z emission location (mm) "
          << zemit/mm << " theta (deg)= "<< theta/deg <<" phi(deg)= "<< phi/deg <<G4endl;

    G4cout<<"Momentum Dir [x,y,z]: ["<< x <<","<< y << "," << z << "]" << G4endl;
   

//---------------------------------------------------------------------------------------//
  }


  // REACTION (values read in from beam.dat in EMMAapp)
  else if (simulateReaction) {
    G4int id=anEvent->GetEventID();
    Ekin = energyBeam[id]; //from initializeReactionSimulation()
    G4ThreeVector dir(dirxBeam[id],diryBeam[id],dirzBeam[id]);	
 
    G4double Eejct;
    G4ThreeVector dir_ejct;

    if (fZ1==0.) {
      G4cout << "ERROR: Two-body reaction not defined" << G4endl;
      exit (EXIT_FAILURE);
    }
    // Pass beam energy and direction to Two body Calculator
    simulateTwoBodyReaction( Ekin, dir, Eejct, dir_ejct); // I think I added the Eejct and dir_ejct

    // Grab product particle IDs and ejectile kinematics. Recoil Kinematics assigned to pre-existing Ebeam and dir 
    G4int Z3=fZ3, A3=fA3; 
    G4int Z4=fZ4, A4=fA4;
/*
	G4String reaction = "reaction";
	EMMANuclearReactionTwoBody(reaction, Z3, A3, Z4, A4);
	particleDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z1,A1,0.0);
	G4LorentzVector fourMomentum(dirxBeam[id],diryBeam[id],dirzBeam[id],energyBeam[id]);
	G4ThreeVector position(posxBeam[id]*mm, y=posyBeam[id]*mm, z=poszBeam[id]*mm);
    G4HadProjectile* hadProjectile = new G4HadProjectile(particleDef, fourMomentum, position);
	G4HadFinalState* EMMANuclearReactionTwoBody::ApplyYourself(hadProjectile, targetNucleus)
*/


    // Location of particle gun same for recoils and ejectiles
    G4double x=posxBeam[id]*mm, y=posyBeam[id]*mm, z=poszBeam[id]*mm;
    G4double dz=depth/2.;
    z = z-dz; // correction needed because target placement refers to center of target ...
    particleGun->SetParticlePosition(G4ThreeVector( x, y, z ));

    // Set Particle Gun to Ejectiles and Fire Away!
    //if (Z4==0)	{	particleDef = G4ParticleTable::GetParticleTable()->FindParticle("neutron");	} // find neutron
    //else {	particleDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z4,A4,0.0);  		} // Create new ion
    //particleGun->SetParticleDefinition(particleDef);
    //particleGun->SetParticleEnergy(Eejct*MeV);
    //particleGun->SetParticleMomentumDirection(dir_ejct);
    //particleGun->SetParticleCharge(Z4);
    
    //particleGun->GeneratePrimaryVertex(anEvent); // Generate Light Particles

    //G4bool printInfo=true;
    //G4double mass_ejct = particleDef->GetPDGMass();
    //G4double pp_ejct = std::sqrt((Eejct + mass_ejct)*(Eejct + mass_ejct)-(mass_ejct*mass_ejct));
    //G4double charge_ejct = particleGun->GetParticleCharge();	
    //mass_ejct = mass_ejct / 931.4940954;  // convert to amu
    //if (printInfo) {
    //  G4cout << "\n Momentum " << pp_ejct << ", Mass " << mass_ejct << " amu, Ekin " 
	//   << Eejct << " MeV, Charge " << charge_ejct << G4endl;
    //}

    //Eject_LabTheta = dir_ejct.theta()/deg;
    //Eject_LabEnergy = Eejct;

    // Reset ParticleGun to Recoils
    G4double Ex = fExcitationEnergy3;
    particleDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z3,A3,Ex);  // Create new ion
    particleGun->SetParticleDefinition(particleDef);
    particleGun->SetParticleEnergy(Ekin*MeV);
    particleGun->SetParticleMomentumDirection(dir);
    particleGun->SetParticleCharge(userCharge);

    Recoil_LabTheta = dir.theta()/deg;
    Recoil_LabEnergy = Ekin;

  }
  
  particleGun->GeneratePrimaryVertex(anEvent); // would need multiple of these to generate protons, alphas, tritons etc from transfer

  // Print info:
  G4bool printInfo=true;
  G4double mass = particleDef->GetPDGMass();
  G4double pp = std::sqrt((Ekin + mass)*(Ekin + mass)-(mass*mass));
  G4double charge = particleGun->GetParticleCharge();	
  mass = mass / 931.4940954;  // convert to amu
  if (printInfo) {
    G4cout << "\n Momentum " << pp << ", Mass " << mass << " amu, Ekin " 
	   << Ekin << " MeV, Charge " << charge << G4endl;
  }

  targetEkin = particleGun->GetParticleEnergy();
  targetX = particleGun->GetParticlePosition().getX();
  targetY = particleGun->GetParticlePosition().getY();
  targetXdir = particleGun->GetParticleMomentumDirection().getX();
  targetYdir = particleGun->GetParticleMomentumDirection().getY();

}

void EMMAPrimaryGeneratorAction::initializeAlphaSimulation()
{
  //std::ofstream outfile;
  //slit1FileName = UserDir;
  //slit1FileName.append("Results/slit1Position.dat"); //Used in EMMASteppingAction
  //outfile.open (slit1FileName);
  //outfile.close();
  //G4cout << "Initialized xslit20 position file" << G4endl;
}

void EMMAPrimaryGeneratorAction::initializeReactionSimulation() // called using /mydet/doReaction
{
  prepareBeam = false;
  simulateReaction = true;
  userCharge = fCharge3; //read in from reaction.dat in EMMAapp
  std::ofstream outfile;
  focalPlaneFileName = UserDir;
  focalPlaneFileName.append("/Results/fp_reaction.dat"); //Used in EMMADriftChamberHit
  outfile.open (focalPlaneFileName);
  outfile.close();
  postTargetFileName = UserDir;
  postTargetFileName.append("/Results/postTarget_reaction.dat"); //Used in EMMASteppingAction
  outfile.open (postTargetFileName);
  outfile.close();
  postDegrader1FileName = UserDir;
  postDegrader1FileName.append("/Results/postDegrader1_reaction.dat"); //Used in EMMASteppingAction
  outfile.open (postDegrader1FileName);
  outfile.close();
  postDegrader2FileName = UserDir;
  postDegrader2FileName.append("/Results/postDegrader2_reaction.dat"); //Used in EMMASteppingAction
  outfile.open (postDegrader2FileName);
  outfile.close();

  // read in previously simulated data
  energyBeam = new G4double[nEvents];
  posxBeam = new G4double[nEvents];
  posyBeam = new G4double[nEvents];
  poszBeam = new G4double[nEvents];
  dirxBeam = new G4double[nEvents];
  diryBeam = new G4double[nEvents];
  dirzBeam = new G4double[nEvents];
  std::ifstream beamFile(inTargetFileName, std::ios::in); //read in from /BeamSampling/beam.dat
  for (int i=0; i<nEvents; i++) {
    beamFile >> energyBeam[i];
    beamFile >> posxBeam[i];
    beamFile >> posyBeam[i];
    beamFile >> poszBeam[i];
    beamFile >> dirxBeam[i];
    beamFile >> diryBeam[i];
    beamFile >> dirzBeam[i];
  }
  beamFile.close();	

  // simulated nEvents
  G4RunManager::GetRunManager()->BeamOn(nEvents);
}



void EMMAPrimaryGeneratorAction::initializeBeamSimulation() // called using /mydet/doBeam
{
  prepareBeam = false;
  simulateReaction = false;
  userCharge = beamCharge; //read in from beam.dat in EMMAapp
  std::ofstream outfile;
  focalPlaneFileName = UserDir;
  focalPlaneFileName.append("/Results/fp_beam.dat"); //Used in EMMADriftChamberHit
  outfile.open (focalPlaneFileName);
  outfile.close();	  
  postTargetFileName = UserDir;
  postTargetFileName.append("/Results/postTarget_beam.dat"); //Used in EMMASteppingAction
  outfile.open (postTargetFileName);
  outfile.close();
  postDegrader1FileName = UserDir;
  postDegrader1FileName.append("/Results/postDegrader1_beam.dat"); //Used in EMMASteppingAction
  outfile.open (postDegrader1FileName);
  outfile.close();
  postDegrader2FileName = UserDir;
  postDegrader2FileName.append("/Results/postDegrader2_beam.dat"); //Used in EMMASteppingAction
  outfile.open (postDegrader2FileName);
  outfile.close();
  // simulated nEvents
  G4RunManager::GetRunManager()->BeamOn(nEvents);
}

void EMMAPrimaryGeneratorAction::initializeBeamPreparation() // called using /mydet/doPrepare
{
  prepareBeam = true;
  simulateReaction = false;
  userCharge = beamCharge; //read in from beam.dat in EMMAapp
  std::ofstream outfile; 
  outfile.open (inTargetFileName); //declared in constructor
  outfile.close();

  // simulated nEvents
  G4RunManager::GetRunManager()->BeamOn(nEvents);
}



void EMMAPrimaryGeneratorAction::simulateTwoBodyReaction( G4double &Ebeam, G4ThreeVector &dir, G4double &Eejct, G4ThreeVector &dir_ejct) 
{

  // Z and A of projectile and target (1+2):
  G4int Z1 = fZ1;
  G4int A1 = fA1;
  G4int Z2 = fZ2;
  G4int A2 = fA2;

  // masses of 1+2
  G4double m1 = G4NucleiProperties::GetNuclearMass(A1, Z1);
  G4double m2 = G4NucleiProperties::GetNuclearMass(A2, Z2);

  // Z and A of reaction products (3+4):
  G4int Z3 = fZ3;
  G4int A3 = fA3;
  G4int Z4 = fZ4;
  G4int A4 = fA4;

  // masses of 3+4
  G4double m3 = G4NucleiProperties::GetNuclearMass(A3, Z3);
  G4double m4 = G4NucleiProperties::GetNuclearMass(A4, Z4);

  // take into account excitation energy of fragment 3
  m3 = m3 + fExcitationEnergy3;

  // 4-momentum of beam
  G4double p1n = sqrt( (m1+Ebeam)*(m1+Ebeam) - m1*m1 );
  G4double dirn = sqrt( dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] );
  G4ThreeVector p1 = dir/dirn*p1n;

  // determine velocity of CM frame relative to LAB frame
  G4LorentzVector lv1(p1,m1+Ebeam);
  G4LorentzVector lv2(0.0,0.0,0.0,m2);   
  G4LorentzVector lv = lv1 + lv2;
  G4ThreeVector bst = lv.boostVector(); // divides the spatial component (p) by the time component (E)

  // transform 4-momenta to CM frame
  lv1.boost(-bst);
  lv2.boost(-bst);
  G4double etot = lv1[3] + lv2[3]; // total energy in CM

  // if energy is insufficient, nothing happens
  if (etot<m3+m4) {
    G4cout << "NOT ENOUGH ENERGY FOR REACTION" << G4endl;
	G4EventManager::GetEventManager()->AbortCurrentEvent();
  }

  // Compute c.m. energies and momentum of reaction products (3+4)
  G4double e3 = ( etot*etot + m3*m3 - m4*m4 ) / (2*etot); 
  G4double e4 = etot - e3; 
  G4double pcm = sqrt( e3*e3 - m3*m3 ); 

  // Max and min angles
  G4double fqrmax = (180-fqmin/deg)*deg; //compute recoil c.m. angles from ejectile c.m. angles
  G4double fqrmin = (180-fqmax/deg)*deg;
  G4double thetaCMmin = fqrmin;
  G4double t1 = std::cos(thetaCMmin/rad);
  G4double thetaCMmax = fqrmax;
  G4double t2 = std::cos(thetaCMmax/rad);
  
  // Sampling of directions in CM system
  G4double t    = G4UniformRand();
  G4double phi  = G4UniformRand()*CLHEP::twopi;
  G4double cost = t1 - (t1-t2)*t;
  G4double sint = std::sqrt((1.0-cost)*(1.0+cost));
  
  // Lorentz vectors of reaction products (3+4)
  G4ThreeVector v3(sint*std::cos(phi),sint*std::sin(phi),cost);
  v3 = v3 * pcm;
  G4ThreeVector v4 = -v3;
  G4LorentzVector lv3(v3.x(),v3.y(),v3.z(),e3);
  G4LorentzVector lv4(v4.x(),v4.y(),v4.z(),e4);
  
  // Transform to LAB frame
  lv3.boost(bst);
  lv4.boost(bst);
    
  // Kinetic energy in lab of product #3
  Ebeam = lv3[3] - m3;

  // Momentum in lab of product #3
  dir[0] = lv3[0];
  dir[1] = lv3[1];
  dir[2] = lv3[2];

  // Kinetic energy in lab of product #4
  Eejct = lv4[3] - m4;

  // Momentum in lab of product #4
  dir_ejct[0] = lv4[0];
  dir_ejct[1] = lv4[1];
  dir_ejct[2] = lv4[2];

}
