{
	TTree *MyTree = new TTree("MyTree", "MyTree");
	MyTree->ReadFile("slit1Position.dat", "xpos:ypos");
	//TTree *MyTree2 = new TTree("MyTree2", "MyTree2");
	//MyTree2->ReadFile("Silicon32Output_beam.txt","Energy_1:Angle_1");
	
	TCanvas *c1 = new TCanvas("c1");

	TH2D *h3 = new TH2D("h3","xslit20 position",200,-100,100,100,-50,50);
	h3->GetXaxis()->SetTitle("X Position (mm)");
	h3->GetYaxis()->SetTitle("Y Position (mm)");
	MyTree->Draw("ypos:xpos>>h3","","colz");
	//h3->SetMarkerStyle(20);
	//h3->SetMarkerColor(1);

	TCanvas *c2 = new TCanvas("c2");

	TH1D *h4 = new TH1D("h4","xslit20 xpos", 200,-100,100);
	TH1D *h6 = new TH1D("h6","xslit20 xpos", 200,-100,100);
	TH1D *h8 = new TH1D("h8","xslit20 xpos", 200,-100,100);
	h4->GetXaxis()->SetTitle("X Position (mm)");
	h4->GetYaxis()->SetTitle("Counts");
	MyTree->Draw("xpos>>h4");	
	MyTree->Draw("xpos>>h6","xpos>-30 && xpos<90","goff");
	MyTree->Draw("xpos>>h8","xpos>-90 && xpos<30","goff");
	h4->SetLineColor(1);
	//h6->SetLineColor(2);

	cout << "right: " << h6->GetEntries() << endl;
	cout << "left: " << h8->GetEntries() << endl;


	//TLegend *leg2 = new TLegend(0.6,0.7,0.9,0.9);
  	//leg2->SetHeader("Legend"); // option "C" allows to center the header
  	//leg2->AddEntry(h4,"^{31}Si(d,p)^{32}Si Recoils","l");
  	//leg2->AddEntry(h6,"^{31}Si Beam","l");
	//leg2->AddEntry(l1,"Angular Acceptance Limit","l");
	//leg2->AddEntry(g1,"#pm10% Central Energy","f");
  	//leg2->Draw();

	TCanvas *c4 = new TCanvas("c4");

	TH1D *h5 = new TH1D("h5","xslit20 ypos", 100,-50,50);
	//TH1D *h7 = new TH1D("h7","^{31}Si(d,p)^{32}Si Experiment (13.5#mum Au Degrader)", 80,0,20);
	h5->GetXaxis()->SetTitle("Y Position (mm)");
	h5->GetYaxis()->SetTitle("Counts");
	MyTree->Draw("ypos>>h5");
	h5->SetLineColor(1);
	//MyTree2->Draw("Angle_1>>h7","","same");
	//h7->SetLineColor(2);

	//leg2->Draw();
}
