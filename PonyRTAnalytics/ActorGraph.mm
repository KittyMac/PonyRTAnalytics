//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "ActorGraph.h"
#import "Actor.h"
#import "PonyEvent.h"
#import "Message.h"
#import <OpenGL/gl.h>
#import "vvector.h"
#import "NSImageAdditions.h"
#import "nodes.h"
#import "node.h"
#import "physics.h"
#import "NSStringTexture.h"

#define kPhysicsScale 100

@interface ActorGraph ()
{
    nodes * my_nodes;      // Our nodes and their connections
    physics * my_physics;  // Our physics module
}

@end

@implementation ActorGraph

- (id) initWithEvents:(NSArray *) ponyEvents {
    self = [super init];
    if (self) {
        // run through all of the events first and find the largest actor tag
        unsigned long maxActorUUID = 0;
        for (PonyEvent * evt in ponyEvents) {
            if (evt.actorUUID > maxActorUUID) {
                maxActorUUID = evt.actorUUID;
            }
            if (evt.toActorUUID > maxActorUUID) {
                maxActorUUID = evt.toActorUUID;
            }
        }
        
        // allocate an array large enough to hold all of them.  Now, I know this is wasteful of memory,
        // but the lookup speeds are as fast as they come and using a hashmap is too slow.
        actorLUT = (void **)calloc(sizeof(Actor *) * (maxActorUUID + 1), 1);
        _actors = [NSMutableArray array];
        
        // go back through an register actors
        for (PonyEvent * evt in ponyEvents) {
            [self registerActor:evt.actorUUID andTag:evt.actorTag];
            [self registerActor:evt.toActorUUID andTag:evt.actorTag];
        }
        
        messages = [[NSMutableArray alloc] init];
        
        node0 = [NSImage imageNamed:@"node0"];
        node1 = [NSImage imageNamed:@"node1"];
        node2 = [NSImage imageNamed:@"node2"];
        
        texture_node0 = [node0 texture];
        texture_node1 = [node1 texture];
        texture_node2 = [node2 texture];
        
        // initiate the node layouts
        my_nodes = new nodes();
        my_physics = new physics();

        my_physics->set_parameters(300, // new_node_mass
                                   0.001, // new_node_charge
                                   3, // new_spring_length
                                   5, // new_spring_constant
                                   0.1, // max_sa_movement
                                   1);
        
        //[self layoutCircle];
        //[self layoutRandom];
        [self layoutRandomX];
        
        
        // run through all actors and add nodes
        for (Actor * actor in _actors) {
            node * node = my_nodes->add_node(actor.uuid);
            node->x_pos = actor.x * kPhysicsScale;
            node->y_pos = actor.y * kPhysicsScale;
        }
        // run through all events add connections
        for (PonyEvent * evt in ponyEvents) {
            if (evt.eventID == ANALYTIC_APP_MESSAGE_SENT &&
                evt.actorUUID != evt.toActorUUID) {
                node * from_node = my_nodes->get_node(evt.actorUUID);
                node * to_node = my_nodes->get_node(evt.toActorUUID);
                if (from_node != NULL && to_node != NULL) {
                    from_node->add_connection(to_node);
                }
            }
        }
        
        my_physics->separate_nodes(my_nodes);
        for(int i = 0; i < 2000; i++) {
            my_physics->advance(my_nodes);
        }
        
        // run through all actor nodes and update their position to the latest
        _minX = 9999999.0f;
        _maxX = -9999999.0f;
        _minY = 9999999.0f;
        _maxY = -9999999.0f;
        for (Actor * actor in _actors) {
            node * actor_node = my_nodes->get_node(actor.uuid);
            actor.x = actor_node->x_pos / kPhysicsScale;
            actor.y = actor_node->y_pos / kPhysicsScale;
            
            if (actor.x < _minX) {
                _minX = actor.x;
            }
            if (actor.x > _maxX) {
                _maxX = actor.x;
            }
            if (actor.y < _minY) {
                _minY = actor.y;
            }
            if (actor.y > _maxY) {
                _maxY = actor.y;
            }
        }
        
    }
    return self;
}

- (void) registerActor:(unsigned long) actorUUID
                andTag:(unsigned long) actorTag {
    if (actorUUID > 0) {
        Actor * actor = (__bridge Actor *)actorLUT[actorUUID];
        if (actor == NULL) {
            Actor * actor = [[Actor alloc] initWithUUID:actorUUID];
            actor.tag = actorTag;
            [_actors addObject:actor];
            actorLUT[actorUUID] = (__bridge void *)actor;
        }
    }
}

- (void) executeEvent:(PonyEvent *)event
           isDragging:(BOOL)isDragging {
    
    totalMemoryUsage = event.totalMemoryUsage;
    
    Actor * actor = (__bridge Actor *)(actorLUT[event.actorUUID]);
    
    actor.tag = event.actorTag;
    actor.numMessages = event.actorNumMessages;
    actor.batchSize = event.actorBatchSize;
    actor.priority = event.actorPriority;
    actor.heapSize = event.actorHeapSize;
    
    if (event.toActorUUID != 0) {
        Actor * to = (__bridge Actor *)actorLUT[event.toActorUUID];
        to.numMessages = event.toActorNumberOfMessages;
    }
    
    switch(event.eventID) {
        case ANALYTIC_MUTE:
            actor.muted = true;
            break;
        case ANALYTIC_NOT_MUTE:
            actor.muted = false;
            break;
        case ANALYTIC_OVERLOADED:
            actor.overloaded = true;
            break;
        case ANALYTIC_NOT_OVERLOADED:
            actor.overloaded = false;
            break;
        case ANALYTIC_UNDERPRESSURE:
            actor.underpressure = true;
            break;
        case ANALYTIC_NOT_UNDERPRESSURE:
            actor.underpressure = false;
            break;
        case ANALYTIC_RUN_START:
            actor.running = true;
            break;
        case ANALYTIC_RUN_END:
            actor.running = false;
            break;
        case ANALYTIC_MESSAGE_SENT:
        case ANALYTIC_APP_MESSAGE_SENT:
            
            if (isDragging == false) {
                Actor * from = (__bridge Actor *)actorLUT[event.actorUUID];
                Actor * to = (__bridge Actor *)actorLUT[event.toActorUUID];
                if (from != NULL && to != NULL && from != to) {
                    
                    // Check the previous few messages added.  If there is one visually close
                    // enough to me save performance by skipping me
                    if ([messages count] > 256) {
                        unsigned long maxIdx = [messages count] - 1;
                        unsigned long minIdx = maxIdx - 256;

                        for (unsigned long i = maxIdx; i > minIdx; i--) {
                            Message * other = [messages objectAtIndex:i];
                            if ([other isNew] &&
                                other.from == from &&
                                other.to == to &&
                                other.isAppMessage == (event.eventID == ANALYTIC_APP_MESSAGE_SENT)) {
                                return;
                            }
                        }
                    }
                    
                    Message * msg = [[Message alloc] initWithEvent:event
                                                         fromActor:from
                                                           toActor:to];
                    [messages addObject:msg];
                }
            }
            break;
    }
}

- (void) layoutRandom {
    for (Actor * actor in _actors) {
        float fX = ((float)(arc4random() % 1000)) / 1000.0f;
        float fY = ((float)(arc4random() % 1000)) / 1000.0f;
        actor.x = -1.0f + 2.0f * fX;
        actor.y = -1.0f + 2.0f * fY;
    }
}

- (void) layoutRandomX {
    // try and force the graph to prefer wide graphs over tall graphs
    for (Actor * actor in _actors) {
        float fX = ((float)(arc4random() % 1000)) / 1000.0f;
        float fY = ((float)(arc4random() % 1000)) / 1000.0f;
        actor.x = -1.0f + 2.0f * fX;
        actor.y = -0.1f + 0.2f * fY;
    }
}

- (void) layoutCircle {
    // sort ascending order by tag, then layout in a simple equidistant circle
    [_actors sortedArrayUsingSelector:@selector(compare:)];
    
    float deg = 0;
    float delta = (M_PI * 2) / [_actors count];
    float base[2] = {0.0f, 0.8f};
    float point[2];
    for (Actor * actor in _actors) {
        VEC_ROT_Z_2(point, base, deg);
        actor.x = point[0];
        actor.y = point[1];
        
        deg += delta;
    }
}

- (void) reset {
    for(Actor * actor in _actors) {
        [actor reset];
    }
    
    [messages removeAllObjects];
}

- (BOOL) update:(float)delta {
    // If you think about it, adding still alive messages to an new array already sized to
    // fit everything will be infinitely more performant than removing items which are
    // dead from a mutable array.
    NSMutableArray * messagesCopy = [NSMutableArray arrayWithCapacity:[messages count]];
    for (Message * message in messages) {
        if( [message update:delta] == YES ){
            [messagesCopy addObject:message];
        }
    }
    messages = messagesCopy;
    return true;
}

- (void) render {
    static unsigned long frameNumber = 0;
    
    float size = 0.05f;
    float labelSize = 0.025f;
    
    BOOL shouldUpdateLabels = (frameNumber++ % 6) == 0;
    
    glDisable(GL_LIGHTING);
    
    // render connections
    glColor4f(0.4f, 0.4f, 0.4f, 0.3f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINES);
    for (int f = 0; f < my_nodes->number_of_nodes(); f++) {
        node * from = my_nodes->get_node_at(f);
        for (int t = 0; t < from->number_of_connections; t++) {
            node * to = from->get_connections()[t];
            glVertex2f(from->x_pos / kPhysicsScale, from->y_pos / kPhysicsScale);
            glVertex2f(to->x_pos / kPhysicsScale, to->y_pos / kPhysicsScale);
        }
    }
    glEnd();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_node0);
    
    // render messages
    glBegin(GL_QUADS);
    for (Message * message in messages) {
        [message renderQuad:size * 0.25f];
    }

    glColor3ub(255, 255, 255);
    for (Actor * actor in _actors) {
        [actor renderQuad:size];
    }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, texture_node1);
    glBegin(GL_QUADS);
    for (Actor * actor in _actors) {
        // if we're overloaded, we draw orange
        if(actor.running) {
            glColor3ub(164, 164, 255);
        } else {
            // if we're idle, we draw grey
            glColor3ub(164, 164, 164);
        }
        [actor renderQuad:size];
    }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, texture_node2);
    glBegin(GL_QUADS);
    for (Actor * actor in _actors) {
        if (actor.muted) {
            glColor3ub(255, 48, 0);
        } else {
            glColor3ub(255, 255, 255);
        }
        [actor renderQuad:size];
    }
    glEnd();
    
    
    unsigned long actorMemoryUsage = 0;
    
    unsigned long actorIdx = 0;
    for (Actor * actor in _actors) {
        // rendering labels is super inefficient; to combat this, only render a subset of lebels every frame
        [actor renderLabels:labelSize
               shouldUpdate:(actorIdx % 8) == (frameNumber % 8)];
        
        actorMemoryUsage += actor.heapSize;
        actorIdx += 1;
    }
    
    
    // render total stats (such as total memory usage)
    if (shouldUpdateLabels && (totalMemoryUsageCache != totalMemoryUsage || actorMemoryUsageCache != actorMemoryUsage)) {
        totalMemoryUsageCache = totalMemoryUsage;
        actorMemoryUsageCache = actorMemoryUsage;
        
        NSString * memoryStatsString = [NSString stringWithFormat:@"OS Memory: %0.2f MB\nActor Memory: %0.2f MB",
                                            ((double)totalMemoryUsage / (1024.0 * 1024.0)),
                                            ((double)actorMemoryUsage / (1024.0 * 1024.0))
                                        ];
        
        if (totalMemoryUsageTexture == 0) {
            totalMemoryUsageTexture = createStringTexture(memoryStatsString, &totalMemoryUsageSize);
        } else {
            totalMemoryUsageTexture = updateStringTexture(totalMemoryUsageTexture, memoryStatsString, &totalMemoryUsageSize);
        }
        
        glEnable(GL_TEXTURE_2D);
    }
    
    if (totalMemoryUsageTexture != 0) {
        float h = 0.08f;
        float w = totalMemoryUsageSize.width * (h / totalMemoryUsageSize.height);
        float x = _minX;
        float y = _maxY;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        
        glBindTexture(GL_TEXTURE_2D, totalMemoryUsageTexture);
        
        glBegin(GL_QUADS);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(x+w*0, y+h*1, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x+w*1, y+h*1, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x+w*1, y+h*2, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x+w*0, y+h*2, 0.0f);
        
        glEnd();
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
}

@end
