void setup() 
{
  Serial.begin(9600);
}

const int buffSize = 16;
unsigned char buff[buffSize];

bool getDataFromCOM ()
{
  Serial.print("START COLLECTING DATA//");
  int var = 0;
  int i = 0;
  while (1)
  {
    var = Serial.read();
    if (var == -1)
    {
      continue;
    }
    
    buff[i] = var;

    Serial.print(buff[i]);
    Serial.print("//");

    if (i == buffSize - 1)
    {
      if (var == 0x0A)
      {
        return true;
      }
      return false;
    }
    ++i;
  }
}

void loop() 
{
  if (Serial.available() > 0)
  {
    if (getDataFromCOM())
    {
      Serial.print("DATA COLLECTED SUCCESSFUL//");
    }
    else
    {
      Serial.print("DATA COLLECTED BUT END FRAME NOT RECEIVED (WRONG PACKET)//");
    }
  }
}