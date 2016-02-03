#include "freetype_wrapper.hpp"
#include <iostream>
#include <stdexcept>

void freetype_wrapper::init()
{
//  fprintf(stdout, "freetype_wrapper::init()\n");
  error_ = FT_Init_FreeType(&library_);      /* initialize library */
  if (error_)
    throw std::logic_error("error is not null!!!!");

  error_ = FT_New_Face(library_, font_filename_.c_str(), 0, &face_);/* create face object */
  if (error_)
    goto free_library;

  try
  {
    update_font();
  }
  catch (...)
  {
    goto done_face;
  }

  if (!error_)
    return;

done_face:
  FT_Done_Face(face_);
free_library:
  FT_Done_FreeType(library_);
  throw std::logic_error("error is not null!!!!");
}

freetype_wrapper::freetype_wrapper(const std::string& font_filename): font_filename_(font_filename),
  angle_(0.0), dpi_(96), fontw_(24), fonth_(24)
{
  init();
}

freetype_wrapper::~freetype_wrapper()
{
  FT_Done_Face(face_);
  FT_Done_FreeType(library_);
}


freetype_wrapper::freetype_wrapper(const freetype_wrapper& rhs): font_filename_(rhs.font_filename_),
  angle_(rhs.angle_), dpi_(rhs.dpi_), fontw_(rhs.fontw_), fonth_(rhs.fonth_)
{
  init();
}

freetype_wrapper& freetype_wrapper::operator=(const freetype_wrapper& rhs)
{
  font_filename_ = rhs.font_filename_;
  angle_ = rhs.angle_;
  dpi_ = rhs.dpi_;
  fontw_ = rhs.fontw_;
  fonth_ = rhs.fonth_;
  init();
  return *this;
}

void freetype_wrapper::setsz(int sz)
{
  fontw_ = fonth_ = sz;
  update_font();
}

void freetype_wrapper::setw(int w)
{
  fontw_ = w;
  update_font();
}

void freetype_wrapper::seth(int h)
{
  fonth_ = h;
  update_font();
}

void freetype_wrapper::update_font()
{
  error_ = FT_Set_Char_Size(face_, fontw_ * 64, fonth_ * 64, dpi_, 0);
  if (error_)
    throw std::logic_error("error is not null!!!!");
  /* set up matrix */
  matrix_.xx = (FT_Fixed)(cos(angle_) * 0x10000L);
  matrix_.xy = (FT_Fixed)(-sin(angle_) * 0x10000L);
  matrix_.yx = (FT_Fixed)(sin(angle_) * 0x10000L);
  matrix_.yy = (FT_Fixed)(cos(angle_) * 0x10000L);
}

void freetype_wrapper::draw_bitmap(rI * ri, FT_Bitmap* bitmap, FT_Int x, FT_Int y, const rColor * clr_dest)
{
  FT_Int i, j, p, q;
  FT_Int x_max = x + bitmap->width;
  FT_Int y_max = y + bitmap->rows;
  int w = ri->rect.x, h = ri->rect.y;
  rColor * buf = ri->buf;
//  fprintf(stderr, "x=%d y=%d xmax=%d ymax=%d\n", x, y, x_max, y_max);
  for (i = x, p = 0; i < x_max; i++, p++)
  {
    for (j = y, q = 0; j < y_max; j++, q++)
    {
      if (i < 0 || j < 0 || i >= w || j >= h)
        continue;
      rColor * clr = &buf[i + j * w];
      if (clr_dest)
      {
        int dr = clr_dest->r - clr->r, dg = clr_dest->g - clr->g, db = clr_dest->b - clr->b;
        int shade = bitmap->buffer[q * bitmap->width + p];
        clr->r += round(dr * shade / 255.0);
        clr->g += round(dg * shade / 255.0);
        clr->b += round(db * shade / 255.0);
      }
      else
      {
        clr->r = clr->g = clr->b = bitmap->buffer[q * bitmap->width + p];
      }
    }
  }
}

void freetype_wrapper::set_text(const std::string& text)
{
  // TODO: use wide strings!!!!
//  fprintf(stdout, "set text %s\n", text.c_str());
  std::string tmp = convert_encoding(text, "utf-8", "utf-32");
  if (tmp.length() % 4)
    throw std::logic_error("Convertion failed!!!");
  text_.clear();
  for (size_t i = 4; i < tmp.length(); i += 4)
  {
    unsigned int x = ((unsigned int)tmp[i])
      + (((unsigned int)tmp[i + 1]) << 8)
      + (((unsigned int)tmp[i + 2]) << 16)
      + (((unsigned int)tmp[i + 3]) << 24);
    text_.push_back(x);
//    fprintf(stdout, "x=%d\n", x);
  }
}

void freetype_wrapper::draw_text(rI * ri, const rCoord2D& rect, const rColor *clr)
{
  FT_GlyphSlot slot = face_->glyph;
  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (rect) relative to the upper left corner  */
  FT_Vector pen;                    /* untransformed origin  */
  pen.x = rect.x * 64;
  pen.y = (ri->rect.y - rect.y) * 64;
  for (size_t n = 0; n < text_.size(); ++n)
  {
    /* set transformation */
    FT_Set_Transform(face_, &matrix_, &pen);
    /* load glyph image into the slot (erase previous one) */
    error_ = FT_Load_Char(face_, text_[n], FT_LOAD_RENDER);
    if (error_)
    {
      std::cerr << "Error while loading char, ignoring...";
      continue;                 /* ignore errors */
    }
    /* now, draw to our target surface (convert position) */
    draw_bitmap(ri, &slot->bitmap,
             slot->bitmap_left,
             ri->rect.y - slot->bitmap_top, clr);
    /* increment pen position */
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }
}

