//
//  Document.m
//  PonyRTAnalytics
//
//  Created by Rocco Bowling on 11/12/19.
//  Copyright © 2019 Rocco Bowling. All rights reserved.
//

#import "Document.h"
#import "DocumentWindowController.h"
#import "DocumentViewController.h"

@interface Document ()

@end

@implementation Document

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

+ (BOOL)autosavesInPlace {
    return YES;
}

- (void)makeWindowControllers {
    
    NSStoryboard * storyboard = [NSStoryboard storyboardWithName:@"Document" bundle:nil];
    DocumentWindowController * wc = [storyboard instantiateControllerWithIdentifier:@"Document Window Controller"];
    [self addWindowController:wc];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError {
    // Insert code here to write your document to data of the specified type. If outError != NULL, ensure that you create and set an appropriate error if you return nil.
    // Alternatively, you could remove this method and override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.
    //[NSException raise:@"UnimplementedMethod" format:@"%@ is unimplemented", NSStringFromSelector(_cmd)];
    return nil;
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError {
    // Insert code here to read your document from the given data of the specified type. If outError != NULL, ensure that you create and set an appropriate error if you return NO.
    // Alternatively, you could remove this method and override -readFromFileWrapper:ofType:error: or -readFromURL:ofType:error: instead.
    // If you do, you should also override -isEntireFileLoaded to return NO if the contents are lazily loaded.
    //[NSException raise:@"UnimplementedMethod" format:@"%@ is unimplemented", NSStringFromSelector(_cmd)];
    return YES;
}


@end
