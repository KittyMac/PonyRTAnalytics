//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>

enum {
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
};

// TIME_OF_EVENT,ACTOR_A_TAG,EVENT_NUMBER,ACTOR_A_NUMBER_OF_MESSAGES,ACTOR_A_BATCH_SIZE,ACTOR_A_PRIORITY,ACTOR_B_TAG,ACTOR_B_NUMBER_OF_MESSAGES
@interface PonyEvent : NSObject
{
    
}

@property (nonatomic, assign) unsigned long time;
@property (nonatomic, assign) unsigned long actorTag;
@property (nonatomic, assign) unsigned long eventID;
@property (nonatomic, assign) unsigned long actorNumMessages;
@property (nonatomic, assign) unsigned long actorBatchSize;
@property (nonatomic, assign) unsigned long actorPriority;

@property (nonatomic, assign) unsigned long toActorTag;
@property (nonatomic, assign) unsigned long toActorNumberOfMessages;

- (id) initWithCSV:(NSString *)string;

@end
