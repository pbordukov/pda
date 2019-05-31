// Made by Denis Peshekhonov, 2013

#ifndef MYMENU_H
#define MYMENU_H

#include "pcd8544.h"

class MItm
{
  public:
    MItm( int8_t _first, int8_t _second = -1);
  void addPars(int8_t _sub_id, int8_t _back_id);
    int8_t first;
    int8_t second;
  int8_t sub_id;
  int8_t back_id;
};

class Menu
{
  public:
    Menu(MItm *_items, int8_t _num_items, pcd8544*_lcd, void (*_callback)(int8_t), int8_t _num_rows = 4);
  void goNext();
  void goBack();
  void go2Item(int8_t item_idx);
  void goMain();
  void goDown();
  void goUp();
  void goLast();
  void goSub(int8_t sub_idx);
  void goItem(int8_t item_idx);
  int8_t cur_id();
  private:
  void drawSub();
  int8_t findFirst(MItm citm);
  void (*callback)(int8_t);

    MItm *items;
    pcd8544 *lcd;  
  int8_t num_rows;
  int8_t cur_sub;
  int8_t cur_item;
  int8_t cur_sub_index;
  
  int8_t subsize;
  int8_t fullsize;
  
  int8_t cur_pos;
};

#endif
