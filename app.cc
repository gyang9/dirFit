#include "dirFit.hh"
#include "TMath.h"
#include "TH3F.h"

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

using namespace std;

int main(int argc, char**argv){

  TH3F* h3 = new TH3F("","", 100,-1200,1200, 100, -1200,1200,100,-1200,1200);
  TH3F* h4 = new TH3F("","", 100,-1200,1200, 100, -1200,1200,100,-1200,1200);

  TH1F* hc = new TH1F("","",50,0,50);  
  TH1F* hhx[1000];
  TH1F* hhz[1000];
  for (int i=0;i<1000;i++){
    hhx[i] = new TH1F("","",20,0,1);
    hhz[i] = new TH1F("","",20,-1200,1200);
  }

  ifstream in;
  in.open("pmtlocation.txt");
  double pmtx[264]={};
  double pmty[264]={};
  double pmtz[264]={};

  int aa, ee;
  double bb,cc,dd;
  while (!in.eof()){
    in>>aa>>bb>>cc>>dd>>ee;
    cout<<aa<<" "<<bb<<" "<<cc<<" "<<dd<<" "<<ee<<endl;
    pmtx[aa] = bb;
    pmty[aa] = cc;
    pmtz[aa] = dd;
    //h3->Fill(bb,cc,dd,1);
  }

  //new TCanvas();
  //h3->SetMarkerSize(1.5);
  //h3->SetMarkerStyle(4);
  //h3->Draw();

  std::vector<std::vector<double> > pmtlist;
  ifstream in2;
  in2.open("eventPMT.txt");

  int aaa,bbb,ddd;
  double ccc;
  int current=0;
  int counter[1000]={};
  while (!in2.eof() ){
    // event id, pmt id, time, light category  
    in2>>aaa>>bbb>>ccc>>ddd;
    //if (aaa != atoi(argv[1])) continue;
    if (ddd != 0) continue;
    std::vector<double> pmtloc;
    pmtloc.push_back(pmtx[bbb]);
    pmtloc.push_back(pmty[bbb]);
    pmtloc.push_back(pmtz[bbb]);
    pmtloc.push_back(ccc);
    // true location x, y, z
    pmtloc.push_back(0); pmtloc.push_back(0); pmtloc.push_back(0);
    // true time, theta, phi
    pmtloc.push_back(0); pmtloc.push_back(0); pmtloc.push_back(0);
    pmtlist.push_back(pmtloc);

/*
    _hitlist[i].x = num[i].at(0);
    _hitlist[i].y = num[i].at(1);
    _hitlist[i].z = num[i].at(2);
    _hitlist[i].t = num[i].at(3);
    _hitlist[i].trX = num[i].at(4);
    _hitlist[i].trY = num[i].at(5);
    _hitlist[i].trZ = num[i].at(6);
    _hitlist[i].trTime = num[i].at(7);
    _hitlist[i].trTheta = num[i].at(8);
    _hitlist[i].trPhi = num[i].at(9);
*/
    pmtloc.clear();
    //h4->Fill(pmtx[bbb],pmty[bbb],pmtz[bbb]);
    //hhx[0] -> Fill (pmty[bbb]/TMath::Sqrt(pmtx[bbb]*pmtx[bbb]+pmty[bbb]*pmty[bbb] ));
    //hhz[aaa] -> Fill (pmtz[bbb]);
    counter[aaa]++;
  }
  cout<<"event induced PMT loaded "<<endl;
  //for (int i=0;i<1000;i++)
  //  hc->Fill(counter[i]);

  RooFitResult* res;
  char formula[10];
  dirFit * rep = new dirFit ("_rep");

  cout<<"reading processing events"<<endl;
  wbPDF* pdfs =  rep->Reading_Processing_Events(pmtlist);
  cout<<"getting pdfs"<<endl;
  TH1F* timepdf = pdfs->GetTimePDF();
  TH1F* thetapdf = pdfs->GetThetaPDF();

  TCanvas* c1 = new TCanvas();
  timepdf->Draw();

  TCanvas* c2 = new TCanvas();
  thetapdf->Draw();
  c1->SaveAs("timepdf.png");
  c2->SaveAs("thetapdf.png");

  rep->SetVertex(0,0,0);
  rep->SetDetectorSize (1100,1100);
  rep->SetPMTList (pmtlist);
  rep->SetAddTime (true);


/*

  RooArgList list("list");
  list.add(*rep);
  sprintf(formula,"%s","@0");
  RooFormulaVar* fcn = new RooFormulaVar("fit","fit",formula,list);

  rep->getParVar(0)->setConstant(true);
  rep->getParVar(1)->setConstant(true);
  rep->getParVar(2)->setConstant(true);

  ofstream out;
  out.open("output_result_time.txt",std::ofstream::out | std::ofstream::app);

  double finalRes = 1e9;
  double dirX = -1, dirY = -1, dirZ = -1;
  double scanningSize = 10;
  for (int i=0;i<scanningSize * 2;i++){
    for (int j=0;j<scanningSize * 2;j++){
      for (int k=0;k<scanningSize * 2;k++){
        rep->getParVar(0)->setVal(-1* i/scanningSize + 1);
	rep->getParVar(1)->setVal(-1* j/scanningSize + 1);
	rep->getParVar(2)->setVal(-1* k/scanningSize + 1);
	if (i == scanningSize && j == scanningSize && k == scanningSize) continue;
	double currRes = rep-> directionMatching (rep->getPullList(), false);
	//cout<<":::::::::::::::::::::::::::::::::::::::: "<<i<<" "<<j<<" "<<k<<" "<<-1* i/scanningSize + 1<<" "<<-1* j/scanningSize + 1<<" "<<-1* k/scanningSize + 1<<" "<<currRes<<endl;
	if (currRes < finalRes){
	  finalRes = currRes;
	  dirX = -1* i/scanningSize + 1;
	  dirY = -1* j/scanningSize + 1;
	  dirZ = -1* k/scanningSize + 1;
	}
      }
    }
  }
  double squareSum = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
  cout<<"final result "<<finalRes<<"  with x y z direction:  "<<dirX/squareSum<<" "<<dirY/squareSum<<" "<<dirZ/squareSum<<endl;
  //cout<<dirX/squareSum<<" "<<dirY/squareSum<<" "<<dirZ/squareSum<<" "<<finalRes<<endl;
  out<<dirX/squareSum<<" "<<dirY/squareSum<<" "<<dirZ/squareSum<<" "<<finalRes<<endl;

  //rep->getParVar(0)->setVal(5);
  cout<<"setting function"<<endl;
  //RooMinuit m(*fcn);
  //m.setStrategy(2);
  //Double_t callsEDM[2] = {10500., 1.e-6};
  //Int_t irf = 0;

  //cout<<"ready to perform the fit"<<endl;
  //gMinuit->mnexcm("MIGRAD",callsEDM,2,irf);
  //m.migrad();
  //m.hesse();
  //m.minos();
  //res = m.save();
  //double bestFit = res->minNll();
  //std::cout<<"fit status code is : "<< res->status()<<std::endl;

  //new TCanvas();
  //h4->SetMarkerSize(1.2);
  //h4->SetMarkerStyle(4);
  //h4->Draw("colz");

  //new TCanvas();
  //hhz[0]->Draw();
  //for (int i=1;i<1000;i++){
  //  if (counter[i]<20) continue;
  //  hhz[i]->Draw("same");
  //}
  //new TCanvas();
  //hhx[0]->Draw();
  //for (int i=1;i<1000;i++){
  //  if (counter[i]<20) continue;	  
  //  hhx[i]->Draw("same");
  //}
  //new TCanvas();
  //hc->Draw();  
*/
}
