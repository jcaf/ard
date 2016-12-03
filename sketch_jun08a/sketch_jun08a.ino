void setup() {
  // put your setup code here, to run once:

  uint8_t h=1;
  uint8_t m=3;
  uint8_t s=5;

  char buf[12]; //hh:mm:ss am0//12 char
  
  char ampm_str[3]="XM";
  int ampm = 1;
  
  if (ampm == 0)
        ampm_str[0]='A';
  else
        ampm_str[0]='P';

    sprintf(buf,"%02u:%02u:%02u %s", h,m,s,ampm_str );//padding

  
  Serial.begin(9600);
  Serial.print(buf);

}

void loop() {
  // put your main code here, to run repeatedly:

}
