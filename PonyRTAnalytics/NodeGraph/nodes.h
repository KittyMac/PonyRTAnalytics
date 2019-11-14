/**
 * nodes.h
 *
 * a class that represents a collection of nodes and the links that connect them
 */


#ifndef __NODES_H__
#define __NODES_H__
#include "node.h"

class nodes {

 public:

  // Constructor and destructor
  nodes();
  ~nodes();


  int number_of_nodes();           // returns the number of nodes we have
  node *get_node_at(int index);       // returns node index
  // returns 0 or the first node encountered whose name is test_name.
  node *get_node(unsigned long tag);
  // returns 0 or the first node encountered whose export_id is test_id.
  node *get_node_with_export_id(char *test_id);

  // checks to see if a node named name already exists.  adds it if not.
  node *add_node(unsigned long tag);
  // checks to see if a node with export_id id already exists. adds it if not.
  node *add_node_with_export_id(char *id);

  // clears the forces pending on each node
  void clear_pending_forces();

  // assign each node to a layer
  void assign_layers();
  int assign_layer(int layer_number);

  // Gathers nodes at the given layer around their parent nodes at the next
  // layer up
  void gather_nodes_with_layer(int which_layer);

  // (debug)
  void count_off();

 private:
  int num_of_nodes;
  std::vector<node *> my_nodes;
  int node_in_vector(node *this_node, std::vector<node *> these_nodes);
  bool all_connections_in_lower_layer(std::vector<node *> node_connections, 
                                      int layer_number);
  bool every_node_is_in_a_layer();
  int nodes_not_in_lower_layer(std::vector<node *> these_nodes, 
                               int layer_number);

};


#endif
