void logb_symbol_get_text(char *text_out)
{
      strcpy_P(text_out, (const char *) PSTR("D1"));
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(230400);
  char text_out[10];
  logb_symbol_get_text(text_out);
  Serial.print(text_out);
}

void loop() {
  // put your main code here, to run repeatedly:

}
