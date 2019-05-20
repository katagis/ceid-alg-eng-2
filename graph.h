#include <vector>
#include <queue>



enum Direction {
    UP, 
    DOWN,
    RIGHT,
    LEFT
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
// The internal format stores for each Node N 2 consecutive distances first to the right then to the down node.

// Our T type was used during development to test different sizes of distances (char vs int)
template<typename T>
struct GraphGeneric { 
    std::vector<T> Distances; 

    size_t Rows;
    size_t Cols;    

private:
    // Direct access to element X,Y in the array.
    // Expects valid x, y within SizeX, SizeY limits
    T GetDistanceInternal(int x, int y, bool down) const {
        return Distances[x * Rows * 2 + y * 2 + down];
    }

public:

    template<Direction D> 
    T GetDistance(int row, int col) const {
        // If D is not a Direction this will produce a compile error.
        return "Get Distance Expects a member of enum Direction as a template parameter.";
    }

    template<>
    T GetDistance<RIGHT> (int row, int col) const {
        return GetDistanceInternal(row, col, false);
    }

    template<>
    T GetDistance<DOWN> (int row, int col) const {
        return GetDistanceInternal(row, col, true);
    }

    template<>
    T GetDistance<LEFT> (int row, int col) const {
        return GetDistanceInternal(row, col - 1, false);
    }

    template<>
    T GetDistance<UP> (int row, int col) const {
        return GetDistanceInternal(row - 1, col, true);
    }
};