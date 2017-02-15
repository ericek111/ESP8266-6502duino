/* Our telnet server
 */
WiFiServer server(23);
WiFiClient client;
bool tchar_waiting = false;
byte tchar_skip = 0;
byte tchar = 0;

/* 10Hz timer interrupt.
 */
os_timer_t t10hz;
void timer_callback(void *arg)
{
    iflag |= IFLAG_TIMER;
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.print("\r\n\n");
    Serial.print("BASIC loaded to ");
    Serial.println((uint32) BASIC, HEX);
    Serial.print("MOS loaded to ");
    Serial.println((uint32) MOS, HEX);

    Serial.print("\r\n\nBooting...\r\n\n\n");

#ifdef AP_SSID
    /* Become an AP.
     */
    Serial.println("Starting an AP with SSID " AP_SSID);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL);
    Serial.print("Starting telnet server on ");
    Serial.println(WiFi.softAPIP());
#else
    /* Connect to WiFi.
     */
    Serial.print("Connecting to " SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("connected");
    Serial.print("Starting telnet server on ");
    Serial.println(WiFi.localIP());
#endif

    /* Start our server
     */
    server.begin();

    /* I'd like to have a 100Hz timer, but the ESP can't keep up
     * when I try to set that. So my little OS will have to cope
     * with 10Hz resolution and multiply by 10 for BBC BASIC 
     * compatibility.
     */
    os_timer_setfn(&t10hz, timer_callback, NULL);
    os_timer_arm(&t10hz, 100, true);

    init_coprocessor();

    reset();

    icounter = 0;
    iflag = 0;
}

void loop(void)
{
    static word icounter1k = 0;

    /* Have we got a new telnet connection? If so, 
     * accept the connection and do some telnetty stuff.
     */
    if (server.hasClient())
    {
        if (!client || !client.connected())
        {
            if (client)
            {
                client.stop();
            }
            client = server.available();

            /* Telnet options.
             */
            enum
            {
                IAC = 255,
                DONT = 254,
                DO = 253,
                WONT = 252,
                WILL = 251,
                SGA = 3,
                ECHO = 1
            };
            char telnetopts[] = {
                IAC, WILL, SGA,
                IAC, DO, SGA,
                IAC, WILL, ECHO,
                IAC, DONT, ECHO
            };
            client.write(telnetopts, sizeof(telnetopts));

            /* Reset the emulation so they get a newly booted BASIC.
             */
            reset();
        }
        else
        {
            /* Already have a connection.
             */
            WiFiClient busy = server.available();
            busy.write("Too Busy, please try later!\r\n");
            busy.stop();
        }
    }


    /* Anything waiting on the serial port?
     */
    if (Serial.available())
    {
        iflag |= IFLAG_SERIAL;
    }

    /* Anything on the telnet connection?
     */
    if (!(iflag & IFLAG_SERIAL) && client && client.connected()
        && client.available())
    {
        byte got = client.read();

        /* We have to handle end of line here, before deciding
         * whether to raise an interrupt.
         */
        tchar = got;
        if (tchar_skip == 0)
        {
            switch (got)
            {
                case 255:
                    /* Suppress this and next two characters - telnet protocol sequence.
                     */
                    tchar_skip = 2;
                    break;
                case '\r':
                    /* Allow this but suppress next character - \r\n or \r\0.
                     */
                    iflag |= IFLAG_SERIAL;
                    tchar_waiting = true;
                    tchar_skip = 1;
                    break;
                default:
                    /* Allow everything else.
                     */
                    tchar_waiting = true;
                    iflag |= IFLAG_SERIAL;
            }
        }
        else
        {
            tchar_skip--;
        }
    }

    /* Run several (thousand) instructions before
     * relinquishing control back to the ESP8266's
     * OS.
     */
    for (int i = 0; i < 10000; i++)
    {
        byte op;

        PC++;

        /* Call IRQ.
         */
        if (iflag && !(SR & bit_I))
        {
            push((byte) (PC >> 8));
            push((byte) (PC & 0xFF));
            push(SR | 0b00100000);
            SR |= bit_I;
            PC = getword(V_IRQ);
        }

        op = memread(PC);
        aflag = 0;
        runop(op);

        /* Every 1000 instructions, we increment the emulated hardware
         * instruction counter.
         */
        icounter1k++;
        if (icounter1k == 1000)
        {
            icounter++;
            icounter1k = 0;
        }
    }
}
