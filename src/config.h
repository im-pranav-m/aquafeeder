// WiFi Credentials
const char* ssid = "Vsatvision _clarvo";
const char* password = "clarvo@123";

//SET TIME IN THE FOLLOWING ARRAY {"Hour:Minute"}
String deploy_time[] = {"20:30","20:40","20:50","21:00"};//Enter in MILITARY Time...

//This should be a factor of 180... Also increasing this may make the motor bit noisy!
const int pumping_speed = 4;

//SET TIME FOR THE RTC MODULE... 
bool setTimeRTC = false; // Make sure at final upload , it is set to false...
// Because we dont want it to change time every time ESP Powers up.
int rtcminute = 54;
int rtchour = 21;
int dayOfWeek = 2;//Mon - Sun
int dayOfMonth = 1;
int month = 10;
int year = 24;