#ifndef APU_H
#define APU_H

class CPUClass;

struct APU_Channel {

	//	these all mean different things depending on the channel, see: https://wiki.nesdev.com/w/index.php/APU
	uint8_t reg1 = 0;
	uint8_t reg2 = 0;
	uint8_t reg3 = 0;
	uint8_t reg4 = 0;

};

class APUClass {

public:
	void storeCPU(CPUClass* _CPU) { this->CPU = _CPU; }

protected:
	APU_Channel pulse1Channel;
	APU_Channel pulse2Channel;
	APU_Channel triangleChannel;
	APU_Channel noiseChannel;
	APU_Channel DMCChannel;

	CPUClass* CPU = nullptr;

};

#endif