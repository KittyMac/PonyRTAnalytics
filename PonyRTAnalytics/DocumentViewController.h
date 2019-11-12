//
//  OpenGLView.h
//  Thrones
//
//  Created by Rocco Bowling on 3/26/09.
//  Copyright 2009 Chimera Software. All rights reserved.
//	Chimera Software grants Small Planet Digital the rights to use the software "AS-IS".
//

#import <Cocoa/Cocoa.h>
#import "OpenGLView.h"

@interface DocumentViewController : NSViewController <OpenGLViewDelegate>
{
    IBOutlet OpenGLView * glview;
}

@end
