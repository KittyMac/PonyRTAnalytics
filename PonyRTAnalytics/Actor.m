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
    if (tagTexture == 0) {
        tagTexture = createStringTexture([NSString stringWithFormat:@"%lu", _tag], &tagSize);
    } else {
        tagTexture = updateStringTexture(tagTexture, [NSString stringWithFormat:@"%lu", _tag], &tagSize);
    }
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
    
    
    unsigned long heapSizeInMB = (_heapSize / (1024 * 1024));
    
    if (msgTexture == 0 || lastMsgValue1 != _numMessages || lastMsgValue2 != heapSizeInMB) {
        
        lastMsgValue1 = _numMessages;
        lastMsgValue2 = heapSizeInMB;
        
        NSString * msgString = [NSString stringWithFormat:@"%lu of %lu\n%lu MB", _numMessages, _batchSize, heapSizeInMB];
        if (msgTexture == 0) {
            msgTexture = createStringTexture(msgString, &msgSize);
        } else {
            msgTexture = updateStringTexture(msgTexture, msgString, &msgSize);
        }
        
        glEnable(GL_TEXTURE_2D);
    }
    
    if (msgTexture != 0) {
        float h = size * 1.0f;
        float w = msgSize.width * (h / msgSize.height);
        float yOffset = -(h * 2.0f);
        
        if (_numMessages > _batchSize) {
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
