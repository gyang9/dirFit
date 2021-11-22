#include <stdio.h>
#include <iostream>
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
#include <RAT/DS/MCSummary.hh>

#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>
#include <TVector3.h>

using namespace std;

void open_file(std::string filename){

    TH1D* hnpe = new TH1D("","",20,0,20);
    TH1D* ht1 = new TH1D("","",1000,-100,100);
    TH1D* ht2 = new TH1D("","",1000,-100,100);
    TH1D* hnph = new TH1D("","",1000,0,10000);

    TH1D* hEdepQ = new TH1D("","",150,0,15);
    TH1D* hEdep = new TH1D("","",150,0,15);

    RAT::DSReader *dsreader = new RAT::DSReader(filename.c_str());
    const unsigned int nevents = dsreader->GetT()->GetEntries();

    double containment = 0;
    double totalEvents = 0;

    // Loop over all triggered events
    for(size_t iev = 0; iev < nevents; iev++){

        RAT::DS::Root *rds = dsreader->GetEvent(iev);
        if(!rds->ExistMC()) continue;
        RAT::DS::MC *mc = rds->GetMC();

        /*
        for(int ip = 0; ip < mc->GetMCParticleCount(); ip++){

            RAT::DS::MCParticle* mcParticle = mc->GetMCParticle(ip);
            TVector3 pos = mcParticle->GetPosition(); 
            double energy = mcParticle->GetKE();
            double r = pos.Mag();
            std::string pname = mcParticle->GetParticleName();

        }
        */

        for(int itr = 0; itr < mc->GetMCTrackCount(); itr++){

            RAT::DS::MCTrack* mcTrack = mc->GetMCTrack(itr);
            std::string pname = mcTrack->GetParticleName();

            if(strcmp(pname.c_str(), "opticalphoton")!=0) continue;

            for(int ist = 0; ist < mcTrack->GetMCTrackStepCount(); ist++){

                RAT::DS::MCTrackStep *mcStep = mcTrack->GetMCTrackStep(ist);
                // Original volume
                double ke = mcStep->GetKE();
                TVector3 pos = mcStep->GetEndpoint();
                std::string process = mcStep->GetProcess();
                std::string volume = mcStep->GetVolume();
                float time = mcStep->GetGlobalTime();

                std::cout << pname << " " << itr << " " << ist << " " << volume << " " << process << " " << time << std::endl;

            }
        }

    }

}

