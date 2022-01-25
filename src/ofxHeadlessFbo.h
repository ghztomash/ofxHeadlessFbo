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

    /// \brief Clear all the data from the ofPixels objects.
	/// After calling this you'll need to allocate()
    /// the ofPixels object again to use it.
    void clear();

    /// \brief fill the buffer with a single color.
    void setBackground(const ofColor &color);
    
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

    /// \section Read Pixel Data
	/// \brief Read current data from the CPU into pixels.
	///
	/// \param pixels Target ofPixels reference.
	void readPixels(ofPixels & pixels) const;

    private:
    size_t w;
    size_t h;
    ofPixels pixels;
    ofColor color;
    bool allocated;
};
