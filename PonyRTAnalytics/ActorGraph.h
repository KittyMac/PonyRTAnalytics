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
    
}

@property (nonatomic, retain) NSMutableDictionary * actors;

- (id) initWithEvents:(NSArray *) ponyEvents;

- (void) executeEvent:(PonyEvent *)event;
- (void) layoutCircle;
- (void) render;

@end
