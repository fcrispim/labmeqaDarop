#include <cstdio>
#include <vector>

using namespace std;

class Node{

public:
	Node(int v, int w): vertex(v), weight(w){}
	virtual ~Node(){}

	void printNode(){printf("Vertex = %d - Weight %lf\n", vertex, weight);}

private:
	int vertex;
	double weight;
};

class Graph{

public:
	Graph(int n);
	virtual ~Graph(){}

	void addNode(Node n, int vertex);
	vector<Node> adjList(int vertex);
	void printGraph();

private:
	vector<vector <Node> > G ;
};