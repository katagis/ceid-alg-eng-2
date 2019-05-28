#ifndef __UTIL_H_
#define __UTIL_H_
#include "testbench.h"
#include <vector>
#include <unordered_set>
#include <iostream>

bool GDirected = true;
bool GDebugResult = false;

static Benchmark Bench;

using CostType = double;

std::string DirectedArrowhead() {
	return GDirected ? "" : ", arrowhead=none";
}

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


	Graph(int rows_, int cols_) {
		rows = rows_;
		cols = cols_;
		for (int i = 0; i < rows * cols; ++i) {
			AddNode();
		}
	}

	Graph(int rows_, int cols_, int max_cost) {
		rows = rows_;
		cols = cols_;
		// Add top left node,
		AddNode();

		// Add first column
		for (int row = 1; row < rows; ++row) {
			int current = AddNode();
			Connect(current - 1, current, std::rand() % max_cost + 1.0f);
		}

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

	void GraphVisPrint() const {
		std::cerr << "digraph G {\n";
		for (const auto& edge : edges) {
			std::cerr << edge.from << " -> " << edge.to
				<< "[label=\"" << edge.cost << "\", weight=\"" << edge.cost << "\"" << DirectedArrowhead() << "];\n";
		}
		std::cerr << "}\n";
	}

	Point ToPoint(int id) {
		return Point(id % rows, id / rows);
	}

	int FromXY(int x, int y) {
		return x + y * rows;
	}

};


CostType Astar(Graph& graph, int from_id, int to_id, int& visits);
CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits);

int round_to_i(float f) {
	return (int)std::floor((f) + 0.5f);
}

bool TestGraph(Graph& graph, int TestNum, const std::string& TestName, int start_id = -1, int end_id = -1) {

	int visits = 0;

	if (start_id == -1) {
		start_id = graph.FromXY(std::rand() % graph.rows, 0);
	}

	if (end_id == -1) {
		end_id = graph.FromXY(graph.rows - (std::rand() % graph.rows) - 1, graph.cols - 1);
	}
	
	std::cout << "From: " << start_id << " To: " << end_id << "\n";


	Bench.StartTest();
	int dijkstra_result = round_to_i(Dijkstra(graph, start_id, end_id, visits));

	Bench.SwitchTest(visits);
	visits = 0;
	int astar_result = round_to_i(Astar(graph, start_id, end_id, visits));
	Bench.StopTest(visits);


	std::ostringstream PaddedName;
	PaddedName << ": " << TestName.substr(0, 10 - 2);

	std::cout << "# Test " << std::setw(2) << TestNum
		<< std::left << std::setw(10) << PaddedName.str() << " > ";

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
	std::cerr << "Result: " << TheResult;
}


#endif