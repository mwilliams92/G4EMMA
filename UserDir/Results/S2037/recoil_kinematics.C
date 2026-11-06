{

  TFile *file=TFile::Open("./Kr86an_beam.root");
  //TFile *file2=TFile::Open("./Kr86an_Ex6MeV.root");
  //TFile *file3=TFile::Open("./Kr86an.root");
  
  TTree *t1 = (TTree*) file->Get("targetplane");
  //TTree *t1b = (TTree*) file2->Get("targetplane");
  //TTree *t1c = (TTree*) file3->Get("targetplane");

  Double_t Ekin_target, Ekin, Xang, Yang, Xang_deg, Yang_deg;

  Double_t r2d = TMath::RadToDeg();

  t1->SetBranchAddress("target_Ekint",&Ekin_target);
  t1->SetBranchAddress("post_degrader_Ekint",&Ekin);
  t1->SetBranchAddress("post_degrader_x_angle",&Xang);
  t1->SetBranchAddress("post_degrader_y_angle",&Yang);

  gStyle->SetLineWidth(2);

  TCanvas *c1 = new TCanvas();

  TH2F *h1 = new TH2F("h1","^{86}Kr(#alpha,n)^{89}Sr",2000,-100,100,2000,-100,100);

  t1->SetAlias("Theta","post_degrader_x_angle*TMath::RadToDeg()");
  t1->SetAlias("Phi","post_degrader_y_angle*TMath::RadToDeg()");

  h1->GetXaxis()->SetTitle("#theta [0.1 ^{o} / bin]");
  h1->GetYaxis()->SetTitle("#phi [0.1 ^{o} / bin]");

  h1->SetTitleSize(0.08);
  h1->GetXaxis()->SetTitleSize(0.07); h1->GetXaxis()->SetLabelSize(0.05); h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->SetTitleSize(0.07); h1->GetYaxis()->SetLabelSize(0.05); h1->GetYaxis()->CenterTitle();
  h1->GetZaxis()->SetLabelSize(0.05);

  h1->GetXaxis()->SetRangeUser(-10,10);
  h1->GetYaxis()->SetRangeUser(-10,10);

  c1->SetTicky(); c1->SetTickx();
  c1->SetLeftMargin(0.17); c1->SetBottomMargin(0.17);

  t1->Draw("Phi:Theta>>h1","post_degrader_Ekint>10.","colz");
  //t1->Draw("Phi:Theta>>h1","target_Ekint>225 && target_Ekint<264","colz");

  TCanvas *c2 = new TCanvas();

  TH1F *h2 = new TH1F("h2","h2",2000,0,200);
  //TH1F *h2b = new TH1F("h2b","h2b",2000,0,200);
  //TH1F *h2c = new TH1F("h2c","h2c",2000,0,200);
  t1->Draw("post_degrader_Ekint>>h2");
  //t1b->Draw("post_degrader_Ekint>>h2b","","same");
  //t1c->Draw("post_degrader_Ekint>>h2c","","same");

  //h2->SetLineColor(1); h2c->SetLineColor(2); h2b->SetLineColor(4);


  int Nentries = t1->GetEntries();

  ofstream outfile;
	outfile.open ("recoil_kinematics.dat");

  for(int i=0; i<Nentries; i++)	{

	t1->GetEntry(i);

	//if (Ekin_target<225. || Ekin_target>265. ) { continue; }
	if (Ekin<1. ) { continue; }

	Xang_deg = Xang*r2d;
        Yang_deg = Yang*r2d;

	outfile << Ekin << " " << Xang_deg << " " << Yang_deg << endl;	

  }	

}
