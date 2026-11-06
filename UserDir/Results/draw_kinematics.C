{
	TTree *MyTree = new TTree("MyTree", "MyTree");
	MyTree->ReadFile("postDegrader1_reaction.dat", "Energy_1:Theta_1:XAngle_1:YAngle_1");

	TTree *CamTree = new TTree("CamTree", "CamTree");
	CamTree->ReadFile("Cameron_Kinematics_97Zr.dat", "Energy_2:XAngle_2:YAngle_2");

	TH1D *h1 = new TH1D("h1","Zr97 Recoil X-Angle",400,-5,5);
	TH1D *h2 = new TH1D("h2","Cameron Data",400,-5,5);
	MyTree->Draw("XAngle_1>>h1");
	CamTree->Draw("XAngle_2>>h2","","same");
	h1->SetLineColor(1);
	h2->SetLineColor(2);

	TLegend *leg1 = new TLegend(0.6,0.7,0.9,0.9);
  	leg1->AddEntry(h1,"Matt");
  	leg1->AddEntry(h2,"Cameron");
  	leg1->Draw();

	TCanvas *c2 = new TCanvas();
	TH2D *hAngles_2D = new TH2D("hAngles_2D","Zr97 Recoil X-Angle",200,-5,5,200,-5,5);
	MyTree->Draw("YAngle_1:XAngle_1>>hAngles_2D","","colz");


	TCanvas *c3 = new TCanvas();
	TH2D *hAngles_2D_Cam = new TH2D("hAngles_2D_Cam","Zr97 Recoil XY-Angle",200,-5,5,200,-5,5);
	CamTree->Draw("YAngle_2:XAngle_2>>hAngles_2D_Cam","","colz");



}