/*
 *  Eos directional fit header file.
 *
 *  Author: Guang Yang
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <TList.h>

#include <TROOT.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TH1.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TFile.h>
#include <TRint.h>
#include <TH2.h>
#include <TFormula.h>
#include <TF1.h>

#include <TF2.h>
#include <TMath.h>
#include <Math/DistFunc.h>
#include <TLine.h>
#include <TTree.h>
#include <TGraph.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TGraphErrors.h>
#include <TVirtualFFT.h>
#include <TFoamIntegrand.h>
#include <TMatrixD.h>
#include <TVectorT.h>
#include <TDecompChol.h>

#include <RooFit.h>
#include "RooGlobalFunc.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"
#include "RooMinuit.h"
#include "RooNLLVar.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooExtendPdf.h"
#include "RooChi2Var.h"
#include "RooMinuit.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooRandom.h"
#include <RooMsgService.h>
#include <RooHist.h>
#include <RooTrace.h>
#include <RooCategory.h>
#include "RooConstVar.h"
#include "RooBinning.h"
#include "TStopwatch.h"
#include "TFile.h"
#include "TMinuit.h"

#include "RooFit.h"
#include "RooMinuit.h"
#include "RooFitResult.h"
#include "TMinuit.h"
#include <RooRealVar.h>

using namespace std;

  class wbPDF {

  public:

    wbPDF(const char* name);
    TH1F* _timePDF; 
    TH1F* _thetaPDF; 

    void SetTimePDFBinning (int nbin, double low, double high){ cout<<"set pdf binning "<< nbin<<endl; cout<<"original bin n. "<<_timePDF->TH1F::GetNbinsX()<<endl; _timePDF->TH1F::SetBins(nbin, low, high); cout<<"binning set"<<endl; }
    void SetThetaPDFBinning (int nbin, double low, double high){ _thetaPDF->TH1F::SetBins(nbin, low, high); }
    TH1F* GetTimePDF(){ return _timePDF; }
    TH1F* GetThetaPDF(){ return _thetaPDF; }
  };


  class wbEvent {

  public:

    wbEvent (const char* name);

    //wbEvent (const wbEvent & other, const char* name = 0): RooAbsReal(other,name) {};
    //virtual TObject* clone(const char* newname) const {return new wbEvent (*this, newname);};
    virtual ~wbEvent () ;
    //wbEvent (const wbEvent & wbEvent );

    struct wbHit {
      double x; double y; double z; double t; double wavelength; double type;
      double px; double py; double pz; double trTime;
      double trX; double trY; double trZ; double trTheta; double trPhi;
      std::string something;
    };

    std::vector<wbHit> _hitlist;

    void SetHitList(std::vector<std::vector<double> > num);
    std::vector<wbHit > GetHitList(){ return _hitlist; }

    std::vector<wbHit> sample_photons(  std::vector<wbHit> list, bool smearVtx = false, bool smearTime = false, bool smearQE = false, bool addNoise =false );

    wbPDF* createPDFs(std::vector<wbHit> list, std::vector<std::vector<double> > tPara, double prompt_cut, double wavelength_cut);
    wbPDF* createPDFs(std::vector<wbHit> list, double prompt_cut, double wavelength_cut);

    private:
    protected:
  };


