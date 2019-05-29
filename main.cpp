#include "testbench.h"
#include "util.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <queue>



CostType CalcH(Point p1, Point p2) {
	return std::sqrt(
		std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)
		);
}

CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits) {
	typedef std::pair<CostType, std::pair<int, int>> CostForNode;
	typedef std::vector<int>::const_iterator edge_it;


	std::vector<CostType> cost(graph.nodes.size(), -1); // cost to reach node with the particular id.
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
	queue.push(CostForNode::pair(0.0f, std::pair<int, int>(from_id, -1)));

	while (!queue.empty()) {
		CostForNode top_pair = queue.top();
		queue.pop();

		const CostType top_cost = top_pair.first;
		const int top = top_pair.second.first;

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

			if (cost[edge.to] < 0) {
				const CostType tot_cost = edge.cost + top_cost;
				queue.push(CostForNode::pair(tot_cost, std::pair<int, int>(edge.to, *it)));
			}
		}
	}
	return -1;
}

void AstarPrepareGraph(Graph& graph, int from_id, int to_id) {
	typedef std::vector<Edge>::iterator edge_it;

	const Point target = graph.ToPoint(to_id);

	for (edge_it it = graph.edges.begin(); it != graph.edges.end(); ++it) {
		CostType u = CalcH(graph.ToPoint(it->from), target);
		CostType v = CalcH(graph.ToPoint(it->to), target);

		it->cost += v - u;
	}
}

CostType Astar(Graph& graph, int from_id, int to_id, int& visits) {
	CostType result = Dijkstra(graph, from_id, to_id, visits);

	CostType h_s = CalcH(graph.ToPoint(from_id), graph.ToPoint(to_id));

	return result + h_s; // essentially: result - (Ht - Hs) but Ht is always 0
}

int main()
{
	std::srand(time(NULL));
	
	Graph g1(30, 1000, 10);
	TestGraph(g1, GetGraphString(g1), 0);
	
	Graph g2(60, 1000, 10);
	TestGraph(g2, GetGraphString(g2), 1);

	Graph g3(80, 1000, 10);
	TestGraph(g3, GetGraphString(g3), 2);

	Graph g4(30, 1000, 100);
	TestGraph(g4, GetGraphString(g4), 3);

	Graph g5(60, 1000, 100);
	TestGraph(g5, GetGraphString(g5), 4);

	Graph g6(80, 1000, 100);
	TestGraph(g6, GetGraphString(g6), 5);

	Bench.Print();
}
