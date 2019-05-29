#ifndef __UTIL_H_
#define __UTIL_H_
#include "testbench.h"
#include <vector>
#include <unordered_set>
#include <iostream>

bool GDirected = true;
bool GDebugResult = false;

static Benchmark Bench;

// Used during development to performance differences between calculation types.
typedef float CostType;

struct Node {
	std::vector<int> edges;
};

struct Edge {
	CostType cost;
	int from;
	int to;
};

struct Point {
	int x;
	int y;

	Point() {}
	
	Point(int X, int Y)
		: x(X)
		, y(Y) {}
};

struct Graph {
	std::vector<Node> nodes;
	std::vector<Edge> edges;

	int rows;
	int cols;
	int max_cost;

	Graph(int rows_, int cols_, int max_cost_) {
		rows = rows_;
		cols = cols_;
		max_cost = max_cost_;

		// Add top left node,
		AddNode();

		// Add first column
		for (int row = 1; row < rows; ++row) {
			int current = AddNode();
			Connect(current - 1, current, std::rand() % max_cost + 1.0f);
		}

		// Add the rest
		for (int x = 1; x < cols; ++x) {
			for (int y = 0; y < rows; ++y) {
				int current = AddNode();
				// The first of each column does not connect to the previous
				if (y > 0) {
					Connect(current - 1, current, std::rand() % max_cost + 1.0f);
				}
				Connect(current - rows, current, std::rand() % max_cost + 1.0f);
			}
		}
	}

	void Connect(int node1, int node2, CostType cost) {
		Edge e;
		e.cost = cost;
		e.from = node1;
		e.to = node2;
		int id = edges.size();
		edges.push_back(e);
		nodes[node1].edges.push_back(id);

		if (!GDirected) {
			nodes[node2].edges.push_back(id);
			return;
		}
		
		e.from = node2;
		e.to = node1;
		id = edges.size();
		edges.push_back(e);
		nodes[node2].edges.push_back(id);
	}

	int AddNode() {
		int id = nodes.size();
		nodes.push_back(Node());
		return id;
	}

	Point ToPoint(int id) {
		return Point(id % rows, id / rows);
	}

	int FromXY(int x, int y) {
		return x + y * rows;
	}
};

int round_to_i(float f) {
	return (int)std::floor((f)+0.5f);
}

CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits);
void AstarPrepareGraph(Graph& graph, int from_id, int to_id);
CostType Astar(Graph& graph, int from_id, int to_id, int& visits);

bool TestGraph(Graph& graph, const std::string& TestName, int TestNum, int start_id = -1, int end_id = -1) {

	int visits = 0;

	// if no start or end given make a random one.
	if (start_id == -1) {
		start_id = graph.FromXY(std::rand() % graph.rows, 0);
	}

	if (end_id == -1) {
		end_id = graph.FromXY(graph.rows - (std::rand() % graph.rows) - 1, graph.cols - 1);
	}
	

	Bench.StartTest();
	int dijkstra_result = round_to_i(Dijkstra(graph, start_id, end_id, visits));
	Bench.SwitchTest(visits);

	// To get any meaningfull runtime results we decided to not count the time Astar requires to change the edge weights.
	// Since the calculation requires calculating sqrt it is expensive enough for Astar to always be worst.
	AstarPrepareGraph(graph, start_id, end_id);

	Bench.StartTest();
	visits = 0;
	int astar_result = round_to_i(Astar(graph, start_id, end_id, visits));
	Bench.StopTest(visits);


	std::cout << "# Test " << TestName << " > ";
	if (dijkstra_result != astar_result) {
		std::cout << "# Test failed. Results are different.\n";
		std::cout << "# Dijk: " << dijkstra_result << " Astar: " << astar_result;
		return false;
	}
	else {
		std::cout << "Distance: " << astar_result;
	}
	
	Bench.PrintLast();
	return true;
}

std::string DirectedArrowhead() {
	return GDirected ? "" : ", arrowhead=none";
}

void GraphVisPrint(const Graph& g) {

	typedef std::vector<Edge>::const_iterator edge_it;
	std::cerr << "digraph G {\n";
	
	for (edge_it it = g.edges.cbegin(); it != g.edges.cend(); ++it) {
		const Edge& edge = *it;
		std::cerr << edge.from << " -> " << edge.to
			<< "[label=\"" << edge.cost << "\", weight=\"" << edge.cost << "\"" << DirectedArrowhead() << "];\n";
	}

	std::cerr << "}\n";
}

std::string GetGraphString(const Graph& g) {
	return std::to_string((long long)g.rows) + "x" + std::to_string((long long)g.cols) + " @" + std::to_string((long long)g.max_cost);
}

// Only used for debugging.
// Exports a dijkstra search to a nice graphviz format with weight cost labels and paths.
void AdvancedPrint(const Graph& g, const std::vector<CostType>& Costs, std::vector<int> Predecessor, int StartId, int TargetId) {
	
	std::unordered_set<int> SelectedPath;

	CostType TheResult = 0;

	int CurrNode = TargetId;
	while (CurrNode != StartId) {
		
		SelectedPath.insert(Predecessor[CurrNode]);
		const Edge& edge = g.edges[Predecessor[CurrNode]];
		TheResult += edge.cost;

		if (!GDirected) {
			CurrNode = edge.to == CurrNode ? edge.from : edge.to;
		}
		else {
			CurrNode = edge.from;
		}		
		
	}

	std::cerr << "digraph G {\n";
	
	for (int i = 0; i < g.nodes.size(); ++i) {
		std::cerr << i << " [label=\"" << i << "|" << std::setprecision(3) << Costs[i] << "\"]\n";
	}

	for (int i = 0; i < g.edges.size(); ++i) {
		const Edge& edge = g.edges[i];


		std::cerr << edge.from << " -> " << edge.to
			<< "[label=\"" << edge.cost << "\", weight=\"" << std::setprecision(4) << edge.cost << "\"" << DirectedArrowhead()
			<< (SelectedPath.count(i) > 0 ? ", color=red, penwidth=3" : "")
			<< "];\n";
	}

	std::cerr << "}\n";
	std::cerr << "Result: " << TheResult << "\n";
}

#endif