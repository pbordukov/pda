#include <SPI.h>
#include <EEPROM.h>
#include "SPIEEP.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "pcd8544.h"
#include "MyMenu.h"
#include "MyMenu_strings.cpp"
#include "printf.h"
  
void Callback(int8_t);
int8_t getLen(long);
void light_event();
void sound_event(uint8_t);
void vibro_event();
void button_event();
void turnoff_events();
void update_slots();
void state_check();
//void serial_diag();

SPIEEP eep1(8, 16, 512);    // for 25LC040
SPIEEP eep2(8, 16, 512);
pcd8544 lcd(4, 2, 3);

MItm items[] = {
  MItm(0, 0), //Меню 0
  MItm(1),  //Статистика 1
  MItm(2),  //Слот1 2
  MItm(3),  //Слот2 3
  MItm(4),  //Настройки 4
  MItm(1, 0), //Статистика 5
  MItm(5),  //Вкл/Выкл перк 6
  MItm(2, 0), //Слот1 7
  MItm(6),  //Слот1 Вкл/Выкл 8
  MItm(3, 0), //Слот2 9
  MItm(7),  //Слот2 Вкл/Выкл 10
  MItm(4, 0), //Настройки 11
  MItm(8),  //Звук 12
  MItm(9),  //Контраст 13
  MItm(10),  //Подсветка 14
  MItm(11),  //Вибро 15
  MItm(12),  //Диагностика 16
  MItm(13),  //Таймер 17
  MItm(14),  //Харакири 18
  MItm(14, 3), //Харакири 19
  MItm(15),  //ДА 20
  MItm(4),  //НЕТ 21
};
Menu menu(items, 22, &lcd, Callback, 5); //так создается меню


RF24 radio(A0, 10);

const int8_t vibropin = 8;
const int8_t soundpin = 9;
const int8_t lightpin = A3;
uint8_t contrast = EEPROM.read(0);


unsigned long curTime = 0;    // таймер
unsigned long tme = 0;   //
unsigned long tme2 = 0;   //
unsigned long tme5 = 0;   //
unsigned long tme6 = 0;   //
unsigned long tme7 = 0;   //
unsigned long zombie_time = 15000; // длительность зомби
unsigned long zombie_timer_start = 0; //
unsigned long death_time = 3000; // длительность мертвяка
unsigned long death_timer_start = 0;   //

int8_t pda_ver = 2;
int8_t role = 0;                     // 0 - stalker, 1 - controller, 2 - scientist, 3 - burer
int8_t screen = 1;
int8_t state = 0;                    // 0 - dead, 1 - alive, 2 - zombie

int8_t tempbuff[15];
int8_t msg[12];              // буффер приёма сигналов
//----------------          RADIO PARAMS        ----------------------
int8_t radio_hp_bonus = 0;
int8_t radio_hp_armor_bonus = 0;
int8_t radio_hp_mod = 0;
int8_t radio_psy_bonus = 0;
int8_t radio_psy_armor_bonus = 0;
int8_t radio_psy_mod = 0;
int8_t radio_rad_bonus = 0;
int8_t radio_rad_armor_bonus = 0;
int8_t radio_rad_mod = 0;
int8_t radio_inf_mod = 0;
int8_t radio_type_mod = 0;
int8_t radio_multi_mod = 0;

int8_t radio_hp_bonus_counter = 0;
int8_t radio_hp_armor_bonus_counter = 0;
int8_t radio_hp_mod_counter = 0;
int8_t radio_psy_bonus_counter = 0;
int8_t radio_psy_armor_bonus_counter = 0;
int8_t radio_psy_mod_counter = 0;
int8_t radio_rad_bonus_counter = 0;
int8_t radio_rad_armor_bonus_counter = 0;
int8_t radio_rad_mod_counter = 0;
int8_t radio_inf_mod_counter = 0;
int8_t radio_type_mod_counter = 0;
int8_t radio_multi_mod_counter = 0;

//----------------          SLOT1 PARAMS        ----------------------
int8_t slot1_auto = 0;
int8_t slot1_counter = 0;
int8_t slot1_period = 0;
int8_t slot1_hp_bonus = 0;
int8_t slot1_hp_armor_bonus = 0;
int8_t slot1_hp_mod = 0;
int8_t slot1_psy_bonus = 0;
int8_t slot1_psy_armor_bonus = 0;
int8_t slot1_psy_mod = 0;
int8_t slot1_rad_bonus = 0;
int8_t slot1_rad_armor_bonus = 0;
int8_t slot1_rad_mod = 0;
int8_t slot1_inf_mod = 0;
int8_t slot1_type = 0;
int8_t slot1_multi_mod = 0;

int8_t slot1_period_now = 0;
//----------------          SLOT2 PARAMS        ----------------------
int8_t slot2_auto = 0;
int8_t slot2_counter = 0;
int8_t slot2_period = 0;
int8_t slot2_hp_bonus = 0;
int8_t slot2_hp_armor_bonus = 0;
int8_t slot2_hp_mod = 0;
int8_t slot2_psy_bonus = 0;
int8_t slot2_psy_armor_bonus = 0;
int8_t slot2_psy_mod = 0;
int8_t slot2_rad_bonus = 0;
int8_t slot2_rad_armor_bonus = 0;
int8_t slot2_rad_mod = 0;
int8_t slot2_inf_mod = 0;
int8_t slot2_type = 0;
int8_t slot2_multi_mod = 0;

int8_t slot2_period_now = 0;
//----------------          HP        ----------------------
const int8_t hp_max = 100;
int now_hp_base = hp_max;
int now_hp = now_hp_base;           // кол-во здоровья
int8_t now_hp_armor_base = 0;
int8_t now_hp_armor = now_hp_armor_base;
//----------------          PSY       ----------------------
const int8_t psy_max = 100;
int now_psy_base = psy_max;
int now_psy;
int8_t now_psy_armor_base = 0;
int8_t now_psy_armor = now_psy_armor_base;
//----------------          RAD       ----------------------
const int8_t rad_max = 100;
int now_rad_base;
int now_rad;
int8_t now_rad_armor_base = 0;
int8_t now_rad_armor = now_rad_armor_base;
//----------------          INF TIME + RAD TIME        ----------------------
int inf_time = -1;
int8_t rad_time = 0;
//-------------------------------------------------------------------------------------------------------------------------------------

int8_t *radio_params[] = {
  &radio_hp_bonus,
  &radio_hp_armor_bonus,
  &radio_hp_mod,
  &radio_psy_bonus,
  &radio_psy_armor_bonus,
  &radio_psy_mod,
  &radio_rad_bonus,
  &radio_rad_armor_bonus,
  &radio_rad_mod,
  &radio_inf_mod,
  &radio_type_mod,
  &radio_multi_mod,
};
int8_t *radio_counters[] = {
  &radio_hp_bonus_counter,
  &radio_hp_armor_bonus_counter,
  &radio_hp_mod_counter,
  &radio_psy_bonus_counter,
  &radio_psy_armor_bonus_counter,
  &radio_psy_mod_counter,
  &radio_rad_bonus_counter,
  &radio_rad_armor_bonus_counter,
  &radio_rad_mod_counter,
  &radio_inf_mod_counter,
  &radio_type_mod_counter,
  &radio_multi_mod_counter,
};
int8_t *slot1_params[] = {
  &slot1_auto,
  &slot1_counter,
  &slot1_period,
  &slot1_hp_bonus,
  &slot1_hp_armor_bonus,
  &slot1_hp_mod,
  &slot1_psy_bonus,
  &slot1_psy_armor_bonus,
  &slot1_psy_mod,
  &slot1_rad_bonus,
  &slot1_rad_armor_bonus,
  &slot1_rad_mod,
  &slot1_inf_mod,
  &slot1_type,
  &slot1_multi_mod
};
int8_t *slot2_params[] = {
  &slot2_auto,
  &slot2_counter,
  &slot2_period,
  &slot2_hp_bonus,
  &slot2_hp_armor_bonus,
  &slot2_hp_mod,
  &slot2_psy_bonus,
  &slot2_psy_armor_bonus,
  &slot2_psy_mod,
  &slot2_rad_bonus,
  &slot2_rad_armor_bonus,
  &slot2_rad_mod,
  &slot2_inf_mod,
  &slot2_type,
  &slot2_multi_mod
};

//-------------------------------------------------------------------------------------------------------------------------------------

boolean respawn_reached = false;
boolean radiated = false;   //  облучен
boolean infected = false;   //  инфицирован
boolean control = false; //включение ауры контролёра
boolean science = false; //включение ауры ученого
boolean slot1 = false;
boolean slot2 = false;
boolean slot1_available = false;
boolean slot2_available = false;
boolean sound = false;      // звук
boolean vibro = false;      // вибро
boolean light = true;      // подсветка
void setup(void)
{
  pinMode(A1, INPUT);       //настройка кнопок
  pinMode(A2, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(5, OUTPUT);       //EEPROM1 master port
  pinMode(6, OUTPUT);       //EEPROM2 master port
  pinMode(10, OUTPUT);      //NRF24 master port
  pinMode(lightpin, OUTPUT);
  pinMode(soundpin, OUTPUT);
  pinMode(vibropin, OUTPUT);
  digitalWrite(soundpin, LOW);
  digitalWrite(lightpin, HIGH);
  digitalWrite(vibropin, LOW);
    Serial.begin(9600);
  eep1.begin_spi(5);
  eep2.begin_spi(6);
  lcd.begin(0x40);
  printf_begin();
  radio.begin();                     // настройка радио
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(1, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(0);
  radio.setAutoAck(0);
  radio.openWritingPipe(0xF0F0F0F0C3LL);
  radio.openReadingPipe(1, 0xF0F0F0F0C3LL);
  radio.startListening();
  radio.printDetails();
  lcd.clear();
  menu.go2Item(1);
  Callback(1);
}

void loop(void)
{
  if (state == 0)
  {
    respawn_reached = false;
    death_timer_start = 0;
    light_event();
    sound_event(8);
    vibro_event();
    if (screen == 1)
    {
      lcd.clearLine(0);
      lcd.clearLine(1);
      lcd.printat(3, 3, F("           "));
      lcd.printat(3, 4, F("           "));
      lcd.printat(3, 5, F("           "));
    }
    //    Serial.println(F("Death takes its toll"));
  }
  while (state == 0)
  {
    //------------------ Сканируем эфир  ----------------------------------
    if (radio.available()) while (radio.available()) radio.read(&msg, sizeof(msg));
    //------------------  читаем данные с радио  ----------------------------------
    if (msg[11] != 0)
    {
      *radio_params[11] = msg[11];
      *radio_counters[11] = *radio_counters[11] + 1;
      msg[11] = 0;           //чистим радио буффер
      state_check();
    }
    //------------------  начало секундного шага  ----------------------------------
    curTime = millis();
    if (curTime - tme >= 1000)
    {
      if (screen == 1)  lcd.printat(4, 0, F("МЕРТВ"));
      *slot1_params[14] = eep1.read(14);
      *slot2_params[14] = eep2.read(14);
      state_check();
      if (respawn_reached)
      {
        if (death_timer_start == 0) death_timer_start = millis();
        else
        {
          if (curTime - death_timer_start <= death_time)          //время мертвяка
          {
            lcd.printat(12, 0, F("  "));
            lcd.printat(12, 0, (death_time + death_timer_start - curTime) / 1000);
          }
          else state = 1;
        }
      }
      //      serial_diag();
      for (int8_t pos = 0; pos <= 11; pos++)
      {
        *radio_params[pos] = 0;
        *radio_counters[pos] = 0;
      }
      tme = curTime ;
    }
    button_event();
    turnoff_events();
  }

  if (state == 1)
  {
    respawn_reached = false;
    infected = false;
    radiated = false;
    slot1 = false;
    slot2 = false;
    now_hp_base = hp_max;
    now_psy_base = psy_max;
    now_rad_base = 0;
    now_hp = now_hp_base;
    now_psy = now_psy_base;
    now_rad = now_rad_base;
    inf_time = -1;
    light_event();
    sound_event(8);
    vibro_event();
    if (screen == 1)
    {
      lcd.clearLine(0);
      lcd.clearLine(1);
      lcd.printat(3, 3, F("           "));
      lcd.printat(3, 4, F("           "));
      lcd.printat(3, 5, F("           "));
    }
    //    Serial.println(F("Back from the dead"));
  }

  while (state == 1)
  {
    //------------------ Сканируем эфир  ----------------------------------
    if (radio.available()) while (radio.available()) radio.read(&msg, sizeof(msg));
    //------------------  читаем данные с радио  ----------------------------------
    for (int8_t i = 0; i <= 11; i++)
    {
      if (msg[i] != 0)
      {
        if ( i <= 8)
        {
          *radio_params[i] += msg[i];
          *radio_counters[i] = *radio_counters[i] + 1;
        }
        else
        {
          *radio_params[i] = msg[i];
          *radio_counters[i] = *radio_counters[i] + 1;
        }
        msg[i] = 0;           //чистим радио буффер
      }
    }
    //------------------  начало секундного шага  ----------------------------------
    curTime = millis();
    if ( curTime - tme >= 1000 )
    {
      update_slots();                                                      // читаем параметры с слотов
      state_check();
      //------------------  рассчёт параметров  ----------------------------------
      now_hp_armor = now_hp_armor_base + radio_hp_armor_bonus + slot1_hp_armor_bonus + slot2_hp_armor_bonus;              // рассчёт брони
      now_psy_armor = now_psy_armor_base + radio_psy_armor_bonus + slot1_psy_armor_bonus + slot2_psy_armor_bonus;
      now_rad_armor = now_rad_armor_base + radio_rad_armor_bonus + slot1_rad_armor_bonus + slot2_rad_armor_bonus;

      if (radio_hp_mod + now_hp_armor <= 0 && radio_hp_mod < 0) radio_hp_mod += now_hp_armor;
      now_hp_base += radio_hp_mod;
      if (radio_psy_mod + now_psy_armor <= 0 && radio_psy_mod < 0) radio_psy_mod += now_psy_armor;
      now_psy_base += radio_psy_mod;
      if (radio_rad_mod - now_rad_armor >= 0 && radio_rad_mod > 0) radio_rad_mod += now_rad_armor;
      now_rad_base += radio_rad_mod;

      //------------------------------------ SLOTS AVAILABLE ---------------------------------------------------
      if (slot1_period_now <= 1)
      {
        slot1_period_now = slot1_period;                 // обновляем счётчик
        slot1_available = true;
      }
      else
      {
        slot1_period_now--;
        slot1_available = false;
      }
      if (slot2_period_now <= 1)
      {
        slot2_period_now = slot2_period;                 // обновляем счётчик
        slot2_available = true;
      }
      else
      {
        slot2_period_now--;
        slot2_available = false;
      }
      //------------------------------------- SLOT1 -------------------------------------------------------------
      if (slot1 && slot1_available)
      {
        if (slot1_auto == 0) slot1 = false;
        if (slot1_counter != 0)                                      // если можно использовать
        {
          if (slot1_counter > 0)                                      //еще есть заряды
          {
            slot1_counter--;                                          // уменьшаем на 1 количество использований (использовали вещь)
            eep1.write(1, slot1_counter);                         // записали это в вещь
          }

          if (slot1_hp_mod + now_hp_armor <= 0 && slot1_hp_mod < 0) slot1_hp_mod += now_hp_armor;
          now_hp_base += slot1_hp_mod;
          if (slot1_psy_mod + now_psy_armor <= 0 && slot1_psy_mod < 0) slot1_psy_mod += now_psy_armor;
          now_psy_base += slot1_psy_mod;
          if (slot1_rad_mod - now_rad_armor >= 0 && slot1_rad_mod > 0) slot1_rad_mod -= now_rad_armor;
          now_rad_base += slot1_rad_mod;
        }
      }
      //------------------------------------- SLOT2 -------------------------------------------------------------
      /*      if (slot2 && slot2_available)
            {
              if (slot2_auto == 0) slot2 = false;
              if (slot2_counter != 0)                                      // если можно использовать
              {
                if (slot2_counter > 0)                                      //еще есть заряды
                {
                  slot2_counter--;                                          // уменьшаем на 1 количество использований (использовали вещь)
                  eep2.write(1, slot2_counter);                         // записали это в вещь
                }

                if (slot2_hp_mod + now_hp_armor <= 0 && slot2_hp_mod < 0) slot2_hp_mod += now_hp_armor;
                now_hp_base += slot2_hp_mod;
                if (slot2_psy_mod + now_psy_armor <= 0 && slot2_psy_mod < 0) slot2_psy_mod += now_psy_armor;
                now_psy_base += slot2_psy_mod;
                if (slot2_rad_mod - now_rad_armor >= 0 && slot2_rad_mod > 0) slot2_rad_mod -= now_rad_armor;
                now_rad_base += slot2_rad_mod;
              }
            }
      */
      //------------------------------------------------Рассчет итоговых параметров---------------------------------------------------
      if (now_hp_base > hp_max) now_hp_base = hp_max;
      if (now_psy_base > psy_max) now_psy_base = psy_max;
      if (now_rad_base < 0) now_rad_base = 0;
      now_hp = now_hp_base + radio_hp_bonus + slot1_hp_bonus + slot2_hp_bonus;
      now_psy = now_psy_base + radio_psy_bonus + slot1_psy_bonus + slot2_psy_bonus;
      now_rad = now_rad_base + radio_rad_bonus + slot1_rad_bonus + slot2_rad_bonus;
      if (now_hp < 0) now_hp = 0;
      if (now_psy < 0) now_psy = 0;
      if (now_rad > 999) now_rad = 999;
      if (now_hp <= 0 || (infected && inf_time == 0)) state = 0;
      if (now_rad >= rad_max) radiated = true;
      if (now_rad <= rad_max / 2) radiated = false;
      if (now_psy <= 0) state = 2;

      if (radiated)
      {
        if (rad_time > 0) rad_time--;
        else
        {
          rad_time = 60;
          now_hp_base--;                                                         //Облучён
        }
      }

      if (infected && (inf_time > 0)) inf_time--;

      if ((radio_inf_mod + slot1_inf_mod + slot2_inf_mod > 0) && !infected)
      {
        infected = true;
        inf_time = (radio_inf_mod + slot1_inf_mod + slot2_inf_mod) * 60;
        if (inf_time > 5999) inf_time = 5999;
      }


      //--------------------------------------- Графика и звук -----------------------------------------------
      if (radio_hp_mod < 0 || radio_psy_mod < 0 || radio_rad_mod > 0)
      {
        light_event();
        sound_event(32);
        vibro_event();
      }
      if (radio_hp_mod > 0 || radio_psy_mod > 0 || radio_rad_mod < 0)
        light_event();

      if (screen == 1)
      {
        lcd.clearLine(0);
        lcd.printat(5, 0, F("ЖИВ"));
        if (infected)                                                          //заражен, но еще есть время
        {
          lcd.printat(9, 0, F("ЗРЖ"));
          lcd.printat(14 - getLen(inf_time / 60), 0, inf_time / 60);
        }
        if (radiated) lcd.printat(0, 0, F("ОБЛЧ"));
        lcd.clearLine(1);
        if (radio_type_mod != 0)                                                            //       распознавание типов аномалий
        {
          if (pda_ver == 1 && radio_type_mod > 1) lcd.printat(3, 1, F("Аномалия"));
          if (pda_ver == 2) lcd.printat(3, 1, reinterpret_cast<const __FlashStringHelper *> pgm_read_word(&(anomaly_table[radio_type_mod])));
          if (radio_type_mod > 1)                                                            //       не база
          {
            light_event();
            sound_event(32);
          }
        }
        lcd.printat(3, 3, F("           "));
        lcd.printat(3, 4, F("           "));
        lcd.printat(3, 5, F("           "));
        lcd.printat(6 - getLen(now_hp), 3, now_hp);
        lcd.printat(10 - getLen(now_psy), 3, now_psy);
        lcd.printat(14 - getLen(now_rad), 3, now_rad);
        lcd.printat(6 - getLen(now_hp_armor), 4, now_hp_armor);
        lcd.printat(10 - getLen(now_psy_armor), 4, now_psy_armor);
        lcd.printat(14 - getLen(now_rad_armor), 4, now_rad_armor);
        if (radio_hp_mod + slot1_hp_mod + slot2_hp_mod != 0)   lcd.printat(6 - getLen(radio_hp_mod + slot1_hp_mod + slot2_hp_mod), 5, radio_hp_mod + slot1_hp_mod + slot2_hp_mod);
        if (radio_psy_mod + slot1_psy_mod + slot2_psy_mod != 0)   lcd.printat(10 - getLen(radio_psy_mod + slot1_psy_mod + slot2_psy_mod), 5, radio_psy_mod + slot1_psy_mod + slot2_psy_mod);
        if (radio_rad_mod + slot1_rad_mod + slot2_rad_mod != 0)   lcd.printat(14 - getLen(radio_rad_mod + slot1_rad_mod + slot2_rad_mod), 5, radio_rad_mod + slot1_rad_mod + slot2_rad_mod);
      }
      else if (screen == 2)
      {
        lcd.clearLine(0);
        if (slot1) lcd.printat(0, 0, F("ВКЛ"));
        else lcd.printat(0, 0, F("ВЫКЛ"));
        lcd.printat(6, 0, reinterpret_cast<const __FlashStringHelper *> pgm_read_word(&(item_table[tempbuff[13]])));
        if (tempbuff[0] == 1) lcd.printat(0, 2, F("АВ"));
        else lcd.printat(0, 2, F("РЧ"));
        if (tempbuff[12] != 0) lcd.printat(10, 1, F("ЗРЖ!"));
        else  lcd.printat(10, 1, F("    "));
        lcd.printat(0, 1, F("   "));
        if (tempbuff[1] >= 0) lcd.printat(3 - getLen(tempbuff[1]), 1, tempbuff[1]);
        else lcd.printat(0, 1, F("***"));
        lcd.printat(5, 1, F("   "));
        lcd.printat(8 - getLen(tempbuff[2]), 1, tempbuff[2]);
        lcd.printat(3, 3, F("   "));
        lcd.printat(6 - getLen(tempbuff[3]), 3, tempbuff[3]);
        lcd.printat(3, 4, F("   "));
        lcd.printat(6 - getLen(tempbuff[4]), 4, tempbuff[4]);
        lcd.printat(3, 5, F("   "));
        lcd.printat(6 - getLen(tempbuff[5]), 5, tempbuff[5]);
        lcd.printat(7, 3, F("   "));
        lcd.printat(10 - getLen(tempbuff[6]), 3, tempbuff[6]);
        lcd.printat(7, 4, F("   "));
        lcd.printat(10 - getLen(tempbuff[7]), 4, tempbuff[7]);
        lcd.printat(7, 5, F("   "));
        lcd.printat(10 - getLen(tempbuff[8]), 5, tempbuff[8]);
        lcd.printat(11, 3, F("   "));
        lcd.printat(14 - getLen(tempbuff[9]), 3, tempbuff[9]);
        lcd.printat(11, 4, F("   "));
        lcd.printat(14 - getLen(tempbuff[10]), 4, tempbuff[10]);
        lcd.printat(11, 5, F("   "));
        lcd.printat(14 - getLen(tempbuff[11]), 5, tempbuff[11]);

        for (int8_t pos = 0; pos <= 14; pos++) tempbuff[pos] = 0;
      }

      //      serial_diag();                                    //вывод параметров для диагностики

      for (int8_t pos = 0; pos <= 11; pos++)
      {
        *radio_params[pos] = 0;
        *radio_counters[pos] = 0;
      }
      tme = curTime;
    }
    //-------------------------------------------------------конец шага 1 сек-----------------------------------
    button_event();
    turnoff_events();
  }

  if (state == 2)
  {
    zombie_timer_start = millis();
    respawn_reached = false;
    light_event();
    sound_event(8);
    vibro_event();
    if (screen == 1)
    {
      lcd.clearLine(0);
      lcd.clearLine(1);
      lcd.printat(3, 3, F("           "));
      lcd.printat(3, 4, F("           "));
      lcd.printat(3, 5, F("           "));
    }
    //    Serial.println(F("Brains! More brains!"));
  }
  while (state == 2)
  {
    //------------------ Сканируем эфир  ----------------------------------
    if (radio.available()) while (radio.available()) radio.read(&msg, sizeof(msg));
    //------------------  читаем данные с радио  ----------------------------------
    if (msg[11] != 0)
    {
      *radio_params[11] = msg[11];
      *radio_counters[11] = *radio_counters[11] + 1;
      msg[11] = 0;           //чистим радио буффер
      if (curTime - zombie_timer_start >= zombie_time) state_check();
    }
    //------------------  начало секундного шага  ----------------------------------
    curTime = millis();
    if (curTime - tme >= 1000)
    {
      if (screen == 1) lcd.printat(4, 0, F("ЗОМБИ"));
      *slot1_params[14] = eep1.read(14);
      *slot2_params[14] = eep2.read(14);
      state_check();
      if (curTime - zombie_timer_start <= zombie_time)          //время zombie
      {
        lcd.printat(12, 0, F("   "));
        lcd.printat(12, 0, (zombie_time - (curTime - zombie_timer_start)) / 1000);
      }
      else if (respawn_reached) state = 1;
      //      serial_diag();
      for (int8_t pos = 0; pos <= 11; pos++)
      {
        *radio_params[pos] = 0;
        *radio_counters[pos] = 0;
      }
      tme = curTime;
    }
    button_event();
    turnoff_events();
  }
}


void Callback (int8_t idx)
{
  if (idx == 1)   //Stats
  {
    lcd.clear();
    lcd.printat(3, 2, F("ЗДР ПСИ РАД"));
    lcd.printat(0, 3, F("ТК"));
    lcd.printat(0, 4, F("ЗЩ"));
    lcd.printat(0, 5, F("УД"));
  }
  else if (idx == 2)     //Slot1
  {
    lcd.clear();
    lcd.printat(3, 1, F("шт"));
    lcd.printat(8, 1, F("с"));
    lcd.printat(3, 2, F("ЗДР ПСИ РАД"));
    lcd.printat(0, 3, F("БН"));
    lcd.printat(0, 4, F("ЗЩ"));
    lcd.printat(0, 5, F("МД"));
  }
  else if (idx == 3)     //Slot2
  {
    lcd.clear();
    lcd.printat(3, 1, F("шт"));
    lcd.printat(8, 1, F("с"));
    lcd.printat(3, 2, F("ЗДР ПСИ РАД"));
    lcd.printat(0, 3, F("БН"));
    lcd.printat(0, 4, F("ЗЩ"));
    lcd.printat(0, 5, F("МД"));
  }
  else if (idx == 4)    //Settings
  {
    lcd.clearLine(0);
    lcd.printat(0, 0, F("Настройки"));
  }
  else if (idx == 5)
  {
    /*    lcd.clearLine(1);
        lcd.setCursor(2, 1);
        if (role == 1)
        {
          if (control) lcd.print(F("PSY atk OFF"));
          else lcd.print(F("PSY atk ON"));
          control = !(control);
        }
        if (role == 2)
        {
          if (science) lcd.print(F("Gadgets OFF"));
          else lcd.print(F("Gadgets ON"));
          science = !(science);
        }
        if (role == 3)
        {
          lcd.print(F("Burer atack!"));
        }
    */
  }
  else if (idx == 6)
  {
    slot1 = !(slot1);
  }
  else if (idx == 7)
  {
    slot2 = !(slot2);
  }
  else if (idx == 8)                                 //звук
  {
    for (int8_t x = 1; x <= 5; x++) lcd.clearLine(x);
    if (sound) lcd.printat(2, 3, F("Звук Выкл"));
    else lcd.printat(2, 3, F("Звук Вкл"));
    sound = !(sound);
    sound_event(2);
  }
  else if (idx == 9)
  {
    if (contrast != 0x40) contrast = 0x40;
    else contrast = 0x33;
    lcd.setcontrast(contrast);
    EEPROM.update(0, contrast);
  }
  else if (idx == 10)                           //подсветка
  {
    for (int8_t x = 1; x <= 5; x++) lcd.clearLine(x);
    if (light) lcd.printat(2, 3, F("Подсв Выкл"));
    else lcd.printat(2, 3, F("Подсв Вкл"));
    light = !(light);
  }
  else if (idx == 11)                             //вибро
  {
    for (int8_t x = 1; x <= 5; x++) lcd.clearLine(x);
    if (vibro) lcd.printat(2, 3, F("Вибро Выкл"));
    else lcd.printat(2, 3, F("Вибро Вкл"));
    vibro = !(vibro);
    vibro_event();
  }
  else if (idx == 12)                                            //diagnostix
  {
    for (int8_t x = 1; x <= 5; x++) lcd.clearLine(x);
    lcd.printat(2, 4, death_time / 1000);
  }

  else if (idx == 13)                                           //таймер
  {
    for (int8_t x = 1; x <= 5; x++) lcd.clearLine(x);
    if (death_time < 300000 || death_time == 3600000) death_time = 300000;
    else if (death_time == 300000) death_time = 600000;
    else if (death_time == 600000) death_time = 900000;
    else if (death_time == 900000) death_time = 1800000;
    else if (death_time == 1800000) death_time = 3600000;
    lcd.printat(1, 2, F("Время респа"));
    lcd.printat(5, 3, death_time / 60000);
    lcd.print(F(" мин"));
  }
  else if (idx == 15)                               //selected harakiri da
  {
    state = 0;
    lcd.clear();
    screen = 1;
    menu.go2Item(1);
    Callback(1);
  }
}

int8_t getLen(long someValue)
{ int8_t valLen = 0;
  if (someValue < 0) valLen++;
  if (sqrt(someValue * someValue) > 9999)
  {
    valLen = valLen + 5;
    return valLen;
  }
  else if (sqrt(someValue * someValue) > 999)
  {
    valLen = valLen + 4;
    return valLen;
  }
  else if (sqrt(someValue * someValue) > 99)
  {
    valLen = valLen + 3;
    return valLen;
  }
  else if (sqrt(someValue * someValue) > 9)
  {
    valLen = valLen + 2;
    return valLen;
  }
  else
  {
    valLen = valLen + 1;
    return valLen;
  }
}

void light_event()
{
  if (light)
  {
    digitalWrite(lightpin, LOW);
    tme5 = millis();
  }
}
void sound_event(uint8_t z)
{
  if (sound)
  {
    analogWrite(soundpin, z);
    tme6 = millis();
  }
}
void vibro_event()
{
  if (vibro)
  {
    digitalWrite(vibropin, HIGH);
    tme7 = millis();
  }
}


void button_event()
{
  curTime = millis();
  if (analogRead(A6) > 900 && curTime - tme2 > 300)             // нажатие кнопки 1
  {
    light_event();
    tme2 = millis();
    if (screen == 1)
    {
      screen = 2;
      menu.go2Item(2);
      Callback(2);
    }
    else if (screen == 2)
    {
      screen = 4;
      menu.go2Item(4);
      Callback(4);
    }
    else if (screen == 3)
    {
      screen = 4;
      menu.go2Item(4);
      Callback(4);
    }
    else if (screen == 4)
    {
      screen = 1;
      menu.go2Item(1);
      Callback(1);
    }
  }
  if (analogRead(A7) > 900 && curTime - tme2 > 300)           // нажатие кнопки 2
  {
    light_event();
    tme2 = millis();
    if (screen == 4) menu.goUp();
  }
  if (analogRead(A1) > 900 && curTime - tme2 > 300)           // нажатие кнопки 3
  {
    light_event();
    tme2 = millis();
    if (screen == 4) menu.goDown();
  }
  if (analogRead(A2) > 900 && curTime - tme2 > 300)             // нажатие кнопки 4
  {
    light_event();
    tme2 = millis();
    menu.goNext();
  }
}

void turnoff_events()
{
  curTime = millis();
  if (curTime - tme5 >= 1000) digitalWrite(lightpin, HIGH);             //гашение подсветки от события
  if (curTime - tme6 >= 80) digitalWrite(soundpin, LOW);                    //отключение звука от события
  if (curTime - tme7 >= 200) digitalWrite(vibropin, LOW);                   //отключение вибро от события
}

void update_slots()
{
  for (int8_t pos = 0; pos <= 14; pos++)                                //чтение буффера
  {
    if (slot1 && eep1.read(1) != 0) *slot1_params[pos] = eep1.read(pos);
    else *slot1_params[pos] = 0;
    if (slot2 && eep1.read(1) != 0) *slot2_params[pos] = eep2.read(pos);
    else *slot2_params[pos] = 0;
    tempbuff[pos] = eep1.read(pos);
  }
}

void state_check()
{
  if (radio_multi_mod == 10 ||  slot1_multi_mod == 10 || slot2_multi_mod == 10) state = 0;
  if (radio_multi_mod == 11 ||  slot1_multi_mod == 11 || slot2_multi_mod == 11) state = 1;
  if (radio_multi_mod == 12 ||  slot1_multi_mod == 12 || slot2_multi_mod == 12) state = 2;

  if (radio_multi_mod == 20 ||  slot1_multi_mod == 20 || slot2_multi_mod == 20) pda_ver = 0;
  if (radio_multi_mod == 21 ||  slot1_multi_mod == 21 || slot2_multi_mod == 21) pda_ver = 1;
  if (radio_multi_mod == 22 ||  slot1_multi_mod == 22 || slot2_multi_mod == 22) pda_ver = 2;

  if (radio_multi_mod == 1 ||  slot1_multi_mod == 1 || slot2_multi_mod == 1) respawn_reached = true;
  if (radio_multi_mod == 2 ||  slot1_multi_mod == 2 || slot2_multi_mod == 2) infected = false;
}
/*
  void serial_diag()
  {
  Serial.println();
  Serial.print(F("HP  ")); Serial.print(radio_hp_mod_counter); Serial.print(F(" ")); Serial.println(radio_hp_mod);
  Serial.print(F("Rad ")); Serial.print(radio_rad_mod_counter); Serial.print(F(" ")); Serial.println(radio_rad_mod);
  Serial.print(F("Psy ")); Serial.print(radio_psy_mod_counter); Serial.print(F(" ")); Serial.println(radio_psy_mod);
  Serial.print(F("Inf ")); Serial.print(radio_inf_mod_counter); Serial.print(F(" ")); Serial.println(inf_time);
  Serial.print(F("Sta ")); Serial.print(radio_multi_mod_counter); Serial.print(F(" ")); Serial.println(radio_multi_mod);
  Serial.print(F("Typ ")); Serial.print(radio_type_mod_counter); Serial.print(F(" ")); Serial.println(radio_type_mod);
  Serial.println(F("Slot1 Parameters"));
  for (int8_t pos = 0; pos <= 14; pos++)                               //чтение буффера
  {
    Serial.print(*slot1_params[pos]);
    Serial.print(F(" "));
  }
  Serial.println();
  Serial.println(F("Slot2 Parameters"));
  for (int8_t pos = 0; pos <= 14; pos++)                                //чтение буффера
  {
    Serial.print(*slot2_params[pos]);
    Serial.print(F(" "));
  }
  }
*/
