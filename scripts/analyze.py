#!/usr/bin/env python3

import sys
import ROOT
from itertools import combinations
from collections import defaultdict

fname = sys.argv[1] if len(sys.argv) > 1 else "data/qcrng.root"

f = ROOT.TFile.Open(fname)
events = f.Get("events")
hits = f.Get("hits")

opposites = {(0,1), (1,0), (2,3), (3,2), (4,5), (5,4)}

def is_opposite(a, b):
    return (a, b) in opposites

def is_neighbor(a, b):
    return a != b and not is_opposite(a, b)

hits_by_event = defaultdict(list)

for h in hits:
    hits_by_event[int(h.eventID)].append({
        "det": int(h.detID),
        "E": float(h.E_meas),
        "Etrue": float(h.E_true),
        "t": float(h.t_meas),
        "ttrue": float(h.t_true)
    })

hDet = ROOT.TH2I("hDet",
                 "511-511 coincidences;Detector 1;Detector 2",
                 6,-0.5,5.5,
                 6,-0.5,5.5)

for eventID, hs in hits_by_event.items():

    # exactly two detector hits
    if len(hs) != 2:
        continue

    # both near 511 keV
    if abs(hs[0]["E"]-511) > 50:
        continue
    if abs(hs[1]["E"]-511) > 50:
        continue

    hDet.Fill(hs[0]["det"], hs[1]["det"])
    hDet.Fill(hs[1]["det"], hs[0]["det"])
    
c = ROOT.TCanvas()
ROOT.gStyle.SetOptStat(0)
hDet.Draw("COLZ TEXT")
c.Update()
input("Press Enter for next plot...")

h_E_single = ROOT.TH1D("h_E_single", "Single detector energy;E_{meas} [keV];Counts", 400, 0, 2000)
h_E_total = ROOT.TH1D("h_E_total", "Total event energy;#Sigma E_{meas} [keV];Counts", 400, 0, 3000)
h_E_addback = ROOT.TH1D("h_E_addback", "Neighbour addback energy;E_{addback} [keV];Counts", 400, 0, 2000)
h_dt_all = ROOT.TH1D("h_dt_all", "Hit time differences;#Delta t [ns];Counts", 400, -10, 10)
h_dt_opp = ROOT.TH1D("h_dt_opp", "Back-to-back hit time differences;#Delta t [ns];Counts", 400, -10, 10)
h_dt_511 = ROOT.TH1D("h_dt_511", "Back-to-back 511-keV selected;#Delta t [ns];Counts", 400, -10, 10)
for eventID, hs in hits_by_event.items():
    if len(hs) == 0:
        continue

    Etot = sum(h["E"] for h in hs)
    h_E_total.Fill(Etot)

    for h in hs:
        h_E_single.Fill(h["E"])

    for h1, h2 in combinations(hs, 2):
        dt = h2["t"] - h1["t"]
        h_dt_all.Fill(dt)

        if is_neighbor(h1["det"], h2["det"]):
            h_E_addback.Fill(h1["E"] + h2["E"])

        if is_opposite(h1["det"], h2["det"]):
            h_dt_opp.Fill(dt)

            e1_ok = 450 < h1["E"] < 570
            e2_ok = 450 < h2["E"] < 570
            if e1_ok and e2_ok:
                h_dt_511.Fill(dt)

c = ROOT.TCanvas("c", "c", 900, 700)

def show(hist):
    c = ROOT.TCanvas()
    hist.Draw("hist")
    c.Update()
    input("Press Enter for next plot...")

show(h_E_single)
show(h_E_total)
show(h_E_addback)
show(h_dt_all)
show(h_dt_opp)
show(h_dt_511)
