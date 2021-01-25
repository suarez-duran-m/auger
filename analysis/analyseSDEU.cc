#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <IoAuger.h>
#include <Ec.h>

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
  
  cout << "======> I am  selecting events containing stations: "; 
  for (  vector<unsigned int>::const_iterator iter= stationsIds.begin();
         iter!= stationsIds.end(); ++iter)
    cout << *iter << " ";
  cout << " <====== " << endl;
  
  IoSd *sdFile = NULL;
  AugerFile *adFile = NULL;
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
      if ( event.Stations[i].IsUUB) {
        cout << "# Event " << event.Id << " Station " << event.Stations[i].Id
             << endl;
        cout << "# Error " << event.Stations[i].Error << endl;
        
        if (event.Stations[i].Error==256) { //0+256
          cout << event.Stations[i].UFadc->Traces.size() << endl;
          for (int k=0;k<event.Stations[i].UFadc->NSample;k++) {
            cout << k;
            for (int j=0;j<5;j++)
              cout << " " << event.Stations[i].UFadc->GetValue(j,0,k) << " "
                   << event.Stations[i].UFadc->GetValue(j,1,k);
            cout << endl;
          }
        }
      }
      
    }
  }

  return 0;
}
