#include <stdio.h>
#include "nodes.h"


nodes::nodes() {
  num_of_nodes = 0;
}

nodes::~nodes() {
}

int nodes::number_of_nodes() {
  return num_of_nodes;
}

node *nodes::get_node_at(int index) {
  return my_nodes[index];
}

node *nodes::add_node(unsigned long name) {
  node *temp = get_node(name);
  if (temp == 0) {
    node *new_node = new node(name);
    my_nodes.push_back(new_node);
    num_of_nodes++;
    return new_node;
  }
  else
    return temp;
}

node *nodes::add_node_with_export_id(char *id) {
  node *temp = get_node_with_export_id(id);
  if (temp == 0) {
    node *new_node = new node(-1);
    new_node->set_export_id(id);
    my_nodes.push_back(new_node);
    num_of_nodes++;
    return new_node;
  }
  else
    return temp;
}

void nodes::clear_pending_forces() {
  for (int x = 0; x < my_nodes.size(); x++) {
    my_nodes[x]->pending_x_force = 0.0;
    my_nodes[x]->pending_y_force = 0.0;
  }
}

node *nodes::get_node(unsigned long tag) {
  for (int x = 0; x < my_nodes.size(); x++)
    if (my_nodes[x]->name_is(tag) == 1)
      return my_nodes[x];
  return 0;
}

node *nodes::get_node_with_export_id(char *test_id) {
   for (int x = 0; x < my_nodes.size(); x++) 
      if (strcmp(my_nodes[x]->get_export_id(), test_id) == 0)
         return my_nodes[x];
   return 0;
}

int nodes::node_in_vector(node *this_node, std::vector<node *> these_nodes) { 
  for (int x = 0; x < these_nodes.size(); x++)
    if (these_nodes[x] == this_node)
      return 1;
  return 0;
}

void nodes::assign_layers() {
   for (int i = 1; assign_layer(i) > 0; i++);
}

int nodes::assign_layer(int layer_number) {
   // implement this algorithm:
   // counter = 0
   // for each node 
   //   if this node is not in a layer lower than the current
   //     if this node has no nodes it points to
   //        or
   //     if every node that this node points to is in a lower layer than the 
   //              current layer
   //       put it in the current layer
   //       increment counter
   //
   //   counter2 = 1
   //   while the counter is 0
   //     and
   //   while every node is not in a layer
   //     for each node
   //       if this node is not in a layer lower than the current
   //         if this node points to only counter2 other nodes that are not 
   //                 in a lower layer than the current
   //           put it in the current layer
   //           increment counter
   //     increment counter2
   //
   // return counter

   int nodes_assigned_to_this_layer = 0;
   for (int x = 0; x < my_nodes.size(); x++)
      if (my_nodes[x]->get_layer() >= layer_number)
         if (my_nodes[x]->get_connections().size() == 0 ||
             all_connections_in_lower_layer(my_nodes[x]->get_connections(), 
                                            layer_number)) {
            my_nodes[x]->set_layer(layer_number);
            nodes_assigned_to_this_layer++;
         }

   int connecting_nodes_threshold = 1;
   while (nodes_assigned_to_this_layer == 0 && ! every_node_is_in_a_layer()) {
      for (int x = 0; x < my_nodes.size(); x++)
         if (my_nodes[x]->get_layer() >= layer_number)
            if (nodes_not_in_lower_layer(my_nodes[x]->get_connections(), 
                                         layer_number) == 
                connecting_nodes_threshold) {
               my_nodes[x]->set_layer(layer_number);
               nodes_assigned_to_this_layer++;
            }
      connecting_nodes_threshold++;
   }

   return nodes_assigned_to_this_layer;
}

bool nodes::all_connections_in_lower_layer
(std::vector<node *> node_connections, int layer_number) {
   for (int x = 0; x < node_connections.size(); x++)
      if (node_connections[x]->get_layer() > layer_number)
         return false;
   return true;
}

bool nodes::every_node_is_in_a_layer() {
   for (int x = 0; x < my_nodes.size(); x++)
      if (my_nodes[x]->get_layer() == 999999)
         return false;
   return true;
}

int nodes::nodes_not_in_lower_layer(std::vector<node *> these_nodes, 
                                    int layer_number) {
   int returnValue = 0;
   for (int x = 0; x < these_nodes.size(); x++)
      if (these_nodes[x]->get_layer() >= layer_number)
         returnValue++;
   return returnValue;
}

void nodes::gather_nodes_with_layer(int which_layer) {
   for (int x = 0; x < my_nodes.size(); x++)
      if (my_nodes[x]->get_layer() > which_layer + 1)
         for (int y = 0; y < my_nodes[x]->get_connections().size(); y++) {
            node *temp = my_nodes[x]->get_connections()[y];
            if (temp->get_layer() == which_layer + 1) {
               temp->x_pos = 
                  my_nodes[x]->x_pos - 0.5 + ((float) random()) / RAND_MAX;
               temp->y_pos =
                  my_nodes[x]->y_pos - 0.5 + ((float) random()) / RAND_MAX;
            }
         }
}

void nodes::count_off() {
   printf("-----------COUNTOFF-------\n");
   for (int x = 0; x < my_nodes.size(); x++) {
      printf("node %d: %lu (%s) (%0.2f, %0.2f) %d conns\n",
             my_nodes[x], my_nodes[x]->get_name(),
             my_nodes[x]->get_export_id(), my_nodes[x]->x_pos,
             my_nodes[x]->y_pos, my_nodes[x]->number_of_connections);
      std::vector<node *> bobs = my_nodes[x]->get_connections();
      for (int y = 0; y < bobs.size(); y++)
         printf("     link to node %d\n", bobs[y]);
   }
}


