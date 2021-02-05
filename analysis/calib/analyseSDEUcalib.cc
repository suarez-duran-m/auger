#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <IoAuger.h>
#include <Ec.h>

#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>

using namespace std;


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
  
  /*cout << "======> I am  selecting events containing stations: "; 
  for (  vector<unsigned int>::const_iterator iter= stationsIds.begin();
         iter!= stationsIds.end(); ++iter)
    cout << *iter << " ";
  cout << " <====== " << endl;
  */
  
  IoSd *sdFile = NULL;
  AugerFile *adFile = NULL;

  TString nameStati = to_string( stationsIds[0] );
  TFile hfile("ch"+nameStati+".root","RECREATE","");
  
  TTree tree("T","");

  TH1F *pmt1l = new TH1F("pmt1l","Charge for PMT1 Low",600,0,601);
  TH1F *pmt1pk = new TH1F("pmt1pk","Peak for PMT1 Low",600,0,601);
  TH1F pmt2l("pmt2l","Charge for PMT2 Low",600,0,601);
  TH1F pmt3l("pmt3l","Charge for PMT3 Low",600,0,601);
  TH1F spmtl("spmtl","Charge for SPMT Low",600,0,601);
  TH1F ssdpmtl("ssdpmtl","Charge for SSDPMT Low",600,0,601);
  
  tree.Branch("pmt1l","TH1F",&pmt1l, 128000, 0);
  tree.Branch("pmt1pk","TH1F",&pmt1pk, 128000, 0);

/*
  TH1F pmt0h("pmt0h","Traces for PMT0 High",2047,0,2048);
  TH1F pmt1h("pmt1h","Traces for PMT1 High",2047,0,2048);
  TH1F pmt2h("pmt2h","Traces for PMT2 High",2047,0,2048);
  TH1F spmth("spmth","Traces for SPMT High",2047,0,2048);
  TH1F ssdpmth("ssdpmth","Traces for SSDPMT High",2047,0,2048);

  TCanvas c1("c1","test",0,0,3600,2400);
  c1.Divide(2,5);
*/
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
      if ( event.Stations[i].IsUUB ) {
        cout << "# Event " << event.Id << " Station " << event.Stations[i].Id
             << endl;
        //cout << "# Error " << event.Stations[i].Error << endl;

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
          pmt1l = event.Stations[i].HCharge(3);
          pmt1pk = event.Stations[i].HPeak(3);

          tree.Fill();
        }
      }
    }
  }
  tree.Print();
  hfile.Write();
  hfile.Close();

  return 0;
}
