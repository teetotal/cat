//
//  ui_bgText.h
//  cat-mobile
//
//  Created by Jung, DaeCheon on 04/01/2019.
//

#ifndef ui_bgText_h
#define ui_bgText_h

#include "ui.h"
class ui_bgText {
public:
    ui_bgText() {
        init(Color3B::GRAY, 32);
    };
    ui_bgText(Color3B bgColor, GLubyte opacity) {
        init(bgColor, opacity);
    };
    ~ui_bgText() {};
    
    void init(Color3B bgColor, GLubyte opacity) {
        mBGColor = bgColor;
        mOpacity = opacity;
        mBGLayerColor = NULL;
        mParent = NULL;
    };
    Node * getNode();
    Vec2 getPosition();
    string getString() const;
    void setString(const string& sz);
    void MoveTo(Vec2 pos);
    void MoveBy(Vec2 pos);
    Label * addLabelAutoDimension(int x
                                  , int y
                                  , const string text
                                  , Node *p
                                  , int fontSize = 0
                                  , ALIGNMENT align = ALIGNMENT_CENTER
                                  , const Color3B color = Color3B::BLACK
                                  , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                  , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                                  , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                                  , const string img = ""
                                  , bool isBGImg = true
                                  )
    {
        return  addLabel(p
                         , x
                         , y
                         , text
                         , fontSize
                         , align
                         , color
                         , p->getContentSize()
                         , grid
                         , origin
                         , margin
                         , img
                         , isBGImg
                         );
    };
    
    Label * addLabel(Node *p
                     , int x
                     , int y
                     , const string text
                     , int fontSize = 0
                     , ALIGNMENT align = ALIGNMENT_CENTER
                     , const Color3B color = Color3B::BLACK
                     , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                     , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                     , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                     , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                     , const string img = ""
                     , bool isBGImg = true
                     );
    
    MenuItemLabel* addTextButtonAutoDimension(int x
                                              , int y
                                              , const string text
                                              , Node* p
                                              , const ccMenuCallback& callback
                                              , int fontSize = 0
                                              , ALIGNMENT align = ALIGNMENT_CENTER
                                              , const Color3B color = Color3B::BLACK
                                              , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                              , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                                              , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                                              , const string img = ""
                                              , bool isBGImg = true
                                              ) {
        return addTextButton(x, y, text, p, callback, fontSize, align, color, p->getContentSize(), grid, origin, margin, img, isBGImg);
    };
    
    MenuItemLabel * addTextButton(int x
                                  , int y
                                  , const string text
                                  , Node* p
                                  , const ccMenuCallback& callback
                                  , int fontSize = 0
                                  , ALIGNMENT align = ALIGNMENT_CENTER
                                  , const Color3B color = Color3B::BLACK
                                  , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                  , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                  , Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                  , Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                                  , const string img = ""
                                  , bool isBGImg = true
                                  ) {
        mIsButton = true;
        mMenuItemLabel = gui::inst()->addTextButtonRaw(mMenu, x, y, text, p, callback, fontSize, align, color, dimension, grid, origin, margin, img, isBGImg, false);
//        int fontSizePixel = (fontSize == 0) ? gui::inst()->mDefaultFontSize : fontSize;
//        float height = gui::inst()->getSizeFromRealPixel(fontSizePixel);
//        float width = height * text.size();
//        Size contentSize = Size(width, height);
//        drawBG(Vec2(mMenu->getPosition().x - contentSize.width / 2, mMenu->getPosition().y - contentSize.height / 2)
//                              , contentSize
//                              , p
//                              );
        
        drawBG(p);
        p->addChild(mMenu);
        return mMenuItemLabel;
    };
    
    
private:
    Color3B mBGColor;
    GLubyte mOpacity;
    Menu * mMenu;
    MenuItemLabel * mMenuItemLabel;
    LayerColor * mBGLayerColor;
    bool mIsButton;
    Node* mParent;
    
    void drawBG(Node * pParent);
    void setBGSize();
};
#endif /* ui_bgText_h */
