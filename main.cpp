#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <queue>

struct Node;
struct Edge;

using CostForNode = std::pair<int, int>;

struct Node {
	std::vector<int> edges;
};

struct Edge {
	int cost;
	int node1;
	int node2;
};

struct Graph {
	std::vector<Node> nodes;
	std::vector<Edge> edges;

	void Connect(int node1, int node2, int cost) {
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
		for (const auto& edge: edges) {
			std::cerr << edge.node1 << " -> " << edge.node2 
				<< "[label=\"" << edge.cost << "\", weight=\"" << edge.cost << "\", arrowhead=none];\n";
		}
		std::cerr << "}\n";
	}
};

int Dijkstra(const Graph& graph, int from_id, int to_id, int& visits) {
	using edge_it = std::vector<int>::const_iterator;

	visits = 0;

	std::vector<int> cost; // cost to reach node with the particular id.
	std::priority_queue<CostForNode, std::vector<CostForNode>, std::greater<CostForNode>> queue;

	cost.resize(graph.nodes.size(), -1);
	queue.push(CostForNode::pair(0, from_id));


	while (!queue.empty()) {
		CostForNode top_pair = queue.top();
		queue.pop();

		int top_cost = top_pair.first;
		int top = top_pair.second;

		if (top == to_id) {
			return top_cost;
		}

		// Someone already found this node. it is certainly a shortest path
		if (cost[top] >= 0) {
			continue;
		}

		cost[top] = top_cost;
		visits++;

		const std::vector<int>& adj_edges = graph.nodes[top].edges;
		for (edge_it it = adj_edges.cbegin(); it != adj_edges.cend(); ++it) {
			const Edge& edge = graph.edges[*it];

			int target = edge.node1 == top ? edge.node2 : edge.node1;

			if (cost[target] < 0) {
				const int tot_cost = edge.cost + top_cost;
				queue.push(CostForNode::pair(tot_cost, target));
			}
		}
	}
	return -1;
}

void MakeGraph(Graph& graph, int rows, int cols, int max_cost) {
	graph.AddNode();
	for (int row = 1; row < rows; ++row) {
		int current = graph.AddNode();
		graph.Connect(current - 1, current, std::rand() % max_cost + 1);
	}

	for (int x = 1; x < cols; ++x) {
		for (int y = 0; y < rows; ++y) {
			int current = graph.AddNode();
			if (y > 0) {
				graph.Connect(current - 1, current, std::rand() % max_cost + 1);
			}
			graph.Connect(current - rows, current, std::rand() % max_cost + 1);
		}
	}
}


int main()
{
	Graph g;

	MakeGraph(g, 3, 3, 9);
	Dijkstra(g, 0, 8);

}
