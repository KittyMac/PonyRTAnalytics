//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "Message.h"
#import "Actor.h"
#import "PonyEvent.h"
#import <OpenGL/gl.h>
#import "vvector.h"
#import "NSImageAdditions.h"
#import "nodes.h"
#import "node.h"
#import "physics.h"
#import "NSStringTexture.h"

#define kPhysicsScale 100

@interface Message ()
{
    float anim;
}

@end

@implementation Message

- (id) initWithEvent:(PonyEvent *) event
           fromActor:(Actor *)from
             toActor:(Actor *)to {
    self = [super init];
    if (self) {
        _isAppMessage = [event eventID] == ANALYTIC_APP_MESSAGE_SENT;
        _from = from;
        _to = to;
    }
    return self;
}

- (BOOL) update:(float)delta {
    anim += delta;
    return (anim <= 1.0f);
}

- (void) renderQuad:(float)size {
    float x, y;
    
    if (_isAppMessage) {
        //glColor3ub(0, 216, 255);
        glColor3ub(0, 0, 0);
    } else {
        //glColor3ub(255, 234, 0);
        glColor3ub(255, 255, 255);
    }
    
    VEC_LERP_1(x, _from.x, _to.x, anim);
    VEC_LERP_1(y, _from.y, _to.y, anim);
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x-size, y-size, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x+size, y-size, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x+size, y+size, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x-size, y+size, 0.0f);
}

@end
