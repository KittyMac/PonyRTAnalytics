/**********************************************************************

Created by Rocco Bowling
Big Nerd Ranch, Inc
OpenGL Bootcamp

Copyright 2006 Rocco Bowling, All rights reserved.

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. It is not resold, in it's current form or in modified, as a
teaching utility or as part of a teaching utility

This code is presented as is. The author of the code takes no
responsibilities for any version of this code.

(c) 2006 Rocco Bowling

*********************************************************************/

#import <Cocoa/Cocoa.h>

extern NSRect NSRectInsideRect(NSRect a, NSRect b);

@interface NSImage (NSImageAdditions)

-(GLuint) texture;
-(GLuint) texture:(int)mode;
-(GLuint) texture:(int)mode
	  compression:(int)compression;
-(GLuint) texture:(int)mode
	  compression:(int)compression
		  replace:(GLint)replace;
-(GLuint) whiteTexture;

- (NSRect) bounds;

- (NSData *) JPEGRepresentationWithQuality:(float)q;
- (NSData *) PNGRepresentation;
- (NSData *) RawRepresentation;

+ (NSImage *) loadImage:(NSString *)image_name;
+ (NSImage *) loadImage:(NSString *)image_name
		   restrictSize:(NSSize) size;
+ (NSImage *) loadImage:(NSString *)image_name
				resized:(NSSize) size;

+ (NSImage *) loadImage:(NSImage *)img
       IntoPowerOf2Size:(int)size;

+ (NSImage *) loadImageAsPowerOf2:(NSString *)image_name;
+ (NSImage *) loadImageAsPowerOf2:(NSImage *)img
                 WithOriginalSize:(CGSize *)size;
+ (NSImage *) loadImageNamedAsPowerOf2:(NSString *)image_name
                      WithOriginalSize:(CGSize *)size;


+ (NSImage *) loadImage:(NSString *)image_name
	   WithOriginalSize:(CGSize *)size;

- (void)drawAtPoint:(NSPoint)point
		  withScale:(NSSize)scale
		  operation:(NSCompositingOperation)op
		   fraction:(float)delta;

- (void)drawAtPoint:(NSPoint)point
		   withSize:(NSSize)scale
		  operation:(NSCompositingOperation)op
		   fraction:(float)delta;

@end


