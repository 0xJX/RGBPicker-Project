#include <Adafruit_NeoPixel.h>

#define PIN 6 
#define NUMPIXELS 20
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String mode = "DEFAULT";
int RGB[] = { 255, 255, 255 };
bool runLeds = false;

void setup() 
{
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object.
}

String GetValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void ParseData(String data)
{
  String s1 = GetValue(data, ':', 1);
  RGB[0] = GetValue(s1, ',', 0).toInt();
  RGB[1] = GetValue(s1, ',', 1).toInt();
  RGB[2] = GetValue(s1, ',', 2).toInt();
  mode = GetValue(s1, ',', 3);
 
  Serial.println(RGB[0]);
  Serial.println(RGB[1]);
  Serial.println(RGB[2]);
  Serial.println(mode);
}

void ReadSerial()
{
  String recStr = "";
  
  while (Serial.available() > 0) 
  {
    recStr = Serial.readStringUntil('\n');
    delayMicroseconds(1000);
  }

  if(recStr != "" && recStr.indexOf("SETCOLOR") >= 0)
  {
    runLeds = false;
    pixels.clear();
    //Serial.print("Received: ");
    //Serial.println(recStr);
    ParseData(recStr);
    runLeds = true;
  }
}

void RunRGB()
{
  if(!runLeds)
    return;

  if(mode == "SINGLE COLOR")
  {
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) 
      { 
        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
      }
      pixels.show(); // Update to hardware
      delay(250);
      runLeds = false;
  }
  else if(mode == "PIXEL BY PIXEL")
  {
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) 
      { 
        ReadSerial();
        if(!runLeds)
        {
          pixels.clear();
          pixels.show();
          break;
        }
        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
        pixels.show(); // Update to hardware
        delay(250);
      }
  }
  else if(mode == "RAINBOW")
  {
    
  }
  else if(mode == "NO LIGHTS")
  {
    pixels.clear();
    pixels.show();
    delay(250);
    runLeds = false;
  }
}

void loop() 
{
  ReadSerial();
  RunRGB();
  delay(0);
}
