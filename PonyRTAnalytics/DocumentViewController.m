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
    }
    
    _graph = [[ActorGraph alloc] initWithEvents:_ponyEvents];
    
    NSLog(@"Loaded %lu ponyrt events\n", (unsigned long)[_ponyEvents count]);
}

- (void) Init {
    
}

- (void) Destruct {
    
}

- (void) Render {
    
    float aspect = glview.frame.size.width / glview.frame.size.height;
    
    glViewport(0, 0, glview.frame.size.width, glview.frame.size.height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    if (aspect > 1.0) {
        glOrtho(-1 * aspect, 1 * aspect, -1, 1, -1, 1);
    } else {
        glOrtho(-1, 1, -1 / aspect, 1 / aspect, -1, 1);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glClearColor (248.0f / 255.0f, 253.0f / 255.0f, 255.0f / 253.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    [_graph render];
    
    glPopMatrix();
    glPopMatrix();
}

- (void) Update {
    
}

- (void) Reshape {
    
}

- (void) Event:(NSEvent *)event {
    
}

@end
