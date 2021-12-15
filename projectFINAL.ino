
#include <M5Core2.h>

#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#include <DabbleESP32.h> //dabble library for GPS


#if defined(ESP32)
  #include <WiFi.h>
#endif //use wifi library of esp32board
#include <ESP_Mail_Client.h> //email library


float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F; //Initialize acceleration values 

// The SMTP Session object used for sending email

SMTPSession smtp;

//Wifi credentials
const char* ssid = "Android Ap";
const char* password = "12345678"; =

//email sender , receipient & mail details
#define emailAcc"vaasystem@gmail.com"
#define emailPswd "systsemaccident1"
#define recipientEmail "as15089@nyu.edu"
#define emailSubject "[ALERT] VEHICLE ACCIDENT- MEDICAL ASSISTANCE REQUIRED"

//SMTP server details for a Gmail account
#define smtpServer "smtp.gmail.com"
#define smtpServerPort 465  //smtp server & account details


double lng;
double lat;
String location; //location variables
 
void setup() { //setup sets the whole device, for the loop to be executed
  
  M5.begin(); //Starts up the device
  M5.Lcd.print("VEHICLE ACCIDENT ALERT SYSTEM");
  Dabble.begin("M5Core2"); //Enables the bluetooth of the device to connect to the smartphone
  M5.IMU.Init(); //Initialize the accelerometer mpu6886 sensor

  WiFi.begin(ssid, password); //Establishing connection to WiFi
  while (WiFi.status() != WL_CONNECTED){
    M5.Lcd.print(".");
    delay(200);
  }
  M5.Lcd.print("");
  M5.Lcd.print("WiFi connected.");
  M5.Lcd.print("IP address: ");
  M5.Lcd.print(WiFi.localIP());
  M5.Lcd.print("");

  smtp.debug(1); //gathers info about the server connection errors etc.
  delay(2000);            


  
}


void loop() 
{  
  Dabble.processInput(); //Allows the m5 to gather info from the phone's sensors
  m5GPS(); //Stores & displays current location on the screen
  accmtr(); //detects change in acceleration/accident
}


void accmtr(){
  
    M5.IMU.getAccelData(&accX,&accY,&accZ); //gets the acceleration data of the 3 axes from the IMU unit & stores them
    
    if(fabs(accX)>4 || fabs(accY)>4){    //If abs. value>set threshold value in x/y axis,the if condition is approved
      
      M5.Lcd.print("Potential Accident detected...Please touch the screen within the next 4 seconds to terminate the Emergency Alert");
      delay(4000);
      if(M5.Touch.ispressed()) { //switch to terminate the alert
        delay(2000);
        m5.Lcd.print("Alert terminated");
        delay(2000);
      }
      
      else{ //if repsonse is not detected
        m5GPS(); //gps function is run again to get latest location
        m5.Lcd.clear();
        M5.Lcd.print("\n SENDING LOCATION");
        emailalert(); //emailalert fucntion is called to send the alert 
        M5.Lcd.print("EMAIL ALERT SENT");
        delay(2000);    
      }
      
    }
}

void m5GPS(){ //User defined function to store & display location

  
  M5.Lcd.clear();
  M5.Lcd.print("\n Current Location: ");
  lng=Sensor.getGPSlongitude(); //gets current longitude from phone's gps sensor using dabble
  M5.Lcd.print("\n Longitude= ");
  M5.Lcd.print(String (lng,6));
  lat=Sensor.getGPSLatitude(); //gets current lattitude from phone's gps sensor using dabble
  M5.Lcd.print("\n Latitude= ");
  M5.Lcd.print(String(lat,6));
  M5.Lcd.setCursor(0, 0);
  delay(500);
  String gmap="http://www.google.com/maps?q="; //default google map link to search using coordinates
  String glat= gmap+String(lat,6);
  String flat= glat + ',';
  location=flat+String(lng,6); //adds the lattitude and longitudes to the previous link 
  M5.Lcd.print("\n");

}                                                                            

void emailalert(){
  
  //Set the callback function to get the sending results 
  smtp.callback(smtpCallback); 
  
  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = smtpServer;
  session.server.port = smtpServerPort;
  session.login.email = emailAcc;
  session.login.password = emailPswd;
  session.login.user_domain = "";

  //Create an object of class SMTP_Message
  SMTP_Message message;

  //Set the message headers 
  message.sender.name = "M5";
  message.sender.email = emailAcc;
  message.subject = emailSubject;
  message.addRecipient("Emergency Contact", recipientEmail);

 
  //Set the content of message to be sent
  String itextMsg= "Car accident reported at: ";
  String textMsg = itextMsg + location;
  /*.c_str  returns a pointer to an array that 
  contains the same sequence of characters that make up the 
  value of the string plus an additional terminating null-character at the end.*/
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  /*7bit means that the data used only consists of US-ASCII character that only use the lower 7 bits
  for each character*/
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  //Connect to server with the session config 
  if (!smtp.connect(&session))
    return;

  // Start sending Email and close the session 
  if (!MailClient.sendMail(&smtp, &message))
    M5.Lcd.print("Error sending Email, " + smtp.errorReason());

}


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  M5.Lcd.print(status.info());

  /* Print the sending result */
  if (status.success()){
    M5.Lcd.print("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    M5.Lcd.print("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    M5.Lcd.print("----------------\n");
  }
}
