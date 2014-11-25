#include "ofApp.h"
#include "colormotor.h"
#include "AnimSys.h"
#include "RunningSkeleton.h"

cm::FileWatcher * reloader;

ofBoxPrimitive box;



//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // initialize the dude before hand because of the parameters in the walking animation
    dude.init();
    
    
    params.addFloat("FPS").setRange(0, 60).setClamp(false);
    params.startGroup("Display"); {
        params.addBool("Debug skeleton");
        params.addBool("Random bones");
    } params.endGroup();
    
    
    params.startGroup("Shader"); {
        params.startGroup("Test box"); {
            params.addFloat("posx").setRange(-100, 100).setIncrement(1.0).setSnap(true);
            params.addFloat("posy").setRange(-100, 100).setIncrement(1.0).setSnap(true);
            params.addFloat("posz").setRange(-100, 100).setIncrement(1.0).setSnap(true);
            params.addFloat("rotx").setRange(-180, 180).setIncrement(1.0).setSnap(true);
            params.addFloat("roty").setRange(-180, 180).setIncrement(1.0).setSnap(true);
            params.addFloat("rotz").setRange(-180, 180).setIncrement(1.0).setSnap(true);
            params.addFloat("scalex").setRange(0, 50).setIncrement(0.1).setSnap(true);
            params.addFloat("scaley").setRange(0, 50).setIncrement(0.1).setSnap(true);
            params.addFloat("scalez").setRange(0, 50).setIncrement(0.1).setSnap(true);
        } params.endGroup();
        
        
        params.startGroup("View"); {
            params.addFloat("distance").setRange(0, 100).setIncrement(1.0);
            params.addFloat("rotx").setRange(-180, 180).setIncrement(1.0);
            params.addFloat("roty").setRange(-180, 180).setIncrement(1.0);
        } params.endGroup();
        
    } params.endGroup();
    
    dude.addParams(params);
    params.loadXmlValues();
    
    gui.addPage(params);
    gui.setDefaultKeys(true);
    gui.show();
    
    reloader = new cm::FileWatcher(ofToDataPath("shaders/raytrace_test.frag"),500);
    reloader->startThread();
    
    shaderRayTracer.load("", "shaders/raytrace_test.frag");
    
    cam = new ofCamera();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    dude.updateParams(params);
    dude.update();
    
    camera.rotx = params["Shader.View.rotx"];
    camera.roty = params["Shader.View.roty"];
    camera.distance = params["Shader.View.distance"];
    camera.update(dude.position,0.1);//Vec3(0,0,0));
}

//--------------------------------------------------------------
void ofApp::draw(){
    params["FPS"] = ofGetFrameRate();
    /*camera.begin();
     box.draw();
     camera.end();
     */
    
    if( reloader->hasFileChanged() )
    {
        shaderRayTracer.load("", "shaders/raytrace_test.frag");
    }
    
    
    gfx::enableDepthBuffer(false);
    gfx::setIdentityTransform();
    
    shaderRayTracer.begin();
    shaderRayTracer.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
    shaderRayTracer.setUniform1f("time", ofGetElapsedTimef());
    
    shaderRayTracer.setUniform3f("box_pos", params["Shader.Test box.posx"], params["Shader.Test box.posy"], params["Shader.Test box.posz"]);
    shaderRayTracer.setUniform3f("box_rot", ofDegToRad(params["Shader.Test box.rotx"]), ofDegToRad(params["Shader.Test box.roty"]), ofDegToRad(params["Shader.Test box.rotz"]));
    shaderRayTracer.setUniform3f("box_scale", params["Shader.Test box.scalex"], params["Shader.Test box.scaley"], params["Shader.Test box.scalez"]);
    
    dude.updateRenderer(shaderRayTracer);
    camera.updateRenderer(shaderRayTracer);
    
    //shaderRayTracer.setUniformMatrix4f("invViewMatrix", viewMat.getInverse());//camera.getModelViewMatrix());
    //shaderRayTracer.setUniform1f("tanHalfFov", tan(ofDegToRad(cam->getFov()/2)));
    //ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    drawUVQuad();
    shaderRayTracer.end();
     
     
    /*
     cam.setTransformMatrix(viewMat);
     cam.begin();
     box.draw();
     cam.end();*/
    
    camera.apply();

    
    if(params["Display.Debug skeleton"]) {
        dude.debugDraw();
        gfx::drawAxis(M44::identityMatrix(),3.0);
    }
    
    ofSetupScreen();
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), ofGetWidth() - 100, 20);
}

//--------------------------------------------------------------
void ofApp::exit(){
    //    params.saveXmlValues();
    reloader->stopThread();
    delete reloader;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 's': params.saveXmlValues();
        case 'l': params.loadXmlValues();
            //        case 'S': params.saveXmlSchema();
            //        case 'L': params.loadXmlSchema();
    }
    
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
