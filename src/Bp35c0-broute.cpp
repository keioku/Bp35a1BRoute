/*
    Bp35c0-broute.cpp - ROHM BP35C0 Wi-SUN B-Route Library for ESP32
    development by nara256  https://github.com/nara256/
    version 0.1

    License MIT
*/

#include "Bp35c0-broute.h"
#include <stdlib.h>

static long hexToLong(String hexStr)
{
    char buf[33];
    hexStr.toCharArray(buf, 32);
    return strtol(buf, NULL, 16);
}

// コンストラクタ
Bp35c0_broute::Bp35c0_broute(Stream &uart)
{
    m_debug = NULL;
    m_uart = &uart;
}

// デストラクタ
Bp35c0_broute::~Bp35c0_broute()
{
}

//------------------　PRIVATE
bool Bp35c0_broute::isResponseOK()
{
    if (!availableUart())
        return false;
    String out = readUart();
    return isResponseOK(out);
}

bool Bp35c0_broute::isResponseOK(const String &res)
{
    return isResponseOK(res, RES_OK);
}

bool Bp35c0_broute::isResponseOK(const String &res, const char *in)
{
    return res.indexOf(in) >= 0;
}

void Bp35c0_broute::debug(const String &val)
{
    if (m_debug != NULL)
    {
        m_debug->println(val);
    }
}

void Bp35c0_broute::debug(const char *val)
{
    String s(val);
    debug(s);
}

//--- SK COMMAND
bool Bp35c0_broute::skVer()
{
    delay(100);
    writeUart("SKVER"); //なぜか初回はエラーになることがあるので、
    writeUart("SKVER"); //2回呼んでおく
    return isResponseOK();
}

bool Bp35c0_broute::skTerm()
{
    writeUart("SKTERM");
    return isResponseOK();
}

bool Bp35c0_broute::skInitSetting()
{
    writeUart("SKSREG SFE 0");
    if (!isResponseOK())
        return false;
    writeUart("WOPT 01");
    return isResponseOK();
}

bool Bp35c0_broute::skSetID(const char *id)
{
    String cmd = "SKSETRBID " + String(id);
    writeUart(cmd);
    return isResponseOK();
}

bool Bp35c0_broute::skSetPass(const char *pass)
{
    String p(pass);
    String cmd = "SKSETPWD " + String(p.length(), HEX) + " " + p;
    writeUart(cmd);
    return isResponseOK();
}

bool Bp35c0_broute::skScanAndRegist(const int time)
{
    String cmd = "SKSCAN 2 FFFFFFFF " + String(time) + " 0";
    writeUart(cmd);
    String out("");
    while (true)
    {
        if (availableUart())
        {
            out += readUart();
        }
        if (isResponseOK(out, "EVENT 22"))
            break;
        delay(500);
    }

    String channel, panId, addr;
    channel = searchValue(out, "Channel:", 2);
    panId = searchValue(out, "Pan ID:", 4);
    addr = searchValue(out, "Addr:", 16);
    debug("channel=" + channel + " panId=" + panId + " addr=" + addr);
    if (channel.equals("") || panId.equals("") || addr.equals(""))
        return false;

    cmd = "SKSREG S2 " + channel;
    writeUart(cmd);
    if (!isResponseOK())
        return false;

    cmd = "SKSREG S3 " + panId;
    writeUart(cmd);
    if (!isResponseOK())
        return false;

    cmd = "SKLL64 " + addr;
    writeUart(cmd);
    m_device = readUart();
    debug("device=" + m_device);

    return !m_device.equals("");
}

String Bp35c0_broute::searchValue(const String &in, const char *key, const int length)
{
    int index = in.indexOf(key);
    if (index < 0)
        return String("");
    int start = index + strlen(key);
    return in.substring(start, start + length);
}

bool Bp35c0_broute::skJoin()
{
    String cmd = "SKJOIN " + m_device;
    writeUart(cmd, 10);
    return isResponseOK();
}

void Bp35c0_broute::writeUart(const String &val, long waitMillisec)
{
    debug(String(">>> ") + val);
    m_uart->println(val);
    delay(waitMillisec);
}

void Bp35c0_broute::writeUart(const char *val, long waitMillisec)
{
    String s(val);
    writeUart(s, waitMillisec);
}

void Bp35c0_broute::writeUart(const char *val)
{
    writeUart(val, DELAY_AFTER_COMMAND_EXEC);
}

void Bp35c0_broute::writeUart(const String &val)
{
    writeUart(val, DELAY_AFTER_COMMAND_EXEC);
}

bool Bp35c0_broute::availableUart()
{
    delay(10);
    yield();
    return m_uart->available();
}

String Bp35c0_broute::readUart()
{
    String out = m_uart->readString();
    debug(String("<<< ") + out);
    return out;
}

bool Bp35c0_broute::isTargetData(const String &line)
{
    if (!line.startsWith("ERXUDP"))
        return false;

    String raw = getRowData(line);
    if (raw.length() == 0)
        return false;
    debug("raw=" + raw);

    //スマートメーターからの値？
    String seoj = raw.substring(8, 8 + 6);
    debug("seoj=" + seoj);
    if (!seoj.equals("028801"))
        return false;
    //プロパティ通知？
    String esv = raw.substring(20, 20 + 2);
    debug("esv=" + esv);
    if (!esv.equals("73"))
        return false;

    return true;
}

String Bp35c0_broute::getRowData(const String &line)
{
    String udp = line.substring(123);
    udp.trim();
    return udp;
}

void Bp35c0_broute::getBRouteValue(const String &line,
                                   char *downsideDatetime, float *downsidePower,
                                   char *upsideDatetime, float *upsidePower)
{
    strcpy(downsideDatetime, "");
    *downsidePower = 0.;
    strcpy(upsideDatetime, "");
    *upsidePower = 0.;

    String raw = getRowData(line);
    int opc = hexToLong(raw.substring(22, 22 + 2));
    debug("opc=" + String(opc));

    for (int i = 0; i < opc; i++)
    {
        int off = 24 + i * 26;
        String epc = raw.substring(off, off + 2);
        debug("epc=" + epc);
        int year = hexToLong(raw.substring(off + 4, off + 4 + 4));
        int month = hexToLong(raw.substring(off + 8, off + 8 + 2));
        int day = hexToLong(raw.substring(off + 10, off + 10 + 2));
        int hour = hexToLong(raw.substring(off + 12, off + 12 + 2));
        int min = hexToLong(raw.substring(off + 14, off + 14 + 2));
        int sec = hexToLong(raw.substring(off + 16, off + 16 + 2));
        float value = (float)hexToLong(raw.substring(off + 18, off + 18 + 8)) / 10.;
        debug("value=" + String(value));

        char datetime[33];
        sprintf(datetime, "%04d%02d%02d%02d%02d%02d", year, month, day, hour, min, sec);
        debug("datetime=" + String(datetime));
        if (epc.equals("EA"))
        {
            *downsidePower = value;
            strcpy(downsideDatetime, datetime);
        }
        else if (epc.equals("EB"))
        {
            *upsidePower = value;
            strcpy(upsideDatetime, datetime);
        }
    }
}

//------------------　PUBLIC
void Bp35c0_broute::setDebugSerial(Stream &debug)
{
    m_debug = &debug;
}

bool Bp35c0_broute::available(void)
{
    debug("----------available");
    return skVer();
}

bool Bp35c0_broute::open(const char *b_route_id, const char *b_route_pass)
{
    debug("----------open");

    //既存セッションがあったら一旦閉じ
    skTerm();
    //初期化
    if (!skInitSetting())
        return false;
    //IDセット
    if (!skSetID(b_route_id))
        return false;
    //PASSセット
    if (!skSetPass(b_route_pass))
        return false;
    //機器スキャン
    if (!skScanAndRegist(6))
        return false;
    //機器JOIN
    if (!skJoin())
        return false;
    m_opened = true;
    return true;
}

void Bp35c0_broute::waitForRecieve(
    char *downsideDatetime, float *downsidePower,
    char *upsideDatetime, float *upsidePower)
{
    debug("----------loop");
    while (true)
    {
        while (!availableUart())
            ;
        String out = readUart();
        if (isTargetData(out))
        {
            getBRouteValue(out, downsideDatetime, downsidePower, upsideDatetime, upsidePower);
            if (strcmp(downsideDatetime, "") == 0 && strcmp(upsideDatetime, "") == 0)
                debug(" - no data");
            else
                return;
        }
        else
            debug(" - no processing");

        delay(100);
    }
}

bool Bp35c0_broute::close(void)
{
    debug("----------close");
    m_opened = false;
    return skTerm();
}

bool Bp35c0_broute::isOpen()
{
    return m_opened;
}
