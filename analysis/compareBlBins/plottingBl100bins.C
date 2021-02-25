void plottingBl100bins(int pmtId, int hg){  
  TString filename;
  TString printname;
  TString hgname;
  if ( pmtId > 0 && pmtId < 4 ){
    filename.Form("bl100binsPMT%d.root", pmtId);
    printname.Form("PMT%d", pmtId);
  }
  else if ( pmtId == 4 ){
    filename = "bl100binsSPMT.root";
    printname = "SPMT";
  }
  else if ( pmtId == 5 ){
    filename = "bl100binsPMTSSD.root";
    printname = "PMTSSD";
  }
  else{
    cout << "==================================================" << endl;
    cout << "Wrong Id for PMT, please introduce a valid PMT Id:" << endl;
    cout << "1 For PMT1; " << "2 For PMT2; " << "3 For PMT3; " 
      << "4 For SPMT; " << "5 For PMTSSD" << endl;
    cout << "==================================================" << endl;
    exit(0);
  }
  if ( hg !=0 && hg !=1 ){
    cout << hg << endl;
    cerr << "Please select HG (1) or LG (0) option" 
      << endl;
    exit(0);
  }
  cout << "You have selected " << filename << " "
    << "with HG option: " << hg << endl;
 
  auto hFile = TFile::Open(filename);

  TH2F *pmtmf;
  TH2F *pmtml;
  TH2F *pmtrf;
  TH2F *pmtrl;
  TH2F *pmtdm;
  TH2F *pmtdr;
  
  if ( hg==1 ){
    pmtmf = (TH2F*)hFile->Get("pmthmeanf");
    pmtml = (TH2F*)hFile->Get("pmthmeanl");

    pmtrf = (TH2F*)hFile->Get("pmthrmsf");
    pmtrl = (TH2F*)hFile->Get("pmthrmsl");

    pmtdm = (TH2F*)hFile->Get("pmthdiffmean");
    pmtdr = (TH2F*)hFile->Get("pmthdiffrms");

    hgname = "Hg";
  }
  else{
    pmtmf = (TH2F*)hFile->Get("pmtlmeanf");
    pmtml = (TH2F*)hFile->Get("pmtlmeanl");

    pmtrf = (TH2F*)hFile->Get("pmtlrmsf");
    pmtrl = (TH2F*)hFile->Get("pmtlrmsl");

    pmtdm = (TH2F*)hFile->Get("pmtldiffmean");
    pmtdr = (TH2F*)hFile->Get("pmtldiffrms");
    
    hgname = "Lg";
  }

  const char *stIds[19] = {"863", "1211", "1217", "1219", "1221", "1222", "1223", "1729", "1735", "1740", "1741", "1743", "1745", "1746", "1747", "1791", "1818", "1819", "1851"};

  auto plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");

  TCanvas *c1 = new TCanvas("c1", "2D", 0, 0,3600,2400);
  c1->cd();
  c1->SetLeftMargin(0.11);
  c1->SetRightMargin(0.135);
  pmtmf->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmtmf->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtmf->GetXaxis()->SetRangeUser(0,52);
  pmtmf->GetXaxis()->SetTickLength(0.);
  pmtmf->GetXaxis()->SetLabelSize(.02);
  pmtmf->GetZaxis()->SetTitle("(Average Mean per day) / FADC");
  pmtmf->GetZaxis()->SetRangeUser(220,280);
  pmtmf->GetZaxis()->SetTickLength(0.);
  pmtmf->SetStats(0);
  pmtmf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtmf->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtmf->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmtmf->GetYaxis()->SetTitle("Station");
  pmtmf->GetYaxis()->SetTitleFont(42);
  pmtmf->GetYaxis()->SetTitleOffset(1.3);
  pmtmf->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  c1->SetGridy();
  c1->SetGridx();
  pmtmf->Draw("COLZ1");
  c1->Print("../../plots/bl"+printname+"MeanFirst100"+hgname+".pdf");

  TCanvas c2("c2", "2D", 0,0,3600,2400);
  c2.cd();
  c2.SetLeftMargin(0.11);
  c2.SetRightMargin(0.135);
  pmtml->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmtml->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtml->GetXaxis()->SetRangeUser(0,52);
  pmtml->GetXaxis()->SetTickLength(0.);
  pmtml->GetXaxis()->SetLabelSize(.02);
  pmtml->GetZaxis()->SetTitle("(Average Mean per day) / FADC");
  pmtml->GetZaxis()->SetRangeUser(220,280);
  pmtml->GetZaxis()->SetTickLength(0.);
  pmtml->SetStats(0);
  pmtml->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtml->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtml->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmtml->GetYaxis()->SetTitle("Station");
  pmtml->GetYaxis()->SetTitleFont(42);
  pmtml->GetYaxis()->SetTitleOffset(1.3);
  pmtml->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  plain->SetGridStyle(9);
  c2.SetGridy();
  c2.SetGridx();
  pmtml->Draw("COLZ1");
  c2.Print("../../plots/bl"+printname+"MeanLast100"+hgname+".pdf");

  TCanvas c3("c3", "2D", 0,0,3600,2400);
  c3.cd();
  c3.SetLeftMargin(0.11);
  c3.SetRightMargin(0.135);
  pmtrf->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmtrf->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtrf->GetXaxis()->SetRangeUser(0,52);
  pmtrf->GetXaxis()->SetTickLength(0.);
  pmtrf->GetXaxis()->SetLabelSize(.02);
  pmtrf->GetYaxis()->SetTitle("Station");
  pmtrf->GetZaxis()->SetTitle("Average RMS per day FADC");
  pmtrf->GetZaxis()->SetRangeUser(0.,5.);
  pmtrf->SetStats(0);
  pmtrf->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtrf->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtrf->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmtrf->GetYaxis()->SetTitle("Station");
  pmtrf->GetYaxis()->SetTitleFont(42);
  pmtrf->GetYaxis()->SetTitleOffset(1.3);
  pmtrf->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  c3.SetGridy();
  c3.SetGridx();
  pmtrf->Draw("COLZ1");
  c3.Print("../../plots/bl"+printname+"RmsFirst100"+hgname+".pdf");

  TCanvas c4("c4", "2D", 0,0,3600,2400);
  c4.cd();
  c4.SetLeftMargin(0.11);
  c4.SetRightMargin(0.135);
  pmtrl->GetXaxis()->SetTitle("Days since December 1st 2020");
  pmtrl->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtrl->GetXaxis()->SetRangeUser(0,52);
  pmtrl->GetXaxis()->SetTickLength(0.);
  pmtrl->GetXaxis()->SetLabelSize(.02);
  pmtrl->GetYaxis()->SetTitle("Station");
  pmtrl->GetZaxis()->SetTitle("Average RMS per day FADC");
  pmtrl->GetZaxis()->SetRangeUser(0.,5.);
  pmtrl->SetStats(0);
  pmtrl->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtrl->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtrl->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  plain->SetPalette(56);
  pmtrl->GetYaxis()->SetTitle("Station");
  pmtrl->GetYaxis()->SetTitleFont(42);
  pmtrl->GetYaxis()->SetTitleOffset(1.3);
  pmtrl->GetYaxis()->SetLabelSize(.05);
  c4.SetGridy();
  c4.SetGridx();
  pmtrl->Draw("COLZ1");
  c4.Print("../../plots/bl"+printname+"RmsLast100"+hgname+".pdf");

  TCanvas c5("c5", "2D", 0,0,3600,2400);
  c5.cd();
  c5.SetLeftMargin(0.11);
  c5.SetRightMargin(0.135);
  pmtdm->GetXaxis()->SetTitle("Days since December 1st 2020.");
  pmtdm->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtdm->GetXaxis()->SetRangeUser(0,52);
  pmtdm->GetXaxis()->SetTickLength(0.);
  pmtdm->GetXaxis()->SetLabelSize(.02);
  pmtdm->GetYaxis()->SetTitle("Station");
  pmtdm->GetZaxis()->SetTitle("FADC");
  pmtdm->GetZaxis()->SetRangeUser(-2.,8.);
  pmtdm->SetStats(0);
  pmtdm->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtdm->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtdm->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmtdm->GetYaxis()->SetTitle("Station");
  pmtdm->GetYaxis()->SetTitleFont(42);
  pmtdm->GetYaxis()->SetTitleOffset(1.3);
  pmtdm->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  c5.SetGridy();
  c5.SetGridx();
  pmtdm->Draw("COLZ1");
  c5.Print("../../plots/bl"+printname+"Diffmean100"+hgname+".pdf");

  TCanvas c6("c6", "2D", 0,0,3600,2400);
  c6.cd();
  c6.SetLeftMargin(0.11);
  c6.SetRightMargin(0.135);
  pmtdr->GetXaxis()->SetTitle("Days since December 1st 2020.");
  pmtdr->GetXaxis()->SetNdivisions(1020*3, "kTRUE");
  pmtdr->GetXaxis()->SetRangeUser(0,52);
  pmtdr->GetXaxis()->SetTickLength(0.);
  pmtdr->GetXaxis()->SetLabelSize(.02);
  pmtdr->GetYaxis()->SetTitle("Station");
  pmtdr->GetZaxis()->SetTitle("FADC");
  pmtdr->GetZaxis()->SetRangeUser(-5.,5.);
  pmtdr->SetStats(0);
  pmtdr->GetYaxis()->SetNdivisions(1020, "kTRUE");
  pmtdr->GetYaxis()->SetTickLength(0.);
  for ( int i=0; i<19; i++)
    pmtdr->GetYaxis()->SetBinLabel(i+1, stIds[i]);
  pmtdr->GetYaxis()->SetTitle("Station");
  pmtdr->GetYaxis()->SetTitleFont(42);
  pmtdr->GetYaxis()->SetTitleOffset(1.3);
  pmtdr->GetYaxis()->SetLabelSize(.05);
  plain->SetPalette(56);
  c6.SetGridy();
  c6.SetGridx();
  pmtdr->Draw("COLZ1");
  c6.Print("../../plots/bl"+printname+"Diffrms100"+hgname+".pdf");

  
  TH1F *st1740 = new TH1F("st1740","Distribution of The Difference of The Mean for The Station 1740's PMT1 HG", 320,-1, 30);
  for(int i=0; i<pmtdm->GetXaxis()->GetNbins(); i++){
    if ( pmtdm->GetBinContent(i+1, 10)!=0 )
      st1740->Fill(pmtdm->GetBinContent(i+1, 10));
  }

  TCanvas c7("c7", "2D", 0,0,3600,2400);
  c7.cd();
  st1740->GetYaxis()->SetTitle("Counts/au");
  st1740->GetXaxis()->SetTitle("Difference of Mean / FADC");
  c7.SetGridx();
  c7.SetGridy();
  st1740->Draw();
  c7.Print("../../plots/blDiffMeanSt1740.pdf");

}
