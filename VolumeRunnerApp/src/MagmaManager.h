#pragma once

#include "ofMain.h"
#include "FloorManager.h"
#include "colormotor.h"

#define kNumMagma   5

class MagmaManager {
public:
    void init() {
        params.setName("MagmaManager");
        params.startGroup("Physics"); {
            params.addFloat("Side speed").setRange(0, 10);
            params.addFloat("Up speed").setRange(-10, 10);
            params.addFloat("Speed lerp speed").setRange(0, 1);
            params.addFloat("Grow speed");
            params.addFloat("Gravity").setRange(-5, 5);
        } params.endGroup();
        params.startGroup("Display"); {
            params.addBool("Debug");
            params.addFloat("Size").setRange(0, 20);
            params.addInt("Weight").setRange(0, 100);
            params.addInt("Radius").setRange(0, 100);
        } params.endGroup();
    }
    
    void addParams(msa::controlfreak::ParameterGroup &parentparams) {
        parentparams.add(&this->params);
    }
    
    void reset() {
        for(int i=0; i<kNumMagma; i++) magma[i].active = false;
    }
    
    bool fire(ofVec3f pos, float heading, int dir) {
        ofVec3f vel(0, (float)params["Physics.Up speed"], (float)params["Physics.Side speed"]);
        vel.rotate(heading, ofVec3f(0, 1, 0));
        
        float size = params["Display.Size"];
        
        // find first available magma
        for(int i=0; i<kNumMagma; i++) {
            Magma &m = magma[i];
            if(!m.active) {
                m.active = true;
                m.pos = pos;
                m.size = 0;
                m.targetvel = vel;
                m.dir = dir;
                m.vel.set(0, 0, 0);
                return true;
            }
        }
        return false;
    }
    
    void updateRenderer(ofShader & shader) {
        shader.setUniform4fv("magma", (float*)&pos, kNumMagma * 4);
    }
    
    void update(FloorManager &floorManager) {
        float gravity = params["Physics.Gravity"];
        float speedLerpSpeed = params["Physics.Speed lerp speed"];
        float size = params["Display.Size"];
        float growSpeed = params["Physics.Grow speed"];
        
        int weight = params["Display.Weight"];
        int radius = params["Display.Radius"];
        
        for(int i=0; i<kNumMagma; i++) {
            Magma &m = magma[i];
            
            // if magma is active
            if(m.active) {
                //                m.mat.translate(m.vel);
                m.pos += m.vel;
                m.targetvel.y += gravity;
                m.vel += (m.targetvel - m.vel) * speedLerpSpeed;
                
                m.size += (size - m.size) * growSpeed;

                if(m.pos.y < floorManager.getHeight(m.pos.x, m.pos.z)) {
                    m.active = false;
                    floorManager.dynamicFloor().incColorAtWorldPos(m.pos.x, m.pos.z, weight * m.dir, radius);
                }
                
                pos[i] = m.pos;
                pos[i].w = m.active ? m.size : 0;
            }
        }
    }
    
    void debugDraw() {
        if(params["Display.Debug"]) {
            for(int i=0; i<kNumMagma; i++) {
                //                Magma &m = magma[i];
                //                if(m.active) {
                ofDrawBox(pos[i], pos[i].w);
                //                }
            }
        }
    }
    
private:
    msa::controlfreak::ParameterGroup params;
    
    struct Magma {
        bool active;
        ofVec3f pos;
        ofVec3f vel;
        ofVec3f targetvel;
        int dir;
        float size;
    } magma[kNumMagma];
    
    ofVec4f pos[kNumMagma];    //xyz: pos, w: size (if 0, not active)
};