#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>
#include "../midi_interface.h"

// MIDIPacketList API is deprecated in macOS 11 but still functional.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

class MIDI_Actual : public MIDI_Interface
{
private:
	MIDIClientRef   client  = 0;
	MIDIPortRef     outPort = 0;
	MIDIEndpointRef dest    = 0;

public:
	MIDI_Actual();
	~MIDI_Actual();
	void SendCommand(const unsigned char cmdBuf[]) override;
	void SendExclusiveCommand(const unsigned char cmdBuf[], int len) override;
};

MIDI_Actual::MIDI_Actual()
{
	if(MIDIClientCreate(CFSTR("Tsugaru"), nullptr, nullptr, &client) != noErr)
		return;
	if(MIDIOutputPortCreate(client, CFSTR("TsugaruOutput"), &outPort) != noErr)
		return;
	if(MIDIGetNumberOfDestinations() == 0)
		return;
	dest = MIDIGetDestination(0);
}

MIDI_Actual::~MIDI_Actual()
{
	if(outPort) { MIDIPortDispose(outPort); outPort = 0; }
	if(client)  { MIDIClientDispose(client); client = 0; }
}

void MIDI_Actual::SendCommand(const unsigned char cmdBuf[])
{
	if(!outPort || !dest)
		return;

	// Determine message length from status byte
	unsigned char status = cmdBuf[0];
	int len = 3;
	if((status & 0xF0) == 0xC0 || (status & 0xF0) == 0xD0)
		len = 2;  // Program Change, Channel Pressure
	else if(status >= 0xF8)
		len = 1;  // System Realtime
	else if(status == 0xF1 || status == 0xF3)
		len = 2;  // Time Code, Song Select

	MIDIPacketList pkList;
	MIDIPacket *pkt = MIDIPacketListInit(&pkList);
	pkt = MIDIPacketListAdd(&pkList, sizeof(pkList), pkt, 0, len, cmdBuf);
	if(pkt)
		MIDISend(outPort, dest, &pkList);
}

void MIDI_Actual::SendExclusiveCommand(const unsigned char cmdBuf[], int len)
{
	if(!outPort || !dest)
		return;

	// Build SysEx: 0xF0 + data + 0xF7 (max len=12, total max 14 bytes)
	unsigned char sysex[16];
	sysex[0] = 0xF0;
	if(len > 12) len = 12;
	for(int i = 0; i < len; i++)
		sysex[i + 1] = cmdBuf[i];
	sysex[len + 1] = 0xF7;
	int total = len + 2;

	MIDIPacketList pkList;
	MIDIPacket *pkt = MIDIPacketListInit(&pkList);
	pkt = MIDIPacketListAdd(&pkList, sizeof(pkList), pkt, 0, total, sysex);
	if(pkt)
		MIDISend(outPort, dest, &pkList);
}

#pragma clang diagnostic pop

MIDI_Interface *MIDI_Interface::Create(void)
{
	return new MIDI_Actual;
}
void MIDI_Interface::Delete(MIDI_Interface *itfc)
{
	auto *ptr = dynamic_cast<MIDI_Actual *>(itfc);
	if(nullptr != ptr)
	{
		delete ptr;
	}
	// If not, let it leak.  What else can I do?
}
