# vaasystem
This includes the code for the vehicle accident detection and alert system using the m5stack Core2. It enables users to detect an accident and notify the set emergency services of the accident site immediately, thus increasing survival rates by timely medical assistance

The code is programmed using Arduino. The user can change the sender's email account & WiFi credentials to their own and set the emergency receiever's contact as required. The m5stack Core2 uses an esp32 board and hence the dabble library is used to retrieve the Gps coordinates. The dabble 
app must be installed on the smartphone to make use of the phone's sesnors. Refer to dabble module on thestempedia.com for further information. No other initialization required. The required IMU sesnors for accident detection are present in the m5Stack device.
