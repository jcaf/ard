char msg1[]="amor";
const char msg2[] PROGMEM=" KATHERINE";
char msg[40];
const char *setting_value = "Prima";
void setup() 
{
  Serial.begin(230400);
  
  // put your setup code here, to run once:
    strcpy(msg, "Hola ");
    strupr(msg1);
    strcat(msg, (const char *)msg1);
    strcat_P(msg, msg2);//(const char *)
    strcat_P(msg, PSTR(" Y JUANCARLOS"));//(const char *)

    //if ( strcmp_P(setting_value, (const char *) PSTR("Prima" )) == 0)
    if ( strcmp_P(setting_value, PSTR("Prima" )) == 0)
    {
    
    Serial.print(msg);
      }
    
}

void loop() {
  // put your main code here, to run repeatedly:

}


                    
