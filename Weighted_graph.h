
/*****************************************
 * Instructions
 *  - Replace 'uwuserid' with your uWaterloo User ID
 *  - Select the current calendar term and enter the year
 *  - List students with whom you had discussions and who helped you
 *
 * uWaterloo User ID:  smawlad@uwaterloo.ca
 * Submitted for ECE 250
 * Department of Electrical and Computer Engineering
 * University of Waterloo
 * Calender Term of Submission:  (Fall) 2017
 *
 * By submitting this file, I affirm that
 * I am the author of all modifications to
 * the provided code.
 *
 * The following is a list of uWaterloo User IDs of those students
 * I had discussions with in preparing this project:
 *    -
 *
 * The following is a list of uWaterloo User IDs of those students
 * who helped me with this project (describe their help; e.g., debugging):
 *    -
 *****************************************/

#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#ifndef nullptr
#define nullptr 0
#endif

#include <iostream>
#include <limits>
#include <list>
#include <vector>
#include <utility>
#include "Exception.h"
#include "Disjoint_sets.h"

using namespace std;

struct Weighted_edge {
	int vertex_1;
	int vertex_2;
	double weight;
};

struct adjList {
	int data;
	list<Weighted_edge>* edgeList;

};

class Weighted_graph {
	private:
		static const double INF;
		int num_nodes;
		int num_edges;

		// Do not implement these functions!
		// By making these private and not implementing them, any attempt
		// to make copies or assignments will result in errors
		Weighted_graph( Weighted_graph const & );
		Weighted_graph &operator=( Weighted_graph );

		// your choice

	public:
		Weighted_graph( int = 10 );
		~Weighted_graph();

		adjList* graphNodes;

		int degree( int ) const;
		int edge_count() const;
		std::pair<double, int> minimum_spanning_tree() const;

		bool insert_edge( int, int, double );
		bool erase_edge( int, int );
		void clear_edges();

		bool check_edge( int, int ) const;
		double get_weight( int , int ) const;

	// Friends

	friend std::ostream &operator<<( std::ostream &, Weighted_graph const & );
};

const double Weighted_graph::INF = std::numeric_limits<double>::infinity();

Weighted_graph::Weighted_graph(int n ) {
	if (n < 0) {
		throw illegal_argument();
	}

	// initialize the adjacency list

	graphNodes = new adjList[n];

	for (int i = 0; i < n; i++) {
		graphNodes[i].data = i;
		graphNodes[i].edgeList = new list<Weighted_edge>;
	}

	num_nodes = n;
	num_edges = 0;


}

Weighted_graph::~Weighted_graph() {
	for (int i = 0; i < num_nodes; i++) {
		delete graphNodes[i].edgeList;
	}

	delete[] graphNodes;

}

/* Helper methods */
// these are used in the MST function

bool Weighted_graph::check_edge(int i, int j) const {
	std::list<Weighted_edge>::iterator it;

	for(it = graphNodes[i].edgeList->begin(); it != graphNodes[i].edgeList->end(); it++) {
		 if((it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i )) {
		 	return true;
		 }
	}

	return false;


}

double Weighted_graph::get_weight(int i, int j) const {
	std::list<Weighted_edge>::iterator it;
	
	for(it = graphNodes[i].edgeList->begin(); it != graphNodes[i].edgeList->end(); it++) {
		 if((it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i )) {
		 	return it->weight;
		 }
	}

	return -1;


}

/* Accessors */

int Weighted_graph::degree(int u) const {
	if ( u > num_nodes - 1 || u < 0) {
		throw illegal_argument();
	}

	return graphNodes[u].edgeList->size();

}

int Weighted_graph::edge_count() const {
	return num_edges;
}


std::pair<double, int> Weighted_graph::minimum_spanning_tree() const {
	double total_weight = 0.0;
	int edge_counter = 0;

	if ( num_edges == 0 ) {
		return std::pair<double, int>( total_weight, edge_counter );
	}

	// intialize disjoint set
	Disjoint_set* disjointSet = new Disjoint_set(num_nodes);

	Weighted_edge edges[num_edges];

	// make the array of edges
	int e = 0;
	for(int i = 0; i < num_nodes; i++){
		for(int j = 0; j < i; j++){
			if( check_edge(i, j) ){
				edges[e].vertex_1 = i;
				edges[e].vertex_2 = j;
				edges[e].weight = get_weight(i, j);
				e++;
			}
		}
	}
	
	// use bubble sort (!) for sorting the array of edges 
	bool swapped = true;
		int m = 0;
		Weighted_edge tmp;
		while (swapped) {
		    swapped = false;
		    m++;
	        for (int j = 0; j < num_edges - m; j++) {

				if (edges[j].weight > edges[j+1].weight) {
					Weighted_edge temp = edges[j];
					edges[j] = edges[j+1];
					edges[j+1] = temp;
					swapped = true;
				}
	        }
      }

    // if set consisting the first vertex doesnt overlap with the set containing the second vertex, join them together using union
	Weighted_edge current_edge;
	for(int i = 0; i < num_edges; i++){
		current_edge = edges[i];
		if(disjointSet->find_set(current_edge.vertex_1) != disjointSet->find_set(current_edge.vertex_2)){		
			disjointSet->union_sets(current_edge.vertex_1,current_edge.vertex_2);
			total_weight += current_edge.weight;
		}		
		edge_counter++;	
		if(disjointSet->num_sets() == 1){
			break;
		}
	}	
	delete disjointSet;
	


	return std::pair<double, int>( total_weight, edge_counter );
	
}

/* Mutators */



bool Weighted_graph::erase_edge(int i, int j) {

	if ( i < 0 || i > num_nodes - 1 || j < 0 || j > num_nodes - 1 ) {
		throw illegal_argument();
	}

	if ( i == j ) {
		return true;
	}

	bool isDeleted = false;

	std::list<Weighted_edge>::iterator it;
	std::list<Weighted_edge>::iterator toBeErased;

	// if the vertex i and j are found (or j and i) initialize the edge to be erased
	// this is for adjacency list starting with j
    for(it = graphNodes[j].edgeList->begin(); it != graphNodes[j].edgeList->end(); it++){
        if((it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i )){
            toBeErased = it;
            isDeleted = true;
        }
    }

    // erase the edge that was found in the above loop
    // repeat the previous loop for adjacency list starting with i
    if(isDeleted){
    	graphNodes[j].edgeList->erase(toBeErased);

        for(it = graphNodes[i].edgeList->begin(); it != graphNodes[i].edgeList->end(); it++){
            if((it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i )){
                toBeErased = it;
            }
        }

       graphNodes[i].edgeList->erase(toBeErased);
       num_edges--;
    }

    return isDeleted;	
	
}

void Weighted_graph::clear_edges() {
	for (int i = 0; i < num_nodes; i++) {
		graphNodes[i].edgeList->clear();
	}

	num_edges = 0;
	return;
}

bool Weighted_graph::insert_edge( int i, int j, double d ) {
	if ( i < 0 || i > num_nodes - 1 || j < 0 || j > num_nodes - 1 || d <= 0 ) {
		throw illegal_argument();
	}

	if ( i == j ) {
		return false;
	}

	bool isPresent = false;

	// if vertices already exist in the graph, but dont form an edge i-j, find them, and set isPresent to true
	// also initialize weight
	list<Weighted_edge>::iterator it;
	for ( it = graphNodes[i].edgeList->begin(); it != graphNodes[i].edgeList->end(); it++) {
		if ( (it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i ) ) {
			it->weight = d;
			isPresent = true;
		}
	}

	// if vertices already exist in the graph, but dont form an edge j-i, find them, and set isPresent to true
	// also initialize weight
	for ( it = graphNodes[j].edgeList->begin(); it != graphNodes[j].edgeList->end(); it++) {
		if ( (it->vertex_1 == i && it->vertex_2 == j ) || (it->vertex_1 == j && it->vertex_2 == i ) ) {
			it->weight = d;
			isPresent = true;
		}
	}

	// if vertices dont exist in graph, create a new edge
	if ( !isPresent ) {
		Weighted_edge edge;
		edge.vertex_1 = i;
		edge.vertex_2 = j;
		edge.weight  = d;
		graphNodes[i].edgeList->push_back(edge);
		graphNodes[j].edgeList->push_back(edge);

		num_edges++;

	}

	return true;


}


std::ostream &operator<<( std::ostream &out, Weighted_graph const &graph ) {
	

	return out;
}


#endif