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

std::map<int, std::vector<double> > pmtmap;
std::ofstream out1;
std::ofstream out2;

void open_ev_branch(RAT::DS::EV* ev){

    cout<<"open_ev_branch"<<endl;
    // Total number of PMTs that detected photons
    int nhits = ev->Nhits();

    // Loop over the EV PMTs        
    for(int ipmt = 0; ipmt < ev->GetPMTCount(); ipmt++){
        // Get the EV PMT
        RAT::DS::PMT* pmt = ev->GetPMT(ipmt);
        // PMTID and hit-time
        int pmtID = pmt->GetID();
        float time = pmt->GetTime();
        // Total charge for each PMT hit (could be multiPE)
        double charge = pmt->GetCharge();
    }
}

void open_mc_branch(int iev, RAT::DS::MC* mc){

   cout<<"open_mc_branch"<<endl;
   // Total number of PEs detected
   int npe = mc->GetNumPE();

   // Loop over all the MC PMTs
   for(int ipmt = 0; ipmt < mc->GetMCPMTCount(); ipmt++){

       // Get the MC PMT
       RAT::DS::MCPMT* mcpmt = mc->GetMCPMT(ipmt);

       // Loop over all the PEs on this PMT
       for(int iPE = 0; iPE < mcpmt->GetMCPhotonCount(); iPE++){
           // True hit time
           double time = mcpmt->GetMCPhoton(iPE)->GetHitTime();
           // Hit time with TTS included 
           double fe_time = mcpmt->GetMCPhoton(iPE)->GetFrontEndTime();
           // Process that created the detected photon
           std::string process = mcpmt->GetMCPhoton(iPE)->GetCreatorProcess();
	   cout<<"PMT number "<<ipmt<<" PMT ID "<< mcpmt->GetID()<< " ( location "<<pmtmap[mcpmt->GetID()].at(0)<<" "<<pmtmap[mcpmt->GetID()].at(1)<<" "<<pmtmap[mcpmt->GetID()].at(2)<<" )"<<endl;
	   cout<<"PE number "<<iPE<<"  process "<<process.c_str()<<" strcmp: "<<strcmp(process.c_str(), "Cerenkov")<<endl; 
	   cout<<"hit position test: "<<mcpmt->GetMCPhoton(iPE)->GetPosition().X()<< " "<<mcpmt->GetMCPhoton(iPE)->GetPosition().Y()<<" "<< mcpmt->GetMCPhoton(iPE)->GetPosition().Z()<<endl;
           cout<<"hit time: "<< mcpmt->GetMCPhoton(iPE)->GetHitTime()<<endl;
           if(strcmp(process.c_str(), "Cerenkov")==0){
	     out2<<iev<<" "<<mcpmt->GetID()<<" "<<mcpmt->GetMCPhoton(iPE)->GetHitTime()<<" "<<endl;
               /* Do something with Cherenkov photons only */
           }
       }
   }


}

void open_file(std::string filename){

    cout<<"opening file"<<endl;
    TH1D* hnpe = new TH1D("","",100,0,100);

    RAT::DSReader *dsreader = new RAT::DSReader(filename.c_str());
    const unsigned int nevents = dsreader->GetT()->GetEntries();

    RAT::DS::Run* run;
    TFile* tfile = new TFile(filename.c_str());
    TTree* runT = (TTree*)tfile->Get("runT");
    runT->SetBranchAddress("run", &run);
    runT->GetEntry(0);
    RAT::DS::PMTInfo* pmtinfo = run->GetPMTInfo();
    int pmtcount = pmtinfo->GetPMTCount();
    printf("TRun level PMTs: %i\n", pmtcount);
    double* pmt_posx = (double*)malloc(pmtcount * sizeof(double));
    double* pmt_posy = (double*)malloc(pmtcount * sizeof(double));
    double* pmt_posz = (double*)malloc(pmtcount * sizeof(double));
    double* pmt_charge = (double*)malloc(pmtcount * sizeof(double));
    double* pmt_time = (double*)malloc(pmtcount * sizeof(double));
    int* pmt_type = (int*)malloc(pmtcount * sizeof(int));
    out1.open("pmtlocation.txt", std::ofstream::out);
    for (int i = 0; i < pmtcount; i++) {
      TVector3 v = pmtinfo->GetPosition(i);
      pmt_posx[i] = v.X();
      pmt_posy[i] = v.Y();
      pmt_posz[i] = v.Z();
      pmt_type[i] = pmtinfo->GetType(i);
      cout<<"PMT "<<i<<"  position "<<pmt_posx[i]<<" "<<pmt_posy[i]<<" "<<pmt_posz[i]<<"   type "<<pmt_type[i]<<endl;
      out1<<i<<" "<<pmt_posx[i]<<" "<<pmt_posy[i]<<" "<<" "<<pmt_posz[i]<<" "<<pmt_type[i]<<endl;
      std::vector<double> pos;
      pos.push_back(pmt_posx[i]);
      pos.push_back(pmt_posy[i]);
      pos.push_back(pmt_posz[i]);
      pos.push_back(pmt_type[i]);
      pmtmap[i] = pos;
    }

    // Loop over all triggered events
    for(size_t iev = 0; iev < nevents; iev++){

        cout<<"event number "<< iev<<endl;
        RAT::DS::Root *rds = dsreader->GetEvent(iev);

        // Check the MC branch exists
        if(!rds->ExistMC()) continue;

        // Get the MC branch
        RAT::DS::MC *mc = rds->GetMC();
        RAT::DS::MCParticle *prim = mc->GetMCParticle(0);
        cout << '{';
            cout << "num. of PE "<< mc->GetNumPE() << ',';
            cout << "PMT count "<<mc->GetMCPMTCount() << ',';
            cout << "prim KE "<<prim->GetKE() << ',';
            cout << "pos "<<'{' << prim->GetPosition().X() << ',' << prim->GetPosition().Y() << ',' << prim->GetPosition().Z() << "},";
            cout << "mom "<<'{' << prim->GetMomentum().X() << ',' << prim->GetMomentum().Y() << ',' << prim->GetMomentum().Z() << "}";
        cout << '}';

        open_mc_branch(iev, mc);

        // If we're interested in triggered events instead we use the EV branch
        // Check the ev branch exists
        if(!rds->ExistEV()) continue;

        // Get the EV branch
        RAT::DS::EV *ev = rds->GetEV(0);

        open_ev_branch(ev);
    }
}
int main(int argc, char *argv[]){
    cout<<"in main"<<endl;
    std::string input_filename("../../bin/output.root");
    out2.open("eventPMT.txt", std::ofstream::out);
    open_file(input_filename);

    return 0;
}

