{
  TH3F* h3 = new TH3F("","", 100,-1200,1200, 100, -1200,1200,100,-1200,1200);
  TH3F* h4 = new TH3F("","", 100,-1200,1200, 100, -1200,1200,100,-1200,1200);

  TH1F* ht = new TH1F("","time dist. for all events; Arrival time (ns); Counts",40,0,20);
  TH1F* htt = new TH1F("","time dist. for all events",40,0,20);
  TH1F* httt = new TH1F("","time dist. for all events",40,0,20);
  TH1F* ht2 = new TH1F("","time spread for each event",40,0,20);
  TH1F* hpe = new TH1F("","pe for each pmt",20,0,20);
  TH1F* hpe2 = new TH1F("","total pe for each event",100,0,100);

  TH1F* hc = new TH1F("","",50,0,100);  
  TH1F* hhx[1000];
  TH1F* hhz[1000];
  TH1F* hht[1000];
  TH1F* hhp[1000];
  for (int i=0;i<1000;i++){
    hhx[i] = new TH1F("","",20,0,1);
    hhz[i] = new TH1F("","",20,-1200,1200);
    hht[i] = new TH1F("","",50,0,50);
    hhp[i] = new TH1F("","",20,0,20);
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
    h3->Fill(bb,cc,dd,1);
  }

  new TCanvas();
  h3->SetMarkerSize(1.5);
  h3->SetMarkerStyle(4);
  h3->Draw();

  ifstream in2;
  in2.open("eventPMT.txt");

  int aaa,bbb,ddd;
  double ccc;
  int current=0;
  int counter[1000]={};
  double pe[1000][264]={};
  while (!in2.eof() ){
    in2>>aaa>>bbb>>ccc>>ddd;
    //if (aaa != 4) continue;
    if (ddd == 1) htt->Fill(ccc);
    if (ddd == 2) httt->Fill(ccc);
    if (ddd != 0) continue;
    h4->Fill(pmtx[bbb],pmty[bbb],pmtz[bbb]);
    hhx[0] -> Fill (pmty[bbb]/TMath::Sqrt(pmtx[bbb]*pmtx[bbb]+pmty[bbb]*pmty[bbb] ));
    hhz[aaa] -> Fill (pmtz[bbb]);
    ht->Fill(ccc);
    hht[aaa]->Fill(ccc);
    pe[aaa][bbb] ++;
    counter[aaa]++;
  }

  for (int i=0;i<1000;i++){
    ht2->Fill(hht[i]->GetRMS());
    //hc->Fill(counter[i]);
    double totpe = 0;
    int counter2 = 0;
    for (int j=0;j<264;j++){
      hpe->Fill(pe[i][j]);
      totpe += pe[i][j];
      if (pe[i][j]!= 0) counter2++;
    }
    hc->Fill(counter2);
    hpe2->Fill(totpe);
  }

  new TCanvas();
  //h4->SetMarkerSize(1.2);
  //h4->SetMarkerStyle(4);
  h4->Draw("colz");

  new TCanvas();
  hhz[0]->Draw();
  for (int i=1;i<1000;i++){
    if (counter[i]<20) continue;
    hhz[i]->Draw("same");
  }
  new TCanvas();
  hhx[0]->Draw();
  //for (int i=1;i<1000;i++){
  //  if (counter[i]<20) continue;	  
  //  hhx[i]->Draw("same");
  //}

  new TCanvas();
  hc->Draw();

  new TCanvas();
  ht->Draw();

  new TCanvas();
  ht2->Draw();  

  new TCanvas();
  hpe->Draw();

  new TCanvas();
  hpe2->Draw();

  new TCanvas();
  ht->SetLineWidth(3);
  htt->SetLineWidth(3);
  httt->SetLineWidth(3);
  ht->SetLineColor(1);
  htt->SetLineColor(2);
  httt->SetLineColor(4);
  ht->Draw();
  htt->Draw("same");
  httt->Draw("same");
  TLegend* leg = new TLegend(0.6,0.68,0.9,0.9,NULL,"brNDC");
  leg->AddEntry(ht ,"WC light","l");
  leg->AddEntry(htt ,"Scint. light","l");
  leg->AddEntry(httt ,"Reemission light","l");
  leg->Draw();

}
