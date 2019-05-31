#include <avr/pgmspace.h>
      
char const menu_0[] PROGMEM = "";  //МЕНЮ
char const menu_1[] PROGMEM = "";  //Статистика
char const menu_2[] PROGMEM = "";  //Слот1
char const menu_3[] PROGMEM = "";  //Слот2
char const menu_4[] PROGMEM = "";  //Настройки
char const menu_5[] PROGMEM = "";  //Статистика
char const menu_6[] PROGMEM = "";  //Актив Умение
char const menu_7[] PROGMEM = "";  //Слот1
char const menu_8[] PROGMEM = "";  //Слот1 вкл/выкл
char const menu_9[] PROGMEM = "";  //Слот2
char const menu_10[] PROGMEM = "";  //Слот2 вкл/выкл
char const menu_11[] PROGMEM = "";  //Настройки
char const menu_12[] PROGMEM = "Звук";  //Звук
char const menu_13[] PROGMEM = "Контраст";  //Контраст
char const menu_14[] PROGMEM = "Подсветка";  //Подсветка
char const menu_15[] PROGMEM = "Вибро";  //Вибро
char const menu_16[] PROGMEM = "Диагностика";  //Диагнстк
char const menu_17[] PROGMEM = "Таймер";  //Таймер
char const menu_18[] PROGMEM = "Харакири";  //Харакири
char const menu_19[] PROGMEM = "";  //Харакири
char const menu_20[] PROGMEM = "Да";  //Да
char const menu_21[] PROGMEM = "Нет";  //Нет

PGM_P  const menu_table[] PROGMEM =
{
  menu_0,  menu_1, menu_2,  menu_3,  menu_4,  menu_5,  menu_6,  menu_7,  menu_8,  menu_9,
  menu_10,  menu_11,  menu_12,  menu_13,  menu_14,  menu_15,  menu_16,  menu_17,  menu_18,
  menu_19,  menu_20,  menu_21
};

char const anomaly0[] PROGMEM = "";
char const anomaly1[] PROGMEM = "  База";
char const anomaly2[] PROGMEM = "Карусель";
char const anomaly3[] PROGMEM = " Жарка";
char const anomaly4[] PROGMEM = "Электра";
char const anomaly5[] PROGMEM = "Газировка";
char const anomaly6[] PROGMEM = "Пси Поле";
char const anomaly7[] PROGMEM = "Радиация";
char const anomaly8[] PROGMEM = "Заражение";
char const anomaly9[] PROGMEM = "Трамплин";

PGM_P  const anomaly_table[] PROGMEM =
{
  anomaly0, anomaly1, anomaly2, anomaly3, anomaly4, anomaly5, anomaly6, anomaly7, anomaly8, anomaly9
};

char const item0[] PROGMEM = "Пусто";
char const item1[] PROGMEM = "Артефакт";
char const item2[] PROGMEM = "Защита";
char const item3[] PROGMEM = "Прибор";
char const item4[] PROGMEM = "Прочее";

PGM_P  const item_table[] PROGMEM =
{
  item0, item1, item2, item3, item4
};

