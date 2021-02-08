#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <IoAuger.h>
#include <Ec.h>

#include <TFile.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TCanvas.h>


using namespace std;

float getmean( int arr[] ){
  float mean = 0.;
  for (int i=0; i<100; i++)
    mean += arr[i];
  return mean/100.;
}

float getrms( int arr[], float meanarr ){
  float rms = 0.;
  for (int i=0; i<100; i++)
    rms += (arr[i] - meanarr)*(arr[i] - meanarr);
  return rms/100.;
}

float kstest( int arr0[], int arr1[] ){
  float ks = 0.;
  int min0 = 1000;
  int max0 = 0;
  int min1 = 1000;
  int max1 = 0;
  int ming = 0;
  int maxg = 0;

  for (int i=0; i<100; i++){
    if (arr0[i]<min0)
      min0 = arr0[i];
    if (arr0[i]>max0)
      max0 = arr0[i];
    if (arr1[i]<min1)
      min1 = arr1[i];
    if (arr1[i]>max1)
      max1 = arr1[i];
  }

  if ( min0 > min1 )
    ming = min1;
  else
    ming = min0;

  if ( max0 > max1 )
    maxg = max0;
  else
    maxg = max1;

  int nbins = 50;
  float delBin = (maxg-ming)/(1.*nbins);
  float cumul0[nbins];
  float cumul1[nbins];

  for (int j=0; j<nbins; j++){
    cumul0[j] = 0.;
    cumul1[j] = 0.;
  }

  float tmpbin = ming;
  for (int j=0; j<nbins; j++){
    for (int i=0; i<100; i++){
      if ( arr0[i] <= tmpbin+0.5 )
        cumul0[j] += 1.;
      if ( arr1[i] <= tmpbin+0.5 )
        cumul1[j] += 1.;
    }
    tmpbin += delBin;
  }
/*
  cout << "tmpbin: " 
    << tmpbin << " "
    << maxg << " "
    << ming << " " 
    << delBin << endl;

  for (int i=0; i<100; i++)
    cout << arr0[i] << " "
      << arr1[i] << endl;

  tmpbin = ming;
  for (int i=0; i<nbins; i++){
    cout << tmpbin << " "
      << cumul0[i] << " " << cumul1[i] << endl;
    tmpbin += delBin;
  }
*/
  for ( int i=0; i<nbins; i++)
    if ( (cumul0[i]-cumul1[i])/100. > ks )
      ks = (cumul0[i]-cumul1[i])/100.;
 
  return ks;
} 


int main (int argc, char *argv[]) {
   if ( argc < 4 ) {
     cout << endl
         << "Usage: " << argv[0] << " <stationsFile>  <output>  <files>" << endl
         << "  <stationsFile>: file with a list of stations" << endl
         << "  <output>: output file with whatever you want inside" << endl
         << "  <files>: IoSd or IoAuger files to be read" << endl;
    exit(0);
  }

  const char* stationsFileName = argv[1];
  const char* outputName = argv[2];
  AugerIoSd input(argc-3, argv+3);
  const unsigned int totalNrEvents = input.NumberOfEvents();
  
  ifstream stationsFile(stationsFileName, ios::in);
  if (!stationsFile.is_open()){
    cout << "Could not open file: " << stationsFileName << endl;
    exit(0);
  }
  vector<unsigned int> stationsIds;
  while (stationsFile.good()) {
    unsigned int st = 0;
    stationsFile >> st;
    if (st)
      stationsIds.push_back(st);
  }
  
  if (stationsIds.empty()){
    cout << "Please specify the stations ids in the file " << endl;
    exit(0);
  }
    
  IoSd *sdFile = NULL;
  AugerFile *adFile = NULL;
  TString nameStati = to_string( stationsIds[0] );

  TFile hfile("baseline100bins.root","RECREATE","");

  //TTree tree("T","");

  // For PMT 1
  sort(stationsIds.begin(), stationsIds.end());
  Double_t stationsBins[stationsIds.size()];
  for ( int i=0; i<stationsIds.size(); i++){
    stationsBins[i] = stationsIds[i];
    cout << i+1 << " -> " << stationsIds[i] << endl;
  }

  // For PMT 1   
  TH2F pmt1lmeanf("pmt1lmeanf", "Mean for First 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1lmeanl("pmt1lmeanl", "Mean for Last 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F pmt1lrmsf("pmt1lrmsf", "RMS for First 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1lrmsl("pmt1lrmsl", "RMS for Last 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F pmt1ldiffmean("pmt1ldiffmean", "Difference of the mean of First and Last 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1ldiffrms("pmt1ldiffrms", "Difference of the RMS of First and Last 100 bins of Baseline PMT 1 LG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());


  TH2F pmt1hmeanf("pmt1hmeanf", "Mean for First 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1hmeanl("pmt1hmeanl", "Mean for Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F pmt1hrmsf("pmt1hrmsf", "RMS for First 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1hrmsl("pmt1hrmsl", "RMS for Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F pmt1hdiffmean("pmt1hdiffmean", "Difference of the Mean for First and Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F pmt1hdiffrms("pmt1hdiffrms", "Difference of the RMS for First and Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F ksdistpmt1h("ksdistpmt1h", "KS-Test for First and Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());


  // For CALIB Channel
  TH2F calibmeanf("calibmeanf", "Mean for First 100 bins of Baseline CALIB", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F calibmeanl("calibmeanl", "Mean for Last 100 bins of Baseline CALIB", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F calibrmsf("calibrmsf", "RMS for First 100 bins of Baseline CALIB", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());
  TH2F calibrmsl("calibrmsl", "RMS for Last 100 bins of Baseline CALIB", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  TH2F ksdistcalib("ksdistcalib", "KS-Test for First and Last 100 bins of Baseline CALIB", totalNrEvents, 0, totalNrEvents, stationsIds.size(), 0, stationsIds.size());

  int mpmt1lbmeanf[100];
  int mpmt1lbmeanl[100];

  int mpmt1hbmeanf[100];
  int mpmt1hbmeanl[100];

  int mpmt2lbmeanf[100];
  int mpmt2lbmeanl[100];

  int mpmt2hbmeanf[100];
  int mpmt2hbmeanl[100];

  int mpmt3lbmeanf[100];
  int mpmt3lbmeanl[100];

  int mpmt3hbmeanf[100];
  int mpmt3hbmeanl[100];

  int mspmtlbmeanf[100];
  int mspmtlbmeanl[100];

  int mcalibbmeanf[100];
  int mcalibbmeanl[100];

  int mssdpmtlbmeanf[100];
  int mssdpmtlbmeanl[100];

  int mssdpmthbmeanf[100];
  int mssdpmthbmeanl[100];


  int previusEvent = 0;
  int sameUtc = 0;
  int nsample = 0;

  unsigned int nrEvents = 0;
  unsigned int nrEventsRead = 0;
  EventPos pos;
  for (pos=input.FirstEvent(); pos<input.LastEvent(); pos=input.NextEvent()) {

    ++nrEventsRead;
    if (nrEventsRead%1000 == 0){
      cout << "====> Read " << nrEventsRead << " out of " << totalNrEvents << endl;
      cout << "      Wrote: " << nrEvents << " events" << endl;
    }

    bool found = false;
    
    IoSdEvent event(pos);
    for (unsigned int i = 0 ; i < event.Stations.size(); ++i){
      found = false;
      for (  vector<unsigned int>::const_iterator iter= stationsIds.begin();
             iter!= stationsIds.end(); ++iter){
        if (event.Stations[i].Id == *iter){
          found = true;
          }
        }
      
      if (!found)
        continue;
      if ( event.Stations[i].IsUUB && event.Id != previusEvent && sameUtc != event.utctime() ) {
        previusEvent = event.Id;
        sameUtc = event.utctime();

        cout << "# Event " << event.Id << " Station " << event.Stations[i].Id
          << " " << nrEventsRead-1
             << endl;
        //cout << "# Error " << event.Stations[i].Error << endl;        

        IoSdEvent event(pos);
        
        if (event.Stations[i].Error==256) { //0+256
          nsample = event.Stations[i].UFadc->NSample;
          for (int k=0;k<100;k++){
            mpmt1lbmeanf[k] = event.Stations[i].UFadc->GetValue(0,1,k);
            mpmt1lbmeanl[k] = event.Stations[i].UFadc->GetValue(0,1,(nsample - k));

            mpmt1hbmeanf[k] = event.Stations[i].UFadc->GetValue(0,0,k);
            mpmt1hbmeanl[k] = event.Stations[i].UFadc->GetValue(0,0,(nsample - k));

            mpmt2lbmeanf[k] = event.Stations[i].UFadc->GetValue(1,1,k);
            mpmt2lbmeanl[k] = event.Stations[i].UFadc->GetValue(1,1,(nsample - k));

            mpmt2hbmeanf[k] = event.Stations[i].UFadc->GetValue(1,0,k);
            mpmt2hbmeanl[k] = event.Stations[i].UFadc->GetValue(1,0,(nsample - k));

            mpmt3lbmeanf[k] = event.Stations[i].UFadc->GetValue(2,1,k);
            mpmt3lbmeanl[k] = event.Stations[i].UFadc->GetValue(2,1,(nsample - k));

            mpmt3hbmeanf[k] = event.Stations[i].UFadc->GetValue(2,0,k);
            mpmt3hbmeanl[k] = event.Stations[i].UFadc->GetValue(2,0,(nsample - k));

            mspmtlbmeanf[k] = event.Stations[i].UFadc->GetValue(3,1,k);
            mspmtlbmeanl[k] = event.Stations[i].UFadc->GetValue(3,1,(nsample - k));

            mcalibbmeanf[k] = event.Stations[i].UFadc->GetValue(3,0,k);
            mcalibbmeanl[k] = event.Stations[i].UFadc->GetValue(3,0,(nsample - k));

            mssdpmtlbmeanf[k] = event.Stations[i].UFadc->GetValue(4,1,k);
            mssdpmtlbmeanl[k] = event.Stations[i].UFadc->GetValue(4,1,(nsample - k));

            mssdpmthbmeanf[k] = event.Stations[i].UFadc->GetValue(4,0,k);
            mssdpmthbmeanl[k] = event.Stations[i].UFadc->GetValue(4,0,(nsample - k));
          }
          //cout << "out: " << kstest(mpmt1lbmeanf, mpmt1lbmeanl) << endl;
            
          for ( int id=0; id<stationsIds.size(); id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              pmt1lmeanf.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanf) );
              pmt1lmeanl.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanl) );
              pmt1lrmsf.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanf, getmean(mpmt1lbmeanf)) );
              pmt1lrmsl.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanl, getmean(mpmt1lbmeanl)) );

              pmt1hmeanf.Fill( nrEventsRead-1 , id, getmean(mpmt1hbmeanf) );
              pmt1hmeanl.Fill( nrEventsRead-1 , id, getmean(mpmt1hbmeanl) );
              pmt1hrmsf.Fill( nrEventsRead-1 , id, getrms(mpmt1hbmeanf, getmean(mpmt1hbmeanf)) );
              pmt1hrmsl.Fill( nrEventsRead-1 , id, getrms(mpmt1hbmeanl, getmean(mpmt1hbmeanl)) );

              calibmeanf.Fill( nrEventsRead-1 , id, getmean(mcalibbmeanf) );
              calibmeanl.Fill( nrEventsRead-1 , id, getmean(mcalibbmeanl) );
              calibrmsf.Fill( nrEventsRead-1 , id, getrms(mcalibbmeanf, getmean(mcalibbmeanf)) );
              calibrmsl.Fill( nrEventsRead-1 , id, getrms(mcalibbmeanl, getmean(mcalibbmeanl)) );

              pmt1ldiffmean.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanf) - getmean(mpmt1lbmeanl) );
              pmt1ldiffrms.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanf, getmean(mpmt1lbmeanf)) - getrms(mpmt1lbmeanl, getmean(mpmt1lbmeanl)) );

              pmt1hdiffmean.Fill( nrEventsRead-1 , id, abs(getmean(mpmt1hbmeanf) - getmean(mpmt1hbmeanl)) );
              pmt1hdiffrms.Fill( nrEventsRead-1 , id, abs(getrms(mpmt1hbmeanf, getmean(mpmt1hbmeanf)) - getrms(mpmt1hbmeanl, getmean(mpmt1hbmeanl))) );

              ksdistpmt1h.Fill( nrEventsRead-1 , id, kstest(mpmt1hbmeanf, mpmt1hbmeanl) );
              ksdistcalib.Fill( nrEventsRead-1 , id, kstest(mcalibbmeanf, mcalibbmeanl) );
              
              if ( kstest(mpmt1hbmeanf, mpmt1hbmeanl) > 0.25 )
                cout << "KSpmt1 " << kstest(mpmt1hbmeanf, mpmt1hbmeanl) << " " << event.Id << " "
                  << event.Stations[i].Id 
                  << endl;
              if ( kstest(mcalibbmeanf, mcalibbmeanl) > 0.25 )
                cout << "KScalib " << event.Id << " "
                  << event.Stations[i].Id 
                  << endl;
            }
        }
      }
    }
  }

  auto plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
  TCanvas c1("c1", "2D", 0,0,3600,2400);
  c1.cd();
  pmt1lmeanf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1lmeanf.GetYaxis()->SetTitle("Station");
  pmt1lmeanf.GetZaxis()->SetRangeUser(180,300);
  pmt1lmeanf.SetStats(0);
  plain->SetPalette(53); 
  pmt1lmeanf.Draw("COLZ");
  c1.Print("../../plots/baselinePmt1MeanFirst100Lg.pdf");

  TCanvas c2("c2", "2D", 0,0,3600,2400);
  c2.cd();
  pmt1lmeanl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1lmeanl.GetYaxis()->SetTitle("Station");
  pmt1lmeanl.GetZaxis()->SetRangeUser(180,300);
  pmt1lmeanl.SetStats(0);
  plain->SetPalette(53); 
  pmt1lmeanl.Draw("COLZ");
  c2.Print("../../plots/baselineMeanPmt1Last100Lg.pdf");

  TCanvas c3("c3", "2D", 0,0,3600,2400);
  c3.cd();
  pmt1lrmsf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1lrmsf.GetYaxis()->SetTitle("Station");
  //pmt1lrmsf.GetZaxis()->SetRangeUser(pmt1rmsf.GetZaxis()->GetXmin()*1.2, pmt1rmsf.GetZaxis()->GetXmax()*1.2);
  pmt1lrmsf.GetZaxis()->SetRangeUser(0, 1.6);
  pmt1lrmsf.SetStats(0);
  plain->SetPalette(53); 
  pmt1lrmsf.Draw("COLZ");
  c3.Print("../../plots/baselineRmsPmt1First100Lg.pdf");

  TCanvas c4("c4", "2D", 0,0,3600,2400);
  c4.cd();
  pmt1lrmsl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1lrmsl.GetYaxis()->SetTitle("Station");
  //pmt1lrmsl.GetZaxis()->SetRangeUser(pmt1rmsl.GetZaxis()->GetXmin()*1.5, pmt1rmsl.GetZaxis()->GetXmax()*1.5);
  pmt1lrmsl.GetZaxis()->SetRangeUser(0, 1.6);
  pmt1lrmsl.SetStats(0);
  plain->SetPalette(53); 
  pmt1lrmsl.Draw("COLZ");
  c4.Print("../../plots/baselinePmt1RmsLast100Lg.pdf");


  // For High Gain
  TCanvas c5("c5", "2D", 0,0,3600,2400);
  c5.cd();
  pmt1hmeanf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hmeanf.GetYaxis()->SetTitle("Station");
  pmt1hmeanf.GetZaxis()->SetRangeUser(180,320);
  pmt1hmeanf.SetStats(0);
  plain->SetPalette(55);
  pmt1hmeanf.Draw("COLZ");
  c5.Print("../../plots/baselinePmt1MeanFirst100Hg.pdf");

  TCanvas c6("c6", "2D", 0,0,3600,2400);
  c6.cd();
  pmt1hmeanl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hmeanl.GetYaxis()->SetTitle("Station");
  pmt1hmeanl.GetZaxis()->SetRangeUser(180,320);
  pmt1hmeanl.SetStats(0);
  plain->SetPalette(55); 
  pmt1hmeanl.Draw("COLZ");
  c6.Print("../../plots/baselinePmt1MeanLast100Hg.pdf");

  TCanvas c7("c7", "2D", 0,0,3600,2400);
  c7.cd();
  pmt1hrmsf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrmsf.GetYaxis()->SetTitle("Station");
  //pmt1hrmsf.GetZaxis()->SetRangeUser(pmt1rmsf.GetZaxis()->GetXmin()*1.2, pmt1rmsf.GetZaxis()->GetXmax()*1.2);
  pmt1hrmsf.GetZaxis()->SetRangeUser(0, 20.);
  pmt1hrmsf.SetStats(0);
  plain->SetPalette(55); 
  pmt1hrmsf.Draw("COLZ");
  c7.Print("../../plots/baselinePmt1RmsFirst100Hg.pdf");

  TCanvas c8("c8", "2D", 0,0,3600,2400);
  c8.cd();
  pmt1hrmsl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hrmsl.GetYaxis()->SetTitle("Station");
  //pmt1hrmsl.GetZaxis()->SetRangeUser(pmt1rmsl.GetZaxis()->GetXmin()*1.5, pmt1rmsl.GetZaxis()->GetXmax()*1.5);
  pmt1hrmsl.GetZaxis()->SetRangeUser(0, 20.);
  pmt1hrmsl.SetStats(0);
  plain->SetPalette(55); 
  pmt1hrmsl.Draw("COLZ");
  c8.Print("../../plots/baselinePmt1RmsLast100Hg.pdf");


  TCanvas c9("c9", "2D", 0,0,3600,2400);
  c9.cd();
  ksdistpmt1h.GetXaxis()->SetTitle("Starts on 0 for first Event");
  ksdistpmt1h.GetYaxis()->SetTitle("Station");
  ksdistpmt1h.GetZaxis()->SetRangeUser(0., 0.7);
  ksdistpmt1h.SetStats(0);
  plain->SetPalette(55); 
  ksdistpmt1h.Draw("COLZ");
  c9.Print("../../plots/baselinePmt1KSHg.pdf");

  TCanvas c10("c10", "2D", 0,0,3600,2400);
  c10.cd();
  calibmeanf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  calibmeanf.GetYaxis()->SetTitle("Station");
  calibmeanf.GetZaxis()->SetRangeUser(180, 260.);
  calibmeanf.SetStats(0);
  plain->SetPalette(55); 
  calibmeanf.Draw("COLZ");
  c10.Print("../../plots/baselineCalibMeanFirst100.pdf");

  TCanvas c11("c11", "2D", 0,0,3600,2400);
  c11.cd();
  calibmeanl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  calibmeanl.GetYaxis()->SetTitle("Station");
  calibmeanl.GetZaxis()->SetRangeUser(180, 260.);
  calibmeanl.SetStats(0);
  plain->SetPalette(55); 
  calibmeanl.Draw("COLZ");
  c11.Print("../../plots/baselineCalibMeanLast100.pdf");

  TCanvas c12("c12", "2D", 0,0,3600,2400);
  c12.cd();
  calibrmsf.GetXaxis()->SetTitle("Starts on 0 for first Event");
  calibrmsf.GetYaxis()->SetTitle("Station");
  calibrmsf.GetZaxis()->SetRangeUser(0, 0.6);
  calibrmsf.SetStats(0);
  plain->SetPalette(55); 
  calibrmsf.Draw("COLZ");
  c12.Print("../../plots/baselineCalibRmsFirst100.pdf");

  TCanvas c13("c13", "2D", 0,0,3600,2400);
  c13.cd();
  calibrmsl.GetXaxis()->SetTitle("Starts on 0 for first Event");
  calibrmsl.GetYaxis()->SetTitle("Station");
  calibrmsl.GetZaxis()->SetRangeUser(0, 0.6);
  calibrmsl.SetStats(0);
  plain->SetPalette(55); 
  calibrmsl.Draw("COLZ");
  c13.Print("../../plots/baselineCalibRmsLast100.pdf");

  TCanvas c14("c14", "2D", 0,0,3600,2400);
  c14.cd();
  ksdistcalib.GetXaxis()->SetTitle("Starts on 0 for first Event");
  ksdistcalib.GetYaxis()->SetTitle("Station");
  ksdistcalib.GetZaxis()->SetRangeUser(0, 0.4);
  ksdistcalib.SetStats(0);
  plain->SetPalette(55); 
  ksdistcalib.Draw("COLZ");
  c14.Print("../../plots/baselineCalibKS.pdf");

  TCanvas c15("c15", "2D", 0,0,3600,2400);
  c15.cd();
  pmt1hdiffmean.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hdiffmean.GetYaxis()->SetTitle("Station");
  pmt1hdiffmean.GetZaxis()->SetRangeUser(0., 20.);
  pmt1hdiffmean.SetStats(0);
  plain->SetPalette(55); 
  pmt1hdiffmean.Draw("COLZ");
  c15.Print("../../plots/baselineDiffMeanPmt1Hg.pdf");

  TCanvas c16("c16", "2D", 0,0,3600,2400);
  c16.cd();
  pmt1hdiffrms.GetXaxis()->SetTitle("Starts on 0 for first Event");
  pmt1hdiffrms.GetYaxis()->SetTitle("Station");
  pmt1hdiffrms.GetZaxis()->SetRangeUser(0., 5);
  pmt1hdiffrms.SetStats(0);
  plain->SetPalette(55); 
  pmt1hdiffrms.Draw("COLZ");
  c16.Print("../../plots/baselineDiffRmsPmt1Hg.pdf");

  hfile.Write();
  hfile.Close();

  return 0;
}
