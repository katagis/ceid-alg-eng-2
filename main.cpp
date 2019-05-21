#include "testbench.h"
#include "graph.h"

#include <iostream>
#include <queue>
#include <unordered_set>


struct DijkstraQueueElement {
    int cost;
    Point p;

    DijkstraQueueElement(Point inP, int inCost)
        : cost(inCost)
        , p(inP) {}

 };

bool operator<(const DijkstraQueueElement& rhs, const DijkstraQueueElement& lhs) {
    return rhs.cost > lhs.cost;
}

long int Dijkstra_sp(const Graph& graph, int from_row, int to_row, int& nodes_visited) {
    using Qe = DijkstraQueueElement;


	Point begin(from_row, 0);
    Point end(to_row, graph.Cols - 1);

    std::priority_queue<Qe> Queue;
    std::unordered_set<Point, PointHash> Visited;

    nodes_visited = 0;

    Queue.push(Qe(Point(from_row, 0), 0));
    Visited.emplace(Point(from_row, 0));

	NodeVector<Point> Predecessor(graph, Point());



    while(Queue.size() > 0) {
        Qe top_element = Queue.top();
        Queue.pop();
        nodes_visited++;
        
        Point top = top_element.p;
        int cost = top_element.cost;

        if (end == top) {
			Point Prev = end;
			std::cout << "-> " << end;
			while (!(Predecessor[Prev] == begin)) {
				std::cout << "-> " << Predecessor[Prev];
				Prev = Predecessor[Prev];
			}
			std::cout << "-> " << begin;
            return cost;
        }

        Point p = top.Up();
        if (p.row >= 0 && std::get<1>(Visited.insert(p))) {
			Predecessor[p] = top;
            Queue.push(Qe(p, graph.GetDistance<UP>(p) + cost));
        }

        p = top.Down();
        if (p.row < graph.Rows && std::get<1>(Visited.insert(p))) {
			Predecessor[p] = top;
            Queue.push(Qe(p, graph.GetDistance<DOWN>(p) + cost));
        }

        p = top.Right();
        if (p.col < graph.Cols && std::get<1>(Visited.insert(p))) {
			Predecessor[p] = top;
            Queue.push(Qe(p, graph.GetDistance<RIGHT>(p) + cost));
        }

        p = top.Left();
        if (p.col >= 0 && std::get<1>(Visited.insert(p))) {
			Predecessor[p] = top;
            Queue.push(Qe(p, graph.GetDistance<LEFT>(p) + cost));
        }
    }


    return -1;
}

int main() {
    Graph graph(3, 3, 5);
    
    std::cout << graph;

    int nodes = 0;
	std::cout << "Cost: " << Dijkstra_sp(graph, 0, 1, nodes) << " Nodes visited: " << nodes << "\n";

 
}