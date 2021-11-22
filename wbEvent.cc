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

wbEvent::wbEvent (const char* name)
{
  
}

std::vector<wbEvent::wbHit> wbEvent::sample_photons( std::vector<wbHit> list, bool smearVtx , bool smearTime , bool smearQE , bool addNoise  ){ 
  // Ben Land's code at the end of the script
  cout<<"currently there is no smearing applied .. "<<endl;
  for (wbHit i: list){
    if(smearVtx){}
    if(smearTime){}
    if(smearQE){}
    if(addNoise){}
  } 
  return list;
}

wbPDF* wbEvent::createPDFs(std::vector<wbHit> list, double prompt_cut, double wavelength_cut){
  // Ben Land's code at the end of the script
  double c = 300/1.333; //mm/ns

  int count = 0;
  wbPDF* wbpdf = new wbPDF("_wbpdf");
  wbpdf->SetTimePDFBinning(20,-10,10);
  wbpdf->SetThetaPDFBinning(48,-6,6);

  cout<<"creating pdf.. total wbhit number "<<list.size()<<endl;
  for (wbHit i: list){
/*	  
    double x     = tPara[count].at(0);
    double y     = tPara[count].at(1);
    double z     = tPara[count].at(2);
    double t     = tPara[count].at(3);
    double theta = tPara[count].at(4);
    double phi   = tPara[count].at(5);
    count++;
*/
    double x     = i.trX;
    double y     = i.trY;
    double z     = i.trZ;
    double t     = i.trTime;
    double theta = i.trTheta;
    double phi   = i.trPhi;     

    cout<<"element "<<count<<"   x y z t theta phi "<<x<<" "<<y<<" "<<z<<" "<<theta<<" "<<phi<<endl;
    cout<<"i.x  i.y  i.z  i.t "<<i.x<<" "<<i.y<<" "<<i.z<<" "<<i.t<<endl;

    TVector3 P (i.x - x, i.y - y, i.z - z);
    double D = TMath::Sqrt(TMath::Power(i.x - x, 2) + TMath::Power(i.y - y, 2) + TMath::Power(i.z - z, 2));
    double T = i.t - t;
    double tresid = T - D/c;
    TVector3 dvec (TMath::Cos(phi)*TMath::Sin(theta), TMath::Sin(phi)*TMath::Sin(theta), TMath::Cos(theta));
    cout<<"test 1"<<endl;
    double costheta = TMath::Cos(dvec.Angle(P));

    cout<<"test 2"<<endl;
    double no_wavelength_cut = wavelength_cut;
    if (i.t > prompt_cut) continue;

    cout<<"test 3 "<<tresid<<endl;
    //wbpdf->SetTimePDFBinning(20,-10,10);
    //wbpdf->SetThetaPDFBinning(48,-6,6);
    wbpdf->GetTimePDF()->TH1F::Fill(tresid);
    cout<<"test 4"<<endl; 
    wbpdf->GetThetaPDF()->TH1F::Fill(costheta);
    count ++;
/*	    
    if coordinate is not None: #coordinate variable should be the truth information
        x,y,z,t,theta,phi = coordinate
        pos = np.asarray([x,y,z])
        P = xyz - pos
        D = np.sqrt(np.sum(np.square(P),axis=1))
        T = times - t
        tresid = T - D/c
        dvec = np.asarray([np.cos(phi)*np.sin(theta), np.sin(phi)*np.sin(theta), np.cos(theta)])
        costheta = np.sum(P*dvec,axis=1)/D
        if prompt_cut is None:
            return tresid,costheta
        else:
            mask = tresid<prompt_cut
            return tresid,costheta[mask]	  
*/  
  
  }

  return wbpdf;
}


void wbEvent::SetHitList(std::vector<std::vector<double> > num){
  cout<<"total hit size "<<num.size()<<endl;
  for (int i=0;i<num.size();i++){
    for (int j=0;j<10;j++){
      cout<<"element "<<j<<"  value  "<<num[i].at(j)<<endl;
    }
    wbEvent::wbHit hit;
    hit.x = num[i].at(0);
    hit.y = num[i].at(1);
    hit.z = num[i].at(2);
    hit.t = num[i].at(3);
    hit.trX = num[i].at(4);
    hit.trY = num[i].at(5);
    hit.trZ = num[i].at(6);
    hit.trTime = num[i].at(7);
    hit.trTheta = num[i].at(8);
    hit.trPhi = num[i].at(9);
    _hitlist.push_back(hit);
  }
}


wbEvent ::~wbEvent ()
{;}

/*
    '''Applies a PMT simulation to the true photon hits (track crossings) given
       in evt, which should be a numpy array with elements: [x,y,z,t,wavelength,type,...]
       
       tts is a gaussian smear to the given times (nanoseconds). 
       coverage is a scaling to the quantum efficiency to account for photon 
           collection probablity PRIOR to the QE simulation
       qe should be a tuple (wavelength,probability,tag) where wavelength and 
           probability give the QE and tag is an identifying string.
       noise is the poisson mean number of noise hits per event.  Currently puts 
            noise at distance d, which makes sense for sphere but nothing else.
            Use with care.
       pmtres will apply a gaussian smearing to the detected hit positions
       
       returns numpy arrays containing positions of hits [x,y,z], times of hits, 
            and origin of the photon. wavelength=True will also return the 
            wavelength of hits.'''
    thresh = np.interp(evt[:,4],qe[0],qe[1])
    if coverage is not None:
        thresh = thresh*coverage
    qe_mask = thresh > np.random.rand(len(thresh))
    
    mask = qe_mask

    xyz = evt[mask,:3]
    t_jitter = np.random.normal(0,tts,len(xyz))
    times = evt[mask,3]+t_jitter
    origins = evt[mask,5]
    
    if noise > 0:
        noise = np.random.poisson(noise)
        theta = np.arccos(2*np.random.random(noise)-1)
        phi = np.pi*2*np.random.random(noise)
        noise = np.asarray([d*np.sin(theta)*np.cos(phi),d*np.sin(theta)*np.sin(phi),d*np.cos(theta)]).T
        xyz = np.concatenate([xyz,noise])
        times = np.concatenate([times,400*np.random.random(len(noise))-200+np.average(times)])
        origins = np.concatenate([origins,np.full(len(noise),-1)])
    
    if pmtres:
        xyz += np.random.normal(0,pmtres,xyz.shape)
        
    if wavelength:
        return xyz,times,origins,evt[mask,4]
    else:
        return xyz,times,origins
*/
/*
    '''Staged position+time -> direction fit

    Inspired by the SNO FTP algorithm
        - Uses 1D pdfs for hit time residuals and cos(theta)
        - All hits used for position+time component
        - Prompt hit time residuals used for direction component (pos,time fixed)
        - Works well for most materials with proper choice of prompt_cut

    xyz is a numpy array of [x,y,z] photon hit positions
    times is a numpy array of the times of the hits in the xyz array
    rindex is the characteristic refractive index for computing hit time residuals
    prompt_cut is either None for no cut, or a time in nanoseconds used to select
        prompt hit-time residuals for a direction fit
    seed is either None which uses a fixed seed or an (x,y,z,t,theta,phi) tuple
    coordinate changes the behavior of the function to return hit time residuals
        and cos(theta) values to be used in PDF generation. In this mode coordinate
        should be a tuple (x,y,z,t,theta,phi) defining the TRUE event information
    pdfs should be set when not coordinating to provide callable objects for to
        evaluate negative log likelihood values in the fit.
    return_nll will return the negative log likelihood function instead of
        minimizing it.

    result will either be (tresids,costhetas) if coordinating or (success,res)
        where res is a MinimizerResult containing the fit information for
        parameters (x,y,z,theta,phi)'''

    c = 299792458*10**3*10**-9/rindex #mm/ns

    if coordinate is not None: #coordinate variable should be the truth information
        x,y,z,t,theta,phi = coordinate
        pos = np.asarray([x,y,z])
        P = xyz - pos
        D = np.sqrt(np.sum(np.square(P),axis=1))
        T = times - t
        tresid = T - D/c
        dvec = np.asarray([np.cos(phi)*np.sin(theta), np.sin(phi)*np.sin(theta), np.cos(theta)])
        costheta = np.sum(P*dvec,axis=1)/D
        if prompt_cut is None:
            return tresid,costheta
        else:
            mask = tresid<prompt_cut
            return tresid,costheta[mask]
*/	    
