#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INT_MAX 2147483647

/**
 * Author: Dan Yee
 * ICSI 516, Project 2
 * Implementation of Dijkstra's (Link State) algorithm.
 * Bellman-Ford (Distance Vector) algorithm is also implemented, but not used.
 */

struct Vertex {
    int value;
    int distance;
    bool isVisited;
};

struct Edge {
    int source;
    int destination;
    int weight;
    bool isVisited;
};

struct Vertex *graph;                                               // keeps track of the Nodes in the graph and the distance to visit them
struct Edge *edges;                                                 // keeps track of the Edges in the graph and the weight of each Edge
int nodeCount;                                                      // number of nodes in the graph
int edgeCount = 0;                                                  // number of edges in the graph

/**
 * Prints the final distances from the first Vertex in the graph to every other Vertex
*/
void printGraph()
{
    printf("\nThe final distances are:\n");
    for (int i = 0; i < nodeCount; i++)
        printf("%d -> %d = %d\n", graph[0].value, graph[i].value, graph[i].distance);
}

/**
 * Implementation of the Bellman-Ford Distance Vector Algorithm
 * Loops n - 1 times where n is the number of Node's in the graph, updating the distance vectors if a new one is less than the current.
 * This algorithm iterates through all the Edges of the graph. The order of iteration does not matter.
 */
int bellmanFord()
{
    int numOfIterations = nodeCount - 1;
    for (int iterationCount = 0; iterationCount < numOfIterations; iterationCount++)
    {
        // Loop through all the edges and update Vertex distance costs
        for (int edgeI = 0; edgeI < edgeCount; edgeI++)
        {
            int src = edges[edgeI].source;
            int dst = edges[edgeI].destination;
            int weight = edges[edgeI].weight;
            int tmpDist = graph[src - 1].distance + weight;

            if (tmpDist < graph[dst - 1].distance)
            {
                if (graph[dst - 1].distance == INT_MAX)
                    printf("Updating distance vector of Node %d; Old: INFINITY, New: %d\n", dst, tmpDist);
                else
                    printf("Updating distance vector of Node %d; Old: %d, New: %d\n", dst, graph[dst - 1].distance, tmpDist);
                graph[dst - 1].distance = tmpDist;
            }
        }
    }
    return 0;
}

/**
 * Implementation of the extract-min operation for Dijkstra's algorithm. 
 * Returns the index of the next unvisited Node in the graph with the smallest distance vector or -1 if there are no more unvisited Nodes
 */
int findMinNodeIndex()
{
    int src = -1;
    int distance = INT_MAX;
    for (int nodeI = 0; nodeI < nodeCount; nodeI++)
    {
        if (!graph[nodeI].isVisited && graph[nodeI].distance <= distance)
        {
            src = nodeI;
            distance = graph[nodeI].distance;
        }
    }
    return src;
}

/**
 * Gets the index in the Edges array of the next unvisited Edge for a Node.
 * Returns -1 once all edges for the Node have been visited.
 */
int findEdgeIndex(int src)
{
    for (int edgeI = 0; edgeI < edgeCount; edgeI++)
    {
        if (!graph[src - 1].isVisited && !edges[edgeI].isVisited && edges[edgeI].source == src)
        {
            edges[edgeI].isVisited = true;
            return edgeI;
        }
    }
    graph[src - 1].isVisited = true;
    return -1;
}

/**
 * Implementation of Dijkstra's Link State Algorithm
 * Loops until all Node's in the graph are visited.
 * This algorithm iterates through all the Edges of each Node, marking them as visited after each pass.
 */
int dijkstra()
{
    int src;                               
    int currEdge;
    // For every node, scan all the edges and update distance vector
    while ((src = findMinNodeIndex()) != -1)
    {
        printf("Next Node = %d\n", graph[src].value);
        while ((currEdge = findEdgeIndex(src + 1)) != -1)                                          // findEdge() returns an Edge with source -1 when all edges of the specific source have been visited
        {
            int tmpDist = graph[src].distance + edges[currEdge].weight;
            int dst = edges[currEdge].destination;
            if (tmpDist < graph[dst - 1].distance)
            {
                if (graph[dst - 1].distance == INT_MAX)
                    printf("Updating distance vector of Node %d; Old: INFINITY, New: %d\n", dst, tmpDist);
                else
                    printf("Updating distance vector of Node %d; Old: %d, New: %d\n", dst, graph[dst - 1].distance, tmpDist);
                graph[dst - 1].distance = tmpDist;
            }
        }
        printf("\n");
    }
    return 0;
}

/**
 * Adds a new Edge to the Array of edges in this Graph
 */
int addEdge(int src, int dst, int weight)
{
    edgeCount++;
    if (edgeCount == 1)                                                 // if initial edge, allocate initial array space
        edges = (struct Edge*) malloc(sizeof(struct Edge));
    else                                                                // otherwise, expand the array by 1 struct Edge
        edges = realloc(edges, edgeCount * sizeof(struct Edge));

    struct Edge newEdge = {.source = src, .destination = dst, .weight = weight, .isVisited = false};
    edges[edgeCount - 1] = newEdge;
    return 0;
}

/**
 * Creates a graph with the specified number of Nodes.
 */
int createGraph()
{
    graph = (struct Vertex*) malloc(nodeCount * sizeof(struct Vertex));
    for (int i = 1; i <= nodeCount; i++)
    {
        struct Vertex newVertex = {.value = i, .distance = INT_MAX, .isVisited = false};
        graph[i - 1] = newVertex;
    }
    graph[0].distance = 0;                                              // set the cost of visiting the source node to be 0.
    return 0;
}

int main(int argc, char** argv)
{
    int tmpSrc, tmpDst, tmpWeight;                                      // for storing the information about edges temporarily as they are being read from file
    FILE* graphInput = fopen("networkGraph.txt", "r");
    if (graphInput == NULL)
    {
        perror("Error opening input file named \"networkGraph.txt\".\n");
        exit(-1);
    }

    /* Read the first line of the file that denotes the number of Nodes in this graph */
    if(fscanf(graphInput, "%d", &nodeCount) != 1)
    {
        perror("Error: Unable to read the number of Nodes in this graph.\n");
        exit(-1);
    }

    /* Create the graph by initializing nodeCount number of Nodes */
    if (createGraph() != 0)
    {
        perror("Error: Unable To Create Graph.\n");
        exit(-1);
    }
    
    /* Read all the remaining lines of the files that represent Edges in the graph */
    while (fscanf(graphInput, "%d,%d,%d", &tmpSrc, &tmpDst, &tmpWeight) == 3)
    {
        /* Make sure the source and destination Node's for each Edge are valid */
        if (tmpSrc < 1 || tmpSrc > nodeCount || tmpDst < 1 || tmpDst > nodeCount)
        {
            perror("Error: Invalid source/destination Node for Edge.");
            exit(-1);
        } 
        else
        {
            /* Add the edge if the Node's are valid */
            /* Add Edge from src->dst and from dst->src because the graph is undirected */
            if (addEdge(tmpSrc, tmpDst, tmpWeight) != 0 || addEdge(tmpDst, tmpSrc, tmpWeight) != 0)
            {
                perror("Error: Unable to add Edge.\n");
                exit(-1);
            }
        }
    }

    // /* Run the Bellman-Ford Distance Vector algorithm */
    // if (bellmanFord() != 0)
    // {
    //     perror("Error: Issue running Bellman-Ford algorithm.\n");
    //     exit(-1);
    // }

    /* Run the Dijkstra's Link State algorithm */
    if (dijkstra() != 0)
    {
        perror("Error: Issue running Dijkstra's algorithm.\n");
        exit(-1);
    }

    // display the final distances
    printGraph();

    // close the input file. Free all the allocated memory for the nodes and the edges of the graph.
    fclose(graphInput);
    if (edges)
        free(edges);
    if (graph)
        free(graph);
    return 0;
}