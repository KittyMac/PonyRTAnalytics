/**
 * physics.h
 *
 * a class to do the dirty work of moving nodes appropriately
 */

#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "nodes.h"

class physics {

 public:
    
  physics();
  ~physics();

  void set_parameters(float new_node_mass,  // Initializes our constants
		      float new_node_charge, 
		      int   new_spring_length, 
		      float new_spring_constant,
		      float new_max_sa_movement,
                      int   new_layers_to_hide);
  // Make sure no two nodes are at the same location
  void separate_nodes(nodes *my_nodes);
  void advance(nodes *my_nodes);  // Move the nodes in my_nodes


 private:
  // Coulomb's constant (8.9875 * 10^9 (Nm^2)/C^2)
    constexpr static const float COULOMBS_CONSTANT = 8987500000.0;

  float node_mass, node_charge, spring_constant;
  int spring_length;
  float max_sa_movement;
  int layers_to_hide;

  // Apply electrical force from other_node on this_node
  void apply_electrical_force(node *other_node, node *this_node,
			      float distance);

  // Apply the forces applied on two nodes by a spring connecting them.
  void apply_spring_force(node *other_node, node *this_node);

  // Effect the movement of all nodes based on all pending forces
  void move_nodes(nodes *my_nodes);  

  // Calculate the distance between two nodes
  float distance_between(node *node_one, node *node_two);

  // Calculate the x and y components of a directed force on this_node and
  // returns them in x and y
  void determine_electric_axal_forces(float *x, float *y, 
				      float directed_force, 
				      float distance,
				      node *other_node, node *this_node);

  // Calculate the x and y components of a directed force on this_node and 
  // returns them in x and y
  void determine_spring_axal_forces(float *x, float *y, 
				    float directed_force, 
				    float distance,
				    int   spring_constant,
				    node *other_node, node *this_node);

};

#endif
