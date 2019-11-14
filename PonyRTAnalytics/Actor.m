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
#import "NSStringTexture.h"

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

- (void) reloadLabels {
    if (tagTexture != 0) {
        glDeleteTextures(1, &tagTexture);
        tagTexture = 0;
    }
    
    tagTexture = createStringTexture([NSString stringWithFormat:@"%lu", _tag], &tagSize);
}

- (void) renderLabels:(float)size {
    if (tagTexture != 0) {
        float h = size * 0.75f;
        float w = tagSize.width * (h / tagSize.height);
        float yOffset = h*0.5f;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        
        glBindTexture(GL_TEXTURE_2D, tagTexture);
        
        glBegin(GL_QUADS);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(_x-w, _y+yOffset-h, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(_x+w, _y+yOffset-h, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(_x+w, _y+yOffset+h, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(_x-w, _y+yOffset+h, 0.0f);
        
        glEnd();
    }
    
    
    if (lastMsgValue != _numMessages) {
        if (msgTexture != 0) {
            glDeleteTextures(1, &msgTexture);
            msgTexture = 0;
        }
        
        lastMsgValue = _numMessages;
        msgTexture = createStringTexture([NSString stringWithFormat:@"%lu of %lu", lastMsgValue, _batchSize], &msgSize);
        
        glEnable(GL_TEXTURE_2D);
    }
    
    if (msgTexture != 0) {
        float h = size * 0.4f;
        float w = msgSize.width * (h / msgSize.height);
        float yOffset = -(h * 1.5f);
        
        if (lastMsgValue > _batchSize) {
            glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        
        glBindTexture(GL_TEXTURE_2D, msgTexture);
        
        glBegin(GL_QUADS);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(_x-w, _y+yOffset-h, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(_x+w, _y+yOffset-h, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(_x+w, _y+yOffset+h, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(_x-w, _y+yOffset+h, 0.0f);
        
        glEnd();
    }
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
