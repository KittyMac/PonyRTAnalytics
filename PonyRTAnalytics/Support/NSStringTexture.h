#import <Cocoa/Cocoa.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern void drawCenteredStringGLUT(int x, int y, const char * str);
extern void drawStringGLUT(int x, int y, const char * str);

extern GLuint createStringTexture(NSString * astring, NSSize * rect);
    extern GLuint updateStringTexture(GLuint oldTexture, NSString * astring, NSSize * size);

#if defined(__cplusplus)
}
#endif
