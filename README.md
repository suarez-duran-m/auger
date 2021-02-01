# First: selectStations 
Filter for stations with UUB and save in root files.

# Second: analyseSDEU
It uses the output root files from selection to get the traces
for all UUB stations.

# Third: analyseSDEUbl
It uses the output Root file from analyseSDEU, plotting the
temporal series of: the baseline Mean and RMS for the first 100
bins of each trace. Here, the RMS is calculated by the method 
GetRMS of TH1F Root class, i.e. (1/n)*sum{ (xi - x)^2 }, where x 
is the Mean. This code saves these information into histograms 
and writes a Root file for each station.

# Fourth: readBl.C
This Root macro uses the output Root files from analyseSDEUbl 
to fit gaussians to baseline Mean and RMS distributions. This 
information can be used to filter events with traces out of the 
mean+-Rms. Three plots are created: 
- distMeanRms<stationId>.pdf  
- filterMeanRms<stationId>.pdf
- timeMeanRMS<stationId>.pdf
