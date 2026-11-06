{

  TChain ch2("fphits");
  ch2.Add("../Si32Exp_noApt.root");
  ch2.Add("../Mo93recoils.root");
  ch2.Add("../Kr89recoils.root");
  ch2.Merge("../Sr93Exp.root");

  TFile *file = TFile::Open("../Sr93Exp.root");
  TFile *file1 = TFile::Open("../Si32Exp_noApt.root");
  TFile *file2 = TFile::Open("../Si32Exp_2deg.root");
  //TFile *file3 = TFile::Open("../Sr93beam_2deg.root");
  //TFile *file4 = TFile::Open("../Kr89recoils.root");

  TTree *fphits = (TTree*) file->Get("fphits");
  TTree *fphits1 = (TTree*) file1->Get("fphits");
  TTree *fphits2 = (TTree*) file2->Get("fphits");
  //TTree *fphits3 = (TTree*) file3->Get("fphits");
  //TTree *fphits4 = (TTree*) file4->Get("fphits");

  fphits->SetAlias("ictot", "fp_Edep + fp_Edep2");
  fphits->SetAlias("fptot", "fp_Edep + fp_Edep2 + fp_EdepSilicon");
  fphits1->SetAlias("ictot", "fp_Edep + fp_Edep2");
  fphits1->SetAlias("fptot", "fp_Edep + fp_Edep2 + fp_EdepSilicon");
 // fphits2->SetAlias("ictot", "fp_Edep + fp_Edep2");
 // fphits2->SetAlias("fptot", "fp_Edep + fp_Edep2 + fp_EdepSilicon");
  //fphits3->SetAlias("ictot", "fp_Edep + fp_Edep2");
  //fphits3->SetAlias("fptot", "fp_Edep + fp_Edep2 + fp_EdepSilicon");

//////////////////////////////////////////////////////////////////////////////////////////////////
//				Plot IC front half vs back half					//
//////////////////////////////////////////////////////////////////////////////////////////////////

  TCanvas *c1 = new TCanvas("c1");

  TH2F *icplot1a = new TH2F("icplot1a","IC Energy Front vs Back",200,0,200,200,0,200);
  fphits1->Draw("fp_Edep:fp_Edep2>>icplot1a");
  icplot1a->GetXaxis()->SetTitle("Back (MeV)");
  icplot1a->GetYaxis()->SetTitle("Front (MeV)");
  icplot1a->SetMarkerColor(1);
  icplot1a->SetMarkerStyle(20);

  TH2F *icplot1b = new TH2F("icplot1b","IC Energy Front vs Back",200,0,200,200,0,200);
  fphits2->Draw("fp_Edep:fp_Edep2>>icplot1b","","same");
  icplot1b->SetMarkerColor(2);
  icplot1b->SetMarkerStyle(21); 

  //TH2F *icplot1c = new TH2F("icplot1c","IC Energy Front vs Back",200,0,40,200,0,40);
  //fphits3->Draw("fp_Edep:fp_Edep2>>icplot1c","","same");
  //icplot1c->SetMarkerColor(4);
  //icplot1c->SetMarkerStyle(22);
 
  TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
  leg->SetHeader("Legend"); // option "C" allows to center the header
  leg->AddEntry(icplot1a,"Sr-93 recoils");
  leg->AddEntry(icplot1b,"Mo-93 recoils");
  //leg->AddEntry(icplot1c,"Ne-22 Beam");
  leg->Draw();

//////////////////////////////////////////////////////////////////////////////////////////////////
//				Plot IC front half IC Total					//
//////////////////////////////////////////////////////////////////////////////////////////////////


  TCanvas *c2 = new TCanvas("c2");

  TH2F *icplot2a = new TH2F("icplot2a","IC Energy Front vs IC Total",400,0,400,200,0,200);
  fphits->Draw("fp_Edep:ictot>>icplot2a","","colz");
  icplot2a->GetXaxis()->SetTitle("Total (MeV)");
  icplot2a->GetYaxis()->SetTitle("Front (MeV)");
  icplot2a->SetMarkerColor(1);
  icplot2a->SetMarkerStyle(20);

  TH2F *icplot2b = new TH2F("icplot2b","IC Energy Front vs Back",400,0,400,200,0,200);
  //fphits2->Draw("fp_Edep:fp_Edep2>>icplot2b","","same");
  icplot2b->SetMarkerColor(2);
  icplot2b->SetMarkerStyle(21); 

  TH2F *icplot2c = new TH2F("icplot2c","IC Energy Front vs Back",400,0,400,200,0,200);
 // fphits3->Draw("fp_Edep:fp_Edep2>>icplot2c","","same");
  icplot2c->SetMarkerColor(4);
  icplot2c->SetMarkerStyle(22);

  //leg->Draw();

//////////////////////////////////////////////////////////////////////////////////////////////////
//				Plot IC Total vs Silicon					//
//////////////////////////////////////////////////////////////////////////////////////////////////
/*

  TCanvas *c3 = new TCanvas("c3");

  TH2F *icplot3a = new TH2F("icplot3a","IC Energy Total vs Silicon",200,0,40,400,0,80);
  fphits1->Draw("ictot:fp_EdepSilicon>>icplot3a");
  icplot3a->GetXaxis()->SetTitle("Silicon (MeV)");
  icplot3a->GetYaxis()->SetTitle("IC Total (MeV)");
  icplot3a->SetMarkerColor(1);
  icplot3a->SetMarkerStyle(20);

  TH2F *icplot3b = new TH2F("icplot3b","IC Energy Front vs Back",200,0,40,200,0,40);
  fphits2->Draw("fp_Edep:fp_Edep2>>icplot3b","","same");
  icplot3b->SetMarkerColor(2);
  icplot3b->SetMarkerStyle(21); 

  TH2F *icplot3c = new TH2F("icplot3c","IC Energy Front vs Back",200,0,40,200,0,40);
  fphits3->Draw("fp_Edep:fp_Edep2>>icplot3c","","same");
  icplot3c->SetMarkerColor(4);
  icplot3c->SetMarkerStyle(22);

  leg->Draw();

//////////////////////////////////////////////////////////////////////////////////////////////////
//				Plot IC Front vs Toal Edep					//
//////////////////////////////////////////////////////////////////////////////////////////////////

  TCanvas *c4 = new TCanvas("c4");

  TH2F *icplot4a = new TH2F("icplot4a","IC Energy Front vs IC+Silicon",800,0,160,200,0,40);
  fphits1->Draw("fp_Edep:fptot>>icplot4a");
  icplot4a->GetXaxis()->SetTitle("Total (MeV)");
  icplot4a->GetYaxis()->SetTitle("IC Front (MeV)");
  icplot4a->SetMarkerColor(1);
  icplot4a->SetMarkerStyle(20);

  TH2F *icplot4b = new TH2F("icplot4b","IC Energy Front vs Back",200,0,40,200,0,40);
  fphits2->Draw("fp_Edep:fp_Edep2>>icplot4b","","same");
  icplot4b->SetMarkerColor(2);
  icplot4b->SetMarkerStyle(21); 

  TH2F *icplot4c = new TH2F("icplot4c","IC Energy Front vs Back",200,0,40,200,0,40);
  fphits3->Draw("fp_Edep:fp_Edep2>>icplot4c","","same");
  icplot4c->SetMarkerColor(4);
  icplot4c->SetMarkerStyle(22);

  leg->Draw();
 */
//////////////////////////////////////////////////////////////////////////////////////////////////
//					Plot Hit Pos						//
//////////////////////////////////////////////////////////////////////////////////////////////////

	TCanvas *c5 = new TCanvas("c5");
	TH1F *Si32hitpos = new TH1F("Si32hitpos","X Position",166,-83,83);
	fphits1->Draw("fp_pos[0]>>Si32hitpos");
	Si32hitpos->SetLineColor(1);
	//TH1F *Mo93hitpos = new TH1F("Mo93hitpos","X Position",166,-83,83);
	//fphits2->Draw("fp_pos[0]>>Mo93hitpos","","same");
	//Mo93hitpos->SetLineColor(2);
	//TH1F *beamhitpos = new TH1F("beamhitpos","X Position",166,-83,83);
	//fphits3->Draw("fp_pos[0]>>beamhitpos","","same");
	//beamhitpos->SetLineColor(2);
	//TH1F *Kr89hitpos = new TH1F("Kr89hitpos","X Position",166,-83,83);
	//fphits4->Draw("fp_pos[0]>>Kr89hitpos","","same");
	//Kr89hitpos->SetLineColor(3);

	Si32hitpos->GetXaxis()->SetTitle("X Position (mm)");
	Si32hitpos->GetYaxis()->SetTitle("Counts / mm");

}
