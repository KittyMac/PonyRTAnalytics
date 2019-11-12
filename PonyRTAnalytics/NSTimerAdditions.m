//
//  NSTimerAdditions.m
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import "NSTimerAdditions.h"


@implementation NSTimer (NSTimerAdditions)

+ (NSTimer *)scheduledInterruptTimerWithTimeInterval:(NSTimeInterval)ti
											  target:(id)aTarget
											selector:(SEL)aSelector
											userInfo:(id)userInfo
											 repeats:(BOOL)yesOrNo
{
	NSTimer * timer = [[NSTimer alloc] initWithFireDate:[NSDate date]
											   interval:ti
												 target:aTarget
											   selector:aSelector
											   userInfo:userInfo
												repeats:yesOrNo];
	
	[[NSRunLoop currentRunLoop] addTimer:timer
								 forMode:NSRunLoopCommonModes];
	
	return timer;
}


@end
