#ifndef __UTIL_H_
#define __UTIL_H_

using CostType = int;

static Benchmark Bench;

struct Node {
	std::vector<int> edges;
};

struct Edge {
	CostType cost;
	int node1;
	int node2;
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

	int row_size;

	Graph(int rows, int cols, int max_cost) {
		row_size = cols;
		// Add top left node,
		AddNode();

		// Add first column
		for (int row = 1; row < rows; ++row) {
			int current = AddNode();
			Connect(current - 1, current, std::rand() % max_cost + 1);
		}

		for (int x = 1; x < cols; ++x) {
			for (int y = 0; y < rows; ++y) {
				int current = AddNode();
				// The first of each column does not connect to the previous
				if (y > 0) {
					Connect(current - 1, current, std::rand() % max_cost + 1);
				}
				Connect(current - rows, current, std::rand() % max_cost + 1);
			}
		}
	}

	void Connect(int node1, int node2, CostType cost) {
		Edge e;
		e.cost = cost;
		e.node1 = node1;
		e.node2 = node2;
		int id = edges.size();
		edges.push_back(e);
		nodes[node1].edges.push_back(id);
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
			std::cerr << edge.node1 << " -> " << edge.node2
				<< "[label=\"" << edge.cost << "\", weight=\"" << edge.cost << "\", arrowhead=none];\n";
		}
		std::cerr << "}\n";
	}

	Point ToPoint(int id) {
		return Point(id % row_size, id / row_size);
	}
};


CostType Astar(Graph& graph, int from_id, int to_id, int& visits);
CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits);

void TestGraph(Graph& graph, int TestNum, const std::string& TestName) {

	int visits = 0;

	int start_id = 0;//std::rand() % row_size;
	int end_id = graph.nodes.size() - 1;// -(std::rand() % row_size) - 1;

	std::cout << "From: " << start_id << " To: " << end_id << "\n";


	Bench.StartTest();
	int dijkstra_result = Dijkstra(graph, start_id, end_id, visits);

	Bench.SwitchTest(visits);
	visits = 0;
	int astar_result = Astar(graph, start_id, end_id, visits);
	Bench.StopTest(visits);


	std::ostringstream PaddedName;
	PaddedName << ": " << TestName.substr(0, 14 - 2);

	std::cout << "# Test " << std::setw(2) << TestNum
		<< std::left << std::setw(14) << PaddedName.str() << "| ";

	if (dijkstra_result == astar_result) {
		std::cout << std::setw(5) << std::right << astar_result;
		Bench.PrintLast();
	}
	else {
		std::cout << "# Test failed. Results are different.\n";
		std::cout << "Dijk: " << dijkstra_result << " Astar: " << astar_result << "\n";
	}
}




#endif