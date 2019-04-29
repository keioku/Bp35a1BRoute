/*
    Bp35c0_broute.h - ROHM BP35C0 Wi-SUN B-Route Library for ESP32
    development by nara256  https://github.com/nara256/
    version 0.1

    License MIT
*/
#ifndef Bp35c0_broute_h_
#define Bp35c0_broute_h_

#include "Arduino.h"

#define DELAY_AFTER_COMMAND_EXEC (500)
#define RES_OK "OK"

class Bp35c0_broute
{
private:
    Stream *m_uart;
    Stream *m_debug;
    String m_device;
    bool m_opened = false;

    bool isResponseOK();
    bool isResponseOK(const String &res);
    bool isResponseOK(const String &res, const char *in);

    void debug(const String &val);
    void debug(const char *val);

    void writeUart(const char *val);
    void writeUart(const String &val);
    void writeUart(const char *val, long waitMillisec);
    void writeUart(const String &val, long waitMilisec);
    String readUart();
    bool availableUart();

    String searchValue(const String &in, const char *key, const int length);

    bool isTargetData(const String &line);
    String getRowData(const String &line);
    void getBRouteValue(const String &line,
                        char *downsideDatetime, float *downsidePower,
                        char *upsideDatetime, float *upsidePower);

    //--- SK COMMAND
    bool skVer();
    bool skTerm();
    bool skInitSetting();
    bool skSetID(const char *id);
    bool skSetPass(const char *pass);
    bool skScanAndRegist(const int time);
    bool skJoin();

public:
    Bp35c0_broute(Stream &uart);
    virtual ~Bp35c0_broute();

    void setDebugSerial(Stream &debug);
    bool available(void);
    bool open(const char *b_route_id, const char *b_route_pass);
    void waitForRecieve(char *downsideDatetime, float *downsidePower,
                        char *upsideDatetime, float *upsidePower);
    bool close(void);
    bool isOpen(void);

    void test_getBRouteValue(const String &line,
                             char *downsideDatetime, float *downsidePower,
                             char *upsideDatetime, float *upsidePower)
    {
        getBRouteValue(line, downsideDatetime, downsidePower, upsideDatetime, upsidePower);
    }
};

#endif