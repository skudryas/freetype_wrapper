#ifndef __FREETYPE_WRAPPER_HPP__
#define __FREETYPE_WRAPPER_HPP__

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "string_helpers.hpp"
#include <vector>

typedef struct
{
    /*unsigned*/ int x;
    /*unsigned*/ int y;
} rCoord2D;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    //char t;//reserved
} rColor;

typedef struct
{
  rCoord2D rect;
  rColor * buf;
} rI;

class freetype_wrapper
{
  private:
    FT_Library library_;
    FT_Face face_;
    FT_Matrix matrix_;                 /* transformation matrix */
    FT_Error error_;

    std::string font_filename_;
    std::vector<unsigned int> text_;
    double angle_;
    int dpi_;
    int fontw_;
    int fonth_;
    void update_font();
    void draw_bitmap(rI * ri, FT_Bitmap* bitmap, FT_Int x, FT_Int y, const rColor * clr_dest);
    void init();
  public:
    freetype_wrapper(const std::string& font_filename);
    freetype_wrapper(const freetype_wrapper& rhs);
    freetype_wrapper& operator=(const freetype_wrapper& rhs);
    ~freetype_wrapper();
    void set_text(const std::string& text);
    void set_angle(double angle)
    {
      angle_ = (angle / 360 ) * 3.14159 * 2;
      update_font();
    }
    void setsz(int sz);
    void setw(int w);
    void seth(int h);
    void draw_text(rI * ri, const rCoord2D& rect, const rColor* clr = NULL);
};

#endif // __FREETYPE_WRAPPER_HPP__

