#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 1;

void Glow::setup(const cv::Rect& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    smooth = cur;
}

void Glow::update(const cv::Rect& track) {
    cur = toOf(track).getCenter();
    smooth.interpolate(cur, .5);
    all.addVertex(smooth);
}

void Glow::kill() {
    float curTime = ofGetElapsedTimef();
    if(startedDying == 0) {
        startedDying = curTime;
    } else if(curTime - startedDying > dyingTime) {
        dead = true;
    }
}

void Glow::draw() {
    ofPushStyle();
    float size = 16;
    ofSetColor(255);
    if(startedDying) {
        ofSetColor(ofColor::red);
        size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
    }
    ofNoFill();
    ofDrawCircle(cur, size);
    ofSetColor(color);
    all.draw();
    ofSetColor(255);
    ofDrawBitmapString(ofToString(label), cur);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetVerticalSync(true);

    grabber.setURI("http://192.168.4.70/mjpeg/1");

    // Connect to the stream.
    grabber.connect();
    
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(100);
    contourFinder.setAutoThreshold(true);
    
    // wait for half a frame before forgetting something
    tracker.setPersistence(15);
    // an object can move up to 50 pixels per frame
    tracker.setMaximumDistance(500);
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
        
    if (grabber.isFrameNew()) {
//        cameraPix = grabber.getPixels();
//
//        // Use or modify pixels in some way, e.g. invert the colors.
//        for (std::size_t x = 0; x < cameraPix.getWidth(); x++)
//        {
//            for (std::size_t y = 0; y < cameraPix.getHeight(); y++)
//            {
//                cameraPix.setColor(x, y, cameraPix.getColor(x, y).getInverted());
//            }
//        }
//
//        // Load the texture.
//        cameraTex.loadData(cameraPix);
        
        contourFinder.findContours(grabber);
        tracker.track(contourFinder.getBoundingRects());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
        
    // Draw the camera.
    grabber.draw(0, 0, cameraWidth, cameraHeight);
    
    // Draw the modified pixels if they are available.
//    if (cameraTex.isAllocated()) {
//        cameraTex.draw(0, cameraHeight, cameraWidth, cameraHeight);
//    }
    
//    contourFinder.draw();
    vector<Glow>& followers = tracker.getFollowers();
    for(int i = 0; i < followers.size(); i++) {
        followers[i].draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
