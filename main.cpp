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


	std::vector<int> cost(graph.nodes.size(), -1); // cost to reach node with the particular id.
	std::priority_queue<CostForNode, std::vector<CostForNode>, std::greater<CostForNode>> queue;

	//
	// In our implementation:
	// The cost[node_id] remains -1 (inf/unvisited) even once it has been inserted to the queue.
	// a node may get multiple times in the priority queue with the path that inserted it initially.
	// 
	// The first time a node is popped from the queue the cost[node_id] is written and it is the
	// shortest possible path. Costs from other paths may still exist in the priority queue 
	// but they will be popped and skipped.
	//

	visits = 0;
	queue.push(CostForNode::pair(0, from_id));


	while (!queue.empty()) {
		CostForNode top_pair = queue.top();
		queue.pop();

		const int top_cost = top_pair.first;
		const int top = top_pair.second;

		if (top == to_id) {
			return top_cost;
		}

		// if there is a recorded cost for the node just popped, it means we popped a non optimal path.
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

struct Point {
	int x;
	int y;
	
	static Point FromId(int id, int row_size) {
		Point p;
		p.x = id % row_size;
		p.y = id / row_size;
		return p;
	}
};

int CalcH(Point p1, Point p2) {
	return (((p1.x - p2.x) << 2) + ((p1.y - p2.y) << 2)) >> 2;
}

int Astar(Graph& graph, int from_id, int to_id, int& visits, int row_size) {
	using edge_it = std::vector<Edge>::iterator;
	
	Point target = Point::FromId(to_id, row_size);
	
	for (edge_it it = graph.edges.begin(); it != graph.edges.end(); ++it) {
		Edge& edge = *it;


		edge.cost = CalcH(Point::FromId(edge.node1, row_size), target)
			- CalcH(Point::FromId(edge.node2, row_size), target)
			+ edge.cost;
	}

	return Dijkstra(graph, from_id, to_id, visits) - CalcH(Point::FromId(from_id, row_size), target);
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

	int row_size = 800;

	MakeGraph(g, row_size, 10000, 100);	
	
	int dv1 = 0;
	int d1 = Dijkstra(g, 0, g.nodes.size() - 1, dv1);

	std::cout << "Dijkstra: " << d1 << " visits: " << dv1 << "\n";

	int dv2 = 0;
	int d2 = Astar(g, 0, 8, dv1, row_size);

	std::cout << "Astar: " << d1 << " visits: " << dv1 << "\n";


}
