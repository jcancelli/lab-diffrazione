/*****************************************************************************************
 *****************************************************************************************

Questo programma esegue il fit della funzione che descrive la figura di
diffrazione prodotta da una fenditura lineare. Due funzioni accessorie
permettono la visualizzazione dei dati sperimentali (utile per stimare i
valori iniziali delle grandezze coinvolte) e di produrre il grafico della
funzione da fittare.

Si possono utilizzare questi comandi da terminale
> root -l --> Avvia ROOT
> .L Fit_Diffrazione.cpp  --> Carica il file Fit_Diffrazione.cpp
> mydata("nomefile.txt") --> costruisce il grafico dei dati sperimentali
> myfunc() --> visualizza la funzione con valori di default
> myfunc(background, normalizzazione, lambda, larghezza fenditura, shift
  lungo x, distanza fenditura - schermo) --> visualizza la funzione con valori
  forniti dall'utente
> myfit("nomefile.txt", background, normalizzazione, lambda,
  larghezza fenditura, shift lungo x, distanza fenditura - schermo) --> esegue
  il fit
  Tutte le grandezze sono in metri.

 ****************************************************************************************/

#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TROOT.h"
#include "parameters.hpp"
#include <TMath.h>
#include <string>

Parameters parameters[3] = {{.file = "./data/monofessura.meters.dat",
                             .d = 0.0015, // 0.15mm
                             .x0 = 0.058, // 58000
                             .L = 4.19,   // 41.9cm
                             .lambda = 0,
                             .norm = 1,
                             .bkg = 0},
                            {.file = "./data/doppia_fessura.meters.dat",
                             .d = 0.15, // 0.15mm
                             .x0 = 62500,
                             .L = 70.5, // 70.5cm
                             .lambda = 0,
                             .norm = 1,
                             .bkg = 0},
                            {.file = "./data/doppia_fessura_2.meters.dat",
                             .d = 0.15, // 0.15mm
                             .x0 = 62000,
                             .L = 95, // 95cm
                             .lambda = 0,
                             .norm = 1,
                             .bkg = 0}};

Double_t Diffrazione(double *x, double *par) {
  const auto d = par[0], x0 = par[1], L = par[2], lambda = par[3],
             norm = par[4], bkg = par[5];
  double Arg = TMath::Pi() * d * (x[0] - x0) /
               TMath::Sqrt((x[0] - x0) * (x[0] - x0) + L * L) / lambda;
  double Diffr = bkg + norm * sin(Arg) * sin(Arg) / Arg / Arg;
  return Diffr;
}

void myfunc() {
  TF1 *f1 = new TF1("myfunc", Diffrazione, 0, 0, 6);
  f1->SetParName(0, "Larghezza fenditura");
  f1->SetParName(1, "shift lungo x");
  f1->SetParName(2, "Distanza fenditura - schermo");
  f1->SetParName(3, "Lambda");
  f1->SetParName(4, "Normalizzazione");
  f1->SetParName(5, "fondo");
}

void mydata(TString fname = " ") {
  TCanvas *c = new TCanvas((std::string(fname) + "datacanvas").c_str());
  TGraphErrors *data = new TGraphErrors(fname, "%lg %lg %lg");
  data->Draw();
  data->SetLineColor(4);
  data->SetMarkerColor(4);
  data->SetTitle("Figura di diffrazione");
  data->GetXaxis()->SetTitle("Posizione, m");
  data->GetYaxis()->SetTitle("Int. luminosa, unit. arb.");
  data->GetXaxis()->CenterTitle(true);
  data->GetXaxis()->CenterTitle(true);
}

void myfit(TString fname = " ", double bkg = 2., double Norm = 500.,
           double lambda = 632.8E-9, double d = 1.E-4, double x0 = 0.057,
           double L = 1.0) {
  TCanvas *c = new TCanvas((std::string(fname) + "fitcanvas").c_str());
  TGraphErrors *data = new TGraphErrors(fname, "%lg %lg %lg");
  TF1 *f1 = (TF1 *)gROOT->GetFunction("myfunc");
  f1->SetRange(data->GetX()[0], data->GetX()[data->GetN() - 1]);
  f1->SetParameter(0, d);
  f1->SetParameter(1, x0);
  f1->SetParameter(2, L);
  f1->SetParameter(3, lambda);
  f1->SetParameter(4, Norm);
  f1->SetParameter(5, bkg);

  f1->FixParameter(0, d);
  f1->FixParameter(2, L);

  //     f1->SetParLimits(1,x0-0.001,x0+0.001);
  //     f1->SetParLimits(4,Norm-10., Norm+10.);
  //     f1->SetParLimits(5,bkg-5., bkg+5.);

  data->Fit("myfunc", "R");
  data->Draw("AP");
  data->SetLineColor(4);
  data->SetMarkerColor(4);
  f1->Draw("same");
  data->SetTitle("Figura di diffrazione");
  data->GetXaxis()->SetTitle("Posizione, m");
  data->GetYaxis()->SetTitle(" ");
  data->GetXaxis()->CenterTitle(true);
  data->GetXaxis()->CenterTitle(true);
  TLegend *leg = new TLegend(.6, .7, .9, .9);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0); // no border for legend
  leg->SetFillColor(0);  // fill color is white
  leg->AddEntry(data, "L= ... m, d=... mm", "p");
  leg->AddEntry(f1, "fit", "l");
  leg->Draw();
}

void data() {
  for (auto const &param : parameters) {
    mydata(param.file);
  }
}

void fit() {
  for (auto const &param : parameters) {
    myfit(param.file, param.bkg, param.norm, param.lambda, param.d, param.x0,
          param.L);
  }
}

void fit_diffrazione() {
  myfunc();
  fit();
}
