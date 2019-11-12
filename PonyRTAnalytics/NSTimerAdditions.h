//
//  NSTimerAdditions.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>


@interface NSTimer (NSTimerAdditions)

+ (NSTimer *)scheduledInterruptTimerWithTimeInterval:(NSTimeInterval)ti target:(id)aTarget selector:(SEL)aSelector userInfo:(id)userInfo repeats:(BOOL)yesOrNo;

@end
