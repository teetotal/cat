//
// Created by Jung, DaeCheon on 19/10/2018.
//

#include "ui_color.h"

ui_color * ui_color::hInst = NULL;

void ui_color::add(int R, int G, int B, string name){
    COLOR_RGB color;
    color.R = R;
    color.G = G;
    color.B = B;
    color.name = name;
    mVec.push_back(color);
}

ui_color::COLOR_RGB ui_color::getColor(int idx){
    return mVec[idx];
}

Color3B ui_color::getColor3B(int idx){
    COLOR_RGB color = getColor(idx);
    Color3B color3B = Color3B(color.R, color.G, color.B);
    return color3B;
}

Color4F ui_color::getColor4F(int idx){
    Color4F color = Color4F(getColor3B(idx));
    return color;
}
