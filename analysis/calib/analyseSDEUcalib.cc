#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <IoAuger.h>
#include <Ec.h>

#include <TFile.h>
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

float kstest( int arr[], int nb){
  float ks = 0.;
  int min0 = 1000;
  int max0 = 0;
  int min1 = 1000;
  int max1 = 0;
  int ming = 0;
  int maxg = 0;

  for (int i=0; i<nb; i++){
    if ( arr[i] > 700 )
    if (arr[i]<min0)
      min0 = arr[i];
    if (arr[i]>max0)
      max0 = arr[i];
    if (arr[2047-i]<min1)
      min1 = arr[2047-i];
    if (arr[2047-i]>max1)
      max1 = arr[2047-i];
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
      if ( arr[i] <= tmpbin+0.5 )
        cumul0[j] += 1.;
      if ( arr[2047-i] <= tmpbin+0.5 )
        cumul1[j] += 1.;
    }
    tmpbin += delBin;
  }
  for ( int i=0; i<nbins; i++)
    if ( (cumul0[i]-cumul1[i]) > ks )
      ks = (cumul0[i]-cumul1[i]);

  //cout << "msd " << ks << endl;
  return ks;
}

int askTrKS(int arr1[], int arr2[], int arr3[],int arrs[], int arrc[], int arrssd[], int nb, float cr){
  int a, b, c, d, e, f;
  a = b = c = d = e = f = 0;
  if ( kstest( arr1, nb ) > cr )
    a = 1;
  if ( kstest( arr2, nb ) > cr )
    b = 1;
  if ( kstest( arr3, nb ) > cr )
    c = 1;
  if ( kstest( arrs, nb ) > cr )
    d = 1;
  if ( kstest( arrc, nb ) > cr )
    e = 1;
  if ( kstest( arrssd, nb ) > cr )
    f = 1;
  return a+b*2+c*4+d*8+e*16+f*32;
}

// ================================
// ************ MAIN **************
// ================================

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
 

  int nblbins = 100;

  int pmt1ht[2048];
  int pmt2ht[2048];
  int pmt3ht[2048];
  int pmtsht[2048];
  int caliht[2048];
  int ssdht[2048];
  
  IoSd *sdFile = NULL;
  AugerFile *adFile = NULL;

  TString nameStati = to_string( stationsIds[0] );
  TFile hfile("ch"+nameStati+".root","RECREATE","");
  
  TTree tree("T","");
  TTree treed("D","");

  TH1F *pmt1hc = new TH1F("pmt1hc","Charge for PMT1 HG",10000,0,10001);
  TH1F *pmt1hp = new TH1F("pmt1hp","Peak for PMT1 HG",1500,0,1501);

  TH1F *pmt2hc = new TH1F("pmt2hc","Charge for PMT2 HG",10000,0,10001);
  TH1F *pmt2hp = new TH1F("pmt2hp","Peak for PMT2 HG",1500,0,1501);

  TH1F *pmt3hc = new TH1F("pmt3hc","Charge for PMT3 HG",10000,0,10001);
  TH1F *pmt3hp = new TH1F("pmt3hp","Peak for PMT3 HG",1500,0,1501);

  TH1F *pmtshc = new TH1F("spmthc","Charge for SPMT",10000,0,10001);
  TH1F *pmtshp = new TH1F("spmthp","Peak for SPMT",1500,0,1501);

  TH1F *calihc = new TH1F("calihc","Charge for CALIB",10000,0,10001);
  TH1F *calihp = new TH1F("calihp","Peak for CALIB",1500,0,1501);

  TH1F *ssdhc = new TH1F("ssdhc","Charge for SSDPMT1 HG",10000,0,10001);
  TH1F *ssdhp = new TH1F("ssdhp","Peak for SSDPMT1 HG",1500,0,1501);

  TH1F pmt1htr("pmt1htr","Trace for PMT1 HG",2048,0,2048);
  TH1F pmt2htr("pmt2htr","Trace for PMT2 HG",2048,0,2048);
  TH1F pmt3htr("pmt3htr","Trace for PMT3 HG",2048,0,2048);
  TH1F pmtshtr("pmtshtr","Trace for SPMT",2048,0,2048);
  TH1F calihtr("calihtr","Trace for CALIB",2048,0,2048);
  TH1F ssdhtr("ssdhtr","Trace for SSDPMT HG",2048,0,2048);
  Int_t evId = 0;

  tree.Branch("pmt1hc","TH1F",&pmt1hc);
  tree.Branch("pmt1hp","TH1F",&pmt1hp);

  tree.Branch("pmt2hc","TH1F",&pmt2hc);
  tree.Branch("pmt2hp","TH1F",&pmt2hp);

  tree.Branch("pmt3hc","TH1F",&pmt3hc);
  tree.Branch("pmt3hp","TH1F",&pmt3hp);

  tree.Branch("pmtshc","TH1F",&pmtshc);
  tree.Branch("pmtshp","TH1F",&pmtshp);

  tree.Branch("calihc","TH1F",&calihc);
  tree.Branch("calihp","TH1F",&calihp);

  tree.Branch("ssdhc","TH1F",&ssdhc);
  tree.Branch("ssdhp","TH1F",&ssdhp);

  tree.Branch("pmt1htr","TH1F",&pmt1htr);
  tree.Branch("pmt2htr","TH1F",&pmt2htr);
  tree.Branch("pmt3htr","TH1F",&pmt3htr);
  tree.Branch("pmtshtr","TH1F",&pmtshtr);
  tree.Branch("calihtr","TH1F",&calihtr);
  tree.Branch("ssdhtr","TH1F",&ssdhtr);
  tree.Branch("evId",&evId,"evId/I");


  // For discarded Events
  TH1F dpmt1htr("dpmt1htr","Trace for PMT1 HG",2048,0,2048);
  TH1F dpmt2htr("dpmt2htr","Trace for PMT2 HG",2048,0,2048);
  TH1F dpmt3htr("dpmt3htr","Trace for PMT3 HG",2048,0,2048);
  TH1F dpmtshtr("dpmtshtr","Trace for SPMT",2048,0,2048);
  TH1F dcalihtr("dcalihtr","Trace for CALIB",2048,0,2048);
  TH1F dssdhtr("dssdhtr","Trace for SSDPMT HG",2048,0,2048);

  Int_t devId = 0;
  Int_t trigg = 0;

  treed.Branch("dpmt1htr","TH1F",&dpmt1htr);
  treed.Branch("dpmt2htr","TH1F",&dpmt2htr);
  treed.Branch("dpmt3htr","TH1F",&dpmt3htr);
  treed.Branch("dpmtshtr","TH1F",&dpmtshtr);
  treed.Branch("dcalihtr","TH1F",&dcalihtr);
  treed.Branch("dssdhtr","TH1F",&dssdhtr);
  treed.Branch("devId",&devId,"devId/I");
  treed.Branch("trigg",&trigg,"trigg/I");
  

  int previusEvent = 0;
  int sameUtc = 0;
  int nsample = 0;
  int ksTri = 0;

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
               << endl;

          IoSdEvent event(pos);
          if (event.RootClassName == "AugerEvent") {
            if (!adFile)
              adFile = new AugerFile(outputName, AugerFile::eWrite);
            adFile->Write(*(event.RawAugerEvent()), false);
          } 
          else { 
            if (!sdFile)
              sdFile = new IoSd(outputName, "w");
            sdFile->Write(event, "");
          }

          if (event.Stations[i].Error==256) { //0+256
            nsample = event.Stations[i].UFadc->NSample;
            for (int k=0; k<nsample; k++){
              pmt1ht[k] = event.Stations[i].UFadc->GetValue(0,0,k);
              pmt2ht[k] = event.Stations[i].UFadc->GetValue(1,0,k);
              pmt3ht[k] = event.Stations[i].UFadc->GetValue(2,0,k);
              pmtsht[k] = event.Stations[i].UFadc->GetValue(3,1,k);
              caliht[k] = event.Stations[i].UFadc->GetValue(3,0,k);
              ssdht[k] = event.Stations[i].UFadc->GetValue(4,0,k);
            }
            ksTri = askTrKS( pmt1ht, pmt2ht, pmt3ht, pmtsht, caliht, ssdht, nblbins, 40. );
            //muTri = askTrMu( pmt1ht );
            if ( ksTri==0 ){
              pmt1hc = event.Stations[i].HCharge(0);
              pmt1hp = event.Stations[i].HPeak(0);

              pmt2hc = event.Stations[i].HCharge(1);
              pmt2hp = event.Stations[i].HPeak(1);

              pmt3hc = event.Stations[i].HCharge(2);
              pmt3hp = event.Stations[i].HPeak(2);

              pmtshc = event.Stations[i].HCharge(3);
              pmtshp = event.Stations[i].HPeak(3);

              calihc = event.Stations[i].HCharge(4);
              calihp = event.Stations[i].HPeak(4);

              ssdhc = event.Stations[i].HCharge(5);
              ssdhp = event.Stations[i].HPeak(5);
              evId = event.Id;

              for (int k=0; k<nsample; k++){
                pmt1htr.Fill(k, pmt1ht[k]);
                pmt2htr.Fill(k, pmt2ht[k]);
                pmt3htr.Fill(k, pmt3ht[k]);
                pmtshtr.Fill(k, pmtsht[k]);
                calihtr.Fill(k, caliht[k]);
                ssdhtr.Fill(k, ssdht[k]);
              }
              tree.Fill();
            }
            else{
              devId = event.Id;
              trigg = ksTri;
              for (int k=0; k<nsample; k++){
                dpmt1htr.Fill(k, pmt1ht[k]);
                dpmt2htr.Fill(k, pmt2ht[k]);
                dpmt3htr.Fill(k, pmt3ht[k]);
                dpmtshtr.Fill(k, pmtsht[k]);
                dcalihtr.Fill(k, caliht[k]);
                dssdhtr.Fill(k, ssdht[k]);
              }
              treed.Fill();

            }
        }
          dpmt1htr.Reset();
          dpmt2htr.Reset();
          dpmt3htr.Reset();
          dpmtshtr.Reset();
          dcalihtr.Reset();
          dssdhtr.Reset();

          pmt1htr.Reset();
          pmt2htr.Reset();
          pmt3htr.Reset();
          pmtshtr.Reset();
          calihtr.Reset();
          ssdhtr.Reset();
      }
    }
  }
  tree.Print();
  hfile.Write();
  hfile.Close();

  return 0;
}
