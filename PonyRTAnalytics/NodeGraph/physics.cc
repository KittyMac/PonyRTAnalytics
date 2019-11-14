#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "physics.h"


physics::physics() {
   node_mass = node_charge = 0;
   spring_length = 0;
   spring_constant = 0;
   max_sa_movement = 0;
   layers_to_hide = 0;
}

physics::~physics() {
}

void physics::set_parameters(float new_node_mass,
			     float new_node_charge, 
			     int   new_spring_length, 
			     float new_spring_constant,
			     float new_max_sa_movement,
                             int   new_layers_to_hide) {
   node_mass       = new_node_mass;
   node_charge     = new_node_charge;
   spring_length   = new_spring_length;
   spring_constant = new_spring_constant;
   max_sa_movement = new_max_sa_movement;
   layers_to_hide  = new_layers_to_hide;
}


void physics::separate_nodes(nodes *my_nodes) {
   node *this_node;
   node *other_node;
   char *message;
   int clear;
   int counter = 0;
   int counter2;

   while (counter < my_nodes->number_of_nodes()) {
      this_node = my_nodes->get_node_at(counter);
      counter2 = counter + 1;
      clear = 1;
      while ((counter2 < my_nodes->number_of_nodes()) && (clear == 1)) {
         // Only if this is not the same node...
         if (counter2 != counter) {
            other_node = my_nodes->get_node_at(counter2);
            if ((this_node->x_pos == other_node->x_pos) &&
                (this_node->y_pos == other_node->y_pos)) {
               printf("Separating cospacial nodes %d and %d (of %d) at (%f, %f)\n",
                      counter, counter2, my_nodes->number_of_nodes(), 
                      this_node->x_pos, this_node->y_pos);
               message = new char[75];
               snprintf(message, 75,
                        "Separating cospacial nodes %d and %d (of %d) at (%f, %f)\n",
                        counter, counter2, my_nodes->number_of_nodes(), 
                        this_node->x_pos, this_node->y_pos);
               delete message;
               // Move one of the nodes a small random amount
               //               this_node->x_pos += 1;
                 	  this_node->x_pos += ((float) random()) / RAND_MAX;
                 	  this_node->y_pos += ((float) random()) / RAND_MAX;
               // drop out of the inner loop and reset the outer loop
               clear = 0;
               counter = 0;
            }
            else 
               counter2++;
         }
         else
            counter2++;
      }
      if (clear == 1)
         counter++;
   }
}


void physics::advance(nodes *my_nodes) {
   node *this_node;
   node *other_node;

   // Clear pending forces on all nodes
   my_nodes->clear_pending_forces();

   // Set a flag to calculate (or not) the electrical forces that the nodes
   // apply on each other based on if both node types' charges are zero.
   bool apply_electric_charges = true;
   if (node_charge == 0.0)
      apply_electric_charges = false;

   // Iterate through all nodes
   int number_of_nodes = my_nodes->number_of_nodes();
   float distance;
   for (int counter = 0; counter < number_of_nodes; counter++) {
      this_node = my_nodes->get_node_at(counter);
      // only determine forces on this node if it isn't in a layer we should
      // hide
      if (this_node->get_layer() > layers_to_hide) {
         if (apply_electric_charges) {
            // Apply electrical force applied by all other nodes
            for (int counter2 = counter + 1; 
                 counter2 < number_of_nodes; 
                 counter2++) {
               other_node = my_nodes->get_node_at(counter2);
               // only proceed if the other node isn't in a layer we should 
               // hide
               if (other_node->get_layer() > layers_to_hide) {
                  distance = distance_between(this_node, other_node);
                  // let's protect ourselves from division by zero by ignoring
                  // two nodes that happen to be in the same place.  Since we 
                  // separate all nodes before we work on any of them, this 
                  // will only happen in extremely rare circumstances, and when
                  // it does, springs will probably pull them apart anyway.
                  // also, if we are more than 50 away, the electric force 
                  // will be negligable.  
                  // ***** may not always be desirable ****
                  if ((distance != 0.0) && (distance < 500.0)) {
                     //	  if (distance != 0.0) {
                     // Apply electrical force from node(counter2) on 
                     // node(counter)
                     apply_electrical_force(other_node, this_node, distance);
                  }
               }
            }
         }
         
         // Apply force from springs
         for (int counter2 = 0; 
              counter2 < this_node->number_of_connections; 
              counter2++) {
            other_node = this_node->connecting_nodes[counter2];
            // only proceed if the other node isn't in a layer we should hide
            if (other_node->get_layer() > layers_to_hide) {
               // Apply spring force on both nodes
               apply_spring_force(other_node, this_node);
            }
         }
      }
   }

   // Effect the movement of the nodes based on all pending forces
   move_nodes(my_nodes);
}


void physics::apply_electrical_force(node *other_node, node *this_node,
				     float distance) {
   // determined using Coulomb's Law:
   //   force = k((q1*q2)/r^2)
   // where:
   //   force is in newtons
   //   k = Coulomb's constant
   //   q1 and q2 are the two electrical charges in coulombs, and
   //   r is the distance between the charges in meters

   float directed_force = COULOMBS_CONSTANT * 
      ((node_charge * node_charge)/(distance * distance));

   float x_force, y_force;
   determine_electric_axal_forces(&x_force, &y_force, 
                                  directed_force, distance, 
                                  other_node, this_node);

   this_node->pending_x_force += x_force;
   this_node->pending_y_force += y_force;
   other_node->pending_x_force -= x_force;
   other_node->pending_y_force -= y_force;
}


void physics::apply_spring_force(node *other_node, node *this_node) {
   // determined using Hooke's Law:
   //   force = -kx
   // where:
   //   k = spring constant
   //   x = displacement from ideal length in meters

   float distance = distance_between(other_node, this_node);
   // let's protect ourselves from division by zero by ignoring two nodes that
   // happen to be in the same place.  Since we separate all nodes before we
   // work on any of them, this will only happen in extremely rare circumstances,
   // and when it does, electrical force will probably push one or both of them
   // one way or another anyway.
   if (distance == 0.0) 
      return;

   float displacement = distance - spring_length;
   if (displacement < 0) 
      displacement = -displacement;
   float directed_force = -1 * spring_constant * displacement;
   // remember, this is force directed away from the spring;
   // a negative number is back towards the spring (or, in our case, back towards
   // the other node)

  // get the force that should be applied to >this< node
   float x_force, y_force;
   determine_spring_axal_forces(&x_force, &y_force, 
                                directed_force, distance, spring_length,
                                other_node, this_node);

   this_node->pending_x_force += x_force;
   this_node->pending_y_force += y_force;
   other_node->pending_x_force -= x_force;
   other_node->pending_y_force -= y_force;
}


void physics::move_nodes(nodes *my_nodes) {
   // Since each iteration is isolated, time is constant at 1.
   // Therefore:
   //   Force effects acceleration.
   //   acceleration (d(velocity)/time) = velocity
   //   velocity (d(displacement)/time) = displacement
   //   displacement = acceleration
  
  // determined using Newton's second law:
  //   sum(F) = ma
  // therefore:
  //   acceleration = force / mass
  //   velocity     = force / mass
  //   displacement = force / mass

   node *this_node;

   for (int counter = 0; counter < my_nodes->number_of_nodes(); counter++) {
      this_node = my_nodes->get_node_at(counter);

      //    printf("-----------------------------------------------\n");
      //    printf("before node position: (%f, %f)\n", this_node->x_pos, this_node->y_pos);
      //    printf("before node force: (%f, %f)\n", this_node->pending_x_force, this_node->pending_y_force);
      float x_movement = this_node->pending_x_force / node_mass;
      if (x_movement > max_sa_movement)
         x_movement = max_sa_movement;
      else if (x_movement < -max_sa_movement)
         x_movement = -max_sa_movement;

      float y_movement = this_node->pending_y_force / node_mass;
      if (y_movement > max_sa_movement)
         y_movement = max_sa_movement;
      else if (y_movement < -max_sa_movement)
         y_movement = -max_sa_movement;


      this_node->x_pos += x_movement;
      this_node->y_pos += y_movement;

      //    printf("after node position: (%f, %f)\n", this_node->x_pos, this_node->y_pos);
   }
}


float physics::distance_between(node *node_one, node *node_two) {
   // distance = |sqrt((x1 - x2)^2 + (y1 - y2)^2)|

   float x_difference = node_one->x_pos - node_two->x_pos;
   float y_difference = node_one->y_pos - node_two->y_pos;
  
   float distance =sqrt((x_difference * x_difference) + 
			(y_difference * y_difference));
   if (distance < 0)
      distance = -distance;

   return distance;
}


void physics::determine_electric_axal_forces(float *x, float *y, 
					     float directed_force, 
					     float distance,
					     node *other_node, 
					     node *this_node) {
   // We know what the directed force is.  We now need to translate it
   // into the appropriate x and y componenets.
   // First, assume: 
   //                 other_node
   //                    /|
   //  directed_force  /  |
   //                /    | y
   //              /______|
   //    this_node     x         
   //
   // other_node.x > this_node.x
   // other_node.y > this_node.y
   // the force will be on this_node away from other_node

  // the proportion (distance/y_distance) is equal to the proportion
  // (directed_force/y_force), as the two triangles are similar.
  // therefore, the magnitude of y_force = (directed_force*y_distance)/distance
  // the sign of y_force is negative, away from other_node

   float y_distance = other_node->y_pos - this_node->y_pos;
   if (y_distance < 0)
      y_distance = -y_distance;
   *y = -1 * ((directed_force * y_distance) / distance);

   // the x component works in exactly the same way.
   float x_distance = other_node->x_pos - this_node->x_pos;
   if (x_distance < 0)
      x_distance = -x_distance;
   *x = -1 * ((directed_force * x_distance) / distance);

  
   // Now we need to reverse the polarity of our answers based on the falsness
   // of our assumptions.
   if (other_node->x_pos < this_node->x_pos)
      *x = *x * -1;
   if (other_node->y_pos < this_node->y_pos)
      *y = *y * -1;
  
   //  printf("/////////////////////////////////////////\n");
   //  printf("electric forces:\n");
   //  printf("distance: %f\n", distance);
   //  printf("directed: %f\n", directed_force);
   //  printf("axal: (%f, %f)\n", *x, *y);
}


void physics::determine_spring_axal_forces(float *x, float *y, 
					   float directed_force, 
					   float distance, 
					   int   spring_length,
					   node *other_node, node *this_node) {
   // if the spring is just the right size, the forces will be 0, so we can
   // skip the computation.
   //
   // if the spring is too long, our forces will be identical to those computed
   // by determine_electrical_axal_forces() (this_node will be pulled toward
   // other_node).
   //
   // if the spring is too short, our forces will be the opposite of those
   // computed by determine_electrical_axal_forces() (this_node will be pushed
   // away from other_node)
   //
   // finally, since both nodes are movable, only one-half of the total force
   // should be applied to each node, so half the forces for our answer.
  
   if (distance == spring_length) {
      *x = 0.0;
      *y = 0.0;
   }
   else {
      determine_electric_axal_forces(x, y, directed_force, distance, 
				     other_node, this_node);
      if (distance < spring_length) {
         *x = -1 * *x;
         *y = -1 * *y;
      }
      *x = 0.5 * *x;
      *y = 0.5 * *y;
   }

   //  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  \n");
   //  printf("spring forces:\n");
   //  printf("directed: %f\n", directed_force);
   //  printf("axal: (%f, %f)\n", *x, *y);
}
