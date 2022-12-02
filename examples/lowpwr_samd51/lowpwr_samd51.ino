/* lowpwr_samd51.ino

Creates an alarm that runs every minute.
Objective : wip the processor will be put into standbymode between runs

*/

#include "rtc_SAMD51.h"
#include "DateTime.h"

extern const String build_ref = "a\\" __FILE__ " " __DATE__ " " __TIME__ " ";


// The SAMD51 has two hardware alarms
#define ALM_ID 0
bool semaAlarm0=false;
#define ALARM0_NEW_TIME_SEC   15

#if ! defined (NO_STANDBYMODE)
#define  USE_STANDBYMODE
#endif 

RTC_SAMD51 internal_rtc;

void setup()
{
    internal_rtc.begin();
    
    //Turn off WiFi
    pinMode(RTL8720D_CHIP_PU, OUTPUT);
    digitalWrite(RTL8720D_CHIP_PU, LOW);

    Serial.begin(115200);

    while (!Serial)
    {
        ;
    }
    
    // Create a build reference
    Serial.print(F("\n\n---Boot Sw Build: "));
    Serial.println(build_ref);

    DateTime now = DateTime(F(__DATE__), F(__TIME__));

    //!!! notice The year is limited to 2000-2099
    Serial.println("adjust time!");
    internal_rtc.adjust(now);

    now = internal_rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    DateTime alarm = DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() + ALARM0_NEW_TIME_SEC );


    internal_rtc.setAlarm(ALM_ID,alarm); // match after 15 seconds
    internal_rtc.enableAlarm(ALM_ID, internal_rtc.MATCH_SS); // match Every minute 

    internal_rtc.attachInterrupt(alarmMatch0); // callback while alarm is match

}

void loop()
{
    if (semaAlarm0) {
        semaAlarm0 = false;
        Serial.print("Alarm Match! ");
        DateTime now = internal_rtc.now();
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println(" ..zzz ");

        //DateTime alarm = DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() + ALARM_NEW_TIME_SEC );
        //internal_rtc.setAlarm(0,alarm);
        //internal_rtc.enableAlarm(0, internal_rtc.MATCH_SS); // match Every 
        #if defined USE_STANDBYMODE
        delay(10);
        //Put into low power. Expect Alarm interrupt to wake up 
        //
        internal_rtc.standbyMode();
        delay(100); //Alow USB to also wakeup
        Serial.print("..zz wakeup ");
        #endif //
    } else {
        #if defined USE_STANDBYMODE
        internal_rtc.standbyMode();
        delay(100); //Alow USB to also wakeup
        Serial.print(" wakeup ");
        #endif //
    }
}

void alarmMatch0(uint32_t flag)
{
    semaAlarm0 = true;
}
