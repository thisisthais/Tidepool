#pragma once

#include "ofMain.h"

#include "ofxBoids.h"

#include "IPVideoGrabber.h"

#include "ofxCv.h"
#include "ofxGui.h"

#include "ofxThreadedImageLoader.h"
#include "ofxPocoDirectoryWatcher.h"
#include "ImageFileWatcher.h"

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
    
    std::size_t cameraWidth = 1600;
    std::size_t cameraHeight = 1200;
    float area;

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
        std::size_t cameraWidth = 1600;
        std::size_t cameraHeight = 1200;
    
        ofxCv::ContourFinder contourFinder;
        ofxCv::RectTrackerFollower<Glow> tracker;
    
        ofxPanel gui;
        ofParameter<float> minArea, maxArea, threshold;
        
        ofImage img;
        ofParameter<int> radius;
        ofParameter<bool> useGaussian;
    
        vector<Glow> centers;
    
        ofxPocoDirectoryWatcher watcher;
        ofxThreadedImageLoader loader;
        ImageFileWatcher imageFileWatcher;
        vector<ofImage*> loadedImages;
    
        ofEasyCam cam;
        int boidNum;
        ofVec3f target;
        vector<SteeredVehicle> boids;
};
