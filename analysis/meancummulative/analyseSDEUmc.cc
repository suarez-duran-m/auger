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

float getGloMean( TH2F *histo, int st ){
  float mean = 0.;
  int tmp = 0;
  int okEvent = 0;
  for (int i=0; i<histo->GetNbinsX(); i++){
    tmp = histo->GetBinContent(i, st+1);
    if ( tmp > 0 ){
      mean += tmp;
      okEvent++;
    }
  }
  return mean/okEvent;
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
 
  TFile *hfile = new TFile("../compareBlBins/baseline100bins.root");
  TFile *outfile = new TFile("filterBaseline100bins.root","RECREATE");

  TH2F *pmt1hmf = (TH2F*)hfile->Get("pmt1hmeanf");
  TH2F *pmt1hml = (TH2F*)hfile->Get("pmt1hmeanl");
  TH2F *pmt1hrf = (TH2F*)hfile->Get("pmt1hrmsf");
  TH2F *pmt1hrl = (TH2F*)hfile->Get("pmt1hrmsl");

  int stSize = stationsIds.size();

  float gmf [stSize];
  float gml [stSize];
  float grf [stSize];
  float grl [stSize];

  float cumeanf = 0.;
  float cumeanl = 0.;

  sort(stationsIds.begin(), stationsIds.end());
  Int_t stationsBins[stSize];
  for ( int i=0; i<stSize; i++){
    stationsBins[i] = stationsIds[i];
    gmf[i] = getGloMean( pmt1hmf, i );
    gml[i] = getGloMean( pmt1hml, i );
    grf[i] = getGloMean( pmt1hrf, i );
    grl[i] = getGloMean( pmt1hrl, i );
    cout << i+1 << " -> " << stationsIds[i] << endl;
    cout << "Mean: " << gmf[i] << " " << gml[i] << endl;
    cout << "RMS: " << grf[i] << " " << grl[i] << endl;
  }

  int nblbins = 100;

  // For PMT 1   
  TH2F pmt1hmOkf("pmt1hmOkf", "Mean for First 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stSize, 0, stSize);
  TH2F pmt1hmOkl("pmt1hmOkl", "Mean for Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stSize, 0, stSize);
  TH2F pmt1hrOkf("pmt1hrOkf", "RMS for First 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stSize, 0, stSize);
  TH2F pmt1hrOkl("pmt1hrOkl", "RMS for Last 100 bins of Baseline PMT 1 HG", totalNrEvents, 0, totalNrEvents, stSize, 0, stSize);

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
          cumeanf = cumeanl = 0.;
          for ( int k=0; k<nblbins; k++ ){
            cumeanf += event.Stations[i].UFadc->GetValue(0, 0, k);
            cumeanl += event.Stations[i].UFadc->GetValue(0, 0, (nsample - k));
          }
          for ( int id=0; id<stSize; id++)
            if ( stationsBins[id] == event.Stations[i].Id ){
              if( cumeanf >= nblbins*(gmf[id]-grf[id]) && cumeanf <= nblbins*(gmf[id]+grf[id]) ){
                pmt1hmOkf.Fill( nrEventsRead-1 , id, pmt1hmf->GetBinContent(nrEventsRead, id+1) );
                pmt1hrOkf.Fill( nrEventsRead-1 , id, pmt1hrf->GetBinContent(nrEventsRead, id+1) );
              }
              else if( cumeanl >= nblbins*(gml[id]-1.5*grl[id]) && cumeanl <= nblbins*(gml[id]+1.5*grl[id]) ){
                pmt1hmOkl.Fill( nrEventsRead-1 , id, pmt1hml->GetBinContent(nrEventsRead, id+1) );
                pmt1hrOkl.Fill( nrEventsRead-1 , id, pmt1hrl->GetBinContent(nrEventsRead, id+1) );
              } 
              else
                cout << "rejected: " << event.Id << " " 
                  << event.Stations[i].Id << " "
                  << gmf[id] << " " << grf[id] << " " 
                  << cumeanf << " "
                  << pmt1hmf->GetBinContent(nrEventsRead, id+1)
                  << endl;
            }
        }
      }
    }
  }

  outfile->Write();
  outfile->Write();

  return 0;
}
