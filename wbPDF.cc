#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

#include "wbEvent.hh"
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

wbPDF::wbPDF (const char* name)
{
    _timePDF = (TH1F*) new TH1F("","",10,0,10);
    _thetaPDF = (TH1F*)new TH1F("","",10,0,10);
}






