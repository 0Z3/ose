#include <stdint.h>

#include "esp_system.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include <ose_conf.h>
#include <ose.h>
#include <ose_context.h>
#include <ose_util.h>
#include <ose_stackops.h>
#include <ose_assert.h>
#include <ose_vm.h>
#include <ose_symtab.h>
#include <ose_print.h>

#include "config.h"

#define pin_led 13

const char * const ssid = CONFIG_MYSSID;
const char * const pass = CONFIG_MYPASS;
IPAddress ip_local, ip_remote(192, 168, 178, 22), ip_bcast;
const unsigned int port_local = 10000;
const unsigned int port_remote = 10001;

WiFiUDP udp;
boolean connected = false;

void wifi_event_handler(WiFiEvent_t e)
{
	switch(e){
	case SYSTEM_EVENT_STA_GOT_IP:
		ip_local = WiFi.localIP();
		udp.begin(ip_local, port_local);
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		connected = false;
		break;
	}
}

void connect_to_wifi(const char *ssid, const char *pass)
{
	WiFi.disconnect(true);
	WiFi.onEvent(wifi_event_handler);
	WiFi.begin(ssid, pass);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}
}

ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

bool addressIsPin(const char * const address, int32_t addresslen)
{
	if(addresslen >= 4){
		if((address[1] == 'a' || address[1] == 'd')
		   && address[0] == '/' && address[2] == '/'){
			if(address[3] >= 48 && address[3] <= 57){
				return true;
			}
		}
	}
	return false;
}

char getPinType(const char * const address, int32_t addresslen)
{
	return address[1];
}

int getPinNumber(const char * const address, int32_t addresslen)
{
	return strtol(address + 3, NULL, 10);
}

bool wantPullup(const char * const address, int32_t addresslen)
{
	if(address[addresslen - 2] == '/'
	   && address[addresslen - 1] == 'u'){
		return true;
	}else{
		return false;
	}
}

void lookup(ose_bundle osevm)
{
	const char * const addy = ose_peekAddress(vm_c);
	int32_t addylen = strlen(addy);
	if(addressIsPin(addy + 2, addylen) == true){
		char pintype = getPinType(addy + 2, addylen - 2);
		int pinnum = getPinNumber(addy + 2, addylen - 2);
		if(wantPullup(addy + 2, addylen - 2) == true){
			pinMode(pinnum, INPUT_PULLUP);
		}else{
			pinMode(pinnum, INPUT);
		}
		switch(pintype){
		case 'a':
			ose_pushMessage(vm_s, addy + 2, addylen - 2,
					1, OSETT_INT32, analogRead(pinnum));
			break;
		case 'd':
			ose_pushMessage(vm_s, addy + 2, addylen - 2,
					1, OSETT_INT32, digitalRead(pinnum));
			break;
		default:
			// unknown pin type!
			break;
		}
	}else{
		ose_lookupStackItemInEnv(osevm);
	}
}

void assign(ose_bundle osevm)
{
	const char * const addy = ose_peekAddress(vm_c);
	int32_t addylen = strlen(addy);
	if(addressIsPin(addy + 2, addylen) == true){
		char pintype = getPinType(addy + 2, addylen - 2);
		int pinnum = getPinNumber(addy + 2, addylen - 2);
		int32_t val = ose_popInt32(vm_s);
		pinMode(pinnum, OUTPUT);
		switch(pintype){
		case 'a':
			break;
		case 'd':
			digitalWrite(pinnum, val);
			break;
		default:
			// unknown pin type!
			break;
		}
	}else{
		ose_assignStackToEnv(osevm);
	}
}

char bytes[65536];
void setup() 
{
	Serial.begin(115200);

	bundle = ose_newBundleFromCBytes(65536, bytes);
	osevm = osevm_init(bundle);
	vm_i = OSEVM_INPUT(osevm);
	vm_s = OSEVM_STACK(osevm);
	vm_e = OSEVM_ENV(osevm);
	vm_c = OSEVM_CONTROL(osevm);
        vm_d = OSEVM_DUMP(osevm);
	vm_o = OSEVM_OUTPUT(osevm);
	
	connect_to_wifi(ssid, pass);
}

char incoming_packet[2048];
void loop() 
{
	int size = udp.parsePacket();
	if(size){
		udp.read(incoming_packet, 2048);
		ose_pushMessage(vm_i, OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN, 
				1, OSETT_BLOB, size, incoming_packet);
		ose_blobToElem(vm_i);
		ose_popAllDrop(vm_i);
		osevm_run(osevm);
		udp.beginPacket(ip_remote, port_remote);
		udp.write((const uint8_t *)ose_getBundlePtr(vm_s),
			  ose_readInt32(vm_s, -4));
		udp.endPacket();
		ose_clear(vm_s);
	}
	yield();
}
