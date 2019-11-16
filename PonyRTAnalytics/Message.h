//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//    Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>

@class Actor;
@class PonyEvent;

@interface Message : NSObject
{
    
}

@property (nonatomic, retain) Actor * from;
@property (nonatomic, retain) Actor * to;
@property (nonatomic, assign) BOOL isAppMessage;


- (id) initWithEvent:(PonyEvent *) event
           fromActor:(Actor *)from
             toActor:(Actor *)to;
- (void) renderQuad:(float)size;
- (BOOL) update:(float)delta;

@end
