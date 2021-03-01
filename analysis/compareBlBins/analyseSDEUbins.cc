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

double getmean( int arr[], int nb, int lok ){
  double mean = 0.;
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
// ========================== 
// ******** The MAIN ********
// ==========================
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
  const char* whichpmt = argv[2];
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
  
  TString nameStati = to_string( stationsIds[0] );
  TString pmtname = whichpmt;
  int pmtId= atoi( pmtname );
  if ( pmtId > 0 && pmtId < 4 ){
     pmtname = "PMT"+to_string( pmtId );
  //c5.Print("testRmsLast100Hg.pdf");
  }
  else if ( pmtId == 4 )
    pmtname = "SPMT";
  else if ( pmtId == 5 )
    pmtname = "PMTSSD";
  else{
    cerr << "==================================================" << endl;
    cerr << "Wrong Id for PMT, please introduce a valid PMT Id:" << endl;
    cerr << "1 For PMT1; " << "2 For PMT2; " << "3 For PMT3; " 
      << "4 For SPMT; " << "5 For PMTSSD" << endl;
    cerr << "==================================================" << endl;
    exit(0);
  }
  cerr << "You have selected " << pmtname << endl;
  
  TFile hfile("bl100bins"+pmtname+".root","RECREATE","");

  int totSt = stationsIds.size();

  vector < int > stckEvt;
  stckEvt.resize(totSt);
  vector < vector < float > > stckMean;
  stckMean.resize(totSt);
  vector < vector < float > > stckRMS;
  stckRMS.resize(totSt);

  sort(stationsIds.begin(), stationsIds.end());
  Double_t stationsBins[totSt];
  for ( int i=0; i<totSt; i++){
    stationsBins[i] = stationsIds[i];
    stckEvt[i] = 0;
    for ( int j=0; j<20; j++ ){
      stckMean[i].push_back(0);
      stckRMS[i].push_back(0.);
    }
    cout << i+1 << " -> " << stationsIds[i] << endl;
  }
  
  int nblbins = 100;
  int nday = 0;
  int totDays = 62; // From 1st December, 2020 to 31st January, 2021
  int cday = 1606867200; //2nd December, 2020;
  int dday = 86400; 
  double tmpMean = 0;
  double tmpMeanSt1740f = 0;
  double tmpMeanSt1740l = 0;
  double tmpMeanSt1740f2 = 0;
  double tmpMeanSt1740l2 = 0;
  double tmpMeanSt1740f3 = 0;
  double tmpMeanSt1740l3 = 0;

  double tmpMeanSt1818f = 0;
  double tmpMeanSt1818l = 0;
  double tmpMeanSt1818f2 = 0;
  double tmpMeanSt1818l2 = 0;
  double tmpMeanSt1818f3 = 0;
  double tmpMeanSt1818l3 = 0;




  TH1F st1740diffDist("st1740diffDist", "Distribution of The Difference of The Mean for he Station 1740's PMT1 HG",31, -15, 15);
  TH1F st1740diffDist2("st1740diffDist2", "Distribution of The Difference of The Mean for he Station 1740's PMT2 HG",31, -15, 15);
  TH1F st1740diffDist3("st1740diffDist3", "Distribution of The Difference of The Mean for he Station 1740's PMT3 HG",31, -15, 15);
  TH1F st1818diffDist("st1818diffDist", "Distribution of The Difference of The Mean for The Station 1818's PMT1 HG", 31, -15, 15); 
  TH1F st1818diffDist2("st1818diffDist2", "Distribution of The Difference of The Mean for The Station 1818's PMT2 HG", 31, -15, 15); 
  TH1F st1818diffDist3("st1818diffDist3", "Distribution of The Difference of The Mean for The Station 1818's PMT3 HG", 31, -15, 15); 
  
  // For Low Gain
  TH2F pmtlmeanf("pmtlmeanf", "Mean for First 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmtlmeanl("pmtlmeanl", "Mean for Last 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmtlrmsf("pmtlrmsf", "RMS for First 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmtlrmsl("pmtlrmsl", "RMS for Last 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmtldiffmean("pmtldiffmean", "Difference of the mean of First and Last 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmtldiffrms("pmtldiffrms", "Difference of the RMS of First and Last 100 bins of Baseline "+pmtname+" LG", totDays, 0, totDays, totSt, 0, totSt);

  // For High Gain
  TH2F pmthmeanf("pmthmeanf", "Mean for First 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmthmeanl("pmthmeanl", "Mean for Last 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmthrmsf("pmthrmsf", "RMS for First 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmthrmsl("pmthrmsl", "RMS for Last 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);

  TH2F pmthdiffmean("pmthdiffmean", "Difference of the Mean for First and Last 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);
  TH2F pmthdiffrms("pmthdiffrms", "Difference of the RMS for First and Last 100 bins of Baseline "+pmtname+" HG", totDays, 0, totDays, totSt, 0, totSt);


  int lengthbl = 2048;

  int blpmth[lengthbl];
  int blpmth2[lengthbl];
  int blpmth3[lengthbl];
  int blpmtl[lengthbl];

  unsigned int previusEvent = 0;
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
          pmthmeanf.Fill( nday , id, stckMean[id][0]/stckEvt[id] );
          pmthmeanl.Fill( nday , id, stckMean[id][1]/stckEvt[id] );
          pmthrmsf.Fill( nday , id, stckRMS[id][0]/stckEvt[id] );
          pmthrmsl.Fill( nday , id, stckRMS[id][1]/stckEvt[id] );
          pmthdiffmean.Fill( nday, id, stckMean[id][0]/stckEvt[id] - stckMean[id][1]/stckEvt[id] );
          pmthdiffrms.Fill( nday, id, stckRMS[id][0]/stckEvt[id] - stckRMS[id][1]/stckEvt[id] );
          //if ( id==0 )
            //cout << "msd: " << nday << " " 
              //<< stckRMS[id][0]/stckEvt[id] - stckRMS[id][1]/stckEvt[id] << endl;

          pmtlmeanf.Fill( nday , id, stckMean[id][2]/stckEvt[id] );
          pmtlmeanl.Fill( nday , id, stckMean[id][3]/stckEvt[id] );
          pmtlrmsf.Fill( nday , id, stckRMS[id][2]/stckEvt[id] );
          pmtlrmsl.Fill( nday , id, stckRMS[id][3]/stckEvt[id] );
          pmtldiffmean.Fill( nday, id, stckMean[id][2]/stckEvt[id] - stckMean[id][3]/stckEvt[id] );
          pmtldiffrms.Fill( nday, id, stckRMS[id][2]/stckEvt[id] - stckRMS[id][3]/stckEvt[id] );

          for ( int k=0; k<4; k++ ){
            stckMean[id][k] = 0;
            stckRMS[id][k] = 0;
            stckEvt[id] = 0;
          }
        }
      }
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
          for (unsigned int k=0;k<event.Stations[i].UFadc->NSample;k++){
            blpmth[k] = event.Stations[i].UFadc->GetValue(pmtId-1,0,k);
            blpmth2[k] = event.Stations[i].UFadc->GetValue(1,0,k);
            blpmth3[k] = event.Stations[i].UFadc->GetValue(2,0,k);
            blpmtl[k] = event.Stations[i].UFadc->GetValue(pmtId-1,1,k);
            if (k==1500 && blpmth[k] > 2000)
              cerr << event.Id << endl;
          }
          for ( int id=0; id<totSt; id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              stckEvt[id]++;
              tmpMean = getmean(blpmth, nblbins, 0);
              stckMean[id][0] += tmpMean;
              stckRMS[id][0] += getrms(blpmth, tmpMean, nblbins, 0);
              tmpMean = getmean(blpmth, nblbins, 1);
              stckMean[id][1] += tmpMean;
              stckRMS[id][1] += getrms(blpmth, tmpMean, nblbins, 1);
    
              tmpMean = getmean(blpmtl, nblbins, 0);
              stckMean[id][2] += tmpMean;
              stckRMS[id][2] += getrms(blpmtl, tmpMean, nblbins, 0);
              tmpMean = getmean(blpmtl, nblbins, 1);
              stckMean[id][3] += tmpMean;
              stckRMS[id][3] += getrms(blpmtl, tmpMean, nblbins, 1);

              if (id==9){
                tmpMeanSt1740f = getmean(blpmth, nblbins, 0);
                tmpMeanSt1740l = getmean(blpmth, nblbins, 1);
                st1740diffDist.Fill( tmpMeanSt1740f-tmpMeanSt1740l );
                tmpMeanSt1740f2 = getmean(blpmth2, nblbins, 0);
                tmpMeanSt1740l2 = getmean(blpmth2, nblbins, 1);
                st1740diffDist2.Fill( tmpMeanSt1740f2-tmpMeanSt1740l2 );
                tmpMeanSt1740f3 = getmean(blpmth3, nblbins, 0);
                tmpMeanSt1740l3 = getmean(blpmth3, nblbins, 1);
                st1740diffDist3.Fill( tmpMeanSt1740f3-tmpMeanSt1740l3 );
              }
              if (id==16){
                tmpMeanSt1818f = getmean(blpmth, nblbins, 0);
                tmpMeanSt1818l = getmean(blpmth, nblbins, 1);
                st1818diffDist.Fill( tmpMeanSt1818f-tmpMeanSt1818l );
                tmpMeanSt1818f2 = getmean(blpmth2, nblbins, 0);
                tmpMeanSt1818l2 = getmean(blpmth2, nblbins, 1);
                st1818diffDist2.Fill( tmpMeanSt1818f2-tmpMeanSt1818l2 );
                tmpMeanSt1818f3 = getmean(blpmth3, nblbins, 0);
                tmpMeanSt1818l3 = getmean(blpmth3, nblbins, 1);
                st1818diffDist3.Fill( tmpMeanSt1818f3-tmpMeanSt1818l3 );
              }
              break;
            }
        }
      }
    }    
  }

  hfile.Write();
  hfile.Close();

  return 0;
}
