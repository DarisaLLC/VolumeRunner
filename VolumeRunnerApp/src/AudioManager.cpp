//
//  AudioManager.cpp
//  VolumeRunnerApp
//
//  Created by Daniel Berio on 11/27/14.
//
//

#include "AudioManager.h"


//This shows how to use maximilian to build a polyphonic synth.

//These are the synthesiser bits
maxiOsc VCO1[6],VCO2[6],LFO1[6],LFO2[6];
maxiFilter VCF[6];
maxiEnvelope ADSR[6];

//These are the control values for the envelope

double adsrEnv[8]={1,5,0.125,100,0.125,200,0,1000};

//This is a bunch of control signals so that we can hear something

maxiOsc timer;//this is the metronome
int currentCount,lastCount,voice=0;//these values are used to check if we have a new beat this sample

//and these are some variables we can use to pass stuff around

double VCO1out[6],VCO2out[6],LFO1out[6],LFO2out[6],VCFout[6],ADSRout[6],mix,pitch[6];


AudioManager::AudioManager()
:
stepPhase(0.0)
{
    startThread();
}

void AudioManager::stop()
{
    stopThread();
}

void AudioManager::setup()
{
    
}

void AudioManager::update()
{
    double t = ofGetElapsedTimeMillis();
    lockThread();
    DelayedNote * cur = delayedNotes.head;
    while(cur)
    {
        DelayedNote * dn = cur;
        cur = cur->next;
        if((t-dn->t) > 300)
        {
            delayedNotes.remove(dn);
            notes.push_back(dn->note);
            delete dn;
        }
    }
    unlockThread();
}

void AudioManager::playNote( int note )
{
    DelayedNote * dn = new DelayedNote(note,ofGetElapsedTimeMillis());
    delayedNotes.insert(dn);
    
    //lockThread();
    //notes.push_back(note);
    //unlockThread();
}

float amts[6] = {1.0,1.0,1.0,1.0,1.0,0.5};

void AudioManager::play( double * output )
{
    
    
    mix=0;//we're adding up the samples each update and it makes sense to clear them each time first.
    
    //so this first bit is just a basic metronome so we can hear what we're doing.
    
    currentCount=(int)timer.phasor(8);//this sets up a metronome that ticks 8 times a second
    
    lockThread();
    
    for( int i = 0; i < notes.size(); i++ )
    {
        voice = voice%5;
        ADSR[voice].trigger(0, adsrEnv[0]);//trigger the envelope from the start
        pitch[voice]=notes[i];//voice+1;
        voice++;
        notes.clear();
    }
    
    ADSR[5].trigger(0, adsrEnv[0]);//trigger the envelope from the start
    pitch[5]=1;//voice+1;
    
    unlockThread();
    
    /*
    if (lastCount!=currentCount) {//if we have a new timer int this sample, play the sound
        
        if (voice==6) {
            voice=0;
        }
        
        ADSR[voice].trigger(0, adsrEnv[0]);//trigger the envelope from the start
        pitch[voice]=voice+1;
        voice++;
        
        lastCount=0;
        
    }*/
    
    //and this is where we build the synth
    
    for (int i=0; i<6; i++) {
        
        
        ADSRout[i]=ADSR[i].line(8,adsrEnv);//our ADSR env has 8 value/time pairs.
        
        LFO1out[i]=LFO1[i].sinebuf(0.2);//this lfo is a sinewave at 0.2 hz
        
        VCO1out[i]=VCO1[i].pulse(33*pitch[i],0.6);//here's VCO1. it's a pulse wave at 55 hz, with a pulse width of 0.6
        VCO2out[i]=VCO2[i].pulse((66*pitch[i])+LFO1out[i],0.2);//here's VCO2. it's a pulse wave at 110hz with LFO modulation on the frequency, and width of 0.2
        
        
        VCFout[i]=VCF[i].lores((VCO1out[i]+VCO2out[i])*0.5, 250+((pitch[i]+LFO1out[i])*1000), 10);//now we stick the VCO's into the VCF, using the ADSR as the filter cutoff
        
        mix+=VCFout[i]*ADSRout[i]/6*amts[i];//finally we add the ADSR as an amplitude modulator
        
        
    }
    
    
    output[0]=mix*0.5;//left channel
    output[1]=mix*0.5;//right channel
    
}
