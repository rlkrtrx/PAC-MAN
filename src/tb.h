#ifndef __TEXT_BOX__
#define __TEXT_BOX__

#define TEXT_WIDTH 8
#define TEXT_HEIGHT 8

struct font
{
  int w, h;
  int* text;
};

struct font* init_font_list(char* path)
{
  struct font* font_list = (struct font*)malloc(79*sizeof(struct font));
  BMP* img = BMP_ReadFile(path);
  if(!img)
    printf("Failed to load image\n");
  int w = BMP_GetWidth(img), h = BMP_GetHeight(img);
  UCHAR r,g,b;
  for(int o = 0; o < 79; o++)
  {
    font_list[o].text = (int*)malloc(w/79*h*sizeof(int));
    font_list[o].w = w/79;
    font_list[o].h = h;
    for(int j = 0; j < h; j++)
    {
      for(int i = 0; i < w/79; i++)
      {
        BMP_GetPixelRGB(img, i + (o*w)/79, j, &r, &g, &b);
        if(r == 255 && g == 255 && b == 255)
        {
          font_list[o].text[j*w/79+i] = 1;
        }
        else 
        {
          font_list[o].text[j*w/79+i] = 0;
        }
      }
    }
  }
  return &font_list[0];
}

struct tb
{
  uint32_t* buf;
  int w, h, x, y, scale_factor;
  GLuint tex, vao;
};

uint32_t rgb(int r, int g, int b)
{
  return (r << 24) | (g << 16) | (b << 8) | 255;
}

void render_text_to_buffer(int x, int y, uint32_t* buffer, int w, int h, uint32_t color, char* text, int length, struct font* f)
{
  for(int g = 0; g < length; g++)
  {
    if(text[g] == ' ')
      continue;
    int fw = f[text[g]-48].w;
    int fh = f[text[g]-48].h;
    int xoff = g%(w/fw)*fw;
    int yoff = (g/(w/fw))*fh; 

    for(int j = 0; j < 8; j++)
    {
      for(int i = 0; i < 8; i++)
      {
        if(f[text[g]-48].text[j*fw+i])
          buffer[(j+yoff)*w+i+xoff] = color;
      }
    }
  }
}

GLuint init_tex(uint32_t* buffer, int w, int h)
{
  GLuint t;
  glGenTextures(1, &t);
  glBindTexture(GL_TEXTURE_2D, t);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glBindTexture(GL_TEXTURE_2D, 0);
  return t;
}

void clear_buffer(uint32_t* buffer, int w, int h, uint32_t color)
{
  for(int i = 0; i < w*h; i++)
    buffer[i]  = color;
}

struct tb init_tb(int x, int y, char* init_str, int max_char_w, int max_char_h, struct font* f, int scale_factor)
{
  struct tb t;
  t.x = x;
  t.y = y;
  t.w = max_char_w * TEXT_WIDTH;
  t.h = max_char_h * TEXT_HEIGHT;
  t.scale_factor = scale_factor;
  t.buf = (uint32_t*)malloc(t.w*t.h*sizeof(uint32_t));
  t.vao = make_quad(t.w*scale_factor, t.h*scale_factor, 1, 1);
  clear_buffer(t.buf, t.w, t.h, rgb(0, 0, 0));
  render_text_to_buffer(0, 0, t.buf, t.w, t.h, rgb(255, 255, 255), init_str, strlen(init_str), f);
  t.tex = init_tex(t.buf, t.w, t.h);
  return t;
}

void center_tb(struct tb* t, int window_width)
{
  t->x = window_width/2 - (t->w*t->scale_factor)/2;
}

void render_text_box(GLuint tb_prg, struct tb t, mat4x4 ortho)
{
  glUseProgram(tb_prg);
  glBindTexture(GL_TEXTURE_2D, t.tex);
  glBindVertexArray(t.vao);
  mat4x4 model;
  mat4x4_translate(model, t.x, t.y, 0.0f);
  glUniformMatrix4fv(glGetUniformLocation(tb_prg, "model"), 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(tb_prg, "proj"), 1, GL_FALSE, &ortho[0][0]);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
