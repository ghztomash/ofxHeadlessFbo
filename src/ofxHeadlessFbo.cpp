/*
Software License Agreement (BSD License)

Copyright (c) 2012 Adafruit Industries.  All rights reserved.
Copyright (c) 2022 Tomash GHz.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "ofxHeadlessFbo.h"
#include <algorithm>
#include <cmath>

namespace {
bool clipTest(float p, float q, float &u1, float &u2) {
    if (p == 0.0f) {
        return q >= 0.0f;
    }

    const float r = q / p;
    if (p < 0.0f) {
        if (r > u2) {
            return false;
        }
        if (r > u1) {
            u1 = r;
        }
    } else {
        if (r < u1) {
            return false;
        }
        if (r < u2) {
            u2 = r;
        }
    }

    return true;
}

bool clipLineToBounds(float &x0, float &y0, float &x1, float &y1, float maxX, float maxY) {
    const float dx = x1 - x0;
    const float dy = y1 - y0;
    float u1 = 0.0f;
    float u2 = 1.0f;

    if (!clipTest(-dx, x0, u1, u2)) {
        return false;
    }
    if (!clipTest(dx, maxX - x0, u1, u2)) {
        return false;
    }
    if (!clipTest(-dy, y0, u1, u2)) {
        return false;
    }
    if (!clipTest(dy, maxY - y0, u1, u2)) {
        return false;
    }

    if (u2 < 1.0f) {
        x1 = x0 + u2 * dx;
        y1 = y0 + u2 * dy;
    }
    if (u1 > 0.0f) {
        x0 = x0 + u1 * dx;
        y0 = y0 + u1 * dy;
    }

    return true;
}

int clampToRange(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

inline unsigned char blendOverOpaqueChannel(unsigned char src, unsigned char dst, unsigned char srcAlpha) {
    const unsigned int invSrcAlpha = 255u - srcAlpha;
    return static_cast<unsigned char>((static_cast<unsigned int>(src) * srcAlpha +
                                       static_cast<unsigned int>(dst) * invSrcAlpha + 127u) /
                                      255u);
}

inline unsigned char blendOverChannel(unsigned char src,
                                      unsigned char dst,
                                      unsigned char srcAlpha,
                                      unsigned char dstAlpha,
                                      unsigned char outAlpha,
                                      unsigned int invSrcAlpha) {
    if (outAlpha == 0) {
        return 0;
    }

    const unsigned int dstPremultiplied =
        (static_cast<unsigned int>(dst) * dstAlpha * invSrcAlpha + 127u) / 255u;
    const unsigned int outPremultiplied = static_cast<unsigned int>(src) * srcAlpha + dstPremultiplied;
    return static_cast<unsigned char>((outPremultiplied + outAlpha / 2u) / outAlpha);
}

inline unsigned char monoFromRgb(unsigned char r, unsigned char g, unsigned char b) {
    unsigned char maxValue = r;
    if (g > maxValue) {
        maxValue = g;
    }
    if (b > maxValue) {
        maxValue = b;
    }
    return maxValue;
}
} // namespace

void ofxHeadlessFbo::allocate(size_t w, size_t h, ofPixelFormat pixelFormat) {
    if (w <= 0 || h <= 0 || pixelFormat == OF_PIXELS_UNKNOWN) {
        return;
    }

    pixels.allocate(w, h, pixelFormat);
    this->w = w;
    this->h = h;
    this->pixelFormat = pixelFormat;
    this->numChannels = pixels.getNumChannels();
    markTextureDirty();
}

bool ofxHeadlessFbo::isAllocated() {
    return pixels.isAllocated();
}

void ofxHeadlessFbo::setColor(const ofColor &color) {
    this->color = color;
}

void ofxHeadlessFbo::clear(const ofColor &color) {
    pixels.setColor(color);
    markTextureDirty();
}

void ofxHeadlessFbo::readPixels(ofPixels &pixels) const {
    pixels = this->pixels;
}

void ofxHeadlessFbo::setFromPixels(ofPixels newPixels, size_t w, size_t h, ofPixelFormat pixelFormat) {
    if (w <= 0 || h <= 0 || pixelFormat == OF_PIXELS_UNKNOWN) {
        return;
    }

    pixels.allocate(w, h, pixelFormat);
    pixels = newPixels;
    this->w = w;
    this->h = h;
    this->pixelFormat = pixelFormat;
    this->numChannels = pixels.getNumChannels();
    markTextureDirty();
}

void ofxHeadlessFbo::setFill() {
    fill = true;
}

void ofxHeadlessFbo::setNoFill() {
    fill = false;
}

void ofxHeadlessFbo::enableAlphaBlending() {
    alphaBlending = true;
}

void ofxHeadlessFbo::disableAlphaBlending() {
    alphaBlending = false;
}

void ofxHeadlessFbo::draw(float x, float y) {
    if (!isAllocated()) {
        return;
    }

    const size_t currentW = pixels.getWidth();
    const size_t currentH = pixels.getHeight();
    const ofPixelFormat currentPixelFormat = pixels.getPixelFormat();

    if (!textureCache.isAllocated() || currentW != textureW || currentH != textureH ||
        currentPixelFormat != texturePixelFormat) {
        textureCache.allocate(pixels);
        textureW = currentW;
        textureH = currentH;
        texturePixelFormat = currentPixelFormat;
        textureDirty = true;
    }

    if (textureDirty) {
        textureCache.loadData(pixels);
        textureDirty = false;
    }

    textureCache.draw(x, y);
}

size_t ofxHeadlessFbo::getWidth() {
    return pixels.getWidth();
}

size_t ofxHeadlessFbo::getHeight() {
    return pixels.getHeight();
}

void ofxHeadlessFbo::drawPoint(float x, float y) {
    writePoint(x, y);
}

void ofxHeadlessFbo::writePoint(size_t x, size_t y) {
    if (!isAllocated() || x < 0 || y < 0 || x >= w || y >= h || numChannels == 0) {
        return;
    }
    writeSpanHFast(x, y, 1);
}

void ofxHeadlessFbo::writeSpanHFast(size_t x, size_t y, size_t span) {
    if (span == 0 || numChannels == 0) {
        return;
    }

    unsigned char *data = pixels.getData();
    if (data == nullptr) {
        return;
    }

    const size_t index = (y * w + x) * numChannels;
    unsigned char *dst = data + index;

    const unsigned char srcR = color.r;
    const unsigned char srcG = color.g;
    const unsigned char srcB = color.b;
    const unsigned char srcA = color.a;

    switch (pixelFormat) {
        case OF_PIXELS_RGBA:
            {
                if (!alphaBlending) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcR;
                        dst[1] = srcG;
                        dst[2] = srcB;
                        dst[3] = srcA;
                        dst += 4;
                    }
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcR;
                        dst[1] = srcG;
                        dst[2] = srcB;
                        dst[3] = 255;
                        dst += 4;
                    }
                    textureDirty = true;
                    return;
                }

                const unsigned int invSrcAlpha = 255u - srcA;
                for (size_t i = 0; i < span; ++i) {
                    const unsigned char dstA = dst[3];
                    const unsigned char outA = static_cast<unsigned char>(
                        srcA + (static_cast<unsigned int>(dstA) * invSrcAlpha + 127u) / 255u);
                    dst[0] = blendOverChannel(srcR, dst[0], srcA, dstA, outA, invSrcAlpha);
                    dst[1] = blendOverChannel(srcG, dst[1], srcA, dstA, outA, invSrcAlpha);
                    dst[2] = blendOverChannel(srcB, dst[2], srcA, dstA, outA, invSrcAlpha);
                    dst[3] = outA;
                    dst += 4;
                }
                textureDirty = true;
                return;
            }
        case OF_PIXELS_BGRA:
            {
                if (!alphaBlending) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcB;
                        dst[1] = srcG;
                        dst[2] = srcR;
                        dst[3] = srcA;
                        dst += 4;
                    }
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcB;
                        dst[1] = srcG;
                        dst[2] = srcR;
                        dst[3] = 255;
                        dst += 4;
                    }
                    textureDirty = true;
                    return;
                }

                const unsigned int invSrcAlpha = 255u - srcA;
                for (size_t i = 0; i < span; ++i) {
                    const unsigned char dstA = dst[3];
                    const unsigned char outA = static_cast<unsigned char>(
                        srcA + (static_cast<unsigned int>(dstA) * invSrcAlpha + 127u) / 255u);
                    dst[0] = blendOverChannel(srcB, dst[0], srcA, dstA, outA, invSrcAlpha);
                    dst[1] = blendOverChannel(srcG, dst[1], srcA, dstA, outA, invSrcAlpha);
                    dst[2] = blendOverChannel(srcR, dst[2], srcA, dstA, outA, invSrcAlpha);
                    dst[3] = outA;
                    dst += 4;
                }
                textureDirty = true;
                return;
            }
        case OF_PIXELS_RGB:
            {
                if (!alphaBlending) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcR;
                        dst[1] = srcG;
                        dst[2] = srcB;
                        dst += 3;
                    }
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcR;
                        dst[1] = srcG;
                        dst[2] = srcB;
                        dst += 3;
                    }
                    textureDirty = true;
                    return;
                }

                for (size_t i = 0; i < span; ++i) {
                    dst[0] = blendOverOpaqueChannel(srcR, dst[0], srcA);
                    dst[1] = blendOverOpaqueChannel(srcG, dst[1], srcA);
                    dst[2] = blendOverOpaqueChannel(srcB, dst[2], srcA);
                    dst += 3;
                }
                textureDirty = true;
                return;
            }
        case OF_PIXELS_BGR:
            {
                if (!alphaBlending) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcB;
                        dst[1] = srcG;
                        dst[2] = srcR;
                        dst += 3;
                    }
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcB;
                        dst[1] = srcG;
                        dst[2] = srcR;
                        dst += 3;
                    }
                    textureDirty = true;
                    return;
                }

                for (size_t i = 0; i < span; ++i) {
                    dst[0] = blendOverOpaqueChannel(srcB, dst[0], srcA);
                    dst[1] = blendOverOpaqueChannel(srcG, dst[1], srcA);
                    dst[2] = blendOverOpaqueChannel(srcR, dst[2], srcA);
                    dst += 3;
                }
                textureDirty = true;
                return;
            }
        case OF_PIXELS_GRAY:
            {
                const unsigned char srcMono = monoFromRgb(srcR, srcG, srcB);
                if (!alphaBlending) {
                    std::fill_n(dst, span, srcMono);
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    std::fill_n(dst, span, srcMono);
                    textureDirty = true;
                    return;
                }

                for (size_t i = 0; i < span; ++i) {
                    dst[i] = blendOverOpaqueChannel(srcMono, dst[i], srcA);
                }
                textureDirty = true;
                return;
            }
        case OF_PIXELS_GRAY_ALPHA:
            {
                const unsigned char srcMono = monoFromRgb(srcR, srcG, srcB);
                if (!alphaBlending) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcMono;
                        dst[1] = srcA;
                        dst += 2;
                    }
                    textureDirty = true;
                    return;
                }

                if (srcA == 0) {
                    return;
                }
                if (srcA == 255) {
                    for (size_t i = 0; i < span; ++i) {
                        dst[0] = srcMono;
                        dst[1] = 255;
                        dst += 2;
                    }
                    textureDirty = true;
                    return;
                }

                const unsigned int invSrcAlpha = 255u - srcA;
                for (size_t i = 0; i < span; ++i) {
                    const unsigned char dstA = dst[1];
                    const unsigned char outA = static_cast<unsigned char>(
                        srcA + (static_cast<unsigned int>(dstA) * invSrcAlpha + 127u) / 255u);
                    dst[0] = blendOverChannel(srcMono, dst[0], srcA, dstA, outA, invSrcAlpha);
                    dst[1] = outA;
                    dst += 2;
                }
                textureDirty = true;
                return;
            }
        default:
            break;
    }

    for (size_t i = 0; i < span; ++i) {
        pixels.setColor(x + i, y, this->color);
    }
    textureDirty = true;
}

void ofxHeadlessFbo::markTextureDirty() {
    textureDirty = true;
}

void ofxHeadlessFbo::drawLine(float x1, float y1, float x2, float y2) {
    if (!isAllocated() || w == 0 || h == 0) {
        return;
    }

    const float maxX = static_cast<float>(w - 1);
    const float maxY = static_cast<float>(h - 1);
    if (!clipLineToBounds(x1, y1, x2, y2, maxX, maxY)) {
        return;
    }

    const int xMax = static_cast<int>(w - 1);
    const int yMax = static_cast<int>(h - 1);
    int ix1 = clampToRange(static_cast<int>(std::lround(x1)), 0, xMax);
    int iy1 = clampToRange(static_cast<int>(std::lround(y1)), 0, yMax);
    int ix2 = clampToRange(static_cast<int>(std::lround(x2)), 0, xMax);
    int iy2 = clampToRange(static_cast<int>(std::lround(y2)), 0, yMax);

    if (ix1 == ix2) { // vertical line
        if (iy1 > iy2) {
            std::swap(iy1, iy2);
        }
        writeLineV(ix1, iy1, iy2 - iy1 + 1);
    } else if (iy1 == iy2) { // horizontal line
        if (ix1 > ix2) {
            std::swap(ix1, ix2);
        }
        writeLineH(ix1, iy1, ix2 - ix1 + 1);
    } else { // diagonal line
        writeLine(ix1, iy1, ix2, iy2);
    }
}

void ofxHeadlessFbo::writeLine(int x1, int y1, int x2, int y2) {
    const bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const int dx = x2 - x1;
    const int dy = std::abs(y2 - y1);
    int err = dx / 2;
    const int ystep = (y1 < y2) ? 1 : -1;

    int y = y1;
    for (int x = x1; x <= x2; ++x) {
        if (steep) {
            writePoint(static_cast<size_t>(y), static_cast<size_t>(x));
        } else {
            writePoint(static_cast<size_t>(x), static_cast<size_t>(y));
        }

        err -= dy;
        if (err < 0) {
            y += ystep;
            err += dx;
        }
    }
}

void ofxHeadlessFbo::writeLineH(int x, int y, int span) {
    if (!isAllocated() || span <= 0) {
        return;
    }
    if (y < 0 || y >= static_cast<int>(h)) {
        return;
    }

    long long start = static_cast<long long>(x);
    long long end = start + static_cast<long long>(span) - 1;
    if (end < 0 || start >= static_cast<long long>(w)) {
        return;
    }

    if (start < 0) {
        start = 0;
    }
    const long long maxX = static_cast<long long>(w) - 1;
    if (end > maxX) {
        end = maxX;
    }

    writeSpanHFast(static_cast<size_t>(start), static_cast<size_t>(y),
                   static_cast<size_t>(end - start + 1));
}

void ofxHeadlessFbo::writeLineV(int x, int y, int span) {
    if (!isAllocated() || span <= 0) {
        return;
    }
    if (x < 0 || x >= static_cast<int>(w)) {
        return;
    }

    long long start = static_cast<long long>(y);
    long long end = start + static_cast<long long>(span) - 1;
    if (end < 0 || start >= static_cast<long long>(h)) {
        return;
    }

    if (start < 0) {
        start = 0;
    }
    const long long maxY = static_cast<long long>(h) - 1;
    if (end > maxY) {
        end = maxY;
    }

    const size_t sx = static_cast<size_t>(x);
    for (long long row = start; row <= end; ++row) {
        writeSpanHFast(sx, static_cast<size_t>(row), 1);
    }
}

void ofxHeadlessFbo::drawRectangle(float x, float y, float w, float h) {
    if (w < 0) {
        x += w;
        w = -w;
    }
    if (h < 0) {
        y += h;
        h = -h;
    }

    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));
    const int x1 = static_cast<int>(std::ceil(x + w));
    const int y1 = static_cast<int>(std::ceil(y + h));
    const int spanW = x1 - x0;
    const int spanH = y1 - y0;
    if (spanW <= 0 || spanH <= 0) {
        return;
    }

    if (fill) {
        for (int row = y0; row < y1; ++row) {
            writeLineH(x0, row, spanW);
        }
    } else {
        writeLineH(x0, y0, spanW);
        writeLineH(x0, y1 - 1, spanW);
        writeLineV(x0, y0, spanH);
        writeLineV(x1 - 1, y0, spanH);
    }
}

void ofxHeadlessFbo::drawSquare(float x, float y, float d) {
    drawRectangle(x, y, d, d);
}

void ofxHeadlessFbo::drawSquareCentered(float x, float y, float d) {
    drawRectangle(x - d / 2, y - d / 2, d, d);
}

void ofxHeadlessFbo::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    if (fill) {
        int a, b, y, last;
        int _x1 = x1, _y1 = y1, _x2 = x2, _y2 = y2, _x3 = x3, _y3 = y3;

        // Sort coordinates by Y order (y3 >= y2 >= y1)
        if (_y1 > _y2) {
            std::swap(_y1, _y2);
            std::swap(_x1, _x2);
        }
        if (_y2 > _y3) {
            std::swap(_y3, _y2);
            std::swap(_x3, _x2);
        }
        if (_y1 > _y2) {
            std::swap(_y1, _y2);
            std::swap(_x1, _x2);
        }

        if (_y1 == _y3) { // Handle awkward all-on-same-line case as its own thing
            a = b = _x1;
            if (_x2 < a)
                a = _x2;
            else if (_x2 > b)
                b = _x2;
            if (_x3 < a)
                a = _x3;
            else if (_x3 > b)
                b = _x3;
            writeLineH(a, _y1, b - a + 1);
            return;
        }

        int dx12 = _x2 - _x1, dy12 = _y2 - _y1, dx13 = _x3 - _x1, dy13 = _y3 - _y1,
            dx23 = _x3 - _x2, dy23 = _y3 - _y2;
        long sa = 0, sb = 0;

        // For upper part of triangle, find scanline crossings for segments
        // 0-1 and 0-2.  If y2=y3 (flat-bottomed triangle), the scanline y2
        // is included here (and second loop will be skipped, avoiding a /0
        // error there), otherwise scanline y2 is skipped here and handled
        // in the second loop...which also avoids a /0 error here if y1=y2
        // (flat-topped triangle).
        if (_y2 == _y3)
            last = _y2; // Include y2 scanline
        else
            last = _y2 - 1; // Skip it

        for (y = _y1; y <= last; y++) {
            a = _x1 + sa / dy12;
            b = _x1 + sb / dy13;
            sa += dx12;
            sb += dx13;
            /* longhand:
            a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
            */
            if (a > b)
                std::swap(a, b);
            writeLineH(a, y, b - a + 1);
        }

        // For lower part of triangle, find scanline crossings for segments
        // 1-3 and 2-3.  This loop is skipped if y2=y3.
        sa = (long)dx23 * (y - _y2);
        sb = (long)dx13 * (y - _y1);
        for (; y <= _y3; y++) {
            a = _x2 + sa / dy23;
            b = _x1 + sb / dy13;
            sa += dx23;
            sb += dx13;
            /* longhand:
            a = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
            b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
            */
            if (a > b)
                std::swap(a, b);
            writeLineH(a, y, b - a + 1);
        }
    } else {
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x3, y3);
        drawLine(x3, y3, x1, y1);
    }
}

void ofxHeadlessFbo::drawCircle(float x, float y, float r) {
    if (r <= 0)
        r = 0;
    if (fill) {
        writeLineV(x, y - r, 2 * r + 1);
        fillCircleHelper(x, y, r, 3, 0);
    } else {
        int f = 1 - r;
        int ddF_x = 1;
        int ddF_y = -2 * r;
        int _x = 0;
        int _y = r;

        writePoint(x, y + r);
        writePoint(x, y - r);
        writePoint(x + r, y);
        writePoint(x - r, y);

        while (_x < _y) {
            if (f >= 0) {
                _y--;
                ddF_y += 2;
                f += ddF_y;
            }
            _x++;
            ddF_x += 2;
            f += ddF_x;

            writePoint(x + _x, y + _y);
            writePoint(x - _x, y + _y);
            writePoint(x + _x, y - _y);
            writePoint(x - _x, y - _y);
            writePoint(x + _y, y + _x);
            writePoint(x - _y, y + _x);
            writePoint(x + _y, y - _x);
            writePoint(x - _y, y - _x);
        }
    }
}

void ofxHeadlessFbo::circleHelper(int x0, int y0, int r, int corners) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (corners & 0x4) {
            writePoint(x0 + x, y0 + y);
            writePoint(x0 + y, y0 + x);
        }
        if (corners & 0x2) {
            writePoint(x0 + x, y0 - y);
            writePoint(x0 + y, y0 - x);
        }
        if (corners & 0x8) {
            writePoint(x0 - y, y0 + x);
            writePoint(x0 - x, y0 + y);
        }
        if (corners & 0x1) {
            writePoint(x0 - y, y0 - x);
            writePoint(x0 - x, y0 - y);
        }
    }
}

void ofxHeadlessFbo::fillCircleHelper(int x0, int y0, int r, int corners, int delta) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;
    int px = x;
    int py = y;

    delta++; // Avoid some +1's in the loop

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        // These checks avoid double-drawing certain lines
        if (x < (y + 1)) {
            if (corners & 1)
                writeLineV(x0 + x, y0 - y, 2 * y + delta);
            if (corners & 2)
                writeLineV(x0 - x, y0 - y, 2 * y + delta);
        }
        if (y != py) {
            if (corners & 1)
                writeLineV(x0 + py, y0 - px, 2 * px + delta);
            if (corners & 2)
                writeLineV(x0 - py, y0 - px, 2 * px + delta);
            py = y;
        }
        px = x;
    }
}

void ofxHeadlessFbo::drawRectRounded(float x, float y, float w, float h, float r) {
    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    if (r < 0)
        r = 0;

    int max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius)
        r = max_radius;

    if (fill) {
        drawRectangle(x + r, y, w - 2 * r, h);
        // draw four corners
        fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1);
        fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1);
    } else {
        writeLineH(x + r, y, w - 2 * r);         // Top
        writeLineH(x + r, y + h - 1, w - 2 * r); // Bottom
        writeLineV(x, y + r, h - 2 * r);         // Left
        writeLineV(x + w - 1, y + r, h - 2 * r); // Right
        // draw four corners
        circleHelper(x + r, y + r, r, 1);
        circleHelper(x + w - r - 1, y + r, r, 2);
        circleHelper(x + w - r - 1, y + h - r - 1, r, 4);
        circleHelper(x + r, y + h - r - 1, r, 8);
    }
}

void ofxHeadlessFbo::drawEllipse(float x, float y, float w, float h) {
    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    int x0 = x - w / 2.0, y0 = y + h / 2.0, x1 = x + w / 2.0, y1 = y - h / 2.0;
    long a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1;      /* values of diameter */
    long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
    long err = dx + dy + b1 * a * a, e2;                      /* error of 1.step */

    if (x0 > x1) {
        x0 = x1;
        x1 += a;
    } /* if called with swapped points */
    if (y0 > y1)
        y0 = y1;       /* .. exchange them */
    y0 += (b + 1) / 2; /* starting pixel */
    y1 = y0 - b1;
    a *= 8 * a;
    b1 = 8 * b * b;

    do {
        if (fill) {
            writeLineV(x1, y1, y0 - y1);
            if (x0 != x1)
                writeLineV(x0, y1, y0 - y1);
        } else {
            drawPoint(x1, y0); /*   I. Quadrant */ // bottom right
            drawPoint(x0, y0); /*  II. Quadrant */ // bottom left
            drawPoint(x0, y1); /* III. Quadrant */ // top left
            drawPoint(x1, y1); /*  IV. Quadrant */ // top right
        }
        e2 = 2 * err;
        if (e2 >= dx) {
            x0++;
            x1--;
            err += dx += b1;
        } /* x step */
        if (e2 <= dy) {
            y0++;
            y1--;
            err += dy += a;
        } /* y step */
    } while (x0 <= x1);

    while (y0 - y1 < b) {        /* too early stop of flat ellipses a=1 */
        drawPoint(x0 - 1, ++y0); /* -> complete tip of ellipse */
        drawPoint(x0 - 1, --y1);
    }
}
