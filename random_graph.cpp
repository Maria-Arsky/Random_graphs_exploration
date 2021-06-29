#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <array>
#include <queue>
#include <string>
using namespace std;

#define V 1000   // number of vertexes
#define L 500   // number of loops at 'find_ratio' function

// functions definition
array<vector<int>, V> build_random_graph(double p);
void print_graph(array<vector<int>, V> graph);
void BFS(array<vector<int>, V> graph, int vertex, int(&dist)[V]);
int connectivity(array<vector<int>, V> graph, int &diam);
int diameter(array<vector<int>, V> graph, int diam);
int is_isolated(array<vector<int>, V> graph);
void create_csv_file(string file_name, double(&probabilities)[10], double(&ratio)[10]);
void find_ratio(double(&probabilities)[10], double(&ratio)[10], int attribute);
int attributes_counters(double p, int attribute);


array<vector<int>, V> build_random_graph(double p) {
	array<vector<int>, V> graph;   // array of vertexes (indexes), each vertex has a linked list of neigbors (can be empty)
	int vertex, neighbor;
	float q;   // the random probability of an edge
	int num_of_random = p * 10000;   // adjusted to probability p

	srand(time(0));
	for (vertex = 0; vertex < V; vertex++) {
		for (neighbor = vertex + 1; neighbor < V; neighbor++) {   // we need to check each edge only once
			q = ((rand() % 10000) + 1);
			if (q <= num_of_random) {
				graph[vertex].push_back(neighbor);   // undirectet graph, save both (e,v) and (v,e)
				graph[neighbor].push_back(vertex);
			}
		}
	}
	return graph;
}

void print_graph(array<vector<int>, V> graph) {
	int vertex;

	for (vertex = 0; vertex < V; vertex++) {
		cout << "vertex number - " << vertex << ", it's neighbors: ";
		for (auto it = graph[vertex].begin(); it != graph[vertex].end(); it++) {   // checking over the nodes via iterator
			cout << *it << ' ';
		}
		cout << endl;
	}
}

void BFS(array<vector<int>, V> graph, int vertex, int (&dist)[V]) {
	int colors[V] = { 0 };   // white = 0, grey = 1, black = 2; initialize all vertexes to white
	fill_n(dist, V, -1);   // initialize all distances to infinity
	queue<int> vertex_Q;
	int temp_vertex;

	dist[vertex] = 0;
	colors[vertex] = 1;
	vertex_Q.push(vertex);
	while (!vertex_Q.empty()) {
		if (graph[vertex].empty())   // the graph isn't connected
			break;
		else {
			temp_vertex = vertex_Q.front();
			vertex_Q.pop();
			colors[temp_vertex] = 2;
			for (auto it = graph[temp_vertex].begin(); it != graph[temp_vertex].end(); it++) {   // check neighbors
				if (colors[*it] == 0) {
					colors[*it] = 1;
					dist[*it] = dist[temp_vertex] + 1;
					vertex_Q.push(*it);
				}
			}
		}
	}
}

int connectivity(array<vector<int>, V> graph, int &diam) {
	int vertex;
	int dist[V];   // if infinity than -1, else positive number

	BFS(graph, 0, dist);
	for (vertex = 1; vertex < V; vertex++) {   // looking for infinity
		if (dist[vertex] == -1) {
			diam = -1;
			return 0;   // disconnected
		}
		else if (diam < dist[vertex]) {
			diam = dist[vertex];   // saving max distance from first vertex
		}
	}
	return 1;   // connected
}

int diameter(array<vector<int>, V> graph, int diam) {
	int vertex, j;
	int dist[V];

	for (vertex = 1; vertex < V; vertex++) {   // running BFS from each vertex between 1 and 999
		BFS(graph, vertex, dist);
		for (j = vertex + 1; j < V; j++) {   // checking each distance between two vertexes only once
			if (diam < dist[j]) {
				diam = dist[j];
			}
		}
	}
	return diam;
}

int is_isolated(array<vector<int>, V> graph) {
	int vertex;

	for (vertex = 0; vertex < V; vertex++) {   // looking for vertex without neighbors
		if (graph[vertex].empty())
			return 1;
	}
	return 0;
}

void create_csv_file(string file_name, double (&probabilities)[10], double (&ratio)[10]) {
	fstream fout;
	int i;

	fout.open(file_name, ios::out | ios::app);

	fout << "probability, ";   // first row shows the probabilities
	for (i = 0; i < 9; i++)
		fout << probabilities[i] << ", ";
	fout << probabilities[9] << "\n";

	fout << "ratio, ";   // second row shows the ratios
	for (i = 0; i < 9; i++)
		fout << ratio[i] << ", ";
	fout << ratio[9] << "\n";

	fout.close();
}

void find_ratio(double (&probabilities)[10], double (&ratio)[10], int attribute) {
	int i, attr_counter;

	for (i = 0; i < 10; i++) {   // 10 different probabilities
		attr_counter = attributes_counters(probabilities[i], attribute);

		if (attribute == 1)   // attribute 1
			ratio[i] = double(attr_counter) / double(L);
		else if (attribute == 2)   // attribute 2
			ratio[i] = double(attr_counter) / double(L);
		else   // attribute 3
			ratio[i] = double(attr_counter) / double(L);
	}
}

int attributes_counters(double p, int attribute) {
	array<vector<int>, V> graph;
	int connected, graph_diam, isolated;
	int diam_vert_0 = NULL, loops, attr_counter = 0;

	for (loops = 0; loops < L; loops++) {   // 500 loops for each probability
		graph = build_random_graph(p);   // p is the current probability

		if (attribute == 1) {   // checking graphs about attribute 1
			connected = connectivity(graph, diam_vert_0);
			if (connected == 1)
				attr_counter++;
		}
		else if (attribute == 3) {   // checking graphs about attribute 3
			isolated = is_isolated(graph);
			if (isolated == 1)
				attr_counter++;
		}
		else {   // checking graphs about attribute 2
			connected = connectivity(graph, diam_vert_0);   // first we will check if the graph is connected
			if (diam_vert_0 != -1) {
				graph_diam = diameter(graph, diam_vert_0);   // and then we will search after the diameter
			}
			else   // if it is disconnected, than the diameter is infinity
				graph_diam = -1;

			if (graph_diam == 2)
				attr_counter++;
		}
	}
	return attr_counter;
}

int main()
{
	// Threshold1 = Threshold3 = 0.0069,   Threshold2 = 0.1175
	double probabilities_att_1_3[10] = { 0.0045, 0.005, 0.0055, 0.006, 0.0065, 0.007, 0.0075, 0.008, 0.0085, 0.009 };
	double probabilities_att_2[10] = { 0.095, 0.1, 0.105, 0.11, 0.115, 0.12, 0.125, 0.13, 0.135, 0.14 };
	double ratio[10];

	// create results file for attribute 1
	find_ratio(probabilities_att_1_3, ratio, 1);
	create_csv_file("connectivity.csv", probabilities_att_1_3, ratio);

	// create results file for attribute 2
	find_ratio(probabilities_att_2, ratio, 2);
	create_csv_file("diameter.csv", probabilities_att_2, ratio);

	// create results file for attribute 3
	find_ratio(probabilities_att_1_3, ratio, 3);
	create_csv_file("isolated_vertex.csv", probabilities_att_1_3, ratio);
}