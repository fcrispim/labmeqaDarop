#include <ilcplex/ilocplex.h>
#include <cstdio>
#include <vector>
#include <list>

using Graph             = std::vector < std::vector< double > >;
using VehiclesCapacity  = std::std::vector< double > ;


void readInstance(Graph &g, VehiclesCapacity &v, int &requests_number) {
	return;
}



int main ( void ) {

	//read data 
	Graph            g;
	VehiclesCapacity v;
	int requests_number;

	readInstance(g, v, requests_number);

	int vehicles_number = v.size(); 
	
	//Creates environment and model
	IloEnv env;
	IloModel model(env, "The dial-a-ride orienteering problem Problem");

	//VARIABLES 
	
	//create and add to the model the variable Y ... Yik = 1 if the vehicle k has served the request i, 0 otherwise.
	IloArray <IloBoolVarArray> Y(env, requests_number*2 + 2); 
	for(int i = 0; i <= requests_number*2+1; ++i) {
		Y[i] = IloBoolVarArray(env, vehicles_number);
		for (int k = 0; k < vehicles_number; ++k) {
			char name[20];
			sprintf(name, "Y[%d][%d]", i, k);
			Y[i][k].setName(name);
			model.add(Y[i][k]);
		}
	}

	//create and add to the model the variable X ... Xijk = 1 if the vehicle k goes straight from node i to j.
	IloArray < IloArray< IloBoolVarArray > > X (env, requests_number*2+2);
	for (int i = 0; i <= requests_number*2+1; ++i) {
		X[i] = IloArray<IloBoolVarArray> (env, requests_number*2+2);
		for (int j = 0; j <= requests_number*2+1; ++j) {
			X[i][j] = IloBoolVarArray(env, vehicles_number);
			for (int k = 0; k < vehicles_number; ++k) {
				char name[20];
				sprintf(name, "X[%d][%d][%d]", i, j, k);
				X[i][j][k].setName(name);
				model.add(X[i][j][k]);
			}
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
	for(int i = 1; i <= requests_number; ++i) {
		for (int k = 0; k < vehicles_number; ++k) {
			OBJ += Y[i][k];
		}
	}
	model.add(IloMaximize(env, OBJ));

	//CONSTRAINTS

	//Every request is served at most once: 
	for (int i = 1; i <= requests_number; ++i) {
		IloExpr expr;
		for (int k = 0; k < vehicles_number; ++k) {
			for (int j = 0; j <= requests_number*2+1; ++j) {
				expr += X[i][j][k];
			}
		}
		model.add(expr <= 1);
	}

	//Same vehicle serves the pickup and delivery:
	for (int i = 1; i <= requests_number; ++i) {
		for (int k = 0; k < vehicles_number; ++k) {
			IloExpr expr1;
			IloExpr expr2;
			for (int j = 0; j <= requests_number*2+1; ++j) {
				expr1 += X[i][j][k];
				expr2 += X[requests_number + i][j][k];
			}
			model.add( (expr1 - expr2) = 0);
		}
	}

	//Same vehicle that enters a node leaves the node:
	for (int i = 1; i <= requests_number*2; ++i) {
		for (int k = 0; i < vehicles_number; ++k) {
			IloExpr expr1;
			IloExpr expr2;
			for (int j = 0; j <= requests_number*2 + 1; ++i) {
				if(j == i) continue;
				expr1 += X[j][i][k];
				expr2 += X[i][j][k];
			}
			model.add( (expr1 - expr2) = 0);
		}
	}

	//Collected prize by each vehicle and each visited node:
	for (int i = 1; i <= requests_number; ++i) {
		for (int k = 0; k < count; ++k) {
			IloExpr expr;
			for (int j = 1; j <= requests_number; ++j) {
				if(j == i) continue;
				expr += X[i][j][k];
			}
			model.add(expr = Y[i][k]);
		}
	}

	//Every vehicle leaves the start terminal:
	for (int k = 0; i < vehicles_number; ++k) {
		IloExpr expr;
		for (int j = 0; j <= requests_number*2 + 1; ++j) {
			expr += X[0][j][k];
		}
		model.add(expr = 1);
	}

	//Every vehicle enters the end terminal:
	IloCplex cplex(model);
	for (int k = 0; i < vehicles_number; ++k) {
		IloExpr expr;
		for (int i = 0; i <= requests_number*2 + 1; ++i) {
			expr += X[i][requests_number*2 + 1][k];
		}
		model.add(expr = 1);
	}


}