//
//  OpenGLView.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "DocumentViewController.h"
#import <OpenGL/gl.h>

@implementation DocumentViewController

- (void)viewDidLoad {
    [glview setDelegate:self];
}

- (void) Init {
    
}

- (void) Destruct {
    
}

- (void) Render {
    glClearColor (248.0f / 255.0f, 253.0f / 255.0f, 255.0f / 253.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

}

- (void) Update {
    
}

- (void) Reshape {
    
}

- (void) Event:(NSEvent *)event {
    
}

@end
