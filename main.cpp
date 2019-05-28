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

	//return (((p1.x - p2.x) << 2) + ((p1.y - p2.y) << 2)) >> 2;
}

#define DEBUG_RESULT
CostType Dijkstra(const Graph& graph, int from_id, int to_id, int& visits) {
	using CostForNode = std::pair<CostType, std::pair<int, int>>;
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
	queue.push(CostForNode::pair(0.0f, std::pair<int, int>(from_id, -1)));

#ifdef DEBUG_RESULT
	std::vector<int> predecessor_edge(graph.nodes.size(), -1);
#endif

	while (!queue.empty()) {
		CostForNode top_pair = queue.top();
		queue.pop();

		const CostType top_cost = top_pair.first;
		const int top = top_pair.second.first;

		if (top == to_id) {
#ifdef DEBUG_RESULT
			if (GDebugResult) {
				cost[to_id] = top_cost;
				predecessor_edge[to_id] = top_pair.second.second;
				AdvancedPrint(graph, cost, predecessor_edge, from_id, to_id);
			}
#endif

			return top_cost;
		}

		// if there is a recorded cost for the node just popped, it means we popped a non optimal path.
		if (cost[top] >= 0) {
			continue;
		}

		cost[top] = top_cost;
		visits++;

#ifdef DEBUG_RESULT
		predecessor_edge[top] = top_pair.second.second;		
#endif

		const std::vector<int>& adj_edges = graph.nodes[top].edges;
		for (edge_it it = adj_edges.cbegin(); it != adj_edges.cend(); ++it) {
			const Edge& edge = graph.edges[*it];
			
			int target = edge.to;
			if (!GDirected) {
				target = edge.to == top ? edge.from : edge.to;
			}

			if (cost[target] < 0) {
				const CostType tot_cost = edge.cost + top_cost;
				queue.push(CostForNode::pair(tot_cost, std::pair<int, int>(target, *it)));
			}
		}
	}
	return -1;
}

CostType Astar(Graph& graph, int from_id, int to_id, int& visits) {
	using edge_it = std::vector<Edge>::iterator;
	
	Point target = graph.ToPoint(to_id);
	
	for (edge_it it = graph.edges.begin(); it != graph.edges.end(); ++it) {
		CostType u = CalcH(graph.ToPoint(it->from), target);
		CostType v = CalcH(graph.ToPoint(it->to), target);


		it->cost += v - u;
	}
	CostType result = Dijkstra(graph, from_id, to_id, visits);


	CostType h_s = CalcH(graph.ToPoint(from_id), target);
	CostType h_t = CalcH(graph.ToPoint(to_id), target);

	std::cerr << "h(s): " << h_s << " h(t): " << h_t << std::endl;

	return result + h_s;
}

int main()
{
	//std::srand(time(NULL));

	// Error 1 example:
	//std::srand(120);
	//Graph g(4, 22, 1000);


	

	//Graph g(3, 3);
	/*
	g.Connect(0, 1, 50);
	g.Connect(1, 2, 50);
	g.Connect(0, 3, 5);
	g.Connect(1, 4, 5);
	g.Connect(2, 5, 10);
	g.Connect(3, 4, 10);
	g.Connect(4, 5, 10);
	

	g.Connect(3, 6, 3);
	g.Connect(4, 7, 5);
	g.Connect(5, 8, 50);


	g.Connect(6, 7, 50);
	g.Connect(7, 8, 50);
	*/

	// std::srand(i);
	// Graph g(4, 4, 10);

	/*
	GDebugResult = true;

	std::srand(1);
	Graph g(3, 3, 10);
	TestGraph(g, 0, "800x1000");
	*/
	for (int i = 0; i < 10000; ++i) {
		std::srand(i);
		Graph g(50, 300, 100);
		if (!TestGraph(g, 0, "800x1000")) {
			std::cout << "ERROR AT SEED: " << i;
			getchar();
			getchar();
			GDebugResult = true;
			--i;
		}
	}
	
	
	

}
