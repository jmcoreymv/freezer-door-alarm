#ifndef CONFIG_H
#define CONFIG_H

// Uncomment the DEBUG_MODE define to enable serial console logging
#define DEBUG_MODE
#define CONSOLE_BAUD_RATE       9600    /* For logging in debug mode */

// Sleep configuration
#define TIME_TO_SLEEP_SEC       180      /* Time to sleep in between checking the door state */

// Sensor configuration
#define DOOR_MAX_OPEN_TIME_SEC  540      /* Duration (in seconds) before door is considered left open */
#define DOOR_SENSOR_PIN         0
#define DOOR_OPEN               HIGH
#define DOOR_CLOSED             LOW

// Alive notification
#define ALIVE_NOTIFICATION_ENABLE                // Uncomment to send ALIVE notifications
#define ALIVE_NOTIFICATION_INTERVAL_DAYS     0.01

// WiFi configuration
#define WIFI_CONNECT_RETRY_DELAY_MS     200
#define WIFI_MAX_CONNECT_DELAY_MS       5000
#define WIFI_SSID                       "<YOUR WIFI SSID>"
#define WIFI_PASSWORD                   "<YOUR WIFI PASSWORD>"

// Email notification configuration
#define EMAIL_SENDER_ACCOUNT    "<YOUR SENDING EMAIL ADDRESS>"    
#define EMAIL_SENDER_PASSWORD   "<YOUR SENDING EMAIL PASSWORD>"
#define EMAIL_SMTP_SERVER       "<YOUR SENDING EMAIL SMTP SERVER>" // e.g. smtp.gmail.com
#define EMAIL_SMTP_PORT         <YOUR SENDING EMAIL SMTP SERVER PORT> // e.g. 465
#define EMAIL_SUBJECT           "Freezer Door Alarm"
#define EMAIL_MAX_ADDR_LEN      50

// Email recipient list
static const char emailRecipients[][EMAIL_MAX_ADDR_LEN] = {
    "<YOUR EMAIL RECIPIENT ADDRESS 1>",
    "<YOUR EMAIL RECIPIENT ADDRESS 2>",
    "<YOUR EMAIL RECIPIENT ADDRESS N>"
};

#endif /* CONFIG_H */