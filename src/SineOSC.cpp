//**************************************************************************************
//SineOSC module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//Is just the tutorial module and nothing else hehe
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"

struct SineOSC : Module {
	enum ParamIds {
		FREQ_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FREQ_CV,
		NUM_INPUTS
	};
	enum OutputIds {
		OSC_OUTPUT,
		TRI_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		FREQ_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0f;
	float blinkPhase = 0.0f;

	SineOSC() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};


void SineOSC::step() {
	// Implement a simple sine oscillator
	// Compute the frequency from the pitch parameter and input
	float pitch = params[FREQ_PARAM].value;
	pitch += inputs[FREQ_CV].value;
	pitch = clampf(pitch, -4.0f, 4.0f);
	float freq = 440.0f * powf(2.0f, pitch);
	// Accumulate the phase
	phase += freq / engineGetSampleRate();
	if (phase >= 1.0f)
		phase -= 1.0f;
	// Compute the sine output
	//correct sine
	float sine = sinf(2.0f * M_PI * (phase+1 * 0.125f)) * 5.0f;
	//original sine
	//float sine = sinf(2 * M_PI * phase)+ sinf(2 * M_PI * phase * 2)*5;
	//mod,like this it gives  a unipolar saw-ish wave
	//float sine = sinf(2.0 * M_PI * (phase * 0.125)) * 5.0;
	outputs[OSC_OUTPUT].value = sine;
    lights[FREQ_LIGHT].value = (outputs[OSC_OUTPUT].value > 0.0f) ? 1.0f : 0.0f;

}

SineOscWidget::SineOscWidget() {
	SineOSC *module = new SineOSC();
	setModule(module);
	box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SineOSC.svg")));
		addChild(panel);
	}
	//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
	addChild(createScrew<as_HexScrew>(Vec(0, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//LIGHT
	addChild(createLight<SmallLight<RedLight>>(Vec(22-15, 57), module, SineOSC::FREQ_LIGHT));
	//PARAMS
	addParam(createParam<as_KnobBlack>(Vec(26-15, 60), module, SineOSC::FREQ_PARAM, -3.0f, 3.0f, 0.0f));
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(33-15, 260), module, SineOSC::FREQ_CV));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(33-15, 310), module, SineOSC::OSC_OUTPUT));
	
}
