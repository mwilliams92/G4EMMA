{

	TFile *file=TFile::Open("./Sr94an.root");
  	TTree *t1 = (TTree*) file->Get("targetplane");

	//t1->SetBranchAddress("target_Ekint",&Ekin_target);
  	//t1->SetBranchAddress("post_degrader_Ekint",&Ekin);
  	//t1->SetBranchAddress("post_degrader_x_angle",&Xang);
  	//t1->SetBranchAddress("post_degrader_y_angle",&Yang);

	t1->SetAlias("Theta","post_degrader_x_angle*TMath::RadToDeg()");
  	t1->SetAlias("Phi","post_degrader_y_angle*TMath::RadToDeg()");

  	gStyle->SetLineWidth(2);

  	TCanvas *c1 = new TCanvas();
	c1->SetTicky(); c1->SetTickx();
  	c1->SetLeftMargin(0.17); c1->SetBottomMargin(0.17);

	TH1F *h1 = new TH1F("h1"," ",150,0,150);

	h1->GetXaxis()->SetTitle("Energy [MeV]");
  	h1->GetYaxis()->SetTitle("Counts [1 MeV / bin]");
	h1->GetXaxis()->SetTitleSize(0.05); h1->GetXaxis()->SetLabelSize(0.05); h1->GetXaxis()->CenterTitle(); h1->GetXaxis()->SetTitleOffset(1.2);
  	h1->GetYaxis()->SetTitleSize(0.05); h1->GetYaxis()->SetLabelSize(0.05); h1->GetYaxis()->CenterTitle();
	h1->GetXaxis()->SetRangeUser(80,150);
  	//h1->GetYaxis()->SetRangeUser(-10,10);

	h1->SetLineColor(1); h1->SetLineWidth(2);
	t1->Draw("post_degrader_Ekint>>h1");

	TCanvas *c2 = new TCanvas();
	c2->SetTicky(); c2->SetTickx();
  	c2->SetLeftMargin(0.17); c2->SetBottomMargin(0.17);

	TH2F *h2 = new TH2F("h2"," ",100,-10,10,100,-10,10);

  	h2->GetXaxis()->SetTitle("#theta [0.2 ^{o} / bin]");
  	h2->GetYaxis()->SetTitle("#phi [0.2 ^{o} / bin]");

  	h2->SetTitleSize(0.08);
  	h2->GetXaxis()->SetTitleSize(0.07); h2->GetXaxis()->SetLabelSize(0.05); h2->GetXaxis()->CenterTitle();
  	h2->GetYaxis()->SetTitleSize(0.07); h2->GetYaxis()->SetLabelSize(0.05); h2->GetYaxis()->CenterTitle();
  	h2->GetZaxis()->SetLabelSize(0.05);

  	h2->GetXaxis()->SetRangeUser(-4,4);
  	h2->GetYaxis()->SetRangeUser(-4,4);

  	t1->Draw("Phi:Theta>>h2","post_degrader_Ekint>100. && post_degrader_Ekint<130.","colz");



}