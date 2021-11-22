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

#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>
#include <TVector3.h>

using namespace std;

TH1D* open_file(std::string filename){

    TH1D* hnpe = new TH1D("","",100,0,100);
    TH1D* ht = new TH1D("","",1000,-100,100);
    TH1D* ht1 = new TH1D("","",1000,-100,100);
    TH1D* ht2 = new TH1D("","",1000,-100,100);

    RAT::DSReader *dsreader = new RAT::DSReader(filename.c_str());
    const unsigned int nevents = dsreader->GetT()->GetEntries();

    // Loop over all triggered events
    for(size_t iev = 0; iev < nevents; iev++){

        if(iev % 1000 == 0){
            std::cout << iev << "/" << nevents << std::endl;
        }

        RAT::DS::Root *rds = dsreader->GetEvent(iev);
        if(!rds->ExistMC()) continue;
        RAT::DS::MC *mc = rds->GetMC();

        hnpe->Fill(mc->GetNumPE());

        // Get the NPE and totalQ from the MCPMT info
        for(int ipmt = 0; ipmt < mc->GetMCPMTCount(); ipmt++){

            RAT::DS::MCPMT* mcpmt = mc->GetMCPMT(ipmt);
            int npe = mcpmt->GetMCPhotonCount();

            for(int iPE = 0; iPE < npe; iPE++){
                //double time = mcpmt->GetMCPhoton(iPE)->GetHitTime();
                double fe_time = mcpmt->GetMCPhoton(iPE)->GetFrontEndTime();
                std::string process = mcpmt->GetMCPhoton(iPE)->GetCreatorProcess();
                if(strcmp(process.c_str(), "Cerenkov")==0){
                    ht1->Fill(fe_time);
                }
                else{
                    ht2->Fill(fe_time);
                }
                ht->Fill(fe_time);
            }
        }
    }

    int b1 = ht->FindBin(3.0);
    int b2 = ht->FindBin(7.5);
    double i1 = ht->Integral(b1, b2);
    double i2 = ht1->Integral(b1, b2);
    double i3 = ht2->Integral(b1, b2);

    std::cout << "Int. 1: " << i1 << std::endl;
    std::cout << "Int. 2: " << i2 << std::endl;
    std::cout << "Int. 3: " << i3 << std::endl;


    ht->SetStats(0);
    ht->GetXaxis()->SetLabelFont(132);
    ht->GetYaxis()->SetLabelFont(132);
    ht->GetXaxis()->SetTitleFont(132);
    ht->GetYaxis()->SetTitleFont(132);
    ht->GetXaxis()->SetTitle("Front-end Time (ns)");
    ht->GetYaxis()->SetTitle("Arb. Units");

    ht->GetXaxis()->SetRangeUser(0.0, 40.0);

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    ht->SetLineColor(kBlack);
    ht->Draw("E hist");
    ht1->SetLineColor(kBlue);
    ht1->Draw("same E hist");
    ht2->SetLineColor(kOrange);
    ht2->Draw("same E hist");

    TLegend* t1 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t1->SetTextFont(132);
    t1->SetBorderSize(0);
    t1->SetFillStyle(0);
    t1->AddEntry(ht, "Sum");
    t1->AddEntry(ht1, "Cherenkov");
    t1->AddEntry(ht2, "Scintillation");
    t1->Draw("");

    c1->Update();

    return ht;
}

int plot(std::string fname1, std::string fname2){

    TH1D* ht1 = open_file(fname1);
    TH1D* ht2 = open_file(fname2);

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    ht1->SetLineColor(kBlack);
    ht1->Draw("E");
    ht2->SetLineColor(kRed);
    ht2->Draw("E same");

    TLegend *t1 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t1->SetTextFont(132);
    t1->SetBorderSize(0);
    t1->SetFillStyle(0);
    t1->AddEntry(ht1, "R14688");
    t1->AddEntry(ht2, "R5912");

    t1->Draw("");

    c1->Update();
}

