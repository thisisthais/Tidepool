#pragma once

#include "ofMain.h"
#include "IPVideoGrabber.h"
#include "ofxCv.h"
#include "ofxGui.h"

class Glow : public ofxCv::RectFollower {
protected:
    ofColor color;
    ofVec3f cur, smooth;
    float startedDying;
    ofPolyline all;
public:
    Glow()
        :startedDying(0) {
    }
    void setup(const cv::Rect& track);
    void update(const cv::Rect& track);
    void kill();
    void draw();
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofx::Video::IPVideoGrabber grabber;

        // These are modified copies of the pixels and the textures.
        ofPixels cameraPix;
        ofTexture cameraTex;
        
        // Set the display size for the cameras.
        std::size_t cameraWidth = 1024;
        std::size_t cameraHeight = 768;
    
        ofxCv::ContourFinder contourFinder;
        ofxCv::RectTrackerFollower<Glow> tracker;
    
        ofxPanel gui;
        ofParameter<float> minArea, maxArea, threshold;
        
        ofImage img;
        ofParameter<int> radius;
        ofParameter<bool> useGaussian;
};
