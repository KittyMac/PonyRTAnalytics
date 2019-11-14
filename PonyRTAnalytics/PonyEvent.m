//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "PonyEvent.h"
#import <OpenGL/gl.h>
#import "vvector.h"

@implementation PonyEvent

- (id) initWithCSV:(NSString *)string {
    self = [super init];
    if (self) {
        
        // ofc we're going drop down to C where its 10000x faster
        const char * cStr = [string UTF8String];
        sscanf(cStr, "%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
               &_time,
               &_actorTag,
               &_eventID,
               &_actorNumMessages,
               &_actorBatchSize,
               &_actorPriority,
               &_actorHeapSize,
               &_toActorTag,
               &_toActorNumberOfMessages);
    }
    return self;
}

@end
