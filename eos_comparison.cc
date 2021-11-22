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

void open_file(std::string filename, TH1D* hnpe, TH1D* hnhits, TH1D* ht, TH1D* htch, TH1D* htsc, TH1D* hpepmt){

    std::cout << filename << std::endl;
    RAT::DSReader *dsreader = new RAT::DSReader(filename.c_str());
    const unsigned int nevents = dsreader->GetT()->GetEntries();

    int ch_count = 0;
    int sc_count = 0;

    int nsc = 0;
    int nrem = 0;
    int nch = 0;

    int nhits = 0;
    int npes = 0;

    int npass = 0;

    // Loop over all triggered events
    for(size_t iev = 0; iev < nevents; iev++){

        RAT::DS::Root *rds = dsreader->GetEvent(iev);
        if(!rds->ExistMC()) continue;
        RAT::DS::MC *mc = rds->GetMC();

        RAT::DS::MCSummary *mcs = mc->GetMCSummary();        

        int numScint = mcs->GetNumScintPhoton();
        int numReemit = mcs->GetNumReemitPhoton();
        int numCher = mcs->GetNumCerenkovPhoton();

        double energy = 0;
        for(int ip = 0; ip < mc->GetMCParticleCount(); ip++){

            RAT::DS::MCParticle* mcParticle = mc->GetMCParticle(ip);
            //TVector3 pos = mcParticle->GetPosition(); 
            //double r = pos.Mag();
            //std::string pname = mcParticle->GetParticleName();
            energy = mcParticle->GetKE();
        }
        //std::cout << "Energy: " << energy << std::endl;

        if(energy < 2.0) continue;

        npass += 1; 

        nsc += numScint;
        nrem += numReemit;
        nch += numCher;

        hnpe->Fill(mc->GetNumPE());
        hnhits->Fill(mc->GetMCPMTCount());

        nhits += mc->GetMCPMTCount();

        // Get the NPE and totalQ from the MCPMT info
        for(int ipmt = 0; ipmt < mc->GetMCPMTCount(); ipmt++){

            RAT::DS::MCPMT* mcpmt = mc->GetMCPMT(ipmt);
            int npe = mcpmt->GetMCPhotonCount();

            npes += npe;

            hpepmt->Fill(npe);

            for(int iPE = 0; iPE < npe; iPE++){
                double fe_time = mcpmt->GetMCPhoton(iPE)->GetFrontEndTime();
                std::string process = mcpmt->GetMCPhoton(iPE)->GetCreatorProcess();
                if(strcmp(process.c_str(), "Cerenkov")==0){
                    htch->Fill(fe_time);
                    ch_count+=1;
                }
                else{
                    htsc->Fill(fe_time);
                    sc_count+=1;
                }
                ht->Fill(fe_time);
            }
        }
    }

    std::cout << "Total Ch count/event: " << float(ch_count)/npass << std::endl;
    std::cout << "Total Sc count/event: " << float(sc_count)/npass << std::endl;

    std::cout << "Total Scint prod/event: " << float(nsc)/npass << std::endl;
    std::cout << "Total Reemit prod/event: " << float(nrem)/npass << std::endl;
    std::cout << "Total Cher prod/event: " << float(nch)/npass << std::endl;

    std::cout << "NPEs/event: " << float(npes)/npass << std::endl;
    std::cout << "NHits/event: " << float(nhits)/npass << std::endl;
    std::cout << std::endl;

}

void pretty_plot(TH1D* h){

    h->Sumw2();
    h->SetStats(0);
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->GetXaxis()->SetTitleFont(132);
    h->GetYaxis()->SetTitleFont(132);
    h->GetYaxis()->SetTitle("Arb. Units");

}

int plot(std::string fname0, std::string fname1, std::string fname2){

    TH1D* hnpe0 = new TH1D("","",100,0,1000);
    TH1D* hnhits0 = new TH1D("","",100,0,100);
    TH1D* ht0 = new TH1D("","",1000,-100,100);
    TH1D* htch0 = new TH1D("","",1000,-100,100);
    TH1D* htsc0 = new TH1D("","",1000,-100,100);
    TH1D* hpepmt0 = new TH1D("","",10,0,10);

    open_file(fname0, hnpe0, hnhits0, ht0, htch0, htsc0, hpepmt0);

    TH1D* hnpe1 = new TH1D("","",100,0,1000);
    TH1D* hnhits1 = new TH1D("","",100,0,100);
    TH1D* ht1 = new TH1D("","",1000,-100,100);
    TH1D* htch1 = new TH1D("","",1000,-100,100);
    TH1D* htsc1 = new TH1D("","",1000,-100,100);
    TH1D* hpepmt1 = new TH1D("","",10,0,10);

    open_file(fname1, hnpe1, hnhits1, ht1, htch1, htsc1, hpepmt1);

    TH1D* hnpe2 = new TH1D("","",100,0,1000);
    TH1D* hnhits2 = new TH1D("","",100,0,100);
    TH1D* ht2 = new TH1D("","",1000,-100,100);
    TH1D* htch2 = new TH1D("","",1000,-100,100);
    TH1D* htsc2 = new TH1D("","",1000,-100,100);
    TH1D* hpepmt2 = new TH1D("","",10,0,10);

    open_file(fname2, hnpe2, hnhits2, ht2, htch2, htsc2, hpepmt2);

    pretty_plot(ht0);
    pretty_plot(ht1);
    pretty_plot(ht2);

    pretty_plot(hnpe0);
    pretty_plot(hnpe1);
    pretty_plot(hnpe2);

    pretty_plot(hpepmt0);
    pretty_plot(hpepmt1);
    pretty_plot(hpepmt2);

    TCanvas *c1 = new TCanvas("c1","c1",800,600);

    ht0->GetXaxis()->SetRangeUser(0.0, 45.0);
    ht0->GetXaxis()->SetTitle("Time (ns)");

    ht0->Scale(1.0/ht0->Integral());
    ht1->Scale(1.0/ht1->Integral());
    ht2->Scale(1.0/ht2->Integral());

    ht0->SetLineColor(kBlack);
    ht0->Draw("");
    ht1->SetLineColor(kBlue);
    ht1->Draw("same");
    ht2->SetLineColor(kRed);
    ht2->Draw("same");

    TLegend *t1 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t1->SetTextFont(132);
    t1->SetBorderSize(0);
    t1->SetFillStyle(0);
    t1->AddEntry(ht0, "Water");
    t1->AddEntry(ht1, "1% WbLS");
    t1->AddEntry(ht2, "5% WbLS");

    t1->Draw("");

    c1->Update();

    TCanvas *c2 = new TCanvas("c2","c2",800,600);

    hnpe0->GetXaxis()->SetRangeUser(0.0, 1000.0);
    hnpe0->GetXaxis()->SetTitle("# PE");

    hnpe0->Scale(1.0/hnpe0->Integral());
    hnpe1->Scale(1.0/hnpe1->Integral());
    hnpe2->Scale(1.0/hnpe2->Integral());

    hnpe0->SetLineColor(kBlack);
    hnpe0->Draw("");
    hnpe1->SetLineColor(kBlue);
    hnpe1->Draw("same");
    hnpe2->SetLineColor(kRed);
    hnpe2->Draw("same");

    TLegend *t2 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t2->SetTextFont(132);
    t2->SetBorderSize(0);
    t2->SetFillStyle(0);
    t2->AddEntry(hnpe0, "Water");
    t2->AddEntry(hnpe1, "1% WbLS");
    t2->AddEntry(hnpe2, "5% WbLS");

    t2->Draw("");

    c2->Update();

    TCanvas *c3 = new TCanvas("c3","c3",800,600);

    hpepmt0->GetXaxis()->SetRangeUser(0.0, 10.0);
    hpepmt0->GetXaxis()->SetTitle("# PE per PMT");

    hpepmt0->Scale(1.0/hpepmt0->Integral());
    hpepmt1->Scale(1.0/hpepmt1->Integral());
    hpepmt2->Scale(1.0/hpepmt2->Integral());

    hpepmt0->SetLineColor(kBlack);
    hpepmt0->Draw("");
    hpepmt1->SetLineColor(kBlue);
    hpepmt1->Draw("same");
    hpepmt2->SetLineColor(kRed);
    hpepmt2->Draw("same");

    TLegend *t3 = new TLegend(0.55, 0.55, 0.85, 0.85);
    t3->SetTextFont(132);
    t3->SetBorderSize(0);
    t3->SetFillStyle(0);
    t3->AddEntry(hpepmt0, "Water");
    t3->AddEntry(hpepmt1, "1% WbLS");
    t3->AddEntry(hpepmt2, "5% WbLS");

    t3->Draw("");

    c3->Update();
}

