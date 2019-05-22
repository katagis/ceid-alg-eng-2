#include "testbench.h"
#include "util.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <queue>

CostType CalcH(Point p1, Point p2) {
	return std::sqrtf(
		std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)
		);

	//return (((p1.x - p2.x) << 2) + ((p1.y - p2.y) << 2)) >> 2;
}

#define DEBUG_RESULT
CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits) {
	using CostForNode = std::pair<CostType, int>;
	using edge_it = std::vector<int>::const_iterator;


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
	queue.push(CostForNode::pair(0.0f, from_id));

#ifdef DEBUG_RESULT
	std::vector<int> predecessor_edge(graph.nodes.size(), -1);

#endif

	while (!queue.empty()) {
		CostForNode top_pair = queue.top();
		queue.pop();

		const CostType top_cost = top_pair.first;
		const int top = top_pair.second;

		if (top == to_id) {
			return top_cost;
		}

		// if there is a recorded cost for the node just popped, it means we popped a non optimal path.
		if (cost[top] >= 0) {
#ifdef DEBUG_RESULT
			std::vector<int> predecessor_edge(graph.nodes.size(), -1);

#endif
			continue;
		}

		cost[top] = top_cost;
		visits++;

		const std::vector<int>& adj_edges = graph.nodes[top].edges;
		for (edge_it it = adj_edges.cbegin(); it != adj_edges.cend(); ++it) {
			const Edge& edge = graph.edges[*it];

			int target = edge.node1 == top ? edge.node2 : edge.node1;

			if (cost[target] < 0) {
				const CostType tot_cost = edge.cost + top_cost;
				queue.push(CostForNode::pair(tot_cost, target));
			}
		}
	}
	return -1;
}

CostType Astar(Graph& graph, int from_id, int to_id, int& visits) {
	using edge_it = std::vector<Edge>::iterator;
	
	Point target = graph.ToPoint(to_id);
	
	for (edge_it it = graph.edges.begin(); it != graph.edges.end(); ++it) {
		CostType n1 = CalcH(graph.ToPoint(it->node1), target);
		CostType n2 = CalcH(graph.ToPoint(it->node2), target);

		it->cost += n1 - n2;
	}
	CostType result = Dijkstra(graph, from_id, to_id, visits);

	return result - CalcH(graph.ToPoint(from_id), target);
}

int main()
{
	Graph g(10, 20, 9);

	
	TestGraph(g, 0,  "800x1000");

}
