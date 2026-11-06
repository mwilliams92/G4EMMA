{
	TTree *MyTree = new TTree("MyTree", "MyTree");
	MyTree->ReadFile("ArgonAng.txt", "Energy_1:Angle_1");
	TTree *MyTree2 = new TTree("MyTree2", "MyTree2");
	MyTree2->ReadFile("Silicon32Output_beam.txt","Energy_1:Angle_1");
	
	TCanvas *c2 = new TCanvas("c2");

	MyTree->SetAlias("Ang","180*TMath::ACos(Angle_1)/TMath::Pi()");
	MyTree->SetAlias("Ene","Energy_1/1E6");

	TH2D *h3 = new TH2D("h3","^{31}Si(d,p)^{32}Si Experiment (13.5#mum Au Degrader)",300,0,30,90,0,90);
	h3->GetXaxis()->SetTitle("Post-Target Energy (MeV)");
	h3->GetYaxis()->SetTitle("Post-Target Angle (deg)");
	MyTree->Draw("Ang:Ene>>h3","","colz");
	h3->SetMarkerStyle(20);
	h3->SetMarkerColor(1);

	TCanvas *c3 = new TCanvas("c3");

	c3->SetLogy();

	TH1D *h4 = new TH1D("h4","Simulated Post-Target Beam Energy", 250,0,25);
	TH1D *h6 = new TH1D("h6","Simulated Post-Target Beam Energy", 250,0,25);
	h4->GetXaxis()->SetTitle("Energy (MeV)");
	h4->GetYaxis()->SetTitle("Counts");
	MyTree->Draw("Ene>>h4");	
	MyTree->Draw("Ene>>h6","Ang<3.6","same");
	h4->SetLineColor(1);
	h6->SetLineColor(1);
	h6->SetFillColor(kGray);

	TArrow *ar1 = new TArrow(15.0,1440.,17.5,1440.,0.03,"|>");
	ar1->SetAngle(40);
	ar1->SetLineWidth(2);
	ar1->Draw();

	TArrow *ar2 = new TArrow(20.8,1440.,18.3,1440.,0.03,"|>");
	ar2->SetAngle(40);
	ar2->SetLineWidth(2);
	ar2->Draw();

	TLatex *txt1 = new TLatex(22.0,2200.,"FWHM = 0.8 MeV");
	txt1->SetTextAlign(22);
   	txt1->SetTextColor(1);
   	txt1->SetTextFont(43);
   	txt1->SetTextSize(15);
   	txt1->Draw();

	TLatex *txt2 = new TLatex(21.7,1000.,"#DeltaE/E #approx 5 %");
	txt2->SetTextAlign(22);
   	txt2->SetTextColor(1);
   	txt2->SetTextFont(43);
   	txt2->SetTextSize(15);
   	txt2->Draw();

	TLegend *leg2 = new TLegend(0.6,0.7,0.9,0.9);
  	//leg2->SetHeader(" "); // option "C" allows to center the header
  	leg2->AddEntry(h4,"Raw distribution","l");
  	leg2->AddEntry(h6,"#theta < 3.6^{o}","f");
	leg2->SetBorderSize(0);
  	leg2->Draw();

	TCanvas *c4 = new TCanvas("c4");

	TH1D *h5 = new TH1D("h5","Simulated Post-Target Beam Angle", 200,0,20);
	//TH1D *h7 = new TH1D("h7","Simulated Post-Target Beam Angle", 200,0,20);
	h5->GetXaxis()->SetTitle("Angle (degrees)");
	h5->GetYaxis()->SetTitle("Counts");
	MyTree->Draw("Ang>>h5");
	//MyTree->Draw("Ang>>h7","Ang<3.6","same");
	h5->SetLineColor(1);
	//h7->SetLineColor(1);
	//h7->SetFillColor(kGray);


	TLine *l1 = new TLine(3.6,0.0,3.6,1300.);
	l1->SetLineColor(1);
	l1->SetLineStyle(9);
	l1->SetLineWidth(2);
	l1->Draw();

	h5->GetYaxis()->SetRangeUser(0.,1300.);

	TLegend *leg3 = new TLegend(0.6,0.7,0.9,0.9);
  	//leg2->SetHeader(" "); // option "C" allows to center the header
  	leg3->AddEntry(l1,"Angular Acceptance Limit","l");
	leg3->SetBorderSize(0);
  	leg3->Draw();

	//MyTree2->Draw("Angle_1>>h7","","same");
	//h7->SetLineColor(2);

	//leg2->Draw();
	

	//TH2D *h4 = new TH2D("h4","^{92}Sr(d,p)^{93}Sr Experiment (8.8#mum Au Degrader)",75,150,300,40,0,20);
	//h4->GetXaxis()->SetTitle("Post-Degrader Recoil Energy (MeV)");
	//h4->GetYaxis()->SetTitle("Post-Degrader Recoil Angle (deg)");
	//MyTree2->Draw("Angle_1:Energy_1>>h4","","same");
	//h4->SetMarkerStyle(20);
	//h4->SetMarkerColor(2);

	//TH2D *h4 = new TH2D("h4","21Na Experiment",100,0,100,200,0,20);
	//MyTree->Draw("Angle_2:Energy_2>>h4","","same");
	//h4->SetMarkerStyle(21);
	//h4->SetMarkerColor(4);
/*
	TLine *l1 = new TLine(250.,3.7,300.,3.7);
	l1->SetLineColor(6);
	l1->SetLineStyle(2);
	l1->SetLineWidth(3);
	l1->Draw();

	const int n=4;

	Double_t x[n] = {65.7,65.7,80.3,80.3};
	Double_t y[n] = {0.,20.,20.,0.};

	//TGraph *g1 = new TGraph(n,x,y);

	//g1->SetFillColor(3);
	//g1->SetFillStyle(3004);
   	//g1->Draw("f"); 

	//TLegend *leg2 = new TLegend(0.6,0.7,0.9,0.9);
  	//leg2->SetHeader("Legend"); // option "C" allows to center the header
  	//leg2->AddEntry(h3,"^{92}Sr(d,p)^{93}Sr Recoils");
  	//leg2->AddEntry(h4,"^{92}Mo(d,p)^{93}Mo Recoils");
	//leg2->AddEntry(l1,"Angular Acceptance Limit","l");
	//leg2->AddEntry(g1,"#pm10% Central Energy","f");
  	//leg2->Draw();

*/	
	
}
