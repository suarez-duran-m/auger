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

float getmean( int arr[], int nb ){
  float mean = 0.;
  for (int i=0; i<nb; i++)
    mean += arr[i];
  return mean/(nb*1.);
}

float getrms( int arr[], float meanarr, int nb ){
  float rms = 0.;
  for (int i=0; i<nb; i++)
    rms += (arr[i] - meanarr)*(arr[i] - meanarr);
  return sqrt(rms/(nb*1.));
}

float kstest( int arr0[], int arr1[], int nb){
  float ks = 0.;
  int min0 = 1000;
  int max0 = 0;
  int min1 = 1000;
  int max1 = 0;
  int ming = 0;
  int maxg = 0;

  for (int i=0; i<nb; i++){
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
    for (int i=0; i<nb; i++){
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
    if ( (cumul0[i]-cumul1[i])/(nb*1.) > ks )
      ks = (cumul0[i]-cumul1[i])/(nb*1.);
 
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

  int nblbins = 100;

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

  int mpmt1lbmeanf[nblbins];
  int mpmt1lbmeanl[nblbins];

  int mpmt1hbmeanf[nblbins];
  int mpmt1hbmeanl[nblbins];

  int mpmt2lbmeanf[nblbins];
  int mpmt2lbmeanl[nblbins];

  int mpmt2hbmeanf[nblbins];
  int mpmt2hbmeanl[nblbins];

  int mpmt3lbmeanf[nblbins];
  int mpmt3lbmeanl[nblbins];

  int mpmt3hbmeanf[nblbins];
  int mpmt3hbmeanl[nblbins];

  int mspmtlbmeanf[nblbins];
  int mspmtlbmeanl[nblbins];

  int mcalibbmeanf[nblbins];
  int mcalibbmeanl[nblbins];

  int mssdpmtlbmeanf[nblbins];
  int mssdpmtlbmeanl[nblbins];

  int mssdpmthbmeanf[nblbins];
  int mssdpmthbmeanl[nblbins];


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

        IoSdEvent event(pos);
        
        if (event.Stations[i].Error==256) { //0+256
          nsample = event.Stations[i].UFadc->NSample;
          for (int k=0;k<nblbins;k++){
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
            
          for ( int id=0; id<stationsIds.size(); id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              pmt1lmeanf.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanf, nblbins) );
              pmt1lmeanl.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanl, nblbins) );
              pmt1lrmsf.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanf, getmean(mpmt1lbmeanf, nblbins), nblbins) );
              pmt1lrmsl.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanl, getmean(mpmt1lbmeanl, nblbins), nblbins) );

              pmt1hmeanf.Fill( nrEventsRead-1 , id, getmean(mpmt1hbmeanf, nblbins) );
              pmt1hmeanl.Fill( nrEventsRead-1 , id, getmean(mpmt1hbmeanl, nblbins) );
              pmt1hrmsf.Fill( nrEventsRead-1 , id, getrms(mpmt1hbmeanf, getmean(mpmt1hbmeanf, nblbins), nblbins) );
              pmt1hrmsl.Fill( nrEventsRead-1 , id, getrms(mpmt1hbmeanl, getmean(mpmt1hbmeanl, nblbins), nblbins) );

              calibmeanf.Fill( nrEventsRead-1 , id, getmean(mcalibbmeanf, nblbins) );
              calibmeanl.Fill( nrEventsRead-1 , id, getmean(mcalibbmeanl, nblbins) );
              calibrmsf.Fill( nrEventsRead-1 , id, getrms(mcalibbmeanf, getmean(mcalibbmeanf, nblbins), nblbins) );
              calibrmsl.Fill( nrEventsRead-1 , id, getrms(mcalibbmeanl, getmean(mcalibbmeanl, nblbins), nblbins) );

              pmt1ldiffmean.Fill( nrEventsRead-1 , id, getmean(mpmt1lbmeanf, nblbins) - getmean(mpmt1lbmeanl, nblbins) );
              pmt1ldiffrms.Fill( nrEventsRead-1 , id, getrms(mpmt1lbmeanf, getmean(mpmt1lbmeanf, nblbins), nblbins) - getrms(mpmt1lbmeanl, getmean(mpmt1lbmeanl, nblbins), nblbins) );

              pmt1hdiffmean.Fill( nrEventsRead-1 , id, abs(getmean(mpmt1hbmeanf, nblbins) - getmean(mpmt1hbmeanl, nblbins)) );
              pmt1hdiffrms.Fill( nrEventsRead-1 , id, abs(getrms(mpmt1hbmeanf, getmean(mpmt1hbmeanf, nblbins), nblbins) - getrms(mpmt1hbmeanl, getmean(mpmt1hbmeanl, nblbins), nblbins)) );

              ksdistpmt1h.Fill( nrEventsRead-1 , id, kstest(mpmt1hbmeanf, mpmt1hbmeanl, nblbins) );
              ksdistcalib.Fill( nrEventsRead-1 , id, kstest(mcalibbmeanf, mcalibbmeanl, nblbins) );
              
              if ( kstest(mpmt1hbmeanf, mpmt1hbmeanl, nblbins) > 0.25 )
                cout << "KSpmt1 " << kstest(mpmt1hbmeanf, mpmt1hbmeanl, nblbins) << " " << event.Id << " "
                  << event.Stations[i].Id 
                  << endl;
              if ( kstest(mcalibbmeanf, mcalibbmeanl, nblbins) > 0.25 )
                cout << "KScalib " << event.Id << " "
                  << event.Stations[i].Id 
                  << endl;
            }
        }
      }
    }
  }

  hfile.Write();
  hfile.Close();

  return 0;
}
