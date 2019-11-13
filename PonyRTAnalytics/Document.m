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
    
    DocumentViewController * vc = (DocumentViewController *)wc.contentViewController;
    [vc parsePonyEvents:csvLines];
    
    [self addWindowController:wc];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError {
    return nil;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError {
    if ([typeName isEqualToString:@"PonyRTAnalytics"]) {
        NSString * contents = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        csvLines = [contents componentsSeparatedByString:@"\n"];
    }
    return YES;
}


@end
