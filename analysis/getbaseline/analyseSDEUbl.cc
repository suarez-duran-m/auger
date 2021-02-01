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
    
  IoSd *sdFile = NULL;
  AugerFile *adFile = NULL;
  TString nameStati = to_string( stationsIds[0] );

  TFile hfile("msdbl"+nameStati+".root","RECREATE","");

  //TH1F pmt0bl("pmt0bl","Station "+nameStati+" PMT0 Low", 5356800, 1606780800, 1612137600);
  TH1F pmt0blmean("pmt0blmean","Station "+nameStati+" PMT0 Low", 5356800, 1606780800, 1612137600);
  TH1F pmt0blrms("pmt0blrms","Station "+nameStati+" PMT0 Low", 5356800, 1606780800, 1612137600);

  TH1F pmt1bl("pmt1bl","Station "+nameStati+" PMT1 Low", 5356800, 1606780800, 1612137600);
  TH1F pmt2bl("pmt2bl","Station "+nameStati+" PMT2 Low", 5356800, 1606780800, 1612137600);
  TH1F spmtbl("spmtbl","Station "+nameStati+" SPMT Low", 5356800, 1606780800, 1612137600);
  TH1F ssdpmtbl("ssdpmtbl","Station "+nameStati+" SSDPMT Low", 5356800, 1606780800, 1612137600);

  TH1F pmt0bh("pmt0bh","Station "+nameStati+" PMT0 High", 5356800, 1606780800, 1612137600);
  TH1F pmt0bhrms("pmt0bhrms","Station "+nameStati+" PMT0 High", 5356800, 1606780800, 1612137600);

  TH1F pmt1bh("pmt1bh","Station "+nameStati+" PMT1 High", 5356800, 1606780800, 1612137600);
  TH1F pmt2bh("pmt2bh","Station "+nameStati+" PMT2 High", 5356800, 1606780800, 1612137600);
  TH1F ssdpmtbh("ssdpmtbh","Station "+nameStati+" SSDPMT High", 5356800, 1606780800, 1612137600);

  TH1F tmp0bl("tmp0bl","Base line PMT0 Low", 2047, 0, 2048);
  TH1F tmp1bl("tmp1bl","Base line PMT1 Low", 2047, 0, 2048);
  TH1F tmp2bl("tmp2bl","Base line PMT2 Low", 2047, 0, 2048);
  TH1F stmpbl("stmpbl","Base line SPMT Low", 2047, 0, 2048);
  TH1F ssdtmpbl("ssdtmpbl","Base line SSDPMT Low", 2047, 0, 2048);

  TH1F tmp0bh("tmp0bh","Base line PMT0 High", 2047, 0, 2048);
  TH1F tmp1bh("tmp1bh","Base line PMT1 High", 2047, 0, 2048);
  TH1F tmp2bh("tmp2bh","Base line PMT2 High", 2047, 0, 2048);
  TH1F ssdtmpbh("ssdtmpbh","Base line SSDPMT High", 2047, 0, 2048);

  TCanvas c1("c1","test",0,0,3600,2400);
  c1.Divide(2,5);

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
        //cout << "# Error " << event.Stations[i].Error << endl;        

        IoSdEvent event(pos);
        /*
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
        */

        if (event.Stations[i].Error==256) { //0+256
          //cout << "# sizeTraces: " 
               //cout << event.Stations[i].UFadc->Traces.size() << endl;
          //for (int k=0;k<event.Stations[i].UFadc->NSample;k++) {
          for (int k=0;k<100;k++){ //event.Stations[i].UFadc->NSample;k++) {
            //cout << k << endl;           

            tmp0bl.Fill(event.Stations[i].UFadc->GetValue(0,1,k));
            tmp1bl.Fill(event.Stations[i].UFadc->GetValue(1,1,k));
            tmp2bl.Fill(event.Stations[i].UFadc->GetValue(2,1,k));
            stmpbl.Fill(event.Stations[i].UFadc->GetValue(3,1,k));
            ssdtmpbl.Fill(event.Stations[i].UFadc->GetValue(4,1,k));

            tmp0bh.Fill(event.Stations[i].UFadc->GetValue(0,0,k));
            tmp1bh.Fill(event.Stations[i].UFadc->GetValue(1,0,k));
            tmp2bh.Fill(event.Stations[i].UFadc->GetValue(2,0,k));
            ssdtmpbh.Fill(event.Stations[i].UFadc->GetValue(4,0,k));
          }

          //for (int j=0;j<5;j++) // Run on type of PMT
              //cout << " " << event.Stations[i].UFadc->GetValue(j,0,k) << " " // 0 for high; 1 for low
              //     << event.Stations[i].UFadc->GetValue(j,1,k);
            //cout << endl;

          pmt0blmean.Fill( event.utctime(), tmp0bl.GetMean() );
          pmt0blrms.Fill( event.utctime(), tmp0bl.GetRMS() );

          pmt1bl.Fill( event.utctime(), tmp1bl.GetMean() );
          pmt2bl.Fill( event.utctime(), tmp2bl.GetMean() );
          spmtbl.Fill( event.utctime(), stmpbl.GetMean() );
          ssdpmtbl.Fill( event.utctime(), ssdtmpbl.GetMean() );

          if ( tmp0bh.GetRMS() > 1.50539 && tmp0bh.GetRMS() < 2.32268 ) // For Station: 1211
          //if ( tmp0bh.GetRMS() > 1.51178 && tmp0bh.GetRMS() < 2.44757 ) // For Station: 1735
          {
            pmt0bh.Fill( event.utctime(), tmp0bh.GetMean() );
            pmt0bhrms.Fill( event.utctime(), tmp0bh.GetRMS() );
          }
          else
            cout << event.utctime() << " "
              << event.Id << endl;

          pmt1bh.Fill( event.utctime(), tmp1bh.GetMean() );
          pmt2bh.Fill( event.utctime(), tmp2bh.GetMean() );
          ssdpmtbh.Fill( event.utctime(), ssdtmpbh.GetMean() );

          tmp0bl.Reset();
          tmp1bl.Reset();
          tmp2bl.Reset();
          stmpbl.Reset();
          ssdtmpbl.Reset();

          tmp0bh.Reset();
          tmp1bh.Reset();
          tmp2bh.Reset();
          ssdtmpbh.Reset();          
        }
      }
    }
  }

  c1.cd(1);
  pmt0blmean.GetXaxis()->SetTimeDisplay(1);
  pmt0blmean.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt0blmean.GetXaxis()->SetLabelOffset(0.020);
  pmt0blmean.GetXaxis()->SetLabelSize(0.03);
  pmt0blmean.GetYaxis()->SetTitle("Mean Baseline (First 100 bins) / FADC");
  pmt0blmean.Draw("P");
  c1.cd(8);
  pmt0blrms.GetXaxis()->SetTimeDisplay(1);
  pmt0blrms.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt0blrms.GetXaxis()->SetLabelOffset(0.020);
  pmt0blrms.GetXaxis()->SetLabelSize(0.03);
  pmt0blrms.GetYaxis()->SetTitle("Baseline RMS (First 100 bins) / FADC");
  pmt0blrms.SetMarkerStyle(20);
  pmt0blrms.Draw("P");
  c1.Update();


  c1.cd(2);
  pmt0bh.GetXaxis()->SetTimeDisplay(1);
  pmt0bh.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt0bh.GetXaxis()->SetLabelOffset(0.020);
  pmt0bh.GetXaxis()->SetLabelSize(0.03);
  pmt0bh.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  pmt0bh.Draw();

  c1.cd(3);
  pmt1bl.GetXaxis()->SetTimeDisplay(1);
  pmt1bl.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt1bl.GetXaxis()->SetLabelOffset(0.020);
  pmt1bl.GetXaxis()->SetLabelSize(0.03);
  pmt1bl.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  pmt1bl.Draw();
  c1.cd(4);
  pmt1bh.GetXaxis()->SetTimeDisplay(1);
  pmt1bh.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt1bh.GetXaxis()->SetLabelOffset(0.020);
  pmt1bh.GetXaxis()->SetLabelSize(0.03);
  pmt1bh.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  pmt1bh.Draw();

  c1.cd(5);
  pmt2bl.GetXaxis()->SetTimeDisplay(1);
  pmt2bl.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt2bl.GetXaxis()->SetLabelOffset(0.020);
  pmt2bl.GetXaxis()->SetLabelSize(0.03);
  pmt2bl.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  pmt2bl.Draw();
  c1.cd(6);
  pmt2bh.GetXaxis()->SetTimeDisplay(1);
  pmt2bh.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  pmt2bh.GetXaxis()->SetLabelOffset(0.020);
  pmt2bh.GetXaxis()->SetLabelSize(0.03);
  pmt2bh.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  pmt2bh.Draw();

  c1.cd(7);
  spmtbl.GetXaxis()->SetTimeDisplay(1);
  spmtbl.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  spmtbl.GetXaxis()->SetLabelOffset(0.020);
  spmtbl.GetXaxis()->SetLabelSize(0.03);
  spmtbl.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  spmtbl.Draw();

  c1.cd(9);
  ssdpmtbl.GetXaxis()->SetTimeDisplay(1);
  ssdpmtbl.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  ssdpmtbl.GetXaxis()->SetLabelOffset(0.020);
  ssdpmtbl.GetXaxis()->SetLabelSize(0.03);
  ssdpmtbl.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  ssdpmtbl.Draw();
  c1.cd(10);
  ssdpmtbh.GetXaxis()->SetTimeDisplay(1);
  ssdpmtbh.GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}%F1970-01-01 05:00:00");
  ssdpmtbh.GetXaxis()->SetLabelOffset(0.020);
  ssdpmtbh.GetXaxis()->SetLabelSize(0.03);
  ssdpmtbh.GetYaxis()->SetTitle("Baseline Mean (First 100 bins) / FADC");
  ssdpmtbh.Draw();

  //c1.Print("../plots/jan2021/bl"+nameStati+".pdf");

  tmp0bl.SetDirectory(0);
  tmp1bl.SetDirectory(0);
  tmp2bl.SetDirectory(0);
  stmpbl.SetDirectory(0);
  ssdtmpbl.SetDirectory(0);
  
  tmp0bh.SetDirectory(0);
  tmp1bh.SetDirectory(0);
  tmp2bh.SetDirectory(0);
  ssdtmpbh.SetDirectory(0);

  hfile.Write();
  hfile.Close();

  return 0;
}
