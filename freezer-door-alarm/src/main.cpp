#include <Arduino.h>
#include "ESP32_MailClient.h"
#include "config.h"
#include "log.h"


#define SEC_TO_USEC     (1000000)  /* Conversion factor for micro seconds to seconds */
#define DAY_TO_SEC      (24*60*60)
#define MIN_TO_SEC      (60)

// Variables to keep track of across deep sleep cycles
// These stick around in the RTC memory
RTC_DATA_ATTR int   doorOpenTimeSec      = 0;
RTC_DATA_ATTR bool  doorNotificationSent = false;

RTC_DATA_ATTR int   timeSinceAliveNotificationSec = 0;
RTC_DATA_ATTR int   timeSinceBootSec = 0;


void incrementTime();

// Sensor related functions
int checkDoorSensor();
int isDoorLeftOpen();

// Notification related functions
int connectToWifi();
void sendEmailNotifications(String messageStr, void(*cb_func)(SendStatus Info));
void sendEmailAliveCallback(SendStatus info);
void sendEmailDoorOpenCallback(SendStatus info);
String addHtmlWrapper(String messageStr);

int doorOpenNotificationCheck();
int aliveNotificationCheck();

void sendDoorOpenNotification();
void sendAliveNotification();



// Main function
void setup(){
    logSetup();

    // Keep track of various time-related variables
    incrementTime();

    float timeAliveInDays = (float)timeSinceBootSec / (float)DAY_TO_SEC;
    log("Time since boot: " + String(timeAliveInDays) + " days");

    if (doorOpenNotificationCheck() < 0) {
        goto prepareForSleep;
    }

    if (aliveNotificationCheck() < 0) {
        goto prepareForSleep;
    }


prepareForSleep:
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_SEC * SEC_TO_USEC);
    log("Sleeping for " + String(TIME_TO_SLEEP_SEC) + " seconds...");
    logFlush();
    esp_deep_sleep_start();
}
  

// Keep track of how many times in a row the door has been open to determine if someone left it open
int isDoorLeftOpen() {
    if (checkDoorSensor() == DOOR_OPEN) {
        doorOpenTimeSec += TIME_TO_SLEEP_SEC;
        log("Door open time: " + String(doorOpenTimeSec) + " seconds");
    }
    else {
        log("Door closed");
        doorNotificationSent = false;
        doorOpenTimeSec = 0;
    }

    if (doorOpenTimeSec >= DOOR_MAX_OPEN_TIME_SEC) {
        return true;
    }

    return false;
}


void incrementTime() {
    timeSinceBootSec += TIME_TO_SLEEP_SEC;
    timeSinceAliveNotificationSec += TIME_TO_SLEEP_SEC;
}

// Check whether the door is currently open
int checkDoorSensor() {
    int doorState = 0;
    pinMode(DOOR_SENSOR_PIN, INPUT);
    doorState = digitalRead(DOOR_SENSOR_PIN);
    return doorState;
}


int connectToWifi() {
    int i = 0;
    log("Connecting");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(WIFI_CONNECT_RETRY_DELAY_MS);
        i += WIFI_CONNECT_RETRY_DELAY_MS;

        if (i > WIFI_MAX_CONNECT_DELAY_MS) {
            log("Unable to connect to WiFi.");
            return -1;
        }
    }

    log("WiFi connected.");
    return 0;
}


int doorOpenNotificationCheck() {
    if (isDoorLeftOpen() && !doorNotificationSent) {

        if (connectToWifi() < 0) {
            return -1;
        }

        sendDoorOpenNotification(); 
    }
    return 0;
}


int aliveNotificationCheck() {
    
#ifdef ALIVE_NOTIFICATION_ENABLE
    if (timeSinceAliveNotificationSec >= (ALIVE_NOTIFICATION_INTERVAL_DAYS * DAY_TO_SEC)) {
        if (connectToWifi() < 0) {
            return -1;
        }
        sendAliveNotification();
    }
#endif

    return 0;
}

String addHtmlWrapper(String messageStr) {
    return "<div style=\"color:#2f4468;\"><h1>" + messageStr + "</h1><p>- Sent from the Freezer Monitor</p></div>";
}


void sendDoorOpenNotification() {
    float openTimeInMinutes = (float)DOOR_MAX_OPEN_TIME_SEC / (float)MIN_TO_SEC;
    String msg = addHtmlWrapper("Alert! Freezer door has been open for " + String(openTimeInMinutes, 1) + " minutes");
    sendEmailNotifications(msg, sendEmailDoorOpenCallback);
}


void sendAliveNotification() {
    float aliveTimeInDays = (float)timeSinceBootSec / (float)DAY_TO_SEC;
    String msg = addHtmlWrapper("Info: Just letting you know I've been alive for  " + String(aliveTimeInDays, 1) + " days");
    sendEmailNotifications(msg, sendEmailAliveCallback);
}


void sendEmailNotifications(String messageStr, void (*cb_func)(SendStatus )) {
    SMTPData smtpData;

    log("Preparing to send email");

    smtpData.setLogin(EMAIL_SMTP_SERVER, EMAIL_SMTP_PORT, EMAIL_SENDER_ACCOUNT, EMAIL_SENDER_PASSWORD);
    smtpData.setSender("Home Monitoring", EMAIL_SENDER_ACCOUNT);
    smtpData.setPriority("High");
    smtpData.setSubject(EMAIL_SUBJECT);
    smtpData.setMessage(messageStr, true);

    // Add recipients
    for (size_t i = 0; i < (sizeof(emailRecipients) / sizeof(emailRecipients[0])); i++) {
        smtpData.addRecipient(emailRecipients[i]);
    }

    smtpData.setSendCallback(cb_func);

    //Start sending Email
    if (!MailClient.sendMail(smtpData)) {
        log("Error sending Email, " + MailClient.smtpErrorReason());
    }

    smtpData.empty();
}


// Callback function to get the Email sending status
void sendEmailDoorOpenCallback(SendStatus msg) {
    log(msg.info());

    if (msg.success()) {
        log("sendEmailDoorOpenCallback - Success");
        log("-----------");
        doorNotificationSent = true;
    }
}


void sendEmailAliveCallback(SendStatus msg) {
    log(msg.info());

    if (msg.success()) {
        log("sendEmailAliveCallback - Success");
        timeSinceAliveNotificationSec = 0;
    }
}


void loop(){
  //This is not going to be called
}
