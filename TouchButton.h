  
  
class TouchButton {

  public:
   int x, y, width, height;
   TouchButton(int x, int y, int width, int height);
   bool isClicked(int x, int y);
};

TouchButton::TouchButton(int x, int y, int width, int height)
{
 this->x = x;
 this->y = y;
 this->height = height;
 this-> width = width;
}


bool TouchButton::isClicked(int x, int y)
{
  return (x >= this->x && x <= (x+width) && y >= this->y && y <= (y+height) );
}

