#import "NSStringTexture.h"
#import "NSImageAdditions.h"

#import <OpenGL/glu.h>
#import <OpenGL/gl.h>
#include <GLUT/glut.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

static NSImage * imageForString(NSString * astring, NSSize * size);

static int glutStringLength(void * font, const char * str)
{
    int w = 0;
    while(*str){
        w += glutBitmapWidth(font, *str);
        str++;
    }
    return w;
}

void drawCenteredStringGLUT(int x, int y, const char * str)
{
    int i = 0;
    int width = glutStringLength(GLUT_BITMAP_8_BY_13, str);
    
    glRasterPos2d(x - (width >> 1), y);
    
    while(str[i]){
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
        i++;
    }
}

void drawStringGLUT(int x, int y, const char * str)
{
    glRasterPos2d(x, y);
    
    while(*str){
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str);
        str++;
    }
}

GLuint updateStringTexture(GLuint oldTexture, NSString * astring, NSSize * size) {
    NSImage * img = imageForString(astring, size);
    return [img texture:GL_CLAMP_TO_EDGE
            compression:0
                replace:oldTexture];
}

GLuint createStringTexture(NSString * astring, NSSize * size)
{
    NSImage * img = imageForString(astring, size);
    return [img texture];
}

NSImage * imageForString(NSString * astring, NSSize * size) {
    NSImage * temp_image;
    NSSize bounding_box;
    
    // Create a dictionary to define the styles of our drawn text...
    // We're going to make an outline font with black shadow...
    NSMutableDictionary * bold12White = [[NSMutableDictionary alloc] init];
    NSMutableDictionary * bold12Black = [[NSMutableDictionary alloc] init];
    NSShadow * shadow = [[NSShadow alloc] init];
    NSMutableParagraphStyle * style = [[NSMutableParagraphStyle alloc] init];
    style.alignment = NSTextAlignmentCenter;
    
    // Bold white font
    [bold12White setObject: [NSFont fontWithName: @"Helvetica-Bold"
                                            size: 32] forKey: NSFontAttributeName];
    [bold12White setObject:style
                    forKey:NSParagraphStyleAttributeName];
    [bold12White setObject: [NSColor whiteColor]
                    forKey:NSForegroundColorAttributeName];
    
    // Bold Black outline
    [bold12Black setObject: [NSFont fontWithName: @"Helvetica-Bold"
                                            size: 32] forKey: NSFontAttributeName];
    [bold12Black setObject:style
                    forKey:NSParagraphStyleAttributeName];
    [bold12Black setObject: [NSColor blackColor]
                    forKey: NSForegroundColorAttributeName];
    [bold12Black setObject: [NSNumber numberWithFloat:-3.0]
                    forKey: NSStrokeWidthAttributeName];
    
    // Shadow attributes
    [shadow setShadowOffset:NSMakeSize(0.0, -1.0)];
    [shadow setShadowColor:[NSColor blackColor]];
    [shadow setShadowBlurRadius:3.0];
    [bold12Black setObject: shadow forKey: NSShadowAttributeName];
    
    // Determine the size of cache needed to hold the string
    bounding_box = [astring sizeWithAttributes:bold12Black];
    
    // Create a new NSImage to render our text into
    temp_image = [[NSImage alloc]
                  initWithSize:bounding_box];
    
    // Lock focus so we render to the image...
    [temp_image lockFocusFlipped:YES];
    
    
    
    [astring drawAtPoint:NSMakePoint(0.0, 0.0)
          withAttributes:bold12Black];
    
    [astring drawAtPoint:NSMakePoint(0.0, 0.0)
          withAttributes:bold12White];
    
    [temp_image unlockFocus];
    
    *size = bounding_box;
    
    return [NSImage loadImage:temp_image
             IntoPowerOf2Size:128];
}



#pragma GCC diagnostic pop
