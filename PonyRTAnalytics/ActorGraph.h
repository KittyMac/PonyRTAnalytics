//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>

@class PonyEvent;

@interface ActorGraph : NSObject
{
    NSImage * node0;
    NSImage * node1;
    NSImage * node2;
    
    GLuint texture_node0;
    GLuint texture_node1;
    GLuint texture_node2;
    
    void ** actorLUT;
    
    NSMutableArray * messages;
}

@property (nonatomic, retain) NSMutableArray * actors;

@property (nonatomic, assign) float minX;
@property (nonatomic, assign) float maxX;
@property (nonatomic, assign) float minY;
@property (nonatomic, assign) float maxY;

- (id) initWithEvents:(NSArray *) ponyEvents;

- (void) executeEvent:(PonyEvent *)event
           isDragging:(BOOL)isDragging;
- (void) layoutCircle;
- (void) render;
- (BOOL) update:(float)delta;
- (void) reset;

@end
