{
  ifstream in;
  in.open("output_result_time.txt");

  TH1F* h = new TH1F("","Angle between true and reco.",40,-3,3);

  double aa,bb,cc,dd;
  while (!in.eof()){
    in>>aa>>bb>>cc>>dd;
    double angle = TMath::ACos(aa* 1 / sqrt(aa*aa+bb*bb+cc*cc));
    h->Fill(angle);
  }
  new TCanvas();
  h->Draw();

}

