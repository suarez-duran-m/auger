void plottingBl100bins(int pmtId){  
  TString filename;
  if ( pmtId < 3 )
    filename.Form("bl100binsPMT%d.root", pmtId+1);
  else if ( pmtId == 3 )
    filename = "bl100binsSPMT.root";
  else if ( pmtId == 4)
    filename = "bl100binsPMTSSD.root";
  else{
    cout << "==================================================" << endl;
    cout << "Wrong Id for PMT, please introduce a valid PMT Id:" << endl;
    cout << "0 For PMT1; " << "1 For PMT2; " << "2 For PMT3; " 
      << "3 For SPMT; " << "4 For PMTSSD" << endl;
    cout << "==================================================" << endl;
    exit(0);
  }
  cout << "You have selected " << filename << endl;
    
  auto hFile = TFile::Open(filename);
  
  auto pmt1hmf = (TH2F*)hFile->Get("pmthmeanf");
  auto pmt1hml = (TH2F*)hFile->Get("pmthmeanl");

  auto pmt1hrf = (TH2F*)hFile->Get("pmthrmsf");
  auto pmt1hrl = (TH2F*)hFile->Get("pmthrmsl");

  auto pmt1hdm = (TH2F*)hFile->Get("pmthdiffmean");
  auto pmt1hdr = (TH2F*)hFile->Get("pmthdiffrms");

  //auto pmt1hks = (TH2F*)hFile->Get("ksdistpmt1h");

  const char *stIds[19] = {"863", "1211", "1217", "1219", "1221", "1222", "1223", "1729", "1735", "1740", "1741", "1743", "1745", "1746", "1747", "1791", "1818", "1819", "1851"};

  auto plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");

  TCanvas *c1 = new TCanvas("c1", "2D", 0, 0,3600,2400);
  c1->cd();
  c1->SetLeftMargin(0.11);
  c1->SetRightMargin(0.135);
  pmt1hmf->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmt1hmf->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hmf->GetXaxis()->SetRangeUser(0,52);
  pmt1hmf->GetXaxis()->SetTickLength(0.);
  pmt1hmf->GetXaxis()->SetLabelSize(.02);
  pmt1hmf->GetZaxis()->SetTitle("FADC");
  pmt1hmf->GetZaxis()->SetRangeUser(180,300);
  pmt1hmf->GetZaxis()->SetTickLength(0.);
  pmt1hmf->SetStats(0);
  pmt1hmf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hmf->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hmf->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmt1hmf->GetYaxis()->SetTitle("Station");
  pmt1hmf->GetYaxis()->SetTitleFont(42);
  pmt1hmf->GetYaxis()->SetTitleOffset(1.3);
  pmt1hmf->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c1->SetGridy();
  c1->SetGridx();
  pmt1hmf->Draw("COLZ");
  //c1->Print("testMeanFirst100Hg.pdf");
  c1->Print("../../plots/blPmt1MeanFirst100Hg.pdf");

  TCanvas c2("c2", "2D", 0,0,3600,2400);
  c2.cd();
  c2.SetLeftMargin(0.11);
  c2.SetRightMargin(0.135);
  pmt1hml->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmt1hml->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hml->GetXaxis()->SetRangeUser(0,52);
  pmt1hml->GetXaxis()->SetTickLength(0.);
  pmt1hml->GetXaxis()->SetLabelSize(.02);
  pmt1hml->GetZaxis()->SetTitle("FADC");
  pmt1hml->GetZaxis()->SetRangeUser(180,300);
  pmt1hml->GetZaxis()->SetTickLength(0.);
  pmt1hml->SetStats(0);
  pmt1hml->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hml->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hml->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmt1hml->GetYaxis()->SetTitle("Station");
  pmt1hml->GetYaxis()->SetTitleFont(42);
  pmt1hml->GetYaxis()->SetTitleOffset(1.3);
  pmt1hml->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c2.SetGridy();
  c2.SetGridx();
  pmt1hml->Draw("COLZ");
  //c2.Print("testMeanLast100Hg.pdf");
  c2.Print("../../plots/blPmt1MeanLast100Hg.pdf");

  TCanvas c3("c3", "2D", 0,0,3600,2400);
  c3.cd();
  c3.SetLeftMargin(0.11);
  c3.SetRightMargin(0.135);
  pmt1hrf->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmt1hrf->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hrf->GetXaxis()->SetRangeUser(0,52);
  pmt1hrf->GetXaxis()->SetTickLength(0.);
  pmt1hrf->GetXaxis()->SetLabelSize(.02);
  pmt1hrf->GetYaxis()->SetTitle("Station");
  pmt1hrf->GetZaxis()->SetTitle("FADC");
  pmt1hrf->GetZaxis()->SetRangeUser(0.,12.);
  pmt1hrf->SetStats(0);
  pmt1hrf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrf->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hrf->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c3.SetGridy();
  c3.SetGridx();
  pmt1hrf->Draw("COLZ");
  //c3.Print("testRmsFirst100Hg.pdf");
  c3.Print("../../plots/blPmt1RmsFirst100Hg.pdf");

  TCanvas c4("c4", "2D", 0,0,3600,2400);
  c4.cd();
  c4.SetLeftMargin(0.11);
  c4.SetRightMargin(0.135);
  pmt1hrl->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmt1hrl->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hrl->GetXaxis()->SetRangeUser(0,52);
  pmt1hrl->GetXaxis()->SetTickLength(0.);
  pmt1hrl->GetXaxis()->SetLabelSize(.02);
  pmt1hrl->GetYaxis()->SetTitle("Station");
  pmt1hrl->GetZaxis()->SetTitle("FADC");
  pmt1hrl->GetZaxis()->SetRangeUser(0.,12.);
  pmt1hrl->SetStats(0);
  pmt1hrl->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hrl->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hrl->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c4.SetGridy();
  c4.SetGridx();
  pmt1hrl->Draw("COLZ");
  //c4.Print("testRmsLast100Hg.pdf");
  c4.Print("../../plots/blPmt1RmsLast100Hg.pdf");


  TCanvas c5("c5", "2D", 0,0,3600,2400);
  c5.cd();
  c5.SetLeftMargin(0.11);
  c5.SetRightMargin(0.135);
  pmt1hdm->GetXaxis()->SetTitle("Days since December 1st 2020.");
  pmt1hdm->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hdm->GetXaxis()->SetRangeUser(0,52);
  pmt1hdm->GetXaxis()->SetTickLength(0.);
  pmt1hdm->GetXaxis()->SetLabelSize(.02);
  pmt1hdm->GetYaxis()->SetTitle("Station");
  pmt1hdm->GetZaxis()->SetTitle("FADC");
  //pmt1hdm->GetZaxis()->SetRangeUser(0.,12.);
  pmt1hdm->SetStats(0);
  pmt1hdm->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hdm->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hdm->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c5.SetGridy();
  c5.SetGridx();
  pmt1hdm->Draw("COLZ");
  //c5.Print("testRmsLast100Hg.pdf");
  c5.Print("../../plots/blPmt1Diffmean100Hg.pdf");


  TCanvas c6("c6", "2D", 0,0,3600,2400);
  c6.cd();
  c6.SetLeftMargin(0.11);
  c6.SetRightMargin(0.135);
  pmt1hdr->GetXaxis()->SetTitle("Days since December 1st 2020.");
  pmt1hdr->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmt1hdr->GetXaxis()->SetRangeUser(0,52);
  pmt1hdr->GetXaxis()->SetTickLength(0.);
  pmt1hdr->GetXaxis()->SetLabelSize(.02);
  pmt1hdr->GetYaxis()->SetTitle("Station");
  pmt1hdr->GetZaxis()->SetTitle("FADC");
  //pmt1hdr->GetZaxis()->SetRangeUser(0.,12.);
  pmt1hdr->SetStats(0);
  pmt1hdr->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmt1hdr->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmt1hdr->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c6.SetGridy();
  c6.SetGridx();
  pmt1hdr->Draw("COLZ");
  //c6.Print("testRmsLast100Hg.pdf");
  c6.Print("../../plots/blPmt1Diffrms100Hg.pdf");
}
