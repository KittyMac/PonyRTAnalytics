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
#import <OpenGL/gl.h>
#import "vvector.h"
#import "NSImageAdditions.h"

@implementation ActorGraph

- (id) initWithEvents:(NSArray *) ponyEvents {
    self = [super init];
    if (self) {
        // run through all of the events first and find the largest actor tag
        unsigned long maxActorTag = 0;
        for (PonyEvent * evt in ponyEvents) {
            if (evt.actorTag > maxActorTag) {
                maxActorTag = evt.actorTag;
            }
        }
        
        // allocate an array large enough to hold all of them.  Now, I know this is wasteful of memory,
        // but the lookup speeds are as fast as they come and using a hashmap is too slow.
        actorLUT = calloc(sizeof(Actor *) * (maxActorTag + 1), 1);
        _actors = [NSMutableArray array];
        
        // go back through an register actors
        for (PonyEvent * evt in ponyEvents) {
            [self registerActor:evt.actorTag];
        }
        
        [self layoutCircle];
        
        node0 = [NSImage imageNamed:@"node0"];
        node1 = [NSImage imageNamed:@"node1"];
        node2 = [NSImage imageNamed:@"node2"];
        
        texture_node0 = [node0 texture];
        texture_node1 = [node1 texture];
        texture_node2 = [node2 texture];
        
    }
    return self;
}

- (void) registerActor:(unsigned long) actorTag {
    Actor * actor = (__bridge Actor *)actorLUT[actorTag];
    if (actor == NULL) {
        Actor * actor = [[Actor alloc] initWithTag:actorTag];
        [_actors addObject:actor];
        actorLUT[actorTag] = (__bridge void *)actor;
    }
}

- (void) executeEvent:(PonyEvent *)event {
    
    Actor * actor = (__bridge Actor *)(actorLUT[event.actorTag]);
    
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
    }
}

/*
 ANALYTIC_MUTE = 1,
 ANALYTIC_NOT_MUTE = 2,
 ANALYTIC_OVERLOADED = 3,
 ANALYTIC_NOT_OVERLOADED = 4,
 ANALYTIC_UNDERPRESSURE = 5,
 ANALYTIC_NOT_UNDERPRESSURE = 6,
 ANALYTIC_RUN_START = 7,
 ANALYTIC_RUN_END = 8,
 ANALYTIC_PRIORITY_RESCHEDULE = 9,
 ANALYTIC_MESSAGE_SENT = 10,
*/

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

- (void) render {
    float size = 0.05f;
    
    glDisable(GL_LIGHTING);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_node0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glBegin(GL_QUADS);
    glColor3ub(255, 255, 255);
    for (Actor * actor in _actors) {
        [actor renderQuad:size];
    }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, texture_node1);
    glBegin(GL_QUADS);
    for (Actor * actor in _actors) {
        // if we're overloaded, we draw orange
        if (actor.overloaded) {
            glColor3ub(255, 126, 0);
        } else if(actor.underpressure) {
            glColor3ub(255, 220, 0);
        } else if(actor.running) {
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
    
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
    
}

@end
