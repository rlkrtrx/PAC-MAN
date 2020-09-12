#ifndef __COL__
#define __COL__
int colU(float x, float y, map m, int box_avail)
{
  int x1 = (int)round(x/TLSZ);
  int y1 = (int)floor(y/TLSZ);
  if(!(x1 < 0 || x1 >= m.w)&&!(y1<0 || y1 >= m.h))
    return (get_tile(m, x1, y1) == 4 && box_avail) ? 0 : get_tile(m, x1, y1);
  else
    return 1;
  return 0;
}

int colR(float x, float y, map m, int box_avail)
{
  int x1 = (int)floor(x/TLSZ);
  int y1 = (int)round(y/TLSZ);
  if(!(x1 < 0 || x1 >= m.w)&&!(y1<0 || y1 >= m.h))
    return (get_tile(m, x1, y1) == 4 && box_avail) ? 0 : get_tile(m, x1, y1);
  return 0;
}

int colD(float x, float y, map m, int box_avail)
{
  int x1 = (int)round(x/TLSZ);
  int y1 = (int)floor(y/TLSZ);
  if(!(x1 < 0 || x1 >= m.w-1)&&!(y1<0 || y1 >= m.h))
    return (get_tile(m, x1, y1) == 4 && box_avail) ? 0 : get_tile(m, x1, y1);
  else
    return 1;
  return 0;
}

int colL(float x, float y, map m, int box_avail)
{
  int x1 = (int)floor(x/TLSZ);
  int y1 = (int)round(y/TLSZ);
  if(!(x1 < 0 || x1 >= m.w)&&!(y1<0 || y1 >= m.h))
    return (get_tile(m, x1, y1) == 4 && box_avail) ? 0 : get_tile(m, x1, y1);
  return 0;
}
#endif
