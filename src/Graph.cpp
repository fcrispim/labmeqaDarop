#include "Graph.h"

Graph::Graph(int n){
	G.resize(n);
}

void Graph::addNode(Node n, int vertex){
	G[vertex].push_back(n);
}

vector<Node> Graph::adjList(int vertex){
	return G[vertex];
}

void Graph::printGraph(){
	for (int i = 0; i < G.size(); ++i){
		for (int j = 0; j < G[i].size(); ++j){
			G[i][j].printNode();
		}
		printf("\n");
	}
}