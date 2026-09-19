// Stub: NSURLHandle is deprecated and removed from iOS SDK
#ifndef _NSURLHANDLE_H_STUB
#define _NSURLHANDLE_H_STUB
@class NSString;
@class NSData;
@class NSURLRequest;
@interface NSURLHandle : NSObject
+ (nullable NSURLHandle *)cachedHandleForURL:(NSURL *)aURL;
- (nullable instancetype)initWithRequest:(NSURLRequest *)request delegate:(nullable id)delegate;
- (void)loadRequest:(NSURLRequest *)request;
- (void)cancelLoad;
- (nullable NSData *)availableResourceData;
- (nullable NSData *)resourceData;
- (void)writeAvailableResourceData:(NSData *)data;
@end
#endif
