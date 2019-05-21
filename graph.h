#include <vector>
#include <queue>
#include <ostream>
#include <iomanip>



enum Direction {
    UP, 
    DOWN,
    RIGHT,
    LEFT
};

struct Point {
	int row;
	int col;

	Point() = default;

	Point(int inRow, int inCol)
		: row(inRow)
		, col(inCol) {}

	Point Right() const {
		return Point(row, col + 1);
	}

	Point Left() const {
		return Point(row, col - 1);
	}

	Point Down() const {
		return Point(row + 1, col);
	}

	Point Up() const {
		return Point(row - 1, col);
	}

	int ToIndex(int GraphRows) const {
		return  GraphRows * row + col;
	}
};


// Due to the nature of the assignment we don't actually need node data.
// Since we have a 2D grid we can only store edge costs and determine the nodes they connect
// by only their position in the array they are stored.

// We used a single std::vector with appropriate get functions to provide similar interface with a 2D C array.

// Our vector contains an extra connection on odd lines just to preserve exact 2D size N*M.
// This makes little difference in ram usage but saves a % 2 calculation on vector access.

// example of our graph:
// * - * - * - ...
// |   |   |   ...
// * - * - * - ...
// 
// Where * is a node, we store all distances where the symbols - and | are.
// The internal format stores for each Node: 2 consecutive distances. first to the right then to the down node.

// Our T type was used during development to test different sizes of distances (char vs int)
template<typename T>
struct GraphGeneric { 
    std::vector<T> Distances; 

    size_t Rows;
    size_t Cols;    

    GraphGeneric(int rows, int cols, T max)
        : Rows(rows)
        , Cols(cols) {
            for(int i = 0; i < rows * cols * 2; ++i) {
                Distances.push_back(std::rand() % max + 1);
            }
        }

private:
    // Direct access to element X,Y in the array.
    // Expects valid x, y within SizeX, SizeY limits
    T GetDistanceInternal(int x, int y, bool down) const {
        return Distances[x * Rows * 2 + y * 2 + down];
    }

public:

    template<Direction D> 
    T GetDistance(Point P) const {
        // If D is not a Direction this will produce a compile error.
        return "Get Distance Expects a member of enum Direction as a template parameter.";
    }

    template<>
    T GetDistance<RIGHT>(Point P) const {
        return GetDistanceInternal(P.row, P.col, false);
    }

    template<>
    T GetDistance<DOWN>(Point P) const {
        return GetDistanceInternal(P.row, P.col, true);
    }

    template<>
    T GetDistance<LEFT>(Point P) const {
        return GetDistanceInternal(P.row, P.col - 1, false);
    }

    template<>
    T GetDistance<UP>(Point P) const {
        return GetDistanceInternal(P.row - 1, P.col, true);
    }
};
using Graph = GraphGeneric<int>;

struct PointHash {
	std::size_t operator()(const Point& p) const {
		return p.row << 31 | p.col;
	}
};

bool operator==(const Point & lhs, const Point & rhs) {
	return lhs.row == rhs.row && lhs.col == rhs.col;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, GraphGeneric<T>& g) {
    os << "Rows: " << g.Rows << " Cols: " << g.Cols << "\n";

    for (int i = 0; i < g.Rows; ++i) {
        for (int j = 0; j < g.Cols; ++j) {
            os << " # " << std::setw(1) << (int)g.GetDistance<RIGHT>(Point(i, j));
        }
        os << "\n";
        for (int j = 0; j < g.Cols; ++j) {
            os << " " << std::setw(1) << (int)g.GetDistance<DOWN>(Point(i, j)) << "  ";
        }
        os << "\n";
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << "[" << p.row << "," << p.col << "]";
	return os;
}

template<typename T>
struct NodeVector {
	int Rows;
	std::vector<T> Vector;
	
	// no vector init version.
	NodeVector(const Graph& G)
		: Rows(G.Rows) {

	}
	
	NodeVector(const Graph& G, const T& defaulValue)
		: Rows(G.Rows) {
		Vector.resize(G.Rows * G.Cols, defaulValue);
	}
	
	T& operator[] (Point p) {
		return Vector[p.ToIndex(Rows)];
	}
};