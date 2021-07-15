
#define TINY_GSM_MODEM_SIM7000

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3); // RX, TX
#endif

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// set GSM PIN, if any
#define GSM_PIN ""
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_DTR 25

#define MODEM_TX 26
#define MODEM_RX 27

// Your GPRS credentials, if any
const char apn[] = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

#include <TinyGsmClient.h>
#include <root_cert.h>

#define CERT_FILE_ROOT_PEM "C:\\Users\\mano\\apps_studio\\rastreador\\trackerTest\\include\\roots.pem"
#define CERT_FILE_ROOT_PRIMARY_CRT "C:\\Users\\mano\\apps_studio\\rastreador\\trackerTest\\include\\gtsltsr_primary.crt"
#define CERT_FILE_THING_PRIV_PEM "C:\\Users\\mano\\apps_studio\\rastreador\\trackerTest\\include\\esp32-device_2.pem"
#define CERT_FILE_THING_PUB_PEM "C:\\Users\\mano\\apps_studio\\rastreador\\trackerTest\\include\\esp32-device-public_2.pem"

#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);

void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);
    delay(10);

    SerialMon.println("Wait...");

    // Set GSM module baud rate

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Set pins power reset modem...");

    pinMode(MODEM_PWKEY, OUTPUT);
    digitalWrite(MODEM_PWKEY, HIGH);
    delay(10);
    digitalWrite(MODEM_PWKEY, LOW);
    delay(1010); //Ton 1sec
    digitalWrite(MODEM_PWKEY, HIGH);

    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, LOW);
    delay(260); //Treset 252ms
    digitalWrite(MODEM_RST, HIGH);
    delay(5000);

    SerialMon.println("Start serial AT modem...");
    SerialAT.begin(9600, SERIAL_8N1, MODEM_TX, MODEM_RX);
    delay(6000);

    SerialMon.println("Init / reset modem...");
    modem.init();
    modem.restart();

    delay(10000);

    SerialMon.println("Waiting testAt...");
    for (int8_t i = 0; i < 100; i++) //timeout 100 x 100ms = 10sec
    {
        if (modem.testAT())
        {
            //Serial.println("Wait time:%F sec\n", i/10));
            Serial.printf("Wait time: %d\n", i);
            break;
        }
        Serial.println("waiting modem...");
    }

    /*
    AT+CSSLCFG="cipher
suite",<ctxindex>,<cip
her_index>,<ciphersuit
e>
    0xC02B QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

Response
+CSSLCFG: "sslversion",(0-5),(0-5)
+CSSLCFG: "ciphersuite",(0-5),(0-7),(0x008A,0x008B,0x008C,0x008D,0x00A8,0x00
A9,0x00AE,0x00AF,0x002F,0x0033,0x0035,0x0039,0xC02A,0xC02B,0x
C02C,0xC02D,0xC02E,0xC02F,0xC030,0xC031,0xC032,0xC09C,0xC09
D,0xC09E,0xC09F,0xC0A0,0xC09F,0xC0A1,0xC0A2,0xC0A3,0xCC13,0x
CC14,0xCC15)
+CSSLCFG: "ignorertctime",(0-5),(0-1)
+CSSLCFG: "protocol",(0-5),(1-2)
+CSSLCFG: "sni",(0-5),<servername>
+CSSLCFG: "ctxindex",(0-5)
+CSSLCFG: "convert",(1-3),(<cname>,[<keyname>[,<passkey>]])

AT+CSSLCFG="sslver
sion",<ctxindex>,<sslv
ersion>
    3 QAPI_NET_SSL_PROTOCOL_TLS_1_2



    mqtt.2030.ltsapis.goog 8883 or 443

    DNS over TCP or UDP

    
    AT+CSSLCFG="protoc
ol",<ctxindex>,<protoc
ol>
    Response
OK
If failed:
+CME ERROR: <err>
Parameters
<ctxindex> (0-5)
<protocol>
1 QAPI_NET_SSL_TLS_E
    */
    SerialMon.println("--------------------- START TEST AT COMMANDS");
    modem.sendAT(GF("+CSSLCFG=\"sslversion\",0,3"));
    if (modem.waitResponse() != 1)
    {
        Serial.println("return sslversion fail");
        return;
    }
    modem.sendAT(GF("+SHSSL=1,") CERT_FILE_ROOT_PRIMARY_CRT);
    if (modem.waitResponse() != 1)
    {
        Serial.println("return  fail");
        return;
    }

        /*    modem.sendAT("CFSINIT");

    const int cert_size = sizeof(CERT_FILE);

    modem.sendAT(GF("CFSWFILE=0," CERT_FILE ",0,"), cert_size, GF(",5000"));
    if (modem.waitResponse() != 1)
    {
        Serial.println("return CFSWFILE fail");
        return;
    }
    SerialMon.println("--------------------- END TEST AT COMMANDS");
*/

    /*
    ////////
    SerialMon.println("--------------------- START TEST AT COMMANDS - SRGDamia cert file");

    modem.sendAT(GF("FSCREATE=" CERT_FILE));
    if (modem.waitResponse() != 1)
        return;

    const int cert_size = sizeof(cert);

    modem.sendAT(GF("FSWRITE=" CERT_FILE ",0,"), cert_size, GF(",10"));
    if (modem.waitResponse(GF(">")) != 1)
    {
        return;
    }

    for (int i = 0; i < cert_size; i++)
    {
        char c = pgm_read_byte(&cert[i]);
        modem.stream.write(c);
    }

    modem.stream.write(GSM_NL);
    modem.stream.flush();

    if (modem.waitResponse(2000) != 1)
        return;

    modem.sendAT(GF("+SSLSETCERT=\"" CERT_FILE "\""));
    if (modem.waitResponse() != 1)
        return;
    if (modem.waitResponse(5000L, GF(GSM_NL "+SSLSETCERT:")) != 1)
        return;
    const int retCode = modem.stream.readStringUntil('\n').toInt();

    SerialMon.println();
    SerialMon.println();
    SerialMon.println(F("****************************"));
    SerialMon.print(F("Setting Certificate: "));
    SerialMon.println((0 == retCode) ? "OK" : "FAILED");
    SerialMon.println(F("****************************"));

    SerialMon.println("--------------------- END TEST AT COMMANDS - SRGDamia cert file");
*/
    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        Serial.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected())
    {
        Serial.println("Network connected");
    }

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    SerialMon.print(F("Connecting to APN: "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected())
    {
        SerialMon.println("GPRS connected.. fim");
    }
}

void loop()
{

    if (SerialAT.available())
    {
        SerialMon.write(SerialAT.read());
    }
    if (SerialMon.available())
    {
        SerialAT.write(SerialMon.read());
    }
    delay(0);
    /*
    // Make sure we're still registered on the network
    if (!modem.isNetworkConnected())
    {
        SerialMon.println("Network disconnected");
        if (!modem.waitForNetwork(180000L))
        {
            SerialMon.println(" fail");
            delay(10000);
            return;
        }
        if (modem.isNetworkConnected())
        {
            SerialMon.println("Network re-connected");
        }

        // and make sure GPRS/EPS is still connected
        if (!modem.isGprsConnected())
        {
            SerialMon.println("GPRS disconnected!");
            SerialMon.print(F("Connecting to "));
            SerialMon.print(apn);
            if (!modem.gprsConnect(apn, gprsUser, gprsPass))
            {
                SerialMon.println(" fail");
                delay(10000);
                return;
            }
            if (modem.isGprsConnected())
            {
                SerialMon.println("GPRS reconnected");
            }
        }
    }
    */
}