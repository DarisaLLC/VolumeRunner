#pragma once

#include "ofMain.h"
#include "FloorManager.h"
#include "colormotor.h"

#define kNumMagma   5

class MagmaManager {
public:
    void init() {
        params.setName("MagmaManager");
        params.addFloat("Side speed").setRange(0, 10);
        params.addFloat("Up speed").setRange(-10, 10);
        params.addFloat("Gravity").setRange(-5, 5);
        params.startGroup("Display"); {
            params.addBool("Debug");
            params.addFloat("size").setRange(0, 20);
        } params.endGroup();
    }
    
    void addParams(msa::controlfreak::ParameterGroup &parentparams) {
        parentparams.add(&this->params);
    }
    
    void reset() {
        for(int i=0; i<kNumMagma; i++) magma[i].active = false;
    }
    
    bool fire(ofVec3f pos, float heading) {
        ofVec3f vel(0, (float)params["Up speed"], (float)params["Side speed"]);
        vel.rotate(heading, ofVec3f(0, 1, 0));
        
        float size = params["Display.size"];
        
        // find first available magma
        for(int i=0; i<kNumMagma; i++) {
            Magma &m = magma[i];
            if(!m.active) {
                m.active = true;
                m.mat.makeIdentityMatrix();
                m.mat.translate(pos);
//                m.mat.rotate(ofRandomf() * 360, ofRandomf(), ofRandomf(), ofRandomf());
//                m.mat.scale(size, size, size);
                
                m.vel = vel;
//                m.pos.set(pos.x, pos.y, pos.z);
//                m.rot.set(ofRandomf() * 360, ofRandomf() * 360, ofRandomf() * 360);
                return true;
            }
        }
        return false;
    }
    
    void updateRenderer(ofShader & shader) {
        float size = params["Display.size"];
        
        shader.setUniformMatrix4f("magma_mat_inv", mat_inv[0], kNumMagma);
//        shader.setUniform1f("magma_size", size);

//        for(int i=0; i<kNumMagma; i++) {
//            ofVec4f &p = pos[i];
//            shader.setUniform4f(<#const string &name#>, <#float v1#>, <#float v2#>, <#float v3#>, <#float v4#>)
//        }

    }
    
    void update(FloorManager &floorManager) {
        float gravity = params["Gravity"];
        
        for(int i=0; i<kNumMagma; i++) {
            Magma &m = magma[i];
            
            // if magma is active
            if(m.active) {
                m.mat.translate(m.vel);
                m.vel.y += gravity;

                ofVec3f pos = m.mat.getRowAsVec3f(3);
                if(pos.y < floorManager.getHeight(pos.x, pos.z)) m.active = false;
                else {
                    mat_inv[i] = m.mat.getInverse();
                }
            }
        }
    }
    
    void debugDraw() {
        if(params["Display.Debug"]) {
            for(int i=0; i<kNumMagma; i++) {
                Magma &m = magma[i];
                if(m.active) {
                    ofPushMatrix();
                    ofMultMatrix(m.mat);
                    ofDrawBox(ofVec3f(), 1);
                    ofPopMatrix();
                    
                }
            }
        }
    }
    
private:
    msa::controlfreak::ParameterGroup params;

    struct Magma {
        bool active;
        ofVec3f vel;
        ofVec3f rotSpeed;
        ofMatrix4x4 mat;
    } magma[kNumMagma];
    
    ofMatrix4x4 mat_inv[kNumMagma]; // doing this separate so I can send to shader in one go
};