#pragma once

#include "ofMain.h"
#include "ofColor.h"
#include "ofPixels.h"

/// \file
/// ofPixels is an object for working with blocks of pixels, those pixels can
/// be copied from an image that you've loaded, something that you've drawn
/// using ofGraphics, or a ofVideoGrabber instance.
///
/// You can create an image from pixels, using on ofPixels object like so:
///
/// ~~~~{.cpp}
/// ofPixels p;
/// ofLoadImage(p, "pathToImage.jpg");
/// ~~~~
///
/// ofPixels represents pixels data on the CPU as opposed to an ofTexture
/// which represents pixel data on the GPU. 

class ofxHeadlessFbo {
    public:

    /// \brief Allocates space for pixel data
	///
	/// The pixelFormat can be one of the following:
	///
	///     OF_PIXELS_RGB
	///     OF_PIXELS_RGBA
	///     OF_PIXELS_BGRA
	///     OF_PIXELS_MONO
	///
	/// \param w Width of pixel array
	/// \param h Height of pixel array
	/// \param pixelFormat ofPixelFormat defining number of channels per pixel
	void allocate(size_t w, size_t h, ofPixelFormat pixelFormat);
    
    /// \brief Get whether memory has been allocated for an ofPixels object or not
    ///
    /// Many operations like copying pixels, etc, automatically allocate
    /// the memory needed, but it's sometimes good to check.
    bool isAllocated();

    /// \brief Sets the draw color with r,g,b,a 0-255.
    ///
    /// For alpha (transparency), you must first enable transparent blending
    /// (turned off by default for performance reasons) with
    /// ofEnableAlphaBlending()
    ///
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     ofEnableAlphaBlending();    // turn on alpha blending
    ///     ofSetColor(255,0,0,127);    // red, 50% transparent
    ///     ofDrawRectangle(20,20,100,100);
    ///     ofDisableAlphaBlending();   // turn it back off, if you don't need it
    /// }
    /// ~~~~
    void setColor(const ofColor & color);

    /// \brief fill the buffer with a single color.
    void clear(const ofColor &color);
    
	/// \brief Read current data from the CPU into pixels.
	///
	/// \param pixels Target ofPixels reference.
	void readPixels(ofPixels & pixels) const;

    /// /brief Set the internal pixels from existing pixel data
    /// 
    /// \param newPixels The new pixel array  
    /// \param w Width of pixel array
	/// \param h Height of pixel array
	/// \param pixelFormat ofPixelFormat defining number of channels per pixel
    void setFromPixels(ofPixels newPixels,size_t w, size_t h, ofPixelFormat pixelFormat);

    /// \brief draw the current data as texture.
    void draw(float x, float y);

    /// Draws a point: (x1,y1).
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawPoint(10,10);
    /// }
    /// ~~~~
    void drawPoint(float x, float y);
    
    /// Draws a line between two points: (x1,y1),(x2,y2).
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawLine(10,10,100,100);
    /// }
    /// ~~~~
    void drawLine(float x1, float y1, float x2, float y2);

    /// \brief Draws a rectangle from point x,y with a given width and height.
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawRect(10,10,100,100);
    /// }
    /// ~~~~
    void drawRectangle(float x, float y, float w, float h);
    /// \brief Draws a square from point x,y with a given dimension.
    void drawSquare(float x, float y, float d);
    /// \brief Draws a square centered at point x,y with a given dimension.
    void drawSquareCentered(float x, float y, float d);

    /// \brief Draws a triangle, with the three points: (x1,y1),(x2, y2),(x3, y3).
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawTriangle(50,10,10,40,90,40);
    /// }
    /// ~~~~
    void drawTriangle(float x1,float y1,float x2,float y2,float x3, float y3);

    /// \brief Draws a circle, centered at x,y, with a given radius.
    ///
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///      hfbo.drawCircle(150,150,100);
    /// }
    /// ~~~~
    void drawCircle(float x, float y, float r);

    /// \brief Draws a rectangle from point X, Y with a given width, height and radius of
    /// rounded corners.
    ///
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawRectRounded(10, 10, 100, 100, 10);
    /// }
    /// ~~~~
    void drawRectRounded(float x, float y, float w, float h, float r);

    /// \brief Draws an ellipse from point (x,y) with a given width (w) and height (h).
    /// ~~~~{.cpp}
    /// void ofApp::draw(){
    ///     hfbo.drawEllipse(10,10,50,30);
    /// }
    /// ~~~~
    void drawEllipse(float x, float y, float w, float h);

    void setFill();
    void setNoFill();

    size_t getWidth();
    size_t getHeight();
    
    private:
    void writePoint(size_t x, size_t y);
    void writeLine(size_t x1, size_t y1, size_t x2, size_t y2);
    void writeLineH(size_t x, size_t y, size_t w);
    void writeLineV(size_t x, size_t y, size_t h);
    void circleHelper(int x0, int y0, int r, int corners);
    void fillCircleHelper(int x0, int y0, int r, int corners, int delta);

    size_t w;
    size_t h;
    bool fill = true;
    ofPixels pixels;
    ofColor color;
};
