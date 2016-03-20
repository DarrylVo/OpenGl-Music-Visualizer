/*
 * fft.hpp
 *
 *  Created on: Mar 3, 2016
 *      Author: darryl
 */
#include "fmod.hpp"
#include "fmod_errors.h"
#include <ctime>
#include <string>
#ifndef SRC_FFT_HPP_
#define SRC_FFT_HPP_
#define FFT_WINDOW_SIZE 2048
using namespace std;
class Fft {
public:
	void init(string songpath);
	void getData(float* data);
	void analyze();
	void pauseSong();
	void playSong();
	bool beat();


private:
	FMOD::System *system;
	FMOD_RESULT result;
	FMOD::Sound *song;
	FMOD::Channel *channel;
	FMOD::DSP *dsp;
	FMOD_DSP_PARAMETER_FFT *fft;
	float out[FFT_WINDOW_SIZE/2];
	float beatThresholdVolume = 0.23f;
	int beatThresholdBar = 2;
	unsigned int beatPostIgnore = 250;
	time_t beatLastTick = 0;
	bool beatDetected = false;


};



#endif /* SRC_FFT_HPP_ */
