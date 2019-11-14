#import <Cocoa/Cocoa.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern void drawCenteredStringGLUT(int x, int y, const char * str);
extern void drawStringGLUT(int x, int y, const char * str);

extern GLuint createStringTexture(NSString * astring, NSSize * rect);

#if defined(__cplusplus)
}
#endif
