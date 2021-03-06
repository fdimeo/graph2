#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <ctime>    // standard C library
#include <cstdlib>  // standard C library

#include <fstream>

// forward class declarations
class graphPoint;
class Graph;


class ShortestPathAlgo
{
private:
   std::list<unsigned int> *pathList;
   int pathCost;  // the path cost, or (-1) if no path exists

public:

   ShortestPathAlgo();
   ~ShortestPathAlgo();

   // returns a count of the nodes
   unsigned int verticies(Graph &G);

   // returns the cost of the path (or -1 if no path exists)
   int path_size( Graph &G, unsigned int originNode, unsigned int destNode );

   // returns a std::list pointer with the path
   std::list<unsigned int> *path( Graph &G, unsigned int originNode, unsigned int destNode);

   // this helps print the path list
   friend std::ostream &operator<< (std::ostream &cout, std::list<unsigned int> *path);


};
//-------------------------------------------------------------------------------------------------------
//  A class defining an entire graph, which is comprised of graphPoints with edges to other graphPoints
//-------------------------------------------------------------------------------------------------------
//
class Graph
{

private:
   std::map< int, graphPoint* > graphNodes;// a map of all graphPoints (i.e. nodes, vertices) in the graph
   unsigned int m_totalNumVerticies;       // the total number of vertices (nodes) in this graph
   unsigned int m_totalNumEdges;           // the total number of edges in this graph
   int m_originNode;                       // the node number of the origin (-1 if not assigned)

public:
   Graph();
   Graph( char* fileName);
   void addNode(unsigned int nodeNumber);
   void removeNode(unsigned int nodeNumber);
   void setNodeValue(unsigned int nodeNumber, unsigned int cost);
   unsigned int getNodeValue(unsigned int nodeNumber);
   void addEdge(unsigned int sourceNodeNumber, unsigned int destNodeNumber, unsigned int edgeWeight);
   bool hasEdge(unsigned int sourceNodeNumber, unsigned int destNodeNumber);

   void deleteEdge(unsigned int sourceNodeNumber, unsigned int destNodeNumber);
   void makeOriginNode(unsigned int nodeNumber);  // origin nodes have a totalcost of 0 and have been visited
   int modifyEdge(unsigned int destNodeNumber, unsigned int weight);
   int getEdgeValue(unsigned int sourceNodeNumber, unsigned int destNodeNumber);
   int setEdgeValue(unsigned int sourceNodeNumber,unsigned int destNodeNumber, unsigned int weight );
   unsigned int getNodeCount(void);
   unsigned int getEdgeCount(void);
   bool isNodeVisited(unsigned int nodeNumber);
   void setNodeVisited(unsigned int nodeNumber);
   void doDijkstra( unsigned int originNode, unsigned int destNode, std::list<unsigned int> *pathResult, int &pathCost);
   void doPrim( unsigned int originNode);

   void printGraph();
};


//-------------------------------------------------------------------------------------------------------
//  A class defining a graphPoint, which may become included in a graph
//-------------------------------------------------------------------------------------------------------
//
class graphPoint 
{

   friend class Graph;

private:
   unsigned int m_nodeNumber;                    // a unique identifier for this node
   std::map<unsigned int, unsigned int> m_edges; // a vector of all edges from the node
   unsigned int m_viaNode;                       // the "from node" to this node for the m_totalCost recorded
   int    m_totalCost;                           // total path cost for this instance
   bool   m_visited;                             // has this instance been visited in the algorthim?
   unsigned int m_numEdges;                      // this number of edges for this instance

public:
   graphPoint( unsigned int nodeNumber, int cost, bool visited);
   void printGraphPoint();
   void createEdge(unsigned int dest_node, unsigned int weight);
   int deleteEdge(unsigned int dest_node);
   int setEdgeValue(unsigned int sourceNodeNumber,unsigned int NodeNumber, unsigned int weight );
   int getEdgeValue(unsigned int sourceNodeNumber );
   int modifyEdge(unsigned int dest_node, unsigned int weight);
   int getPointCost ();
   void setPointCost (int cost);
   bool modifyPoint (int cost, bool visited);
   void setVisited ();
   bool getVisited ();
   void cleanNode();


};
 
//*****************************************************************
//**
//** graphPoint methods
//**
//*****************************************************************


graphPoint::graphPoint( unsigned int nodeNumber, int cost = (-1), bool visited = false )
{
   m_totalCost = cost;          // the accumulated cost of this node
   m_visited = visited;         // not visited when created (but can be overriden for the source node)
   
   m_nodeNumber = nodeNumber;   // this node's number
   m_viaNode = nodeNumber;      // the node number that this node was reached from (start with self)
   m_numEdges = 0;              // there are no edges to start

   //   std::cout << "Node number " << nodeNumber << " added" << std::endl;

}

bool graphPoint::modifyPoint (int cost, bool visited = false)
{
   m_totalCost = cost;
   m_visited = visited;
}

void graphPoint::setVisited ()
{
   m_visited = true;
}

bool graphPoint::getVisited ()
{
   return m_visited;
}


int graphPoint::getPointCost ()
{
   return m_totalCost;
}

void graphPoint::setPointCost (int cost)
{
   m_totalCost = cost;
}

void graphPoint::cleanNode()
{
   m_viaNode = 0;
   m_totalCost = -1;
   m_visited = false;
}


void graphPoint::printGraphPoint()
{
   std::cout << "Graph point #" << m_nodeNumber
             << ((m_totalCost == 0) ? " (ORIGIN)" : "")
             << " has a cost of " 
             << m_totalCost 
             << " and has" << (m_visited ? "" : " not") << " been visited" << std::endl;
   
   if(m_numEdges) std::cout << "Edge at:" << std::endl;
   
   for(std::map<unsigned int, unsigned int>::iterator it=m_edges.begin(); it != m_edges.end(); ++it)
   {
      std::cout << "-- to node:" << it->first << " (" << it->second << ")" << std::endl;
   } 
}

// create a new edge to "dest_node" with a cost of "weight"
void graphPoint::createEdge(unsigned int dest_node, unsigned int weight)
{
      std::map<unsigned int, unsigned int>::iterator it;

      // replace any duplicate entry with a new weight
      if((it = m_edges.find(dest_node)) != m_edges.end())
      {
         // std::cout << "duplicate edge \"dest node\"found: "
         //           << it->first << ", replacing " << it->second 
         //           << " with " << weight << std::endl;

         it->second = weight;
         return;
      }

      // otherwise, insert a new edge in the set of all edges from this node
      else
      {
         m_edges.insert(std::pair<unsigned int,unsigned int>(dest_node, weight));
         m_numEdges++;
      }
}

// remove the edge to "dest_node"
int graphPoint::deleteEdge(unsigned int dest_node)
{
   int retval = -1;
   std::map<unsigned int, unsigned int>::iterator it;
   
   // find and delete the edge.  If not found, do nothing
   if((it = m_edges.find(dest_node)) != m_edges.end())
   {
      std::cout << "\"dest node\"found: "
                << it->first << ", erasing edge to it " << it->second 
                << std::endl;
      
      m_edges.erase(it);
      m_numEdges--;
      retval = 0;
   }

   return retval;
   
}

// modify the edge to "dest_node"
//
// return -1 if error, 0 if ok.
//
int graphPoint::modifyEdge(unsigned int dest_node, unsigned int weight)
{
   int retval = -1;
   std::map<unsigned int, unsigned int>::iterator it;
   
   // find and delete the edge.  If not found, do nothing
   if((it = m_edges.find(dest_node)) != m_edges.end())
   {
      // std::cout << "\"dest node\"found: "
      //           << it->first << ", modifying the edge to " << it->second 
      //           << std::endl;
      
      it->second = weight;
      return 0;
   }

   return -1;

}

// get the edge to "dest_node"
//
// return -1 if error, edge weight if ok
//
int graphPoint::getEdgeValue(unsigned int node)
{
      int retval = -1;
      std::map<unsigned int, unsigned int>::iterator it;

      // std::cout << "Finding edge cost from " << m_nodeNumber << " to " << node << std::endl;

      // find the edge.  If not found, return a cost of -1 (this mean infinity)
      if((it = m_edges.find(node)) != m_edges.end())
      {

         // std::cout << "Found a cost of : " << it->second << std::endl;

         retval = (it->second);
      }

      // std::cout << "done" << std::endl;

      return retval;

}


//*****************************************************************
//**
//** Graph methods
//**
//*****************************************************************
//

Graph::Graph()
{
   m_totalNumVerticies = 0;
   m_totalNumEdges = 0;
}

// add a node to the graph
void Graph::addNode(unsigned int nodeNumber)
{
   std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

   if(it == graphNodes.end())
   {
      std::cout << "adding node " << nodeNumber << std::endl;
      graphNodes[nodeNumber] = new graphPoint(nodeNumber);
      m_totalNumVerticies++;
      m_originNode = -1;
   }
}

// // remove a node to the graph (but only if it exists)
// void Graph::removeNode(unsigned int nodeNumber)
// {
//    std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

//    if( it != graphNodes.end())
//    {
//       graphNodes.erase(it);
//       m_totalNumVerticies--;
//    }
// }


void Graph::addEdge(unsigned int sourceNodeNumber, unsigned int destNodeNumber, unsigned int edgeWeight)
{
   std::map<int, graphPoint* >::iterator it_s = graphNodes.find(sourceNodeNumber);
   std::map<int, graphPoint* >::iterator it_d = graphNodes.find(destNodeNumber);

   if( it_s != graphNodes.end())
   {
      it_s->second->createEdge(destNodeNumber, edgeWeight);
      m_totalNumEdges++;
   }
}

bool Graph::hasEdge(unsigned int sourceNodeNumber, unsigned int destNodeNumber)
{
   return (graphNodes.find(sourceNodeNumber)->second->getEdgeValue(sourceNodeNumber) == destNodeNumber);
}


// void Graph::deleteEdge(unsigned int sourceNodeNumber,unsigned int destNodeNumber)
// {
//    std::map<int, graphPoint* >::iterator it = graphNodes.find(sourceNodeNumber);

//    if( it != graphNodes.end())
//    {
//       if(!(it->second->deleteEdge(destNodeNumber))) m_totalNumEdges--;
//    }
// }

   
//  make the specified node the origin
void Graph::makeOriginNode(unsigned int nodeNumber)
{
   std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

   // To be complete, we should check if m_originNode is -1 before we set the origin.
   // If m_originNode != -1, then the user had already set the origin, and is modifying it.
   // In that case, we should remove "orgin-ness" from the originally set origin point
   // For now, assume that the origin will not be changed once it's set.

   if( it != graphNodes.end())
   {
      it->second->modifyPoint (0, true);
      m_originNode = nodeNumber;
   }
}

void Graph::setNodeValue(unsigned int nodeNumber, unsigned int cost)
{
   std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

   if( it != graphNodes.end())
   {
      it->second->setPointCost (cost);
   }
}

// unsigned int Graph::getNodeValue(unsigned int nodeNumber)
// {
//    unsigned int retval = -1;
//    std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

//    if( it != graphNodes.end())
//    {
//       retval = it->second->getPointCost ();
//    }

//    return retval;
// }

// int Graph::setEdgeValue(unsigned int sourceNodeNumber,unsigned int destNodeNumber, unsigned int weight )
// {
//    unsigned int retval = -1;
//    std::map<int, graphPoint* >::iterator it = graphNodes.find(sourceNodeNumber);

//    if( it != graphNodes.end())
//    {
//       retval = it->second->modifyEdge (destNodeNumber, weight);
//    }

//    return retval;
// }


//returns -1 if not found
int Graph::getEdgeValue(unsigned int sourceNodeNumber,unsigned int destNodeNumber)
{
   unsigned int retval = -1;
   std::map<int, graphPoint* >::iterator it = graphNodes.find(sourceNodeNumber);

   if( it != graphNodes.end())
   {
      retval = it->second->getEdgeValue (destNodeNumber);
   }

   return retval;
}

bool Graph::isNodeVisited(unsigned int nodeNumber)
{

   bool retval = false;
   std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

   if( it != graphNodes.end())
   {
      retval = it->second->getVisited();
   }

   return retval;

}

void Graph::setNodeVisited(unsigned int nodeNumber)
{
   std::map<int, graphPoint* >::iterator it = graphNodes.find(nodeNumber);

   if( it != graphNodes.end())
   {
      it->second->setVisited();
   }

}


unsigned int Graph::getNodeCount()
{
   return m_totalNumVerticies;
}

unsigned int Graph::getEdgeCount()
{
   return m_totalNumEdges;
}

void Graph::printGraph()
{
   unsigned int retval = -1;
   

   for(std::map<int, graphPoint* >::iterator it = graphNodes.begin(); it != graphNodes.end(); ++it)
   {
      it->second->printGraphPoint();
   }

   std::cout << "TOTAL NODES: " << getNodeCount() << "\tTOTAL EDGES: " << getEdgeCount() << "\n" << std::endl;
 

}

Graph::Graph(char *fileName)
{

   m_totalNumVerticies = 0;
   m_totalNumEdges = 0;

   int graphSize;
   
   // read the graph from a file.
   // the first int is the size, and followed by tuples of int node1, int node2, int cost
   // open the graph file for read only
   std::fstream filestream(fileName, std::fstream::in );

   filestream >> graphSize;

   std::cout << " graph size is " << graphSize << " nodes" << std::endl;

   while(true)
   {    
      unsigned int node1, node2, cost;
      
      // collect a edge data
      filestream >> node1 >> node2 >> cost;

      // only read with the input is valid
      if(filestream.eof()) break;

      std::cout << "node1: " << node1 << " node2: " << node2 << " cost: " << cost << std::endl;
      addNode(node1);       // these addNode calls are idempotent 
      addNode(node2);
      addEdge(node1, node2 , cost);
   }
}

// a type which defines a conceptual two dimensional vector of ints (or at least can be addressed as such)
typedef typename std::vector< std::vector<int> > mst_result;

typedef typename std::map<unsigned int, unsigned int> edge_type;

// a type which describes a node in the graph
typedef typename std::map< int, graphPoint* > node_type;

const int NODENUM_IDX = 0;
const int EDGEWEIGHT_IDX = 1;

void Graph::doPrim( unsigned int originNode)
{

   int debug;
   int i;
   // initialize the 2d vector of [numNodes][2] ints. (nodenumber, weight) will be stored as we compute the prim solution
   mst_result mst_for_graph(m_totalNumVerticies, std::vector<int> (2));

   std::cout << "Starting Prim MST algorithm for " << m_totalNumVerticies << " nodes...";

   // std::cin >> debug;

   // initialize the solution 
   for(i=0; i<m_totalNumVerticies; i++)
   {
      mst_for_graph[i][NODENUM_IDX]=(-1);
   }

   // add the origin node to the solved set
   mst_for_graph[0][NODENUM_IDX] = originNode;
   mst_for_graph[0][EDGEWEIGHT_IDX] = 0;
   setNodeVisited(originNode);

   int solution_points_found=1;

    // now build the MST while iterating through the graph
   for(i=(m_totalNumVerticies-1); i>0; i--)
   {
      unsigned int lowest_cost_edge_this_iteration = 100; // the lowest cost and node for this iteration
      unsigned int lowest_cost_node_this_iteration = (-1);

      std::cout << "iterating through the mst solution " << i << " more nodes left in graph" << std::endl;
      // std::cin >> debug;

      for(int j=0; j<solution_points_found; j++)
      {
         std::cout << "looking at solution location " << j << " node: " << mst_for_graph[j][NODENUM_IDX] << std::endl;
         // std::cin >> debug;

         // find the node info for the node being examined (this lookup cannot fail)
         node_type::iterator itNode=graphNodes.find(mst_for_graph[j][NODENUM_IDX]);
        
         std::cout << "examining the edges connected to node: " << itNode->second->m_nodeNumber << std::endl;
         // std::cin >> debug;

         // look through the edges of all the nodes in the solution so far
         for(edge_type::iterator itEdge=itNode->second->m_edges.begin(); itEdge != itNode->second->m_edges.end(); ++itEdge)
         {
            std::cout << "found edge to other node: " << itEdge->first << std::endl;
            // std::cin >> debug;
            

            if(isNodeVisited(itEdge->first))
            {
                  std::cout << "***node is already in solution, skipping" << std::endl;
                  continue;
            }

            std::cout << "now checking if this is the lowest cost: " << (itEdge->second) << std::endl;
            std::cout << "lowest_cost_edge_this_iteration: " << lowest_cost_edge_this_iteration << std::endl;

            // now check if its the lowest cost 
            if((itEdge->second) < lowest_cost_edge_this_iteration)
            {
               std::cout << "**new lowest code node found: Node: " << itEdge->first << " cost: " << itEdge->second << std::endl;
               lowest_cost_edge_this_iteration = itEdge->second;
               lowest_cost_node_this_iteration = itEdge->first;
            }
         }
      }

      std::cout << "\n==== adding node " << lowest_cost_node_this_iteration << " to solution" << " with a cost of "
                << lowest_cost_edge_this_iteration << std::endl;
      // std::cin >> debug;

      // add a newly found lowest node to the solution
      mst_for_graph[solution_points_found][NODENUM_IDX] = lowest_cost_node_this_iteration;
      mst_for_graph[solution_points_found][EDGEWEIGHT_IDX] = lowest_cost_edge_this_iteration;
      setNodeVisited(lowest_cost_node_this_iteration);

      solution_points_found++;

   }

   std::cout << "----------- MST path ---------------" << std::endl;

   unsigned int mst_total_cost = 0;

   for(i=0; i<solution_points_found; i++)
   {
      std::cout << "Node: " << mst_for_graph[i][NODENUM_IDX]  << "  \t"
               << " Cost: " << mst_for_graph[i][EDGEWEIGHT_IDX] <<std::endl;
      mst_total_cost += mst_for_graph[i][EDGEWEIGHT_IDX];
   }

   std::cout << "Total MST cost: " << mst_total_cost << std::endl;

}

void Graph::doDijkstra( unsigned int originNode, unsigned int destNode, std::list<unsigned int> *pathList, int &pathCost)
{
   bool validRouteFoundToDestination = false;
   unsigned int closedNodeNum = originNode;

   // std::cout << "---Running shortest path algorithm from node "<<originNode << " to node " << destNode << std::endl;

   // initialize the outcome
   pathCost = 0;
   pathList->clear();

   // special case for origin == destination, just return
   if(originNode == destNode)
   {
      return;
   }

   // before starting, clean the nodes of computed values in case we're re-running the algorythm
   for(std::map<int, graphPoint* >::iterator itGraphNode = graphNodes.begin(); itGraphNode != graphNodes.end(); ++itGraphNode)
   {
      itGraphNode->second->cleanNode();
   }

   //
   // run the shortest path algorithm on the graph passed in
   //
   makeOriginNode(originNode);

   std::vector<unsigned int> openSet;    // a set of all "not yet visited" Nodes


   while(true)
   {
      int numNodesAddedToOpenSet = 0;
         
      // std::cout << "New closed node: " << closedNodeNum << std::endl;
      //
      // add the connected nodes from this node to the open set (Step N+1)
      //
      std::map<int, graphPoint* >::iterator itGraphNode = graphNodes.find(closedNodeNum);

      unsigned int closedNodeCost = itGraphNode->second->m_totalCost;

      // std::cout << "The closed node cost is "<< closedNodeCost << std::endl;

      // get all the nodes connected to this one and 
      // adjust the costs and (from node) values of each connected node
      // itGraphEdge->first is the connected node number and itGraphEdge->second is the edge cost
      for(std::map<unsigned int, unsigned int>::iterator itGraphEdge= itGraphNode->second->m_edges.begin(); 
          itGraphEdge != itGraphNode->second->m_edges.end(); 
          ++itGraphEdge)
      {

         // first, mark this node as visited
         itGraphNode->second->setVisited();

         //         std::cout << "Examining node " << itGraphEdge->first << std::endl;

         std::map<int, graphPoint* >::iterator itNextEdgeNode = graphNodes.find(itGraphEdge->first);

         if(itNextEdgeNode == graphNodes.end()) continue;  // no node actually exists 

         int i;
         for(i=0; i<openSet.size(); i++)
         {
            if(openSet[i] == itNextEdgeNode->second->m_nodeNumber) break;
         }

          // add it to the open set if it's not there already and it hasn't already been visited
         if(i == openSet.size() && (itNextEdgeNode->second->getVisited() == false))
         {
            openSet.push_back(itNextEdgeNode->second->m_nodeNumber);
            numNodesAddedToOpenSet++;
          }

          // now see if this is a lower cost path to this connected node
          if(itNextEdgeNode->second->getPointCost() == (-1) || 
             (closedNodeCost + itGraphEdge->second) <  itNextEdgeNode->second->getPointCost())
          {

              // new lower cost found
             itNextEdgeNode->second->setPointCost(static_cast<int>(closedNodeCost + itGraphEdge->second));
             itNextEdgeNode->second->m_viaNode = closedNodeNum;


             // std::cout << "found new lower cost (" << itNextEdgeNode->second->getPointCost() 
             //           << ") to node " << itGraphEdge->first << " from node " 
             //           << closedNodeNum << std::endl;

          } 
       }

       // std::cout << "Openset now has " << openSet.size() << " members " << std::endl;

       //if we didn't add any new members to the openset and it's empty, we are done
       if ( (openSet.size() == 0) &&  (numNodesAddedToOpenSet == 0) )
       {
          //          std::cout << "openset is empty" << std::endl;
          break;
       }

       // Now, find the lowest cost member of the open set and make it the new closed set member

       unsigned int lowest_cost_node;
       unsigned int lowest_cost_index;

       for (int i=0; i<openSet.size(); i++)
       {
          int lowest_cost_val;

          std::map<int, graphPoint* >::iterator itGraphNode = graphNodes.find(openSet[i]);

          // std::cout << "i: " << i << " openset[i]: " << openSet[i] <<" nodenum: " <<  
          //    itGraphNode->second->m_nodeNumber << " cost: " << itGraphNode->second->m_totalCost
          //           << std::endl;

          if((i==0) || (itGraphNode->second->m_totalCost < lowest_cost_val ))
          {

             lowest_cost_node = itGraphNode->second->m_nodeNumber;
             lowest_cost_val = itGraphNode->second->m_totalCost;
             lowest_cost_index = i;
             // std::cout << "found new lowest cost node : " << lowest_cost_node << std::endl;
          }
       }


       // this is the node that we'll be evaluating in the next iteration
       closedNodeNum = lowest_cost_node;

       // remove that node from the open set
       // std::cout << "deleting node: " << lowest_cost_node << std::endl;

       for(std::vector<unsigned int>::iterator vit = openSet.begin(); vit != openSet.end(); ++vit)
       {

          // std::cout << "lowest_cost_node is: " << lowest_cost_node << " and found openSet member number " << *vit << std::endl;

          if (*vit == lowest_cost_node)
          {
             // std::cout << "erasing member!" << std::endl;
             openSet.erase(vit);
             break;
          }
       }

       // std::cout << "Post delete: Openset now has " << openSet.size() << " members " << std::endl;


       //if that node is the dest node, we have succeeded and we are done
       if (closedNodeNum == destNode)
       {
          validRouteFoundToDestination = true;
          // std::cout << "***Found a route to the destination node!" << std::endl;


          break;
       }

       //else keep on truckin'
    }

   //
   // Now tell the user whether or not we've been able to find a route
   //   If so, print out the route
   //   else, well..there's not much we can do except break the bad news
   //
    if(validRouteFoundToDestination == true)
    {
       std::vector<unsigned int> routeSet;

       // now print out the route

       unsigned int routeNode = destNode;
       unsigned int lastRouteNode = routeNode;

       // reuse the openSet vector to record the final route
       pathCost = 0;

       while(true)
       {
          pathList->push_front(routeNode);

          // if we've added the orig to the route record, then we're done
          if(routeNode == originNode) break;

          //         std::cout << "route reclaimation looking for node " << routeNode << std::endl;
          //         std::cout << "route reclaimation found node " << 
          //            graphNodes.find(routeNode)->second->m_nodeNumber << std::endl;

          // remember the "from" node for the cost computation
          lastRouteNode = routeNode;

          // find the "next" node
          routeNode = graphNodes.find(routeNode)->second->m_viaNode;

          //         std::cout << "route reclaimation found viaNode..." << routeNode << std::endl;

          // record this hop in the total cost of the path

          // std::cout << "Cost of route from " << routeNode << " to " << lastRouteNode << 
          //    " is " << getEdgeValue(routeNode, lastRouteNode) << std::endl;

          pathCost += getEdgeValue(routeNode, lastRouteNode);

          
       }
    }
    else
    {
       // std::cout << "Count not find a route from " << originNode << " to " << destNode << std::endl;
       pathList->clear();  // no elements in the list
       pathCost = -1;
    }

 }

ShortestPathAlgo::ShortestPathAlgo() : pathCost(-1)
{
   pathList = new std::list<unsigned int>;
} 

ShortestPathAlgo::~ShortestPathAlgo()
{
   delete pathList;
} 
 
// returns a count of the nodes
unsigned int ShortestPathAlgo::verticies(Graph &G)
{
   return G.getNodeCount();
}

// returns the cost of the path (or -1 if no path exists)
int ShortestPathAlgo::path_size( Graph &G, unsigned int originNode, unsigned int destNode )
{
   G.doDijkstra(originNode, destNode, pathList, pathCost);
   return pathCost;
}

// returns a list with the path
std::list<unsigned int> *ShortestPathAlgo::path( Graph &G, unsigned int originNode, unsigned int destNode)
{
   G.doDijkstra(originNode, destNode, pathList, pathCost);
   return pathList;
}

std::ostream &operator<< (std::ostream &cout, std::list<unsigned int> *path)
{
   unsigned routeLen = path->size();

   if(path->size())
   {
      cout << "==== Shortest Route has " << routeLen << " nodes" << std::endl;

      cout << "===== Shortest Route is : ";

      // There's a valid route, so iterate through the list and print each member...
      for(std::list<unsigned int>::iterator listIt = path->begin(); listIt != path->end(); ++listIt)
      {
         cout << *listIt << " ";
      }
   }
   else
   {
      std::cout << "No route found" << std::endl;
   }
      
   return cout;
}

//#define USING_KNOWN_GRAPH

 int main()
 {

#ifdef USING_KNOWN_GRAPH

    int originNode = 1;
    int destNode = 6;

    std::cout << "Using a known graph for testing" << std::endl;
    
    Graph G;

    G.addNode(1);
    G.addEdge(1, 2, 1);
    G.addEdge(1, 3, 3);

    G.addNode(2);
    G.addEdge(2, 4, 1);
    G.addEdge(2, 3, 1);

    G.addNode(3);
    G.addEdge(3, 1, 1);
    G.addEdge(3, 6, 4);
    G.addEdge(3, 5, 7);

    G.addNode(4);
    G.addEdge(4, 6, 5);

    G.addNode(5);

    G.addNode(6);


    G.printGraph();


#else

    /* initialize random seed: */
    srand (time(NULL));

    char print_graph_entry;

    Graph G(const_cast<char *>("graph.txt")); // a new graph G
    
    std::cout << "start prim" << std::endl;

    G.doPrim(0);

    // print it out but only if the user wants to take a look at it
    if(print_graph_entry == 'y' )
    {
       G.printGraph();
    }
    else
    {
       std::cout <<"Graph has " << G.getNodeCount() << " nodes and " << G.getEdgeCount() << " indicies" << std::endl;
    }

#endif

    return(1);

}


