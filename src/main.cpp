#include <ilcplex/ilocplex.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include "Graph.h"
#include "globals.h"


using namespace std;


//Graph &g, VehiclesCapacity &v, int &requests_number
void readInstance(char *filename) {

	FILE *input_file = fopen( filename , "r" );
	fscanf( input_file, "%d %d", &n, &K);


	RD.resize(n);
	VC.resize(K);
	
	V = n*2;
	N = V+2;

	double X[V], Y[V];

	for (int i = 0; i < n; ++i){
		fscanf(input_file, "%d", &RD[i]);
	}

	for (int i = 0; i < K; ++i){
		fscanf(input_file, "%d", &VC[i]);
	}

	for(int i = 0; i < V; ++i){
		fscanf(input_file, "%lf", &X[i]);
	}

	for(int i = 0; i < V; ++i){
		fscanf(input_file, "%lf", &Y[i]);
	}

	
	
	//Creates Graph
	G.resizeGraph(N);

	//Add for each vertex i a new node j using euclidean distance
	for(int i = 1; i <= V; ++i){
		for(int j = 1; j <= V; ++j){
			if(i == j) continue;
			Node node = Node(j, hypot( (X[i-1]-X[j-1]), (Y[i-1]-Y[j-1])) );
			G.addNode(node,i);
		}
	}

	//Add for vertex 0 edges with vertices in n
	for(int i = 1; i <= n; ++i){
		Node n(i, 0);
		G.addNode(n, 0);
	}

	for (int i = n+1; i <= V; ++i){
		Node node((V+1), 0);
		G.addNode(node, i );
	}

	G.printGraph();

	return;
}



int main ( int argc, char **argv ) {
	//read data 
	/*Graph            g;
	VehiclesCapacity v;
	int requests_number;*/

	readInstance(argv[1]);
	//readInstance(g, v, requests_number);

	//int vehicles_number = v.size(); 

	//Creates environment and model
	IloEnv env;
	IloModel model(env, "The dial-a-ride orienteering problem Problem");

	//VARIABLES 
	
	//create and add to the model the variable Y ... Yik = 1 if the vehicle k has served the request i, 0 otherwise.
	IloArray <IloBoolVarArray> Y(env, n); 

	for(int i = 0; i < n; ++i) {
		Y[i] = IloBoolVarArray(env, K);
		for (int k = 0; k < K; ++k) {
			//char name[20];
			//sprintf(name, "Y[%d][%d]", i, k);
			//Y[i][k].setName(name);
			model.add(Y[i][k]);
		}
	}

	//create and add to the model the variable X ... Xijk = 1 if the vehicle k goes straight from node i to j.
	IloArray < IloArray< IloBoolVarArray > > X (env, N);
	for (int i = 0; i < N; ++i) {
		X[i] = IloArray<IloBoolVarArray> (env, N);
		for (int j = 0; j < N; ++j) {
			X[i][j] = IloBoolVarArray(env, K);
			for (int k = 0; k < K; ++k) {
				//char name[20];
				//sprintf(name, "X[%d][%d][%d]", i, j, k);
				//X[i][j][k].setName(name);
				model.add(X[i][j][k]);
			}
		}
	}

	//Flow variable, for each car
	IloArray <IloNumVarArray>  f(env, N);
	for (int i = 0; i < N; ++i){
		IloExpr expr(env);
		f[i] = IloNumVarArray(env, N, 0, IloInfinity);
		for (int j = 0; j < N; ++j){
			for (int k = 0; k < K; ++k) {
				expr += VC[k] * X[i][j][k]; 
			}
			model.add(f[i][j]);
			model.add(f[i][j] <= expr);
		}
	}



	/*
	//Gik = load of vehicle k after visiting node i
	IloArray < IloNumVarArray > G(env, requests_number*2+2);
	for (int i = 0; i <= requests_number*2 + 1; ++i) {
		G[i] = IloNumVarArray(env, vehicles_number);
		for (int k = 0; k < vehicles_number; ++k) {
			char name[20];
			sprintf(name, "G[%d][%d]", i, k);
			G[i][k].setName(name);
			model.add(G[i][k]);
		}
	}

	//Bik = time when vehicle k starts visiting node i
	IloArray < IloNumVarArray > B(env, requests_number*2+2);
	for (int i = 0; i <= requests_number*2 + 1; ++i) {
		B[i] = IloNumVarArray(env, vehicles_number);
		for (int k = 0; k < vehicles_number; ++k) {
			char name[20];
			sprintf(name, "B[%d][%d]", i, k);
			B[i][k].setName(name);
			model.add(B[i][k]);
		}
	}

	//Lik = the ride time of request i on vehicle k
	IloArray < IloNumVarArray > L(env, requests_number*2+2);
	for (int i = 0; i <= requests_number*2 + 1; ++i) {
		L[i] = IloNumVarArray(env, vehicles_number);
		for (int k = 0; k < vehicles_number; ++k) {
			char name[20];
			sprintf(name, "B[%d][%d]", i, k);
			L[i][k].setName(name);
			model.add(L[i][k]);
		}
	}
	*/

	//OBJECTIVE FUNCTION 

	//Maximize the number of served requests:
	IloExpr OBJ(env);
	for(int i = 0; i < n; ++i) {
		for (int k = 0; k < K; ++k) {
			OBJ += Y[i][k];
		}
	}
	model.add(IloMaximize(env, OBJ));

	
	//CONSTRAINTS

	//proibited arcs to all cars
	for (int i = 0; i < N; ++i) {
		for (int k = 0; k < K; ++k) {
			model.add(X[i][i][k] == 0); //a car cannot make cycles (a node to itself arc)
		}
	}

	for (int i = 1; i <= n; ++i) {
		for (int k = 0; k < K; ++k) {
			model.add(X[N-1][i][k] == 0); //end node to pickups
			model.add(X[i][N-1][k] == 0); //pickups to end node
			model.add(X[i][0][k] == 0); //if a car is on a pickup node ... it cannot get back to the start node
		}
	}

	for (int i = n+1; i < N; ++i) {
		for (int k = 0; k < K; ++k) {
			model.add(X[0][i][k] == 0); //start node to delivers or end node
			model.add(X[i][0][k] == 0); //delivers or end node to start node
			model.add(X[N-1][i][k] == 0); //a car cannot leave end node to a delivers node
		}
	}



	//Every request is served at most once: 
	for (int i = 1; i <= n; ++i) {
		IloExpr expr(env);
		for (int k = 0; k < K; ++k) {
			for (int j = 1; j < N; ++j) {
				expr += X[i][j][k];
			}
		}
		model.add(expr <= 1);
	}
	
	
	//Same vehicle serves the pickup and delivery:
	for (int i = 1; i <= n; ++i) {
		for (int k = 0; k < K; ++k) {
			IloExpr expr1(env);
			IloExpr expr2(env);
			for (int j = 1; j < N; ++j) {
				expr1 += X[i][j][k];
				expr2 += X[n + i][j][k];
			}
			model.add( (expr1 - expr2) == 0);
		}
	}

	
	//Same vehicle that enters a node leaves the node:
	for (int i = 1; i <= V; ++i) {
		for (int k = 0; k < K; ++k) {
			IloExpr expr1(env);
			IloExpr expr2(env);
			for (int j = 0; j < N; ++j) {
				if(j == i) continue;
				expr1 += X[j][i][k];
				expr2 += X[i][j][k];
			}
			model.add( (expr1 - expr2) == 0);
		}
	}

	//Collected prize by each vehicle and each visited node:
	for (int i = 1; i <= n; ++i) {
		for (int k = 0; k < K; ++k) {
			IloExpr expr(env);
			for (int j = 1; j <= V; ++j) {
				if(j == i) continue;
				expr += X[i][j][k];
			}
			model.add(expr == Y[i-1][k]);
		}
	}

	//Every used vehicle leaves the start terminal:
	for (int k = 0; k < K; ++k) {
		IloExpr expr(env);
		for (int j = 1; j <= n; ++j) {
		//for (int j = 0; j < N; ++j) {
			expr += X[0][j][k];
		}
		model.add(expr == 1);
	}

	//Every used vehicle enters the end terminal:
	IloCplex cplex(model);
	for (int k = 0; k < K; ++k) {
		IloExpr expr(env);
		for(int i = n+1; i <= V; ++i){
		//for (int i = 0; i < N; ++i) {
			expr += X[i][N - 1][k];
		}
		model.add(expr == 1);
	}
	
	
	//Flow Constraints to each Car
	for(int i = 1; i < N; ++i){
		IloExpr FC1(env);
		IloExpr FC2(env);
		for (int j = 1; j < N; ++j){
			if(j == i) continue;
			FC1 += f[i][j];
			FC2 += f[j][i];
		}	
		//if(i <= n)  model.add((FC1-FC2) == RD[i-1]);
		//else       model.add((FC1-FC2) == -RD[i - n - 1]);
	}
	/*
	for(int i = 1; i <= V; ++i){
		for (int j = 1; j < N; ++i){
			for(int j = 0; j < N; j++){

				IloExpr FC3(env);
				IloExpr FC4(env);
				FC3 = f[i][j][k];
				FC4 = (N-1)*X[i][j][k];
				model.add(FC3 <= FC4);
			}
		}
	}
*/	


	IloCplex darop(model);
	darop.solve();
	double value = darop.getObjValue();

	for(int k = 0; k < K; ++k){
		for (int i = 0; i < N; ++i){
			for(int j = 0; j < N; ++j){
				cout << "X[" << i << "]["<< j << "][" << k << "]: " << darop.getValue(X[i][j][k]) << "   ";	
			}
			cout << endl;		
		}
		cout << "\n\n\n";
	}
	for (int k = 0; k < K; ++k){
		for(int i = 0; i < n; ++i){
			cout << "Y[" << (i+1) << "][" << k << "]: " << darop.getValue(Y[i][k]) << " ";
		}
		cout << endl;
	}
	cout << "\n\n\n";

	for (int j = 0; j < N; ++j){
		for(int i = 0; i < N; ++i){
			cout << "f[" << j << "][" << i << "]: " << darop.getValue(f[j][i]) << " ";
		}
		cout << endl;
	}



	cout << "OBJ " << value << endl;
}
