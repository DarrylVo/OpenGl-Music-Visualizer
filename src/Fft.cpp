/*
 * fft.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: darryl
 */
#include "Fft.hpp"
#include <iostream>
using namespace std;
void Fft::init(string songpath) {
	for(int i = 0; i < FFT_WINDOW_SIZE/2;i++) {
		out[i] = 0;
	}

	result = FMOD::System_Create(&system);

	result = system->init(32, FMOD_INIT_NORMAL, NULL);
	cout << "initializing fmod...." <<endl;
	cout<< FMOD_ErrorString(result)<< endl;

	result = system->createSound(&songpath[0u],FMOD_DEFAULT, 0, &song);
	cout << "creating sound... " <<endl;
	cout<< FMOD_ErrorString(result)<< endl;


	result = song->setMode(FMOD_LOOP_NORMAL);


	cout << "playing sound... on channel" << endl;
	result = system->playSound(song, 0, true, &channel);

	cout<< FMOD_ErrorString(result)<< endl;

	cout << "creating analyzer.. " << endl;

	result = system->createDSPByType(FMOD_DSP_TYPE_FFT,&dsp);
	cout<< FMOD_ErrorString(result)<< endl;

	cout<< "changing fft window size to  128..." <<endl;
	result = dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, FFT_WINDOW_SIZE);
	cout<< FMOD_ErrorString(result)<< endl;

	cout << "adding analyzer to channel...." <<endl;
	result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL,dsp);
	cout<< FMOD_ErrorString(result)<< endl;
	dsp->setActive(true);

}

void Fft::playSong() {
	channel->setPaused(false);
}

void Fft:: pauseSong() {
	channel->setPaused(true);
}

void Fft::analyze() {
	system->update();
	result = dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);
	for (int bin = 0; bin < fft->length/2; bin++) {
		out[bin] = (fft->spectrum[1][bin]+fft->spectrum[0][bin])/2.0f;
	}
	beatDetected = false;
	time_t seconds = time(NULL);
	if ((out[2] >= beatThresholdVolume) && beatLastTick == 0)
	{
	  beatLastTick = seconds;
	  beatDetected = true;
	  cout <<"BEAT DETEETED" <<endl;
	}

	if ((difftime(seconds, beatLastTick	))*1000 >= beatPostIgnore)
	  beatLastTick = 0;

}

bool Fft::beat() {
	return beatDetected;
}

void Fft::getData(float* data) {
	copy(&out[0],&out[FFT_WINDOW_SIZE/2],data);

}
