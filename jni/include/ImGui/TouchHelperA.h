/* 
 * by币子
 
 
 */

#pragma once


bool Touch_Init(int w, int h, uint32_t orientation_, bool readOnly);
void UpdateScreenData(int w, int h, uint32_t orientation_);

void Touch_Close();
void Touch_Down(int id,float x, float y);
void Touch_Move(int id,float x, float y);
void Touch_Up(int id);

