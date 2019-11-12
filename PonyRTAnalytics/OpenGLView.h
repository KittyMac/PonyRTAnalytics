//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>

@protocol OpenGLViewDelegate
- (void) Init;
- (void) Destruct;
- (void) Render;
- (void) Update;
- (void) Reshape;
- (void) Event:(NSEvent *)event;
@end


@interface OpenGLView : NSOpenGLView
{
	NSTimer * update_timer;
	NSDate * clock_time;
	double clock_overflow;
		
	float frustum[6][4];
	
	BOOL didInit;
}

@property (nonatomic, retain) id <OpenGLViewDelegate> delegate;

- (void) ExtractFrustum;
- (BOOL) SphereInFrustum:(float[3])center
			  withRadius:(float) radius;

- (void) stopAnimation;

-(NSImage*)image;

@end
