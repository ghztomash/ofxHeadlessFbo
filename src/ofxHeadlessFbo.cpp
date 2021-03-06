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

void ofxHeadlessFbo::allocate(size_t w, size_t h, ofPixelFormat pixelFormat) {
    if (w <= 0 || h <= 0 || pixelFormat == OF_PIXELS_UNKNOWN) {
        return;
    }

    pixels.allocate(w, h, pixelFormat);
    this->w = w;
    this->h = h;
}

bool ofxHeadlessFbo::isAllocated() {
    return pixels.isAllocated();
}

void ofxHeadlessFbo::setColor(const ofColor & color) {
    this->color = color;
}

void ofxHeadlessFbo::clear(const ofColor &color) {
    pixels.setColor(color);
}

void ofxHeadlessFbo::readPixels(ofPixels & pixels) const {
    pixels = this->pixels;
}

void ofxHeadlessFbo::setFromPixels(ofPixels newPixels,size_t w, size_t h, ofPixelFormat pixelFormat) {
    if (w <= 0 || h <= 0 || pixelFormat == OF_PIXELS_UNKNOWN) {
        return;
    }

    pixels.allocate(w, h, pixelFormat);
    pixels = newPixels;
    this->w = w;
    this->h = h;
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
    if (this->isAllocated()) {
        ofTexture tex;
        tex.allocate(pixels);
        tex.draw(x, y);
    }
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
    // bound the coordinates
    if ((x < 0) || (y < 0) || (x >= w) || (y >= h))
        return;

    if (alphaBlending) {
        ofColor cb = pixels.getColor(x,y);
        ofColor ca = this->color;
        float aa = ca.a/255.0;
        float ab = cb.a/255.0;
        float a0 = aa + ab*(1.0-aa);
        float r0 = (ca.r*aa + cb.r*ab*(1.0-aa))/a0;
        float g0 = (ca.g*aa + cb.g*ab*(1.0-aa))/a0;
        float b0 = (ca.b*aa + cb.b*ab*(1.0-aa))/a0;
        ofColor c0(r0,g0,b0, a0*255);
        pixels.setColor(x, y, c0);
    } else {
        pixels.setColor(x, y, this->color);
    }
}

void ofxHeadlessFbo::drawLine(float x1, float y1, float x2, float y2){
    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (y1 < 0)
        y1 = 0;
    if (y2 < 0)
        y2 = 0;

    if (x1 == x2) { // vertical line
        if (y1 > y2 )
            std::swap(y1, y2);
        writeLineV(x1, y1, y2 - y1 + 1 );
    } else if (y1 == y2) { // horizontal line
        if (x1 > x2 )
            std::swap(x1, x2);
        writeLineH(x1, y1, x2 - x1 + 1);
    } else { // diagonal line
        writeLine(x1, y1, x2, y2);
    }
}

void ofxHeadlessFbo::writeLine(size_t x1, size_t y1, size_t x2, size_t y2) {
    bool steep = abs((int)(y2 - y1)) > abs((int)(x2 - x1));
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int dx, dy;
    dx = x2 - x1;
    dy = abs((int)(y2 - y1));

    int err = dx / 2;
    int ystep;

    if (y1 < y2) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x1 <= x2; x1++) {
        if (steep) {
            writePoint(y1, x1);
        } else {
            writePoint(x1, y1);
        }
        err -= dy;
        if (err < 0){
            y1 += ystep;
            err += dx;
        }
    }
}

void ofxHeadlessFbo::writeLineH(size_t x, size_t y, size_t w) {
    for (size_t i = 0; i < w; i++) {
        writePoint(x+i, y);
    }
}

void ofxHeadlessFbo::writeLineV(size_t x, size_t y, size_t h) {
    for (size_t i = 0; i < h; i++) {
        writePoint(x, y+i);
    }
}

void ofxHeadlessFbo::drawRectangle(float x, float y, float w, float h) {
    if (fill) {
        for (int i = x; i< x + w; i++){
            writeLineV(i, y, h);
        }
    } else {
        writeLineH(x, y, w);
        writeLineH(x, y+h-1, w);
        writeLineV(x, y, h);
        writeLineV(x+w-1, y, h);
    }
}

void ofxHeadlessFbo::drawSquare(float x, float y, float d) {
    drawRectangle(x, y, d, d);
}

void ofxHeadlessFbo::drawSquareCentered(float x, float y, float d) {
    drawRectangle(x-d/2, y-d/2, d, d);
}


void ofxHeadlessFbo::drawTriangle(float x1,float y1,float x2,float y2,float x3, float y3) {
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
        writeLineV(x, y-r, 2*r+1);
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
    if (w<0)
        w=0;
    if (h<0)
        h=0;
    if (r<0)
        r=0;

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
    if (w<0)
        w=0;
    if (h<0)
        h=0;
    int x0 = x - w/2.0, y0 = y + h/2.0, x1 = x+w/2.0, y1 = y-h/2.0;
    long a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1; /* values of diameter */
    long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
    long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

    if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
    if (y0 > y1) y0 = y1; /* .. exchange them */
    y0 += (b + 1) / 2; /* starting pixel */
    y1 = y0 - b1;
    a *= 8 * a;
    b1 = 8 * b * b;

    do {
        if(fill){
            writeLineV(x1, y1, y0-y1);
            if(x0!=x1)
                writeLineV(x0, y1, y0-y1);
        }else{
            drawPoint(x1, y0); /*   I. Quadrant */ //bottom right
            drawPoint(x0, y0); /*  II. Quadrant */ //bottom left
            drawPoint(x0, y1); /* III. Quadrant */ //top left
            drawPoint(x1, y1); /*  IV. Quadrant */ //top right
        }
        e2 = 2 * err;
        if (e2 >= dx) { x0++; x1--; err += dx += b1; } /* x step */
        if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
    } while (x0 <= x1);

    while (y0 - y1 < b) {  /* too early stop of flat ellipses a=1 */
        drawPoint(x0 - 1, ++y0); /* -> complete tip of ellipse */
        drawPoint(x0 - 1, --y1);
    }
}
