#include <Adafruit_NeoPixel.h>

#define PIN 6 
#define NUMPIXELS 20
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String mode = "DEFAULT";
int RGB[] = { 255, 255, 255 };

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

  Serial.println("Parsed values:");
  Serial.println(RGB[0]);
  Serial.println(RGB[1]);
  Serial.println(RGB[2]);
  Serial.println(mode);
}

bool ReadSerial()
{
  String recStr = "";
  
  while (Serial.available() > 0) 
  {
    recStr = Serial.readStringUntil('\n');
    delayMicroseconds(1000);
  }

  if(recStr != "" && recStr.indexOf("SETCOLOR") >= 0)
  {
    pixels.clear();
    Serial.print("Received: ");
    Serial.println(recStr);
    ParseData(recStr);
    return false;
  }
  return true;
}

void RunRGB()
{
  if(mode == "SINGLE COLOR")
  {
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) 
      { 
        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
      }
      pixels.show(); // Update to hardware
      delay(20);
  }
  else if(mode == "PIXEL BY PIXEL")
  {
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) 
      { 
        if(!ReadSerial())
            return;

        pixels.setPixelColor(i, pixels.Color(RGB[0], RGB[1], RGB[2]));
        pixels.show(); // Update to hardware
        delay(150);
      }
  }
  else if(mode == "RAINBOW")
  {
    pixels.clear();
    for(uint16_t j=0; j < 256 * 5; j++)  // 5 cycles of all colors
    {
      for(uint16_t i=0; i< NUMPIXELS; i++) 
      {
        if(!ReadSerial())
          return;

        static byte c[3];
        byte tempPos = (((i * 256 / NUMPIXELS) + j) & 255);
        if(tempPos < 85) 
        {
          c[0] = tempPos * 3;
          c[1] = 255 - tempPos * 3;
          c[2] = 0;
        } 
        else if(tempPos < 170) 
        {
          tempPos -= 85;
          c[0] = 255 - tempPos * 3;
          c[1] = 0;
          c[2] = tempPos * 3;
        } 
        else 
        {
          tempPos -= 170;
          c[0] = 0;
          c[1] = tempPos * 3;
          c[2] = 255 - tempPos * 3;
        }
        pixels.setPixelColor(i, *c, *(c+1), *(c+2));
      }
      pixels.show();
      delay(20);
    }
  }
  else if(mode == "NO LIGHTS")
  {
    pixels.clear();
    pixels.show();
    delay(20);
  }
}

void loop() 
{
  if(ReadSerial())
  {
    RunRGB();
  }
  else
  {
    delay(20);
    pixels.clear();
    pixels.show();
    delay(20);
  }
}
