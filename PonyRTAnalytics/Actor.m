//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "Actor.h"
#import <OpenGL/gl.h>
#import "vvector.h"

@implementation Actor

- (id) initWithTag:(unsigned long) actorTag {
    self = [super init];
    if (self) {
        _tag = actorTag;
        _numMessages = 0;
        _batchSize = 0;
        _priority = 0;
    }
    return self;
}

- (long) compare:(Actor *) other {
    return (long)other.tag - (long)_tag;
}

- (void) renderQuad:(float)size {
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(_x-size, _y-size, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(_x+size, _y-size, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(_x+size, _y+size, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(_x-size, _y+size, 0.0f);
}

@end
