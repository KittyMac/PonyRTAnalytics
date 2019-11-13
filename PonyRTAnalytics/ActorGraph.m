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

@implementation ActorGraph

- (id) initWithEvents:(NSArray *) ponyEvents {
    self = [super init];
    if (self) {
        _actors = [[NSMutableDictionary alloc] init];
        
        for (PonyEvent * evt in ponyEvents) {
            [self registerActor:evt.actorTag];
        }
        
        [self layoutCircle];
    }
    return self;
}

- (Actor *) actorWithTag:(unsigned long) actorTag {
    return [_actors objectForKey:[NSNumber numberWithUnsignedInteger:actorTag]];
}

- (void) registerActor:(unsigned long) actorTag {
    Actor * actor = [self actorWithTag:actorTag];
    if (actor == NULL) {
        actor = [[Actor alloc] initWithTag:actorTag];
        [_actors setObject:actor forKey:[NSNumber numberWithUnsignedInteger:actorTag]];
    }
}

- (void) executeEvent:(PonyEvent *)event {
    
    Actor * actor = [self actorWithTag:event.actorTag];
    
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
    NSArray * keyArray = [_actors allKeys];
    [keyArray sortedArrayUsingSelector:@selector(compare:)];
    
    float deg = 0;
    float delta = (M_PI * 2) / [keyArray count];
    float base[2] = {0.0f, 0.8f};
    float point[2];
    for (NSNumber * tag in keyArray) {
        Actor * actor = [_actors objectForKey:tag];
        
        VEC_ROT_Z_2(point, base, deg);
        actor.x = point[0];
        actor.y = point[1];
        
        deg += delta;
    }
}

- (void) render {
    float radius = 0.05f;
    float oradius = 0.07f;
    
    glBegin(GL_TRIANGLES);
    
    for (Actor * actor in [_actors allValues]) {
        float x = actor.x;
        float y = actor.y;
        
        
        // if we're muted, we draw an angry red outline
        if (actor.muted) {
            glColor3ub(255, 48, 0);
            glVertex3f(x-oradius, y-oradius, 0.0f);
            glVertex3f(x+oradius, y-oradius, 0.0f);
            glVertex3f(x+0.0, y+oradius*0.8f, 0.0f);
        }
        
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
        
        glVertex3f(x-radius, y-radius, 0.0f);
        glVertex3f(x+radius, y-radius, 0.0f);
        glVertex3f(x+0.0, y+radius*0.8f, 0.0f);
    }
    
    glEnd();
}

@end
