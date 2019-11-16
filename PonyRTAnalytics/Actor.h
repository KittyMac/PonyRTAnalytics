//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>

@interface Actor : NSObject
{
    GLuint tagTexture;
    NSSize tagSize;
    unsigned long lastTag;
    
    GLuint msgTexture;
    NSSize msgSize;
    unsigned long lastMsgValue1;
    unsigned long lastMsgValue2;
}

@property (nonatomic, assign) unsigned long uuid;
@property (nonatomic, assign) unsigned long tag;
@property (nonatomic, assign) unsigned long numMessages;
@property (nonatomic, assign) unsigned long batchSize;
@property (nonatomic, assign) long priority;
@property (nonatomic, assign) unsigned long heapSize;

@property (nonatomic, assign) bool running;
@property (nonatomic, assign) bool muted;
@property (nonatomic, assign) bool overloaded;
@property (nonatomic, assign) bool underpressure;

@property (nonatomic, assign) float x;
@property (nonatomic, assign) float y;


- (id) initWithUUID:(unsigned long) actorUUID;
- (void) reset;
- (void) renderLabels:(float)size;
- (void) renderQuad:(float)size;

@end
