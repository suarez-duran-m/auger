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

float getmean( int arr[], int nb, int lok ){
  float mean = 0.;
    for (int i=0; i<nb; i++){
      if ( lok == 0 )
        mean += arr[i];
      else
        mean += arr[2047-i];
    }
  return mean/(nb*1.);
}

float getrms( int arr[], float meanarr, int nb, int lok ){
  float rms = 0.;
  for (int i=0; i<nb; i++){
    if ( lok == 0 )
      rms += (arr[i] - meanarr)*(arr[i] - meanarr);
    else
      rms += (arr[2047-i] - meanarr)*(arr[2047-i] - meanarr);
  }
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

  TFile hfile("bl100binsPmtSsd.root","RECREATE","");

  //TTree tree("T","");

  int totSt = stationsIds.size();

  vector < int > stckEvt;
  stckEvt.resize(totSt);
  vector < vector < int > > stckMean;
  stckMean.resize(totSt);
  vector < vector < float > > stckRMS;
  stckRMS.resize(totSt);
  vector < vector < int > > stckDmean;
  stckDmean.resize(totSt);
  vector < vector < int > > stckDrms;
  stckDrms.resize(totSt);

  // For PMT 1
  sort(stationsIds.begin(), stationsIds.end());
  Double_t stationsBins[totSt];
  for ( int i=0; i<totSt; i++){
    stationsBins[i] = stationsIds[i];
    stckEvt[i] = 0;
    for ( int j=0; j<20; j++ ){
      stckMean[i].push_back(0);
      stckRMS[i].push_back(0.);
      stckDmean[i].push_back(0.);
      stckDrms[i].push_back(0.);
    }
    cout << i+1 << " -> " << stationsIds[i] << endl;
  }

    int nblbins = 100;
    int nday = 0;
  int totDays = 62; // From 1st December, 2020 to 31st January, 2021
  int cday = 1606867200; //2nd December, 2020;
  int dday = 86400; 
  int tmpMean = 0;

  // For PMT 1   
  TH2F pmt1lmeanf("pmt1lmeanf", "Mean for First 100 bins of Baseline PMTSSD LG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmt1lmeanl("pmt1lmeanl", "Mean for Last 100 bins of Baseline PMTSSD LG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmt1lrmsf("pmt1lrmsf", "RMS for First 100 bins of Baseline PMTSSD LG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmt1lrmsl("pmt1lrmsl", "RMS for Last 100 bins of Baseline PMTSSD LG", totDays, 0, totDays, totSt, 0, totSt);

  //TH2F pmt1ldiffmean("pmt1ldiffmean", "Difference of the mean of First and Last 100 bins of Baseline PMT 1 LG", totDays, 0, totDays, totSt, 0, totSt);
  //TH2F pmt1ldiffrms("pmt1ldiffrms", "Difference of the RMS of First and Last 100 bins of Baseline PMT 1 LG", totDays, 0, totDays, totSt, 0, totSt);


  TH2F pmt1hmeanf("pmt1hmeanf", "Mean for First 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmt1hmeanl("pmt1hmeanl", "Mean for Last 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmt1hrmsf("pmt1hrmsf", "RMS for First 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmt1hrmsl("pmt1hrmsl", "RMS for Last 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmt1hdiffmean("pmt1hdiffmean", "Difference of the Mean for First and Last 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmt1hdiffrms("pmt1hdiffrms", "Difference of the RMS for First and Last 100 bins of Baseline PMTSSD HG", totDays, 0, totDays, totSt, 0, totSt);

  //TH2F ksdistpmt1h("ksdistpmt1h", "KS-Test for First and Last 100 bins of Baseline PMT 1 HG", totDays, 0, totDays, totSt, 0, totSt);


  // For SSD
  /*
  TH2F ssdmeanf("ssdmeanf", "Mean for First 100 bins of Baseline ", totDays, 0, totDays, totSt, 0, totSt);
  TH2F ssdmeanl("ssdmeanl", "Mean for Last 100 bins of Baseline CALIB", totDays, 0, totDays, totSt, 0, totSt);

  TH2F ssdrmsf("ssdrmsf", "RMS for First 100 bins of Baseline CALIB", totDays, 0, totDays, totSt, 0, totSt);
  TH2F ssdrmsl("ssdrmsl", "RMS for Last 100 bins of Baseline CALIB", totDays, 0, totDays, totSt, 0, totSt);

  TH2F ksdistcalib("ksdistcalib", "KS-Test for First and Last 100 bins of Baseline CALIB", totDays, 0, totDays, totSt, 0, totSt);
  */

  int lengthbl = 2048;

  int blpmt1h[lengthbl];
  int blpmt1l[lengthbl];
/*
  int blpmt2h[lengthbl];
  int blpmt2l[lengthbl];

  int blpmt3h[lengthbl];
  int blpmt3l[lengthbl];

  int blspmt[lengthbl];
  int blcali[lengthbl];

  int blssdh[lengthbl];
  int blssdl[lengthbl];
*/
  int previusEvent = 0;
  int sameUtc = 0;

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

    if ( event.Id == previusEvent ) //&& sameUtc == event.utctime() )
      continue;

    previusEvent = event.Id;
    sameUtc = event.utctime();
    
    if ( sameUtc > cday ){
      for ( int id=0; id<totSt; id++){
        if ( stckEvt[id] > 0 ){
          pmt1hmeanf.Fill( nday , id, stckMean[id][0]/stckEvt[id] );
          pmt1hmeanl.Fill( nday , id, stckMean[id][1]/stckEvt[id] );
          pmt1lmeanf.Fill( nday , id, stckMean[id][2]/stckEvt[id] );
          pmt1lmeanl.Fill( nday , id, stckMean[id][3]/stckEvt[id] );
          pmt1hdiffmean.Fill( nday, id, stckMean[id][0]/stckEvt[id] - stckMean[id][1]/stckEvt[id] );
          pmt1hrmsf.Fill( nday , id, stckRMS[id][0]/stckEvt[id] );
          pmt1hrmsl.Fill( nday , id, stckRMS[id][1]/stckEvt[id] );
          pmt1lrmsf.Fill( nday , id, stckRMS[id][2]/stckEvt[id] );
          pmt1lrmsl.Fill( nday , id, stckRMS[id][3]/stckEvt[id] );
          pmt1hdiffrms.Fill( nday, id, stckRMS[id][0]/stckEvt[id] - stckRMS[id][1]/stckEvt[id] );
          for ( int k=0; k<4; k++ ){
            stckMean[id][k] = 0;
            stckRMS[id][k] = 0;
            stckEvt[id] = 0;
          }
        }
      }
      cout << "msd " << cday << " " << nday << endl;
      cday += dday;
      nday++;
    }

    for (unsigned int i = 0 ; i < event.Stations.size(); ++i){
      found = false;
      for (  vector<unsigned int>::const_iterator iter= stationsIds.begin();
          iter!= stationsIds.end(); ++iter)
        if (event.Stations[i].Id == *iter )
          found = true;
      if ( !found )
        continue;     
      
      if ( event.Stations[i].IsUUB ){
        cout << "# Event " << event.Id << " Station " << event.Stations[i].Id
          << " " << nrEventsRead-1 << " " << sameUtc
          << endl;
 
        IoSdEvent event(pos);
        
        if (event.Stations[i].Error==256) { //0+256
          for (int k=0;k<event.Stations[i].UFadc->NSample;k++){
            blpmt1h[k] = event.Stations[i].UFadc->GetValue(4,0,k);
            blpmt1l[k] = event.Stations[i].UFadc->GetValue(4,1,k);
    /*
            blpmt2h[k] = event.Stations[i].UFadc->GetValue(0,0,k);
            blpmt2l[k] = event.Stations[i].UFadc->GetValue(0,1,k);
    
            blpmt3h[k] = event.Stations[i].UFadc->GetValue(0,0,k);
            blpmt3l[k] = event.Stations[i].UFadc->GetValue(0,1,k);
    
            blspmth[k] = event.Stations[i].UFadc->GetValue(0,0,k);
            blspmtl[k] = event.Stations[i].UFadc->GetValue(0,1,k);
    
            blssdh[k] = event.Stations[i].UFadc->GetValue(0,0,k);
            blssdl[k] = event.Stations[i].UFadc->GetValue(0,1,k);
            */
          }
          for ( int id=0; id<totSt; id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              stckEvt[id]++;
              tmpMean = getmean(blpmt1h, nblbins, 0);
              stckMean[id][0] += tmpMean;
              stckRMS[id][0] += getrms(blpmt1h, tmpMean, nblbins, 0);
              tmpMean = getmean(blpmt1h, nblbins, 1);
              stckMean[id][1] += tmpMean;
              stckRMS[id][1] += getrms(blpmt1h, tmpMean, nblbins, 1);
    
              tmpMean = getmean(blpmt1l, nblbins, 0);
              stckMean[id][2] += tmpMean;
              stckRMS[id][2] += getrms(blpmt1l, tmpMean, nblbins, 0);
              tmpMean = getmean(blpmt1l, nblbins, 1);
              stckMean[id][3] += tmpMean;
              stckRMS[id][3] += getrms(blpmt1l, tmpMean, nblbins, 1);
              break;
            } 
             /*
          for ( int id=0; id<totSt; id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              pmt1hmeanf.Fill( nrEventsRead-1 , id, getmean(blpmt1h, nblbins, 0) );
              pmt1hmeanl.Fill( nrEventsRead-1 , id, getmean(blpmt1h, nblbins, 1) );

              pmt1hrmsf.Fill( nrEventsRead-1 , id, getrms(blpmt1h, getmean(blpmt1h, nblbins, 0), nblbins, 0) );
              pmt1hrmsl.Fill( nrEventsRead-1 , id, getrms(blpmt1h, getmean(blpmt1h, nblbins, 0), nblbins, 0) );

              pmt1lmeanf.Fill( nrEventsRead-1 , id, getmean(blpmt1l, nblbins, 0) );
              pmt1lmeanl.Fill( nrEventsRead-1 , id, getmean(blpmt1l, nblbins, 1) );

              pmt1lrmsf.Fill( nrEventsRead-1 , id, getrms(blpmt1l, getmean(blpmt1l, nblbins, 0), nblbins, 0) );
              pmt1lrmsl.Fill( nrEventsRead-1 , id, getrms(blpmt1l, getmean(blpmt1l, nblbins, 1), nblbins, 0) );

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
            */
          
        }
      }
    }    
  }

  hfile.Write();
  hfile.Close();

  return 0;
}
