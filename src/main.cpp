#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiUdp.h>

//#define SERVER // for UDP Server

const char *ssid = "DualAccUDP_AP";
const char *password = "12345678";

WiFiUDP udp;
const int UDPPort = 12345;
const char *serverIP = "192.168.4.1"; // APのIP
#ifdef SERVER
char buf[1024];
#else
#endif

void setup()
{
	M5.begin();
	Serial.begin(115200);

#ifdef SERVER
	// アクセスポイントモードに設定
	WiFi.softAP(ssid, password);
	Serial.println("WiFi AP started");
	Serial.print("AP IP Address: ");
	Serial.println(WiFi.softAPIP());
	// UDP受信開始
	udp.begin(UDPPort);
	Serial.printf("Listening on UDP port %d\n", UDPPort);
#else
	WiFi.begin(ssid, password);
	Serial.print("Connecting to WiFi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("\nConnected to WiFi!");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
#endif
}

void loop()
{
#ifdef SERVER
	int packetSize = udp.parsePacket();
	if (packetSize)
	{
		int len = udp.read(buf, 1023);
		if (len > 0)
		{
			buf[len] = '\0';
		}
		Serial.printf("Received packet: %s\n", buf);
	}
#else
	const char *message = "Hello from M5Stack_B!";
	udp.beginPacket(serverIP, UDPPort);
	udp.print(message);
	udp.endPacket();

	Serial.println("Sent packet: " + String(message));

	delay(1000); // 1秒ごとに送信
#endif
}
