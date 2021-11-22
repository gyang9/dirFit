#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

#include "dirFit.hh"
#include "TMath.h"
#include "RooArgList.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "TString.h"
#include <TVectorD.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TFrame.h>
#include <TFile.h>
#include <TGraph.h>
#include <TF1.h>
#include <TH1D.h>
#include <TVector3.h>

using namespace std;

dirFit ::dirFit (const char* name)
  : RooAbsReal(name,name)
{
  _pulls     = new RooListProxy("_pulls","_pulls",this);
  RooRealVar* Par1 = new RooRealVar("par1","par1",100,-1e6,1e6);
  RooRealVar* Par2 = new RooRealVar("par2","par2",1,-1e6,1e6);
  RooRealVar* Par3 = new RooRealVar("par3","par3",1,-1e6,1e6);
  RooRealVar* Par4 = new RooRealVar("par4","par4",0,-100,100);
  RooRealVar* Par5 = new RooRealVar("par5","par5",0,-10,10);
  RooRealVar* Par6 = new RooRealVar("par6","par6",0,0,10); // 0.00238,-10,10);
  RooRealVar* Par7 = new RooRealVar("par7","par7",0,0,10);
  RooRealVar* Par8 = new RooRealVar("par8","par8",1,0.,100);
  RooRealVar* Par9 = new RooRealVar("par9","par9",1,0.,100);

  Par1->setConstant(false);
  Par2->setConstant(false);
  Par3->setConstant(false);
  Par4->setConstant(true);
  Par5->setConstant(true);
  Par6->setConstant(true);
  Par7->setConstant(true);
  Par8->setConstant(true);
  Par9->setConstant(true);

  _parlist.add(*Par1);
  _parlist.add(*Par2);
  _parlist.add(*Par3);
  _parlist.add(*Par4);
  _parlist.add(*Par5);
  _parlist.add(*Par6);
  _parlist.add(*Par7);
  _parlist.add(*Par8);
  _parlist.add(*Par9);
  _pulls->add(_parlist);

  this->addServerList(*_pulls);
}

dirFit ::~dirFit ()
{;}


Double_t dirFit ::getPar(int i) {
(((RooAbsReal*)_pulls->at(i))->getVal());
}

RooRealVar* dirFit ::getParVar(int i) {
return ((RooRealVar*)_pulls->at(i));
}

void dirFit :: setPull(TH1D* pullvecCV){
  pullCV = new TVectorD(11);
  for(Int_t i=0;i<11;i++){
    (*pullCV)[i] =  pullvecCV->GetBinContent(i+1);
  }
}

void dirFit :: setPullUnc(TH1D* pullvecUnc){
  pullUnc = new TVectorD(11);
  for(Int_t i=0;i<11;i++){
    (*pullUnc)[i] = pullvecUnc->GetBinContent(i+1);
  }
}

Double_t dirFit::getPullUnc(Int_t pN){
  return (*pullUnc)[pN];
}

Double_t dirFit::ExtraPull (RooListProxy* _pulls) const
{
  return 0;
}

Double_t dirFit ::evaluate() const
{
  cout<<"evaluating .. "<<endl;
  double result =  this->directionMatching(_pulls, _addTime);
  cout<<"evaluated result  "<<result<<endl;
  return result;
}

Double_t dirFit:: directionMatching ( RooListProxy* _pulls, bool addtime) const 
{
  //cout<<"directionMatching .. "<<endl;
  //cout<<((RooAbsReal*)_pulls->at(0))->getVal()<<" "<<((RooAbsReal*)_pulls->at(1))->getVal()<<" "<<((RooAbsReal*)_pulls->at(2))->getVal()<<endl;
  std::vector<std::vector<double> > angleList = angleSplit( ((RooAbsReal*)_pulls->at(0))->getVal(), ((RooAbsReal*)_pulls->at(1))->getVal(), ((RooAbsReal*)_pulls->at(2))->getVal(), 0.02); 
  std::vector<std::vector<double> > landingList = landingLocation(_x, _y, _z, angleList, _detR, _detZ);
  double result = matchingPMT(_pmtList, landingList , _x, _y, _z, addtime);
  return result;

}

Double_t dirFit::matchingPMT(std::vector<std::vector<double> > pmtList, std::vector<std::vector<double> > predList, double x, double y, double z, bool addTime) const{

  //cout<<"matching PMT .. "<<"size of predList "<<predList.size()<<endl;
  double totalSum = 0;
  for (int j=0; j< pmtList.size(); j++){
    double saveSum = 1e9;
    int bestPred = -1;
    //cout<<"pmt list number "<<j<<endl;
    for (int i=0;i< predList.size(); i++){
      //cout<<"data list "<<pmtList[j].at(0)<<" "<<pmtList[j].at(1)<<" "<<pmtList[j].at(2)<<endl;
      //cout<<"pred list "<<predList[i]. at(0)<<" "<<predList[i]. at(1)<<" "<<predList[i]. at(2)<<endl;      
      double sum = sqrt((predList[i]. at(0) - pmtList[j].at(0)) * (predList[i]. at(0) - pmtList[j].at(0))
		      + (predList[i]. at(1) - pmtList[j].at(1)) * (predList[i]. at(1) - pmtList[j].at(1))
		      + (predList[i]. at(2) - pmtList[j].at(2)) * (predList[i]. at(2) - pmtList[j].at(2)) );
      //cout<<"current sum "<<sum<<"  at "<<i<<endl;
      double timeToVertex = sqrt((predList[i]. at(0) - x) * (predList[i]. at(0) - x)
                      + (predList[i]. at(1) - y) * (predList[i]. at(1) - y)
                      + (predList[i]. at(2) - z) * (predList[i]. at(2) - z) ) /200. ;
      if (addTime){
        sum *= abs(timeToVertex - pmtList[j].at(3));
      }
      if (sum < saveSum ){
        saveSum = sum;
	bestPred = i;
      }
    }
    if (bestPred == -1 ) saveSum = 1e9;
    else{
      //cout<<"  ::best fit sum::  "<<saveSum<<"  at "<<bestPred<<endl;
      //cout<<"data list      "<<pmtList[j].at(0)<<" "<<pmtList[j].at(1)<<" "<<pmtList[j].at(2)<<endl;
      //cout<<"best pred list "<<predList[bestPred]. at(0)<<" "<<predList[bestPred]. at(1)<<" "<<predList[bestPred]. at(2)<<endl;
    }
    totalSum += saveSum;
  }
  for (int j=0; j< pmtList.size(); j++){
    //cout<<"data list      "<<pmtList[j].at(0)<<" "<<pmtList[j].at(1)<<" "<<pmtList[j].at(2)<<endl;
  }
  for (int i=0;i< predList.size(); i++){
    //cout<<"best pred list "<<predList[i]. at(0)<<" "<<predList[i]. at(1)<<" "<<predList[i]. at(2)<<endl;
  }


  //cout<<" --- best fit sum ---  "<<totalSum<<endl;
  return totalSum*0.0001; //* (1./predList.size());
}

std::vector<std::vector<double> > dirFit::angleSplit(double px, double py, double pz, double variation) const{

  //cout<<"angle splitting .."<<endl; 
  double ppx = px/sqrt(px*px + py*py + pz*pz);
  double ppy = py/sqrt(px*px + py*py + pz*pz);
  double ppz = pz/sqrt(px*px + py*py + pz*pz);

  //cout<<" ppx ppy ppz :  "<<ppx<<" "<<ppy<<" "<<ppz<<endl;
  std::vector<std::vector<double> > hh;
  for (int i=0;i<40;i++){
    for (int j=0;j<40;j++){
      double xlist = -i/20.+1. ;
      double ylist = -j/20.+1. ;
      if (sqrt(xlist*xlist + ylist*ylist)>1) continue;
      double zlist = 1 - xlist*xlist - ylist*ylist;

      // 0.755 is 41 degree Cherenkov cone
      if ( ppx * xlist + ppy * ylist + ppz * zlist < 0.755- variation || ppx * xlist + ppy * ylist + ppz * zlist > 0.755+ variation) continue;
      //cout<<"adding one pred. direction "<<xlist<<" "<<ylist<<" "<<zlist<<endl;
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

std::vector<std::vector<double> > dirFit::landingLocation(double x, double y, double z, std::vector<std::vector<double> > dirList, double detR, double detZ) const{

  double stepSize = 2;
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

