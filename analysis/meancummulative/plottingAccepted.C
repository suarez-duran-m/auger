void plottingAccepted(){

  auto hfile = TFile::Open("filterBaseline100bins.root");

  auto pmt1hmf = (TH2F*)hfile->Get("pmt1hmOkf");
  auto pmt1hml = (TH2F*)hfile->Get("pmt1hmOkl");
  
  auto pmt1hrf = (TH2F*)hfile->Get("pmt1hrOkf");
  auto pmt1hrl = (TH2F*)hfile->Get("pmt1hrOkl");

  //TH1F *sgSt = new TH1F("sgSt","", pmt1hml->GetNbinsX(), 0, pmt1hml->GetNbinsX());

  //for(int i=0; i<pmt1hml->GetNbinsX(); i++ )
    //sgSt->Fill( pmt1hml->GetBinContent(i, 8) ); 

  TStyle *plain = new TStyle("Plain","Plain Style (no colors/fill areas)");

  TCanvas c1("c1", "2D", 0,0,3600,2400);
  c1.cd();
  pmt1hmf->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hmf->GetYaxis()->SetTitle("Station");
  pmt1hmf->GetZaxis()->SetRangeUser(200,300);
  pmt1hmf->SetStats(0);
  pmt1hmf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hmf->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c1.SetGridy();
  pmt1hmf->Draw("COLZ");
  c1.Print("pmt1hmf.pdf");
 
  TCanvas c2("c2", "2D", 0,0,3600,2400);
  c2.cd();
  pmt1hml->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hml->GetYaxis()->SetTitle("Station");
  //pmt1hml->GetZaxis()->SetTitle("FADC");
  pmt1hml->GetZaxis()->SetRangeUser(200,300);
  pmt1hml->SetStats(0);
  pmt1hml->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hml->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c2.SetGridy();
  pmt1hml->Draw("COLZ");
  c2.Print("pmt1hml.pdf");

  TCanvas c3("c3", "2D", 0,0,3600,2400);
  c3.cd();
  pmt1hrf->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrf->GetYaxis()->SetTitle("Station");
  //pmt1hrf->GetZaxis()->SetRangeUser(200,300);
  pmt1hrf->SetStats(0);
  pmt1hrf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrf->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c3.SetGridy();
  pmt1hrf->Draw("COLZ");
  c3.Print("pmt1hrf.pdf");
 
  TCanvas c4("c4", "2D", 0,0,3600,2400);
  c4.cd();
  pmt1hrl->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrl->GetYaxis()->SetTitle("Station");
  //pmt1hrl->GetZaxis()->SetTitle("FADC");
  //pmt1hrl->GetZaxis()->SetRangeUser(200,300);
  pmt1hrl->SetStats(0);
  pmt1hrl->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrl->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c4.SetGridy();
  pmt1hrl->Draw("COLZ");
  c4.Print("pmt1hrl.pdf");
/*
  TCanvas c5("c5", "2D", 0,0,3600,2400);
  c5.cd();
  sgSt->SetStats(0);
  //sgSt->GetYaxis()->SetNdivisions(1020, "kTRUE");
  //sgSt->GetYaxis()->SetTickLength(0.);
  //plain->SetPalette(55);
  //plain->SetGridStyle(9);
  c5.SetGridy();
  sgSt->Draw();
  c5.Print("sgSt.pdf");
*/
}
