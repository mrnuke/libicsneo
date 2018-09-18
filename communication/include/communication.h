#ifndef __COMMUNICATION_H_
#define __COMMUNICATION_H_

#include "communication/include/icommunication.h"
#include "communication/include/network.h"
#include "communication/include/messagecallback.h"
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <queue>
#include <map>

namespace icsneo {

class Communication {
public:
	static uint8_t ICSChecksum(const std::vector<uint8_t>& data);

	Communication(std::shared_ptr<ICommunication> com) : impl(com) {}
	virtual ~Communication() { close(); }

	bool open();
	bool close();
	virtual void spawnThreads();
	virtual void joinThreads();
	bool rawWrite(const std::vector<uint8_t>& bytes) { return impl->write(bytes); }
	std::vector<uint8_t>& packetWrap(std::vector<uint8_t>& data, bool addChecksum = true);
	bool sendPacket(std::vector<uint8_t>& bytes);

	enum class Command : uint8_t {
		EnableNetworkCommunication = 0x07,
		RequestSerialNumber = 0xA1
	};
	virtual bool sendCommand(Command cmd, bool boolean) { return sendCommand(cmd, std::vector<uint8_t>({ (uint8_t)boolean })); }
	virtual bool sendCommand(Command cmd, std::vector<uint8_t> arguments = {});
	
	int addMessageCallback(const MessageCallback& cb);
	bool removeMessageCallback(int id);

	void setAlign16Bit(bool enable) { align16bit = enable; }

	class Packet {
	public:
		Network network;
		std::vector<uint8_t> data;
	};

protected:
	std::shared_ptr<ICommunication> impl;
	static int messageCallbackIDCounter;
	std::map<int, MessageCallback> messageCallbacks;
	std::atomic<bool> closing{false};

private:
	bool isOpen = false;
	bool align16bit = true; // Not needed for Gigalog, Galaxy, etc and newer

	std::thread readTaskThread;
	void readTask();
};

};

#endif