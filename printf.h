/*

 
 
 
 void slots_decrypt()
{
  for (int8_t pos = 0; pos <= 14; pos++)                                //чтение буффера
  {
    if (screen == 2) tempbuff[pos] = eep1.read(pos);
    if (screen == 3) tempbuff[pos] = eep2.read(pos);
    if (slot1) *slot1_params[pos] = eep1.read(pos);
    else *slot1_params[pos] = 0;
    if (slot2) *slot2_params[pos] = eep2.read(pos);
    else *slot2_params[pos] = 0;
  if (slot1_multi_mod == 1 || slot2_multi_mod == 1) state = 0;
  if (slot1_multi_mod == 2 || slot2_multi_mod == 2) state = 1;
  if (slot1_multi_mod == 3 || slot2_multi_mod == 3) state = 2;
  if (slot1_multi_mod == 4 || slot2_multi_mod == 4) respawn_reached = true;
  if (slot1_multi_mod == 5 || slot2_multi_mod == 5) respawn_reached = false;
  if (slot1_multi_mod == 6 || slot2_multi_mod == 6) role = 0;
  if (slot1_multi_mod == 7 || slot2_multi_mod == 7) role = 1;
  if (slot1_multi_mod == 8 || slot2_multi_mod == 8) role = 2;
  if (slot1_multi_mod == 9 || slot2_multi_mod == 9) role = 3;
  if (slot1_multi_mod == 10 || slot2_multi_mod == 10) pda_ver = 0;
  if (slot1_multi_mod == 11 || slot2_multi_mod == 11) pda_ver = 1;
  if (slot1_multi_mod == 12 || slot2_multi_mod == 12) pda_ver = 2;
  }
}

void radio_decrypt()
{
  for (int8_t i = 0; i <= 7; i = i + 2)
  {
    if (rpd)
    {
      if (msg[i] == -2)
      {
        if (msg[i + 1] == 0) state = 0;
        if (msg[i + 1] == 1) state = 1;
        if (msg[i + 1] == 2) state = 2;
      }
      if (msg[i] == -3)
      {
        if (msg[i + 1] == 0) pda_ver = 0;
        if (msg[i + 1] == 1) pda_ver = 1;
        if (msg[i + 1] == 2) pda_ver = 2;
      }
      if (msg[i] == -4)
      {
        if (msg[i + 1] == 0) role = 0;
        if (msg[i + 1] == 1) role = 1;
        if (msg[i + 1] == 2) role = 2;
        if (msg[i + 1] == 3) role = 3;
      }
    }
    if (state == 1)
    {
      if (rpd)
      {
        if (msg[i] == 1) radio_hp_mod = msg[i + 1];
        if (msg[i] == 2) radio_psy_mod = msg[i + 1];
        if (msg[i] == 3) radio_rad_mod = msg[i + 1];
        if (msg[i] == 4) radio_hp_bonus = msg[i + 1];
        if (msg[i] == 5) radio_psy_bonus = msg[i + 1];
        if (msg[i] == 6) radio_rad_bonus = msg[i + 1];
        if (msg[i] == 7) radio_hp_armor_bonus = msg[i + 1];
        if (msg[i] == 8) radio_psy_armor_bonus = msg[i + 1];
        if (msg[i] == 9) radio_rad_armor_bonus = msg[i + 1];
        if (msg[i] == 10) radio_inf_mod = msg[i + 1];
      }
      if (msg[i] == 11) radio_type_mod = msg[i + 1];
    }
    else if (msg[i] == -1)
    {
      if (msg[i + 1] == 0) respawn_reached = false;
      if (msg[i + 1] == 1) respawn_reached = true;
    }
  }
  for (int8_t pos = 0; pos <= 7; pos++)  msg[pos] = 0;
  rpd = false;
}

 
 
 
 
 
 
 
 
 
 
 
 
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 /*  Galileo support from spaniakos <spaniakos@gmail.com> */

/**
 * @file printf.h
 *
 * Setup necessary to direct stdout to the Arduino Serial library, which
 * enables 'printf'
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#if defined (ARDUINO) && !defined (__arm__) && !defined(__ARDUINO_X86__)

int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

#elif defined (__arm__)

void printf_begin(void){}

#elif defined(__ARDUINO_X86__)
int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}

void printf_begin(void)
{
  //JESUS - For reddirect stdout to /dev/ttyGS0 (Serial Monitor port)
  stdout = freopen("/dev/ttyGS0","w",stdout);
  delay(500);
  printf("redirecting to Serial...");
  
  //JESUS -----------------------------------------------------------
}
#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif // __PRINTF_H__




/*
for (int8_t z = 4; z<=12, z++)
for (int8_t y = 3; y<=11; y = y+4)
for (int8_t i = 3; i<=5; i++)
{
lcd.printat(y, i, F("   "));
lcd.printat(y + 3 - getLen(tempbuff[z]), i, tempbuff[z]);
}
 */













