#include "communication/include/messagedecoder.h"
#include "communication/include/communication.h"
#include "communication/include/command.h"
#include "device/include/device.h"
#include <iostream>

using namespace icsneo;

uint64_t MessageDecoder::GetUInt64FromLEBytes(uint8_t* bytes) {
	uint64_t ret = 0;
	for(int i = 0; i < 8; i++)
		ret |= (bytes[i] << (i * 8));
	return ret;
}

std::shared_ptr<Message> MessageDecoder::decodePacket(const std::shared_ptr<Packet>& packet) {
	switch(packet->network.getType()) {
		case Network::Type::CAN: {
			if(packet->data.size() < 24)
				break; // We would read garbage when interpereting the data

			HardwareCANPacket* data = (HardwareCANPacket*)packet->data.data();
			auto msg = std::make_shared<CANMessage>();
			msg->network = packet->network;
			msg->arbid = data->header.SID;
			msg->data.reserve(data->dlc.DLC);

			// Timestamp calculation
			msg->timestamp = data->timestamp.TS * 25; // Timestamps are in 25ns increments since 1/1/2007 GMT 00:00:00.0000

			for(auto i = 0; i < data->dlc.DLC; i++)
				msg->data.push_back(data->data[i]);
			return msg;
		}
		default:
			// TODO Implement others
			break;
	}

	auto msg = std::make_shared<Message>();
	msg->network = packet->network;
	msg->data = packet->data;
	return msg;
}