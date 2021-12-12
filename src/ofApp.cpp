#include "ofApp.h"
#include <iostream>
#include <GLUT/GLUT.h>

using namespace ofxCv;
using namespace cv;

const float dyingTime = -1;

void Glow::setup(const cv::Rect& track) {
    area = track.area();
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    cur.x = xStart + (ofGetWindowWidth() - (ofGetWindowWidth()*cur.x/1024.0f))*xScale;
    cur.y = yStart + (ofGetWindowHeight()*cur.y/768.0f)*yScale;
    active = true;
    smooth = cur;
}

void Glow::update(const cv::Rect& track) {
    area = track.area();
    cur = toOf(track).getCenter();
    cur.x = xStart + (ofGetWindowWidth() - (ofGetWindowWidth()*cur.x/1024.0f))*xScale;
    cur.y = yStart + (ofGetWindowHeight()*cur.y/768.0f)*yScale;
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
//    ofSetColor(255);
    if(startedDying) {
        ofSetColor(ofColor::red);
        size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
    }
    ofNoFill();
    ofDrawCircle(cur, size);
//    ofSetColor(color);
//    all.draw();
//    ofSetColor(255);
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
    tracker.setPersistence(15);
//    // an object can move up to 500 pixels per frame
//    tracker.setMaximumDistance(500);
    
    string share_data_path;
    share_data_path = ofFilePath::getAbsolutePath("images/");
    //loadedImages = imageFileWatcher.getLoaded();
    
    ofDirectory dir(share_data_path);
    //only show png files
    dir.allowExt("png");
    //populate the directory object
    dir.listDir();

    //go through and print out all the paths
    images = new ofImage[dir.size()];
    for(int i = 0; i < dir.size(); i++){
        images[i].load(dir.getPath(i));
        loadedImages.push_back(&images[i]);
    }
//    loadedImages =

    watcher.watch(share_data_path);
    imageFileWatcher.setup(&loader);
    
//    ofEnableSmoothing();
//    ofEnableAlphaBlending();
    //ofHideCursor();
    
//    glNewList(1, GL_COMPILE);
//    glutSolidSphere(1, 40, 40);
//    ofDrawCircle(150,150,100);

//    glEndList();
    
    boidNum = 150;
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
    gui.add(threshold.set("Threshold", 130, 0, 255));
    useGaussian = false;
    gui.add(useGaussian.set("Use Gaussian", false));
    gui.add(radius.set("Radius", 0, 0, 100));
    gui.add(xStart.set("xStart", -85, -500, 500));
    gui.add(xScale.set("xScale", 1, 0.8, 1.4));
    gui.add(yStart.set("yStart", 10, -500, 500));
    gui.add(yScale.set("yScale", 1.15, 0.8, 1.4));
    contourFinder.setSimplify(true);
    largestCenter = *(new Glow());
    largestCenter.active = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
        
    if (grabber.isFrameNew()) {
        cameraPix = grabber.getPixels();

        // Use or modify pixels in some way, e.g. invert the colors.
//        for (std::size_t x = 0; x < cameraPix.getWidth(); x++)
//        {
//            for (std::size_t y = 0; y < cameraPix.getHeight(); y++)
//            {
//                cameraPix.setColor(x, y, cameraPix.getColor(x, y) - 100);
//            }
//        }

        // Load the texture.
        cameraTex.loadData(cameraPix);
        
//        contourFinder.findContours(grabber);
        
//        ofxCv::copy(grabber, img);
        ofxCv::copy(cameraPix, img);
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
//    ofVec2f attractor(ofGetMouseX()-ofGetWidth()*0.5, (ofGetHeight()*.5) - ofGetMouseY());
    
        ofVec2f attractor(largestCenter.cur.x - ofGetWidth()*0.5,ofGetHeight()*0.5 - largestCenter.cur.y );

    for (int i = 0; i < boidNum; i++) {
        float type = float(
                           
                           float( i % loadedImages.size())
                           
                           /float (loadedImages.size()));
        boids[i].mass = 0.5 + type * 3.;
        boids[i].flock(boids, attractor, largestCenter.active);
//        boids[i].flock(boids, ofGetMouseX(), ofGetMouseY());
        boids[i].update();
        boids[i].bounce(1500, 900, 20);
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
        
    // Draw the camera.
//    ofPushMatrix();
//    ofTranslate(cameraWidth, 0, 0);
//    ofScale(-1, 1, 0);
//    grabber.draw(0, 0, cameraWidth, cameraHeight);
//    ofPopMatrix();

    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    // Draw the modified pixels if they are available.
    if (cameraTex.isAllocated()) {
        ofPushMatrix();
        ofTranslate(cameraWidth, 0, 0);
        ofScale(-1, 1, 0);
        cameraTex.draw(0, 0, cameraWidth, cameraHeight);
        ofPopMatrix();
    }
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//
//    if(img.isAllocated()) {
//        ofPushMatrix();
//        ofTranslate(cameraWidth, 0, 0);
//        ofScale(-1, 1, 0);
//        img.draw(0, 0, cameraWidth, cameraHeight);
//        ofPopMatrix();
//    }
    
    for(int i = 0; i < loadedImages.size(); i++) {
        ofSetColor(0xffffff);
//        loadedImages[i]->draw(200*1, 200, 200, 200);
    }
    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
    
    cam.begin();
    
    ofSetColor(255, 200,255);
    ofVec2f attractor(ofGetMouseX()-ofGetWidth()*0.5, (ofGetHeight()*.5) - ofGetMouseY());

    ofDrawCircle(attractor.x,attractor.y,3);
    for (int i = 0; i < boidNum; i++) {
        glPushMatrix();
        float angle = boids[i].velocity.angle(ofVec2f(1.,0));
        glTranslatef(boids[i].position.x, boids[i].position.y, boids[i].position.z);
        ofRotateDeg(angle);

        ofSetColor(200, 255,255);
        ofDrawCircle(0,0,1 + (boids[i].position.z * 0.05));
        
        ofDrawLine(0,0,-boids[i].velocity.x*2.0,-boids[i].velocity.y*2.0);
        int imgi = i % loadedImages.size();
        ofSetColor(0xffffff);
        float size = 30 + boids[i].position.z*1.0;
        loadedImages[imgi]->draw(-size/2,-size/2, size, size);
        
//        glMaterialfv(RONT_AGL_FND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
//        glCallList(1);
        glPopMatrix();
    }
    
    cam.end();
    
//    ofSetColor(255);
//    ofDrawBitmapString(ofToString(grabber.getWidth()), 400, 400);
    ofPushMatrix();
    ofTranslate(cameraWidth, 0, 0);
    ofScale(-1, 1, 0);
    contourFinder.draw();
    ofPopMatrix();
    centers = tracker.getFollowers();
    largestCenter.active = false;
    for(int i = 0; i < centers.size(); i++) {
        centers[i].xStart = xStart;
        centers[i].xScale = xScale;
        centers[i].yStart = yStart;
        centers[i].yScale = yScale;
        if(i==0){
            largestCenter= centers[i];
            largestCenter.active = true;
        }else
            if(largestCenter.area <centers[i].area)
            {
                largestCenter = centers[i];
                largestCenter.active = true;

        }
        ofSetColor(255);
        centers[i].draw();
    }
    ofSetColor(0,255,0);
    largestCenter.draw();
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
