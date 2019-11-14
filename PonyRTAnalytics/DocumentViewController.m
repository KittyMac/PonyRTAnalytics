//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "DocumentViewController.h"
#import <OpenGL/gl.h>
#import "vvector.h"
#import "PonyEvent.h"
#import "ActorGraph.h"

// Phew, we need to:
// 1. parse all of the events out of the .ponyrt_analytics file
// 2. generate a graph of nodes, each node representing actor tagged in the events file
// 3. play back the events of the analytics file
// 4. display pertinent information for each actor, such as
//    - message queue size
//    - batch size
//    - priority
//    - muted/unmuted
//    - overloaded/nominal
//    - sending messages to someone

@implementation DocumentViewController

- (void)viewDidLoad {
    [glview setDelegate:self];
}

- (void) parsePonyEvents:(NSArray *)eventsAsStrings {
    
    _ponyEvents = [NSMutableArray array];
    for (NSString * csvLine in eventsAsStrings) {
        PonyEvent * event = [[PonyEvent alloc] initWithCSV:csvLine];
        if ([event actorTag] > 0) {
            [_ponyEvents addObject:event];
        }
        
        if (event.time > endTime) {
            endTime = event.time;
        }
    }
    
    ponyEventIdx = 0;
    currentTime = 0;
    startTime = 0;
    
    NSLog(@"Loaded %lu ponyrt events\n", (unsigned long)[_ponyEvents count]);
}

- (void) Init {
    
    _graph = [[ActorGraph alloc] initWithEvents:_ponyEvents];
    
}

- (void) Destruct {
    
}

- (void) Render {
    
    float aspect = glview.frame.size.width / glview.frame.size.height;
    
    glViewport(0, 0, glview.frame.size.width, glview.frame.size.height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    glOrtho(-1, 1, -1, 1, -1, 1);
    glClearColor (248.0f / 255.0f, 253.0f / 255.0f, 255.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render the timeline in the background
    float timeOffset = (currentTime / (endTime - startTime)) * 2.0f;
    glColor3f (248.0f / 285.0f, 253.0f / 285.0f, 255.0f / 285.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(-1.0f+timeOffset, -1.0f, 0.0f);
    glVertex3f(-1.0f+timeOffset, -0.95f, 0.0f);
    glVertex3f(-1.0f, -0.95f, 0.0f);
    glEnd();
    
    // find the extents of the graph and make sure we can see the whole thing...
    float padding = 0.2f;
    float minX = [_graph minX] - padding;
    float maxX = [_graph maxX] + padding;
    float minY = [_graph minY] - padding;
    float maxY = [_graph maxY] + padding;
    
    float graphAspect = (maxX - minX) / (maxY - minY);
    
    float combinedAspect = (aspect / graphAspect);
    
    if (combinedAspect > 1.0) {
        glOrtho(minX * combinedAspect, maxX * combinedAspect, minY, maxY, -1, 1);
    } else {
        glOrtho(minX, maxX, minY / combinedAspect, maxY / combinedAspect, -1, 1);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    
    
    
    [_graph render];
    
    glPopMatrix();
    glPopMatrix();
}

- (void) Update {
    
    // dragging on the window should allow the user to set the current time
    if (isDraggingTimeline == false) {
        currentTime += kMillisecondsPerFrame;
        if (currentTime > endTime) {
            currentTime = endTime;
        }
    }
    
    // we want to execute any events in the time period >= currentTime but < currentTime + kMillisecondsPerFrame
    while(true) {
        PonyEvent * event = [_ponyEvents objectAtIndex:ponyEventIdx];
        if (event.time >= currentTime) {
            break;
        }
        
        [_graph executeEvent:event];
        
        ponyEventIdx += 1;
    }
    
    
}

- (void) Reshape {
    
}

- (void) Event:(NSEvent *)event {
    if ([event type] == NSEventTypeLeftMouseDown || [event type] == NSEventTypeLeftMouseDragged) {
        isDraggingTimeline = true;
        
        NSPoint p = [event locationInWindow];
        float windowWidth = [glview bounds].size.width;
        float normalizedTime = p.x / windowWidth;
        
        currentTime = startTime + ((endTime - startTime) * normalizedTime);
        ponyEventIdx = 0;
        
    }
    if ([event type] == NSEventTypeLeftMouseUp) {
        isDraggingTimeline = false;
    }
}

@end
