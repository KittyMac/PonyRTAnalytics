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


#import "NSImageAdditions.h"

#import <OpenGL/glu.h>
#import <OpenGL/gl.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

static GLboolean renderCapabilitiesChecked = 0;

static GLboolean isTextureRectangle = 0;
static GLint maxRectTextureSize = 0;


static char isPowerOf2(unsigned int n)
{
	int popcnt = 0;
	unsigned int mask = 0x1;
	
	while(mask)
	{
		if(n & mask)
		{
			popcnt++;
		}
		mask = mask << 1;
	}
	
	return (popcnt == 1);
}

NSRect NSRectInsideRect(NSRect a, NSRect b)
{
	float ideal_ratio = b.size.width / b.size.height;
	float view_ratio = a.size.width / a.size.height;
	NSRect bounds;
	float scale;
	
	if(view_ratio < ideal_ratio)
	{
		scale = a.size.width / b.size.width;
	}
	else
	{
		scale = a.size.height / b.size.height;
	}
	
	bounds = NSMakeRect((a.size.width - (b.size.width * scale)) * 0.5,
						(a.size.height - (b.size.height * scale)) * 0.5,
						b.size.width * scale,
						b.size.height * scale);
	
	return bounds;
}


@implementation NSImage (NSImageAdditions)

-(void) checkRendererCapabilities
{
	const GLubyte * strVersion;
	const GLubyte * strExt;
	
	strVersion = glGetString (GL_VERSION);
	strExt = glGetString (GL_EXTENSIONS);
	
	isTextureRectangle = gluCheckExtension ((const GLubyte*)
											"GL_ARB_texture_rectangle", strExt);
	if(isTextureRectangle)
	{
		glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &maxRectTextureSize);
	}
	
	renderCapabilitiesChecked = 1;
}

-(GLuint) texture:(int)mode
	  compression:(int)compression
		  replace:(GLint) replace_texture
		makeWhite:(BOOL)makeWhite
// Derived from http://developer.apple.com/qa/qa2001/qa1325.html
{
    // Bitmap generation from source view
    NSBitmapImageRep * bitmap = [NSBitmapImageRep alloc];
    int samplesPerPixel = 0;
	GLuint texName;
	NSRect bounds = NSMakeRect(0, 0, 0, 0);
	unsigned char * actual_bytes = 0;
	
	int is_power_of_2 = 0;
	unsigned int texture_mode = GL_TEXTURE_RECTANGLE_ARB;
	BOOL isFlipped = [self isFlipped];
	
	bounds.size = [self size];
	bounds.size.width = ceil(bounds.size.width);
	bounds.size.height = ceil(bounds.size.height);
	
	// Flip it to make it OpenGL friendly in its origin...
	[self setFlipped:!isFlipped];
	
    [self lockFocus];
    (void)[bitmap initWithFocusedViewRect:bounds];
    [self unlockFocus];
	
	[self setFlipped:isFlipped];
	
	if(!renderCapabilitiesChecked)
	{
		[self checkRendererCapabilities];
	}
	
	if(isPowerOf2(bounds.size.width) && 
	   isPowerOf2(bounds.size.height))
	{
		is_power_of_2 = 1;
	}
			
	if(is_power_of_2)
	{
		texture_mode = GL_TEXTURE_2D;
	}
	else
	{
		if(!isTextureRectangle)
		{
			// No texture rectangle extension... we're unable to handle
			// non-power-of-2 textures easily.
			
			fprintf(stderr, "Error: Unable to load non-power-of-2 texture\n");
			
			return -1;
		}
	}
	
	glEnable(texture_mode);
	
    // Set proper unpacking row length for bitmap
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)[bitmap pixelsWide]);
	
    // Set byte aligned unpacking (needed for 3 byte per pixel bitmaps)
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	
    // Generate new texture object if none passed in
	if(replace_texture == -1)
	{
		glGenTextures (1, &texName);
	}
	else
	{
		texName = replace_texture;
	}
	
	glBindTexture (texture_mode, texName);
	
    samplesPerPixel = (int)[bitmap samplesPerPixel];
    
    // Non-planar, RGB 24 bit bitmap, or RGBA 32 bit bitmap
    if(![bitmap isPlanar] && 
       (samplesPerPixel == 3 || samplesPerPixel == 4))
	{
		actual_bytes = [bitmap bitmapData];
		
		if(makeWhite)
		{
			unsigned char * ptr = actual_bytes;
			int k = (int)([bitmap pixelsWide] * [bitmap pixelsHigh]);
			
			
			if(samplesPerPixel == 3)
			{
				while(k--)
				{
					*(ptr++) = 255;
					*(ptr++) = 255;
					*(ptr++) = 255;
				}
			}
			else
			{
				while(k--)
				{
					*(ptr++) = 255;
					*(ptr++) = 255;
					*(ptr++) = 255;
					ptr++;
				}
			}
		}
		
		if(compression)
		{
			if(compression == GL_COMPRESSED_RGB_ARB ||
			   compression == GL_COMPRESSED_RGBA_ARB)
			{
				if(replace_texture == -1)
				{
					glTexImage2D(texture_mode, 
								 0, 
								 samplesPerPixel == 4 ? GL_COMPRESSED_RGBA_ARB : GL_COMPRESSED_RGB_ARB,
								 (GLsizei)[bitmap pixelsWide],
								 (GLsizei)[bitmap pixelsHigh],
								 0, 
								 samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
								 GL_UNSIGNED_BYTE, 
								 actual_bytes);
				}
				else
				{
					glTexSubImage2D(texture_mode, 
									0, 
									0, 
									0, 
									(GLsizei)[bitmap pixelsWide],
									(GLsizei)[bitmap pixelsHigh],
									samplesPerPixel == 4 ? GL_RGBA : GL_RGB, 
									GL_UNSIGNED_BYTE, 
									actual_bytes);
				}
			}
			
			if(compression == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
			   compression == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
			{
				if(replace_texture == -1)
				{
					glTexImage2D(texture_mode, 
								 0, 
								 samplesPerPixel == 4 ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
								 (GLsizei)[bitmap pixelsWide],
								 (GLsizei)[bitmap pixelsHigh],
								 0, 
								 samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
								 GL_UNSIGNED_BYTE, 
								 actual_bytes);
				}
				else
				{
					glTexSubImage2D(texture_mode, 
									0, 
									0, 
									0, 
									(GLsizei)[bitmap pixelsWide],
									(GLsizei)[bitmap pixelsHigh],
									samplesPerPixel == 4 ? GL_RGBA : GL_RGB, 
									GL_UNSIGNED_BYTE, 
									actual_bytes);
				}
			}
			
		}
		else
		{
			if(replace_texture == -1)
			{
				glTexImage2D(texture_mode, 
							 0, 
							 samplesPerPixel == 4 ? GL_RGBA8 : GL_RGB8,
							 (GLsizei)[bitmap pixelsWide],
							 (GLsizei)[bitmap pixelsHigh],
							 0, 
							 samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
							 GL_UNSIGNED_BYTE, 
							 actual_bytes);
			}
			else
			{
				glTexSubImage2D(texture_mode, 
								0, 
								0, 
								0, 
								(GLsizei)[bitmap pixelsWide],
								(GLsizei)[bitmap pixelsHigh],
								samplesPerPixel == 4 ? GL_RGBA : GL_RGB, 
								GL_UNSIGNED_BYTE, 
								actual_bytes);
			}
		}
    }
	else
	{
		/*
		 Error condition...
		 The above code handles 2 cases (24 bit RGB and 32 bit RGBA),
		 it is possible to support other bitmap formats if desired.
		 
		 So we'll log out some useful information.
		 */
        NSLog (@"-textureFromView: Unsupported bitmap data format: isPlanar:%d, samplesPerPixel:%d, bitsPerPixel:%d, bytesPerRow:%d, bytesPerPlane:%d",
			   [bitmap isPlanar], 
			   (int)[bitmap samplesPerPixel],
			   (int)[bitmap bitsPerPixel],
			   (int)[bitmap bytesPerRow],
			   (int)[bitmap bytesPerPlane]);
    }
	
	// Non-mipmap filtering (redundant for texture_rectangle)
    glTexParameteri(texture_mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture_mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture_mode, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(texture_mode, GL_TEXTURE_WRAP_T, mode);
	//glGenerateMipmapEXT(texture_mode);
			
	glDisable(texture_mode);
		
	return texName;
}

-(GLuint) texture:(int)mode
	  compression:(int)compression
		  replace:(GLint) replace_texture
{
	return [self texture:mode
			 compression:compression
				 replace:replace_texture
			   makeWhite:NO];
}

-(GLuint) texture:(int)mode
	  compression:(int)compression
{
	return [self texture:mode
			 compression:compression
				 replace:-1];
}

-(GLuint) texture:(int)mode
{
	return [self texture:mode
			 compression:0
				 replace:-1];
}

-(GLuint) texture
{
	return [self texture:GL_CLAMP_TO_EDGE
			 compression:0
				 replace:-1];
}

-(GLuint) whiteTexture
{
	return [self texture:GL_CLAMP_TO_EDGE
			 compression:0
				 replace:-1
			   makeWhite:YES];
}

- (NSRect) bounds
{
	NSRect r = NSZeroRect;
	r.size = [self size]; 
	return r;
}

- (NSData *) JPEGRepresentationWithQuality:(float)q
{
	NSBitmapImageRep * bitmap = [NSBitmapImageRep alloc];
	
    [self lockFocus];
    (void) [bitmap initWithFocusedViewRect:[self bounds]];
    [self unlockFocus];
		
	return [bitmap representationUsingType:NSJPEGFileType properties:[NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:q] forKey:NSImageCompressionFactor]];
}

- (NSData *) PNGRepresentation
{
	NSBitmapImageRep * bitmap = [NSBitmapImageRep alloc];

    [self lockFocus];
    (void) [bitmap initWithFocusedViewRect:[self bounds]];
    [self unlockFocus];
		
	return [bitmap representationUsingType:NSPNGFileType properties:[NSDictionary dictionary]];
}

- (NSData *) RawRepresentation
{
	// Bitmap generation from source view
    NSBitmapImageRep * bitmap = [NSBitmapImageRep alloc];
    int samplesPerPixel = 0;
	NSRect bounds = NSMakeRect(0, 0, 0, 0);
	BOOL isFlipped = [self isFlipped];
	
	bounds.size = [self size];
	bounds.size.width = ceil(bounds.size.width);
	bounds.size.height = ceil(bounds.size.height);
	
	// Flip it to make it OpenGL friendly in its origin...
	[self setFlipped:!isFlipped];
	
    [self lockFocus];
    (void) [bitmap initWithFocusedViewRect:bounds];
    [self unlockFocus];
	
	[self setFlipped:isFlipped];
	
	
	
    samplesPerPixel = (int)[bitmap samplesPerPixel];
	
	NSData * data = nil;
	// Non-planar, RGB 24 bit bitmap, or RGBA 32 bit bitmap
    if(![bitmap isPlanar] && 
       (samplesPerPixel == 3 || samplesPerPixel == 4))
	{
		data = [NSData dataWithBytes:[bitmap bitmapData] length:([bitmap pixelsWide] * [bitmap pixelsHigh] * samplesPerPixel)];
	}
	return data;
}

+ (NSImage *) loadImage:(NSString *)image_name
{
	// Have we already been loaded or are we in the normal
	// resource tree?
	NSImage * img = [NSImage imageNamed:image_name];
	
	if(!img)
	{
		// If not, try and treat this like a path to an image file...
		img = [[NSImage alloc] initWithContentsOfFile:image_name];
		
		if(!img)
		{
			// If not, try and get the icon associated with the file...
			img = [[NSWorkspace sharedWorkspace] iconForFile:image_name];
		}
		
		//[img setName:image_name];
	}
	
	NSImageRep * rep = [[img representations] objectAtIndex:0]; 
	[img setScalesWhenResized:YES]; 
	[img setSize:NSMakeSize([rep pixelsWide], [rep pixelsHigh])];
	
	return img;
}

+ (NSImage *) loadImage:(NSString *)image_name
		   restrictSize:(NSSize) size
{
	// Have we already been loaded or are we in the normal
	// resource tree?
	NSImage * img = [NSImage imageNamed:image_name];
	NSRect orig_bounds;
	NSRect max_bounds = NSZeroRect;
	
	if(!img)
	{
		// If not, try and treat this like a path to an image file...
		img = [[NSImage alloc] initWithContentsOfFile:image_name];
		
		if(!img)
		{
			// If not, try and get the icon associated with the file...
			img = [[NSWorkspace sharedWorkspace] iconForFile:image_name];
		}
	}
	
	NSImageRep * rep = [[img representations] objectAtIndex:0]; 
	[img setScalesWhenResized:YES]; 
	[img setSize:NSMakeSize([rep pixelsWide], [rep pixelsHigh])];
	
	// Is the image too big?  If so, resize it...
	orig_bounds = [img bounds];
	max_bounds.size = size;
	if(orig_bounds.size.width > max_bounds.size.width ||
	   orig_bounds.size.height > max_bounds.size.height)
	{
		NSRect new_bounds = NSRectInsideRect(max_bounds, orig_bounds);
		NSImage * new_img = [[NSImage alloc] initWithSize:new_bounds.size];
		
		new_bounds.origin.x = 0.0;
		new_bounds.origin.y = 0.0;
		
		[new_img lockFocus];
		
		[img drawInRect:new_bounds
			   fromRect:orig_bounds
              operation:NSCompositingOperationCopy
			   fraction:1.0];
		
		[new_img unlockFocus];
		
		//[new_img setName:image_name];
		
		return new_img;
	}
	
	// Name the image for easy access...
	//[img setName:image_name];
	
	return img;
}

+ (NSImage *) loadImage:(NSString *)image_name
				resized:(NSSize) size
{
	// Have we already been loaded or are we in the normal
	// resource tree?
	NSImage * img = [NSImage imageNamed:image_name];
	NSRect orig_bounds;
	
	if(!img)
	{
		// If not, try and treat this like a path to an image file...
		img = [[NSImage alloc] initWithContentsOfFile:image_name];
		
		if(!img)
		{
			// If not, try and get the icon associated with the file...
			img = [[NSWorkspace sharedWorkspace] iconForFile:image_name];
		}
	}
	
	NSImageRep * rep = [[img representations] objectAtIndex:0]; 
	[img setScalesWhenResized:YES]; 
	[img setSize:NSMakeSize([rep pixelsWide], [rep pixelsHigh])];
	orig_bounds = [img bounds];
	
	// Is the image too big?  If so, resize it...
	NSRect new_bounds = NSMakeRect(0, 0, size.width, size.height);
	NSImage * new_img = [[NSImage alloc] initWithSize:new_bounds.size];
	
	[new_img lockFocus];
	
	[img drawInRect:new_bounds
		   fromRect:orig_bounds
          operation:NSCompositingOperationCopy
		   fraction:1.0];
	
	[new_img unlockFocus];
	
	//[new_img setName:image_name];
	
	return new_img;	
}



+ (NSImage *) loadImageAsPowerOf2:(NSString *)image_name
				 WithOriginalSize:(CGSize *)size
{
	// Have we already been loaded or are we in the normal
	// resource tree?
	NSImage * img = NULL;
	NSRect orig_bounds;
	int i;
	
	if(!img)
	{
		// If not, try and treat this like a path to an image file...
		img = [[NSImage alloc] initWithContentsOfFile:image_name];
		
		if(!img)
		{
			// If not, try and get the icon associated with the file...
			img = [[NSWorkspace sharedWorkspace] iconForFile:image_name];
		}
	}
	
	NSImageRep * rep = [[img representations] objectAtIndex:0]; 
	[img setScalesWhenResized:YES]; 
	[img setSize:NSMakeSize([rep pixelsWide], [rep pixelsHigh])];
	
	if(size)
	{
		size->width = [rep pixelsWide];
		size->height = [rep pixelsHigh];
	}
	
	// Is the image too big?  If so, resize it...
	orig_bounds = [img bounds];
	
	for(i = 1; i <= 4096; i *= 2)
	{
		if((orig_bounds.size.width-(i * 0.1f)) < i &&
		   (orig_bounds.size.height-(i * 0.1f)) < i)
		{
			break;
		}
	}

	NSRect new_bounds = NSMakeRect(0,0,i,i);
	NSImage * new_img = [[NSImage alloc] initWithSize:new_bounds.size];
	
	new_bounds.origin.x = 0.0;
	new_bounds.origin.y = 0.0;
	
	[new_img lockFocus];
	
	[img drawInRect:new_bounds
		   fromRect:orig_bounds
		  operation:NSCompositeCopy
		   fraction:1.0];
	
	[new_img unlockFocus];
	
	//[new_img setName:image_name];
	
	return new_img;
}

+ (NSImage *) loadImageAsPowerOf2:(NSString *)image_name
{
	return [self loadImageAsPowerOf2:image_name
					WithOriginalSize:NULL];
			
}

+ (NSImage *) loadImage:(NSString *)image_name
	   WithOriginalSize:(CGSize *)size
{
	// Have we already been loaded or are we in the normal
	// resource tree?
	NSImage * img = NULL;
	NSRect orig_bounds;
	
	if(!img)
	{
		// If not, try and treat this like a path to an image file...
		img = [[NSImage alloc] initWithContentsOfFile:image_name];
		
		if(!img)
		{
			// If not, try and get the icon associated with the file...
			img = [[NSWorkspace sharedWorkspace] iconForFile:image_name];
		}
	}
	
	NSImageRep * rep = [[img representations] objectAtIndex:0]; 
	[img setScalesWhenResized:YES]; 
	[img setSize:NSMakeSize([rep pixelsWide], [rep pixelsHigh])];
	
	if(size)
	{
		size->width = [rep pixelsWide];
		size->height = [rep pixelsHigh];
	}
	
	// Is the image too big?  If so, resize it...
	orig_bounds = [img bounds];
	
	NSRect new_bounds = orig_bounds;
	NSImage * new_img = [[NSImage alloc] initWithSize:new_bounds.size];
	
	new_bounds.origin.x = 0.0;
	new_bounds.origin.y = 0.0;
	
	[new_img lockFocus];
	
	[img drawInRect:new_bounds
		   fromRect:orig_bounds
		  operation:NSCompositeCopy
		   fraction:1.0];
	
	[new_img unlockFocus];
	
	//[new_img setName:image_name];
	
	return new_img;
}

- (void)drawAtPoint:(NSPoint)point
		  withScale:(NSSize)scale
		  operation:(NSCompositingOperation)op
		   fraction:(float)delta
{
	NSSize orig_size = [self size];
	NSRect dst_rect;
	
	dst_rect.size.width = scale.width * orig_size.width;
	dst_rect.size.height = scale.height * orig_size.height;
	dst_rect.origin.x = point.x - dst_rect.size.width * 0.5;
	dst_rect.origin.y = point.y - dst_rect.size.height * 0.5;
	
	[self drawInRect:dst_rect
			fromRect:[self bounds]
		   operation:op
			fraction:delta];
}

- (void)drawAtPoint:(NSPoint)point
		  withSize:(NSSize)scale
		  operation:(NSCompositingOperation)op
		   fraction:(float)delta
{
	NSRect dst_rect;
	
	dst_rect.size.width = scale.width;
	dst_rect.size.height = scale.height;
	dst_rect.origin.x = point.x - dst_rect.size.width * 0.5;
	dst_rect.origin.y = point.y - dst_rect.size.height * 0.5;
	
	[self drawInRect:dst_rect
			fromRect:[self bounds]
		   operation:op
			fraction:delta];
}

@end

#pragma GCC diagnostic pop
