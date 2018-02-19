//**************************************************************************************
//SawOSC module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from RODENTCAT https://github.com/RODENTCAT/RODENTMODULES
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"

struct SawOSC : Module {
	enum ParamIds {
		PITCH_PARAM,
		 PW_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		PW_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OSC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		FREQ_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0f;
	float blinkPhase = 0.0f;

	SawOSC() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void SawOSC::step() {
	// Implement a simple sine oscillator
	float deltaTime = 1.0f / engineGetSampleRate();
	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += inputs[PITCH_INPUT].value;
	pitch = clampf(pitch, -4.0f, 4.0f);
	float freq = 440.0f * powf(2.0f, pitch);

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;

   //Mod param
    float pw = params[PW_PARAM].value*0.1f+1.0f;
    //Mod input
    float minput = inputs[PW_INPUT].value*0.3f;
    //Mod param+input
    float pinput = (pw + minput);

	// Compute the sine output
	//float sine = sinf(2 * M_PI * phase);
	//outputs[SINE_OUTPUT].value = 5.0 * sine;
    
    //saw stuff, original dev says square, but it sounds more like a SAW wave, hence this module name hehe
    float saw = cos(exp(pinput * M_PI * phase));///0.87;
	//dc block
	
	float block_coeff = 1.0f - (2.0f * M_PI * (10.0f / 44100.0f));
	float m_prev_in = 0.0f;
	float m_prev_out = 0.0f;
	m_prev_out = saw - m_prev_in + block_coeff * m_prev_out;
	m_prev_in = saw;

    //outputs[OSC_OUTPUT].value = 5 * saw;
    outputs[OSC_OUTPUT].value = m_prev_out*5;
	lights[FREQ_LIGHT].value = (outputs[OSC_OUTPUT].value > 0.0f) ? 1.0f : 0.0f;
}

SawOscWidget::SawOscWidget() {
	SawOSC *module = new SawOSC();
	setModule(module);
	box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SawOSC.svg")));
		addChild(panel);
	}
	//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
	addChild(createScrew<as_HexScrew>(Vec(0, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//LIGHT
	addChild(createLight<SmallLight<RedLight>>(Vec(22-15, 57), module, SawOSC::FREQ_LIGHT));
	//PARAMS
	//addParam(createParam<as_KnobBlack>(Vec(26, 60), module, SawOSC::PITCH_PARAM, -3.0, 3.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(26-15, 60), module, SawOSC::PITCH_PARAM, -4.0, 4.0, 0.0));
	//addParam(createParam<as_KnobBlack>(Vec(26, 125), module, SawOSC::PW_PARAM, -4.0, 5.0, -4.0));
	addParam(createParam<as_KnobBlack>(Vec(26-15, 125), module, SawOSC::PW_PARAM, -4.2, 5.0, -4.2));
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(33-15, 200), module, SawOSC::PW_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(33-15, 260), module, SawOSC::PITCH_INPUT));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(33-15, 310), module, SawOSC::OSC_OUTPUT));
}
