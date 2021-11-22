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

TH1D* open_file(std::string filename){

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

        bool infv = false;
        for(int ip = 0; ip < mc->GetMCParticleCount(); ip++){

            RAT::DS::MCParticle* mcParticle = mc->GetMCParticle(ip);
            TVector3 pos = mcParticle->GetPosition(); 
            double energy = mcParticle->GetKE();
            double r = pos.Mag();
            std::string pname = mcParticle->GetParticleName();
            //std::cout << pname << " Energy: " << energy << std::endl; 
            if(r < 500.0) infv = true;

        }
        //if(!infv) continue;

        double max_ke_compt = 0;
        double max_pos = 0;
        std::string max_volume;

        RAT::DS::MCSummary *mcs = mc->GetMCSummary();

        double scintEdep = mcs->GetTotalScintEdep();
        double scintEdepQ = mcs->GetTotalScintEdepQuenched();

        double scintPhotons = mcs->GetNumScintPhoton();
        double remPhotons0 = mcs->GetNumReemitPhoton0();
        double remPhotons1 = mcs->GetNumReemitPhoton1();
        double cherPhotons = mcs->GetNumCerenkovPhoton();

        double total_ph = scintPhotons + remPhotons0 + remPhotons1 + cherPhotons;
        hnph->Fill(total_ph);

        //if((scintEdep-5.0) >= 5.0) containment +=1;

        totalEvents +=1; 

        double totalEdep = 0;
        double previous_ke = 0;

        //std::cout << "Track start" << std::endl;
        //std::cout << "------------" << std::endl;
        double totalDep = 0;

        for(int itr = 0; itr < mc->GetMCTrackCount(); itr++){

            RAT::DS::MCTrack* mcTrack = mc->GetMCTrack(itr);
            std::string pname = mcTrack->GetParticleName();

            if(strcmp(pname.c_str(), "opticalphoton")==0) continue;

            double dep = mcTrack->GetDepositedEnergy();
            if(strcmp(pname.c_str(), "e-")==0){
                totalDep += dep;
            }
            //std::cout << pname << " " << dep << std::endl;

            for(int ist = 0; ist < mcTrack->GetMCTrackStepCount(); ist++){

                RAT::DS::MCTrackStep *mcStep = mcTrack->GetMCTrackStep(ist);
                // Original volume
                double ke = mcStep->GetKE();
                TVector3 pos = mcStep->GetEndpoint();
                std::string process = mcStep->GetProcess();
                std::string volume = mcStep->GetVolume();

                if(strcmp(process.c_str(), "compt")==0){
                    if(ke > max_ke_compt){
                        max_ke_compt = ke;
                        max_pos = pos.Mag();
                        max_volume = volume;
                    }
                }

                if(strcmp(pname.c_str(), "e-")==0 && ist == 0){
                    totalEdep += ke;
                }
            }
        }

        hEdepQ->Fill(scintEdepQ);
        hEdep->Fill(totalDep); 

        //std::cout << "Edep: " << " " << scintEdep << " " << scintEdepQ << std::endl;
        //std::cout << "Total Edep: " << totalEdep << std::endl;
        //std::cout << "Total Dep: " << totalDep << std::endl;
        //std::cout << "Max KE: " << max_ke_compt << " " << max_pos << " " << max_volume << std::endl;
        if(scintEdepQ > 7.0) containment +=1;

        if(!rds->ExistEV()) continue;
        RAT::DS::EV *ev = rds->GetEV(0);

        hnpe->Fill(mc->GetNumPE());

        // Get the NPE and totalQ from the MCPMT info
        for(int ipmt = 0; ipmt < mc->GetMCPMTCount(); ipmt++){

            RAT::DS::MCPMT* mcpmt = mc->GetMCPMT(ipmt);
            int pmtID = mcpmt->GetID();
            int npe = mcpmt->GetMCPhotonCount();
            int type = mcpmt->GetType(); 

        }

        for(int ipmt = 0; ipmt < ev->GetPMTCount(); ipmt++){

            RAT::DS::PMT* pmt = ev->GetPMT(ipmt);
            int pmtID = pmt->GetID();
            float time = pmt->GetTime();
            if(pmtID == 0 || pmtID == 1){
                ht1->Fill(time);
            }
            else{
                ht2->Fill(time);
            }
        }
    }

    std::cout << "Containment: " << containment/totalEvents << std::endl;

    /*
    TCanvas *c1 = new TCanvas("c1","c1",800,600);
    hEdepQ->Draw("");

    c1->Update();

    TCanvas *c2 = new TCanvas("c2","c2",800,600);

    hEdep->Draw("");

    c2->Update();

    TCanvas *c3 = new TCanvas("c3","c3",800,600);

    hnph->Draw("");

    c3->Update();
    */

    return hnph;
}

int plot(std::string fname1, std::string fname2){

    TH1D* hnph1 = open_file(fname1);
    TH1D* hnph2 = open_file(fname2);

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    hnph1->SetLineColor(kBlack);
    hnph1->Draw("E");
    hnph2->SetLineColor(kRed);
    hnph2->Draw("E same");

    TLegend *t1 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t1->SetTextFont(132);
    t1->SetBorderSize(0);
    t1->SetFillStyle(0);
    t1->AddEntry(hnph1, "5 MeV e-");
    t1->AddEntry(hnph2, "5 MeV neutrons");

    t1->Draw("");

    c1->Update();
}

int main(int argc, char *argv[]){

    std::string input_filename(argv[1]);

    //open_file(input_filename);

    return 0;
}

