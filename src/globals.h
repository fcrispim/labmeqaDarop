#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <cstdio>
#include <vector>
#include "Graph.h"
#define NMAX 100

extern int K, N, V, n;
extern Graph G;
extern double matG[NMAX][NMAX];
extern std::vector<int> VC, RD;
extern std::vector<double> RT, AL;

#endif