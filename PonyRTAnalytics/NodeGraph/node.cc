#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


node::node(unsigned long tag) {
  name = tag;
  export_id = 0;
  x_pos = y_pos = 0;
  pending_x_force = pending_y_force = 0.0;
  number_of_connections = 0;
  layer_number = 999999;
}

node::~node() {
    
}

void node::set_name(unsigned long tag) {
  name = tag;
}

int node::name_is(unsigned long tag) {
  if (name == tag)
    return 1;
  else
    return 0;
}

unsigned long node::get_name() {
   return name;
}

void node::set_export_id(const char *new_export_id) {
   if (export_id != 0)
      free(export_id);
   export_id = strdup(new_export_id);
}

const char *node::get_export_id() {
   return export_id;
}

std::vector<node *> node::get_connections() {
   return connecting_nodes;
}

void node::add_connection(node *destination_node) {
  node *new_node_pointer = destination_node;
  // don't add it if it's already there...
  for (int i = 0; i < connecting_nodes.size(); i++) {
     if (new_node_pointer == connecting_nodes[i]) {
        return;
     }
  }
  connecting_nodes.push_back(new_node_pointer);
  number_of_connections++;
}

void node::set_layer(int new_layer) {
   layer_number = new_layer;
}

int node::get_layer() {
   return layer_number;
}
