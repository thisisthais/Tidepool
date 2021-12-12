#include "ofApp.h"
#include <iostream>
#include <GLUT/GLUT.h>

using namespace ofxCv;
using namespace cv;

const float dyingTime = -1;

void Glow::setup(const cv::Rect& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    cur.x = 1600.0f - (1600.0f*cur.x/1024.0f);
    cur.y = 1200.0f - (1200.0f*cur.y/768.0f);
    area = track.area();
    smooth = cur;
}

void Glow::update(const cv::Rect& track) {
    cur = toOf(track).getCenter();
    cur.x = 1600.0f - (1600.0f*cur.x/1024.0f);
    cur.y = 1200.0f - (1200.0f*cur.y/768.0f);
    area = track.area();
//    smooth.interpolate(cur, .5);
//    all.addVertex(smooth);
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
//    all.draw();
    ofSetColor(255);
    ofDrawBitmapString(ofToString(cur), cur);
    ofDrawBitmapString(ofToString(area), cur.x, cur.y + 30.0f);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetVerticalSync(true);

    grabber.setURI("http://192.168.4.1/mjpeg/1");
    // Connect to the stream.
    grabber.connect();
    
//    contourFinder.setMinAreaRadius(10);
//    contourFinder.setMaxAreaRadius(100);
//    contourFinder.setAutoThreshold(true);
    
//    // wait for half a frame before forgetting something
//    tracker.setPersistence(15);
//    // an object can move up to 50 pixels per frame
//    tracker.setMaximumDistance(500);
    
    string share_data_path;
    share_data_path = ofFilePath::getAbsolutePath("images/");
    watcher.watch(share_data_path);
    imageFileWatcher.setup(&loader);
    
//    ofEnableSmoothing();
//    ofEnableAlphaBlending();
    //ofHideCursor();
    
//    glNewList(1, GL_COMPILE);
//    glutSolidSphere(1, 40, 40);
//    ofDrawCircle(150,150,100);

//    glEndList();
    
    boidNum = 100;
    target = ofVec3f(0, 0, 0);
    
    for (int i = 0; i < boidNum; i++) {
        SteeredVehicle v(ofRandom(100)-50, ofRandom(100)-50, ofRandom(100)-50);
        v.maxForce = 0.5f;
        v.inSightDist = 60.0f;
        boids.push_back(v);
    }
    
//    cam.setDistance(500);
//    GLfloat color[] = { 1.0, 0.2, 0.2 };
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glLightfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    
    gui.setup();
    gui.add(minArea.set("Min area", 10, 1, 100));
    gui.add(maxArea.set("Max area", 200, 1, 500));
    gui.add(threshold.set("Threshold", 55, 0, 255));
    useGaussian = false;
    gui.add(useGaussian.set("Use Gaussian", false));
    gui.add(radius.set("Radius", 50, 0, 100));
    contourFinder.setSimplify(true);
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
        
//        contourFinder.findContours(grabber);
        
        ofxCv::copy(grabber, img);
        if(useGaussian) {
            ofxCv::GaussianBlur(img, radius);
        } else {
            ofxCv::blur(img, radius);
        }
        img.update();
        
        contourFinder.setMinAreaRadius(minArea);
        contourFinder.setMaxAreaRadius(maxArea);
        contourFinder.setThreshold(threshold);

        contourFinder.findContours(img);
        tracker.track(contourFinder.getBoundingRects());
    }
    
    for (int i = 0; i < boidNum; i++) {
        boids[i].flock(boids);
        boids[i].update();
        boids[i].wrap(300, 300, 300);
    }
    
    int oldNum = imageFileWatcher.getNumLoaded();
    imageFileWatcher.update();
    int num = imageFileWatcher.getNumLoaded();
    if (oldNum != num) {
        loadedImages = imageFileWatcher.getLoaded();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
        
    // Draw the camera.
    ofPushMatrix();
    ofTranslate(cameraWidth, 0, 0);
    ofScale(-1, 1, 0);
    grabber.draw(0, 0, cameraWidth, cameraHeight);
    ofPopMatrix();

    // Draw the modified pixels if they are available.
    if (cameraTex.isAllocated()) {
//        cameraTex.draw(0, cameraHeight, cameraWidth, cameraHeight);
    }
    
    if(img.isAllocated()) {
//        ofPushMatrix();
//        ofScale(1, -1, 1);
//        ofTranslate(0, -cameraHeight, 0);
//        img.draw(0, 0, cameraWidth, cameraHeight);
//        ofPopMatrix();
    }
    
    for(int i = 0; i < loadedImages.size(); i++) {
        ofSetColor(0xffffff);
        loadedImages[i]->draw(200*1, 200, 200, 200);
    }
    
    contourFinder.draw();
    
    centers = tracker.getFollowers();
    for(int i = 0; i < centers.size(); i++) {
        centers[i].draw();
    }
    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
    
    cam.begin();
    
    for (int i = 0; i < boidNum; i++) {
        glPushMatrix();
        glTranslatef(boids[i].position.x, boids[i].position.y, boids[i].position.z);
        
        GLfloat color[] = { 0.8, 0.2, 0.2, 1.0 };
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glCallList(1);
        glPopMatrix();
    }
    
    cam.end();
    
    ofSetColor(255);
    ofDrawBitmapString(ofToString(grabber.getWidth()), 400, 400);
    
    gui.draw();
    
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
    vector<string> msgs = ofSplitString(msg.message, "|");

    if (msgs.size() > 0) {
        if (msgs[0] == "Added") {
            imageFileWatcher.addFile(msgs[1]);
        } else if (msgs[0] == "Modified") {
            imageFileWatcher.modifyFile(msgs[1]);
        }
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
