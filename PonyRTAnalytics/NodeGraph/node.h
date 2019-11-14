/**
 * node.h
 *
 * a class that represents a node and the links to nodes it points to
 */


#ifndef __NODE_H__
#define __NODE_H__
#include <vector>

class node {

 public:

  // Constructors and destructor
  node(unsigned long tag);
  ~node();


  // Node functions
  void set_name(unsigned long tag);
  int name_is(unsigned long tag); // returns 1 if test_name is our name, 0 if not
  unsigned long get_name();
  void set_export_id(const char *new_export_id);
  const char *get_export_id();
  void set_layer(int new_layer);
  int get_layer();


  // Connection functions

  // adds destination_node to the list of nodes that we point to
  void add_connection(node *destination_node);  
  std::vector<node *> get_connections();

  // Attributes

  double x_pos, y_pos;
  double pending_x_force, pending_y_force;

  unsigned long name;
  int number_of_connections;
  char *export_id;
  int layer_number;

  std::vector<node *> connecting_nodes;
};


#endif
