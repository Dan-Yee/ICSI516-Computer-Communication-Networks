#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MAX 2147483647

/**
 * Author: Dan Yee
 * ICSI 516, Project 2
 * Implementation of Bellman-Ford Distance Vector algorithm
 */

struct Vertex {
    int value;
    int distance;
};

struct Edge {
    int source;
    int destination;
    int weight;
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
 * Adds a new Edge to the Array of edges in this Graph
 */
int addEdge(int src, int dst, int weight)
{
    edgeCount++;
    if (edgeCount == 1)                                                 // if initial edge, allocate initial array space
        edges = (struct Edge*) malloc(sizeof(struct Edge));
    else                                                                // otherwise, expand the array by 1 struct Edge
        edges = realloc(edges, edgeCount * sizeof(struct Edge));

    struct Edge newEdge;
    newEdge.source = src;
    newEdge.destination = dst;
    newEdge.weight = weight;
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
        struct Vertex newVertex;
        newVertex.value = i;
        newVertex.distance = INT_MAX;
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
    while (!feof(graphInput))
    {
        /* Read each line that represents an Edge from the file */
        if (fscanf(graphInput, "%d,%d,%d", &tmpSrc, &tmpDst, &tmpWeight) != 3)
        {
            perror("Error: Problem reading specific edge from file.\n");
            exit(-1);
        }

        if (tmpSrc < 1 || tmpSrc > nodeCount || tmpDst < 1 || tmpDst > nodeCount)
        {
            perror("Error: Invalid source/destination Node for Edge.");
            exit(-1);
        } 
        else
        {
            /* Add the edge if the Node's are valid */
            if (addEdge(tmpSrc, tmpDst, tmpWeight) != 0)
            {
                perror("Error: Unable to add Edge.\n");
                exit(-1);
            }
        }
    }

    /* Run the Bellman-Ford Distance Vector algorithm */
    if (bellmanFord() != 0)
    {
        perror("Error: Issue running Bellman-Ford algorithm.\n");
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