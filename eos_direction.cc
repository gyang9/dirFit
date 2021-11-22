#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

#include <RAT/DSReader.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/PMTInfo.hh>
#include <RAT/DS/MCPMT.hh>
#include <RAT/DS/MCParticle.hh>

#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>
#include <TVector3.h>

using namespace std;

std::vector<std::vector<double> > angleSplit(double px, double py, double pz, double variation){

  double ppx = px/sqrt(px*px + py*py + pz*pz);
  double ppy = px/sqrt(px*px + py*py + pz*pz);
  double ppz = px/sqrt(px*px + py*py + pz*pz);

  std::vector<std::vector<double> > hh;
  for (int i=0;i<200;i++){
    for (int j=0;j<200;j++){
      double xlist = -i/100.+1. ;
      double ylist = -j/100.+1. ;
      if (sqrt(xlist*xlist + ylist*ylist)>1) continue;
      double zlist = 1 - xlist*xlist - ylist*ylist;
      if ( ppx * xlist + ppy * ylist + ppz * zlist < 0.755- variation || ppx * xlist + ppy * ylist + ppz * zlist > 0.755+ variation) continue;
      std::vector<double> h;
      h.push_back(xlist);
      h.push_back(ylist);
      h.push_back(zlist);
      hh.push_back(h);
      h.clear();
    }
  }

  //std::vector<double> h;
  //h.push_back(0);
  //std::vector<std::vector<double> > hh;
  //hh.push_back(h);
  return hh;
}

std::vector<std::vector<double> > landingLocation(double x, double y, double z, std::vector<std::vector<double> > dirList, double detR, double detZ){

  double stepSize = 1;
  double currx = x;
  double curry = y;
  double currz = z;
  std::vector<std::vector<double> > hh;
  for (size_t i = 0; i< dirList.size(); i++){

    std::vector<double> h;
    for (int j=0;j< sqrt((detR*2 )*(detR*2 ) + (detZ*2)*(detZ*2)); j++ ){
      currx = currx + dirList[i].at(0)/sqrt(dirList[i].at(0)*dirList[i].at(0)+dirList[i].at(1)*dirList[i].at(1)+dirList[i].at(2)*dirList[i].at(2))* stepSize;
      curry = curry + dirList[i].at(1)/sqrt(dirList[i].at(0)*dirList[i].at(0)+dirList[i].at(1)*dirList[i].at(1)+dirList[i].at(2)*dirList[i].at(2))* stepSize;
      currz = currz + dirList[i].at(2)/sqrt(dirList[i].at(0)*dirList[i].at(0)+dirList[i].at(1)*dirList[i].at(1)+dirList[i].at(2)*dirList[i].at(2))* stepSize;
      if (sqrt(currx* currx + curry*curry)> detR || abs(currz) > detZ){
        h.push_back(currx);
        h.push_back(curry);
        h.push_back(currz);
        hh.push_back(h);
	h.clear();
	currx = x; curry = y; currz = z;
        break;
      }
    }
  }
  //std::vector<double> h;
  //h.push_back(0);
  //std::vector<std::vector<double> > hh;
  //hh.push_back(h);
  return hh;

}
