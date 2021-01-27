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

  TFile hfile("rms"+nameStati+"jan2021.root","RECREATE","");

  TH1F pmt0rl("pmt0rl","RMS base line PMT0 Low", 10000, 0, 10);
  TH1F pmt1rl("pmt1rl","RMS base line PMT1 Low", 10000, 0, 10);
  TH1F pmt2rl("pmt2rl","RMS base line PMT2 Low", 10000, 0, 10);
  TH1F spmtrl("spmtrl","RMS base line SPMT Low", 10000, 0, 10);
  TH1F ssdpmtrl("ssdpmtrl","RMS base line SSDPMT Los", 10000, 0, 10);

  TH1F pmt0rh("pmt0rh","RMS base line PMT0 High", 10000, 0, 10);
  TH1F pmt1rh("pmt1rh","RMS base line PMT1 High", 10000, 0, 10);
  TH1F pmt2rh("pmt2rh","RMS base line PMT2 High", 10000, 0, 10);
  TH1F spmtrh("spmtrh","RMS base line SPMT High", 10000, 0, 10);
  TH1F ssdpmtrh("ssdpmtrh","RMS base line SSDPMT Los", 10000, 0, 10);

  TH1F tmp0rl("tmp0rl","RMS base line PMT0 Low", 2047, 0, 2048);
  TH1F tmp1rl("tmp1rl","RMS base line PMT1 Low", 2047, 0, 2048);
  TH1F tmp2rl("tmp2rl","RMS base line PMT2 Low", 2047, 0, 2048);
  TH1F stmprl("stmprl","RMS base line SPMT Low", 2047, 0, 2048);
  TH1F ssdtmprl("ssdtmprl","RMS base line SSDPMT Los", 2047, 0, 2048);

  TH1F tmp0rh("tmp0rh","RMS base line PMT0 High", 2047, 0, 2048);
  TH1F tmp1rh("tmp1rh","RMS base line PMT1 High", 2047, 0, 2048);
  TH1F tmp2rh("tmp2rh","RMS base line PMT2 High", 2047, 0, 2048);
  TH1F stmprh("stmprh","RMS base line SPMT High", 2047, 0, 2048);
  TH1F ssdtmprh("ssdtmprh","RMS base line SSDPMT Los", 2047, 0, 2048);

  double rms0l, rms1l, rms2l, rmssl, rmsssdl;
  double rms0h, rms1h, rms2h, rmssh, rmsssdh;

  rms0l = rms0h = 0;
  rms1l = rms1h = 0;
  rms2l = rms2h = 0;
  rmssl = rmssh = 0;
  rmsssdl = rmsssdh =0;


  TCanvas c1("c1","test",0,0,3600,2400);
  c1.Divide(2,5);

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
          //cout << "# sizeTraces: " 
               //cout << event.Stations[i].UFadc->Traces.size() << endl;
          //for (int k=0;k<event.Stations[i].UFadc->NSample;k++) {
          for (int k=0;k<100;k++){ //event.Stations[i].UFadc->NSample;k++) {
            //cout << k << endl;

            tmp0rl.Fill(event.Stations[i].UFadc->GetValue(0,1,k));
            tmp1rl.Fill(event.Stations[i].UFadc->GetValue(1,1,k));
            tmp2rl.Fill(event.Stations[i].UFadc->GetValue(2,1,k));
            stmprl.Fill(event.Stations[i].UFadc->GetValue(3,1,k));
            ssdtmprl.Fill(event.Stations[i].UFadc->GetValue(4,1,k));

            tmp0rh.Fill(event.Stations[i].UFadc->GetValue(0,0,k));
            tmp1rh.Fill(event.Stations[i].UFadc->GetValue(1,0,k));
            tmp2rh.Fill(event.Stations[i].UFadc->GetValue(2,0,k));
            stmprh.Fill(event.Stations[i].UFadc->GetValue(3,0,k));
            ssdtmprh.Fill(event.Stations[i].UFadc->GetValue(4,0,k));
          }
          //for (int j=0;j<5;j++) // Run on type of PMT
              //cout << " " << event.Stations[i].UFadc->GetValue(j,0,k) << " " // 0 for high; 1 for low
              //     << event.Stations[i].UFadc->GetValue(j,1,k);
            //cout << endl;
          
        }
          pmt0rl.Fill(tmp0rl.GetRMS());
          pmt1rl.Fill(tmp1rl.GetRMS());
          pmt2rl.Fill(tmp2rl.GetRMS());
          spmtrl.Fill(stmprl.GetRMS());
          ssdpmtrl.Fill(ssdtmprl.GetRMS());

          pmt0rh.Fill(tmp0rh.GetRMS());
          pmt1rh.Fill(tmp1rh.GetRMS());
          pmt2rh.Fill(tmp2rh.GetRMS());
          spmtrh.Fill(stmprh.GetRMS());
          ssdpmtrh.Fill(ssdtmprh.GetRMS());

          tmp0rl.Reset();
          tmp1rl.Reset();
          tmp2rl.Reset();
          stmprl.Reset();
          ssdtmprl.Reset();

          tmp0rh.Reset();
          tmp1rh.Reset();
          tmp2rh.Reset();
          stmprh.Reset();
          ssdtmprh.Reset();
      }
    }
  }

  c1.cd(1);
  pmt0rl.Draw();
  c1.cd(2);
  pmt0rh.Draw();
  c1.cd(3);
  pmt1rl.Draw();
  c1.cd(4);
  pmt1rh.Draw();
  c1.cd(5);
  pmt2rl.Draw();
  c1.cd(6);
  pmt2rh.Draw();
  c1.cd(7);
  spmtrl.Draw();
  c1.cd(8);
  spmtrh.Draw();
  c1.cd(9);
  ssdpmtrl.Draw();
  c1.cd(10);
  ssdpmtrh.Draw();

  c1.Print("../plots/jan2021/rms"+nameStati+".pdf");

  hfile.Write();
  hfile.Close();

  return 0;
}
