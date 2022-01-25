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

    pixels.setColor(x, y, this->color);
}

void ofxHeadlessFbo::drawLine(float x1, float y1, float x2, float y2){
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

        // Sort coordinates by Y order (y3 >= y2 >= y1)
        if (y1 > y2) {
            std::swap(y1, y2);
            std::swap(x1, x2);
        }
        if (y2 > y3) {
            std::swap(y3, y2);
            std::swap(x3, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
            std::swap(x1, x2);
        }

        if (y1 == y3) { // Handle awkward all-on-same-line case as its own thing
            a = b = x1;
            if (x2 < a)
                a = x2;
            else if (x2 > b)
                b = x2;
            if (x3 < a)
                a = x3;
            else if (x3 > b)
                b = x3;
            writeLineH(a, y1, b - a + 1);
            return;
        }

        int dx12 = x2 - x1, dy12 = y2 - y1, dx13 = x3 - x1, dy13 = y3 - y1,
            dx23 = x3 - x2, dy23 = y3 - y2;
        long sa = 0, sb = 0;

        // For upper part of triangle, find scanline crossings for segments
        // 0-1 and 0-2.  If y2=y3 (flat-bottomed triangle), the scanline y2
        // is included here (and second loop will be skipped, avoiding a /0
        // error there), otherwise scanline y2 is skipped here and handled
        // in the second loop...which also avoids a /0 error here if y1=y2
        // (flat-topped triangle).
        if (y2 == y3)
            last = y2; // Include y2 scanline
        else
            last = y2 - 1; // Skip it

        for (y = y1; y <= last; y++) {
            a = x1 + sa / dy12;
            b = x1 + sb / dy13;
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
        sa = (long)dx23 * (y - y2);
        sb = (long)dx13 * (y - y1);
        for (; y <= y3; y++) {
            a = x2 + sa / dy23;
            b = x1 + sb / dy13;
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
