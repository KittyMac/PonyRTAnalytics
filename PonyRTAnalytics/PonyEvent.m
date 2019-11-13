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
        NSArray * parts = [string componentsSeparatedByString:@","];
        
        if ([parts count] < 8) {
            _actorTag = 0;
        } else {
            _time = [[parts objectAtIndex:0] integerValue];
            _actorTag = [[parts objectAtIndex:1] integerValue];
            _eventID = [[parts objectAtIndex:2] integerValue];
            _actorNumMessages = [[parts objectAtIndex:3] integerValue];
            _actorBatchSize = [[parts objectAtIndex:4] integerValue];
            _actorPriority = [[parts objectAtIndex:5] integerValue];
            
            _toActorTag = [[parts objectAtIndex:6] integerValue];
            _toActorNumberOfMessages = [[parts objectAtIndex:7] integerValue];
        }
    }
    return self;
}

@end
