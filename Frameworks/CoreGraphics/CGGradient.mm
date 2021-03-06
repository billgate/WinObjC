//******************************************************************************
//
// Copyright (c) 2016 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

#import <StubReturn.h>
#import <Starboard.h>
#import <CoreGraphics/CGContext.h>
#import "CGColorSpaceInternal.h"
#import "CGGradientInternal.h"
#import "UIColorInternal.h"
#import "_CGLifetimeBridgingType.h"

@interface CGNSGradient : _CGLifetimeBridgingType
@end

@implementation CGNSGradient
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-missing-super-calls"
- (void)dealloc {
    delete (__CGGradient*)self;
}
#pragma clang diagnostic pop
@end

__CGGradient::__CGGradient() : _components(NULL), _locations(NULL) {
    object_setClass((id) this, [CGNSGradient class]);
}

__CGGradient::~__CGGradient() {
    if (_components) {
        delete[] _components;
        _components = NULL;
    }
    if (_locations) {
        delete[] _locations;
        _locations = NULL;
    }
}

void __CGGradient::initWithColorComponents(const float* components, const float* locations, size_t count, CGColorSpaceRef colorspace) {
    DWORD componentCount = 0;
    __CGColorSpace* cs = (__CGColorSpace*)colorspace;
    _colorSpace = cs->colorSpace;

    switch (_colorSpace) {
        case _ColorRGB:
            componentCount = 3;
            break;

        case _ColorRGBA:
            componentCount = 4;
            break;

        case _ColorGrayscale:
            componentCount = 2;
            break;

        default:
            assert(0);
            break;
    }

    _components = new float[count * componentCount];
    memcpy(_components, components, sizeof(float) * count * componentCount);

    _locations = new float[count];

    if (locations) {
        memcpy(_locations, locations, sizeof(float) * count);
    } else {
        for (unsigned i = 0; i < count; i++) {
            _locations[i] = (float)i / (float)(count - 1);
        }
    }

    _count = count;
}

void __CGGradient::initWithColors(CFArrayRef componentsArr, const float* locations, CGColorSpaceRef colorspace) {
    NSArray* components = (NSArray*)componentsArr;
    size_t componentCount = 0;
    __CGColorSpace* cs = (__CGColorSpace*)colorspace;

    _colorSpace = _ColorRGBA;

    switch (_colorSpace) {
        case _ColorRGB:
            componentCount = 3;
            break;

        case _ColorRGBA:
            componentCount = 4;
            break;

        case _ColorGrayscale:
            componentCount = 2;
            break;

        default:
            assert(0);
            break;
    }

    int count = [components count];

    _components = new float[count * componentCount];

    for (int i = 0; i < count; i++) {
        UIColor* curColor = [components objectAtIndex:i];

        float colorArray[4];
        ColorQuad color;
        [curColor getColors:&color];

        ColorQuadToFloatArray(color, colorArray);

        memcpy(&_components[i * componentCount], colorArray, sizeof(float) * componentCount);
    }

    _locations = new float[count];

    if (locations) {
        memcpy(_locations, locations, sizeof(float) * count);
    } else {
        for (int i = 0; i < count; i++) {
            _locations[i] = (float)i / (float)(count - 1);
        }
    }

    _count = count;
}

/**
 @Status Interoperable
*/
CGGradientRef CGGradientCreateWithColorComponents(CGColorSpaceRef colorSpace,
                                                  const float* components,
                                                  const float* locations,
                                                  size_t count) {
    CGGradientRef ret = new __CGGradient();
    ret->initWithColorComponents(components, locations, count, colorSpace);

    return ret;
}

/**
 @Status Interoperable
*/
CGGradientRef CGGradientCreateWithColors(CGColorSpaceRef colorSpace, CFArrayRef colors, const float* locations) {
    CGGradientRef ret = new __CGGradient();
    ret->initWithColors(colors, locations, colorSpace);

    return ret;
}

/**
 @Status Interoperable
*/
void CGGradientRelease(CGGradientRef gradient) {
    CFRelease((id)gradient);
}

/**
 @Status Interoperable
*/
CGGradientRef CGGradientRetain(CGGradientRef gradient) {
    CFRetain((id)gradient);
    return gradient;
}

/**
 @Status Stub
 @Notes
*/
CFTypeID CGGradientGetTypeID() {
    UNIMPLEMENTED();
    return StubReturn();
}