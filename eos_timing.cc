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

void open_file(std::string filename){

    TH1D* ht = new TH1D("","",1000,-100,100);

    RAT::DSReader *dsreader = new RAT::DSReader(filename.c_str());
    const unsigned int nevents = dsreader->GetT()->GetEntries();

    // Loop over all triggered events
    for(size_t iev = 0; iev < nevents; iev++){

        //if(iev % 1000 == 0){
        //    std::cout << iev << "/" << nevents << std::endl;
        //}

        RAT::DS::Root *rds = dsreader->GetEvent(iev);
        if(!rds->ExistMC()) continue;
        RAT::DS::MC *mc = rds->GetMC();

        if(!rds->ExistEV()) continue;
        RAT::DS::EV *ev = rds->GetEV(0);

        //hnpe->Fill(mc->GetNumPE());

        // Get the NPE and totalQ from the MCPMT info
        /*
        for(int ipmt = 0; ipmt < mc->GetMCPMTCount(); ipmt++){

            RAT::DS::MCPMT* mcpmt = mc->GetMCPMT(ipmt);
            int pmtID = mcpmt->GetID();
            int npe = mcpmt->GetMCPhotonCount();
            int type = mcpmt->GetType(); 

        }*/

        for(int ipmt = 0; ipmt < ev->GetPMTCount(); ipmt++){

            RAT::DS::PMT* pmt = ev->GetPMT(ipmt);
            int pmtID = pmt->GetID();
            float time = pmt->GetTime();
            //if(pmtID < 2){
            //    std::cout << time << std::endl;
            ht->Fill(time);
            //}
        }
    }

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    ht->GetXaxis()->SetTitleFont(132);
    ht->GetXaxis()->SetLabelFont(132);
    ht->GetYaxis()->SetTitleFont(132);
    ht->GetYaxis()->SetLabelFont(132);
    ht->GetXaxis()->SetTitle("Hit times (ns)");
    ht->GetYaxis()->SetTitle("Arb. Units");
    ht->GetYaxis()->SetTitleOffset(1.25);
    ht->SetStats(0);

    ht->SetLineColor(kBlack);
    ht->Draw("E hist");

    c1->Update();
}

/*
int plot(std::string fname1, std::string fname2){

    TH1D* hnpe2 = new TH1D("","",20,0,20);

    open_file(fname1, hnpe1);
    open_file(fname2, hnpe2);

    pretty_plot(hnpe1, "# PE", "Arb. Units", 6);
    pretty_plot(hnpe2, "# PE", "Arb. Units", 6);

    double red_chi2 = chi2(hnpe1, hnpe2);
    std::cout << "reduced chi2: " << red_chi2 << std::endl;

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    hnpe1->SetLineColor(kBlack);
    hnpe1->Draw("E");
    hnpe2->SetLineColor(kRed);
    hnpe2->Draw("E same");

    TLegend *t1 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t1->SetTextFont(132);
    t1->SetBorderSize(0);
    t1->SetFillStyle(0);
    t1->AddEntry(hnpe1, "Scat. Length 31m");
    t1->AddEntry(hnpe2, "Scat. Length 9m");

    t1->Draw("");

    c1->Update();
}
*/

int main(int argc, char *argv[]){

    std::string input_filename(argv[1]);

    //open_file(input_filename);

    return 0;
}

