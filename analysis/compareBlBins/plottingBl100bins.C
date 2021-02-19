void plottingBl100bins() {

  auto hFile = TFile::Open("baseline100bins.root");
  
  auto pmt1hmf = (TH2F*)hFile->Get("pmt1hmeanf");
  auto pmt1hml = (TH2F*)hFile->Get("pmt1hmeanl");

  auto pmt1hrf = (TH2F*)hFile->Get("pmt1hrmsf");
  auto pmt1hrl = (TH2F*)hFile->Get("pmt1hrmsl");

  auto pmt1hdm = (TH2F*)hFile->Get("pmt1hdiffmean");
  auto pmt1hdr = (TH2F*)hFile->Get("pmt1hdiffrms");

  auto pmt1hks = (TH2F*)hFile->Get("ksdistpmt1h");

  auto plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");

  TCanvas c1("c1", "2D", 0,0,3600,2400);
  c1.cd();
  pmt1hmf->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hmf->GetYaxis()->SetTitle("Station");
  //pmt1hmf->GetZaxis()->SetTitle("FADC");
  pmt1hmf->GetZaxis()->SetRangeUser(180,320);
  pmt1hmf->SetStats(0);
  pmt1hmf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hmf->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c1.SetGridy();
  pmt1hmf->Draw("COLZ");
  c1.Print("../../plots/baselinePmt1MeanFirst100Hg.pdf");

  TCanvas c2("c2", "2D", 0,0,3600,2400);
  c2.cd();
  pmt1hml->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hml->GetYaxis()->SetTitle("Station");
  //pmt1hmf->GetZaxis()->SetTitle("FADC");
  pmt1hml->GetZaxis()->SetRangeUser(180,320);
  pmt1hml->SetStats(0);
  pmt1hml->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hml->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c2.SetGridy();
  pmt1hml->Draw("COLZ");
  c2.Print("../../plots/baselinePmt1MeanLast100Hg.pdf");

  TCanvas c3("c3", "2D", 0,0,3600,2400);
  c3.cd();
  pmt1hrf->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrf->GetYaxis()->SetTitle("Station");
  //pmt1hrf->GetZaxis()->SetTitle("FADC");
  pmt1hrf->GetZaxis()->SetRangeUser(0.,120.);
  pmt1hrf->SetStats(0);
  pmt1hrf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrf->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c3.SetGridy();
  c3.SetLogz();
  pmt1hrf->Draw("COLZ");
  c3.Print("../../plots/baselinePmt1RmsFirst100Hg.pdf");

  TCanvas c4("c4", "2D", 0,0,3600,2400);
  c4.cd();
  pmt1hrl->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrl->GetYaxis()->SetTitle("Station");
  //pmt1hrf->GetZaxis()->SetTitle("FADC");
  pmt1hrl->GetZaxis()->SetRangeUser(0.,120.);
  pmt1hrl->SetStats(0);
  pmt1hrl->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrl->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c4.SetGridy();
  c4.SetLogz();
  pmt1hrl->Draw("COLZ");
  c4.Print("../../plots/baselinePmt1RmsLast100Hg.pdf");

  TCanvas c5("c5", "2D", 0,0,3600,2400);
  c5.cd();
  pmt1hdm->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hdm->GetYaxis()->SetTitle("Station");
  //pmt1hdm->GetZaxis()->SetTitle("FADC");
  pmt1hdm->GetZaxis()->SetRangeUser(0.01,120.);
  pmt1hdm->SetStats(0);
  pmt1hdm->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hdm->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c5.SetGridy();
  c5.SetLogz();
  pmt1hdm->Draw("COLZ");
  c5.Print("../../plots/baselineDiffMeanPmt1Hg.pdf");

  TCanvas c6("c6", "2D", 0,0,3600,2400);
  c6.cd();
  pmt1hdr->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hdr->GetYaxis()->SetTitle("Station");
  //pmt1hdr->GetZaxis()->SetTitle("FADC");
  pmt1hdr->GetZaxis()->SetRangeUser(0.01,120.);
  pmt1hdr->SetStats(0);
  pmt1hdr->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hdr->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c6.SetGridy();
  c6.SetLogz();
  pmt1hdr->Draw("COLZ");
  c6.Print("../../plots/baselineDiffRmsPmt1Hg.pdf");

  TCanvas c7("c7", "2D", 0,0,3600,2400);
  c7.cd();
  pmt1hks->GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hks->GetYaxis()->SetTitle("Station");
  //pmt1hdr->GetZaxis()->SetTitle("FADC");
  pmt1hks->GetZaxis()->SetRangeUser(0.001,1.);
  pmt1hks->SetStats(0);
  pmt1hks->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hks->GetYaxis()->SetTickLength(0.);
  plain->SetPalette(55);
  plain->SetGridStyle(9);
  c7.SetGridy();
  c7.SetLogz();
  pmt1hks->Draw("COLZ");
  c7.Print("../../plots/baselinePmt1KSHg.pdf");

}
