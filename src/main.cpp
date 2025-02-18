#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Ticker.h>

//#define SERVER // for UDP Server

#define TCP // use TCP, instead of UDP

const char *ssid = "DualAccUDP_AP";
const char *password = "12345678";
#define PORT 12345
Ticker ticker;
#define SAMPLE_FREQ 250

 #ifdef TCP
WiFiServer server(PORT);
WiFiClient client;
 #else // UDP
WiFiUDP udp;
 #endif
const char *serverIP = "192.168.4.1"; // APのIP
char buf[1025];

#ifdef SERVER
uint32_t t0;
#else
int n = 0;
#endif

volatile uint8_t fSample = 0;

void IRAM_ATTR onTicker()
{
//	printf("fSample = %d", fSample);
	fSample = 1;
//	printf("-> %d\n", fSample);
}

void setup()
{
	M5.begin();

#ifdef SERVER
	// アクセスポイントモードに設定
	WiFi.softAP(ssid, password);
	printf("WiFi AP started, IP Address: %s\n", WiFi.softAPIP().toString());
 #ifdef TCP
	server.begin();
	printf("TCP server started\n");
 #else // UDP
	udp.begin(PORT);
	printf("Listening on UDP port %d\n", PORT);
 #endif
	t0 = millis();
#else // not SERVER
	WiFi.begin(ssid, password);
	printf("Connecting to WiFi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		printf(".");
	}
	printf("\nConnected to WiFi, IP Address: %s\n", WiFi.localIP().toString());
//	for (uint16_t i = 0; i < 1000; i++) buf[i] = '0' + (i % 10); buf[1000] = '\0';
 sprintf(buf, "0.123,0.234,0.345,0.456,0.567,0.678\n");
 #ifdef TCP
 	printf("Connecting to TCP server...");
 	while (!client.connect(serverIP, PORT)) {
		printf(".");
		delay(1000);
	}
	printf("\nConnected to TCP server\n");
 #endif
	ticker.attach_ms((int)(1000 / SAMPLE_FREQ), onTicker);
#endif
}

void loop()
{
#ifdef SERVER
 #ifdef TCP
	if (!client) {
		client = server.available();
		if (client) printf("Client connected.\n");
	} else {
		if (client.available()) {
			int len = client.available();
			if (len > 1024) len = 1024;
			int bytesRead = client.readBytes(buf, len);
			printf("%d,%d\n", millis() - t0, bytesRead);
	}
}

 #else // UDP
	int packetSize = udp.parsePacket();
	if (packetSize)
	{
		int len = udp.read(buf, 1024);
		if (len > 0)
		{
			buf[len] = '\0';
		}
		printf("%d,%d\n", millis() - t0, len);
	}
 #endif
#else // not SERVER
	fSample = 0;
	while(fSample == 0) delayMicroseconds(10);;
 #ifdef TCP
 	if (client.connected()) {
		client.print(buf);
	}
	else {
		printf("Disconnected from server, attempting to reconnect...\n");
		client.connect(serverIP, PORT);
	}
 #else // UDP
	udp.beginPacket(serverIP, PORT);
	udp.print(buf);
	udp.endPacket();
 #endif
#endif
}
