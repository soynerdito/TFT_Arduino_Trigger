
  
class TouchButton {

  public:
   int x, y, width, height,textMarginX ,textMarginY;
   int color, textColor, textSize;
   char *text = NULL;
   TFT *tft;   
   TouchButton(TFT *tft, int x, int y, int width, int height, int color, int textColor = WHITE, char *text = NULL, int textSize = 2 );
   bool isClicked(int x, int y);
   void draw( );
   void setColor(int color);
   void setTextColor(int color);
   void setCaption(char *text);   
   void setTextMargin(int x, int y);
};

TouchButton::TouchButton(TFT *tft, int x, int y, int width, int height, int color, int textColor, char *text, int textSize)
{
 this->x = x;
 this->y = y;
 this->height = height;
 this-> width = width;
 this->color = color;
 this->textColor = textColor;
 this->textSize = textSize;
 this->text = text;
 this->tft = tft;
 this->textMarginX = 20;
 this->textMarginY = 8;
}

void TouchButton::setTextMargin(int x, int y)
{
  this->textMarginX = x;
  this->textMarginY = y;
}

void TouchButton::setCaption(char *text)
{
  this->text = text;
}

void TouchButton::setColor(int color)
{
  this->color = color;
}

void TouchButton::setTextColor(int color)
{
  this->textColor = color;
}

bool TouchButton::isClicked(int x, int y)
{
  return (x > (this->x+1) && y > this-> y ) && ( x < this->x+this->width ) && y < (this-> y+this->height-2) ;
}

void TouchButton::draw( )
{
  this->tft->fillRectangle( this->x, this->y+this->height, this->width, this->height, this->color);
  //Draw Button caption
  this->tft->setDisplayDirect(DOWN2UP);
  //this->tft->drawString(this->text, this->x + 20,(this->y + this->height -8),this->textSize,this->textColor);
  this->tft->drawString(this->text, this->x + this->textMarginX,(this->y + this->height - this->textMarginY),this->textSize,this->textColor);    
}

