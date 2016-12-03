/*
  LOG.cpp - Library for standard logging convention.
  Created by Meir Michanie, June 9, 2010.
  Released into the public domain.
  Version 0.1
*/

#include "LOG.h"

LOG::LOG(int level)
{
  setLevel(level);
}

void LOG::DATA(const char* string)
{
 if (_level > 4){
 #ifdef DEBUG_ON
  Serial.print(string);
  #endif // DEBUG_ON
 }
}

void LOG::DATA(int number)
{
 if (_level > 4){
 #ifdef DEBUG_ON
  Serial.print(number);
  #endif // DEBUG_ON
 }
}

void LOG::DEBUG(const char* string)
{
 if (_level > 3){
 #ifdef DEBUG_ON
  Serial.print("\n[DEBUG]: ");
  Serial.println(string);
  #endif // DEBUG_ON
 }
}

void LOG::INFO(const char* string)
{
 if (_level > 2){
 #ifdef DEBUG_ON
   Serial.print("\n[INFO]: ");
   Serial.println(string);
   #endif // DEBUG_ON
 }
}

void LOG::WARNING(const char* string)
{
 if (_level > 1){
 #ifdef DEBUG_ON
  Serial.print("\n[WARNING]: ");
  Serial.println(string);
  #endif // DEBUG_ON
 }
}

void LOG::CRITICAL(const char* string)
{
 if (_level > 0){
 #ifdef DEBUG_ON
  Serial.print("\n[CRITICAL]: ");
  Serial.println(string);
  #endif // DEBUG_ON
 }
}
