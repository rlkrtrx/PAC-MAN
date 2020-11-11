#ifndef __SPR__
#define __SPR__

#define NONE 0
#define NORM 1

#define RED 1
#define PINK 2
#define BLUE 3
#define ORANGE 4

#define CHASE 1
#define SCATTER 2
#define FRIGHT 3
#define HOME 4

typedef struct
{
  GLuint vao;
  GLuint tex;
  float x, y, w, h, *c, nxtfrm, angle;
  int direction, wantDirection, cfrmx, cfrmy, nfrmx, nfrmy, mfrm, status, l_x, l_y;
}spr;

typedef struct ghostStruct
{
  spr* s; // Spr for render data, position and size. Trying to reuse the struct
  struct ghostStruct* nxt; // Pointer to next ghost size. To iterate through all the ghost in order to update them.
  int type, mode, last_x, last_y;
  int* last_contact;
}ghost;

spr* Spr(const char* imgPath, float sx, float sy, float w, float h, float r, float g, float b, int ctfrmx, int ctfrmy)
{
  spr* s = (spr*)malloc(sizeof(spr));
  s->x = sx;
  s->y = sy;
  s->w = w;
  s->h = h;
  s->l_x = 0;
  s->l_y = 0;
  s->vao = make_quad(w, h, ctfrmx, ctfrmy);
  s->direction = NONE;
  s->tex = make_tex(imgPath, GL_RGBA);
  s->nfrmx = ctfrmx;
  s->nfrmy = ctfrmy;
  s->nxtfrm = 0.0f;
  s->angle = 0.0f;
  s->cfrmx = 1;
  s->cfrmy = 1;
  s->mfrm = 1;
  s->status = NORM;
  s->c = (float*)malloc(3*sizeof(float));
  s->c[0] = r;
  s->c[1] = g;
  s->c[2] = b;
  return s;
}

void animate_spr(spr* s, int fps)
{
  if(s->nxtfrm <= glfwGetTime())
  {
    if(s->cfrmx == s->nfrmx)
      s->mfrm = -s->mfrm;
    s->cfrmx += s->mfrm;
    s->nxtfrm = glfwGetTime() + 1.0f/(float)fps;
  }
}

void animate_ghost_spr(spr* s, int fps)
{
  if(s->nxtfrm <= glfwGetTime())
  {
    if(s->cfrmx == s->mfrm * 2 - 1)
      s->cfrmx = s->mfrm * 2 - 2;
    else
      s->cfrmx = s->mfrm * 2 - 1;
    s->nxtfrm = glfwGetTime() + 1.0f/(float)fps;
  }
}

void set_angle_spr(spr* s, float angle)
{
  s->angle = angle;
}

int get_pos(float x, float y, map m)
{
  int x1 = (int)floor(x/TLSZ), y1 = (int)ceil(y/TLSZ)-1;
  return get_tile(m, x1, y1);
}

float posX(spr s)
{
  return (int)((s.x+TLSZ/2)/TLSZ)*TLSZ;
}

float posY(spr s)
{
  return (int)((s.y+TLSZ/2)/TLSZ)*TLSZ;
}

int x(spr s)
{
  return (int)((s.x+TLSZ/2)/TLSZ);
}

int y(spr s)
{
  return (int)((s.y+TLSZ/2)/TLSZ);
}

int oob(spr s, map m)
{
  int x = posX(s)/TLSZ;
  int y = posY(s)/TLSZ;
  if(x < 0 || y < 0 || x >= m.w || y >= m.h)
          return 1;
  return 0;
}

int posUy(spr s)
{
  return (int)((s.y+TLSZ/2-TLSZ)/TLSZ);
}

int posRx(spr s)
{
  return (int)((s.x+TLSZ/2+TLSZ)/TLSZ);
}

int posDy(spr s)
{
  return (int)((s.y+TLSZ/2+TLSZ)/TLSZ);
}

int posLx(spr s)
{
  return (int)((s.x+TLSZ/2-TLSZ)/TLSZ);
}

int goDown(spr* s, map m, int box_avail)
{
  if(!m.tileMap[posDy(*s)*m.w+x(*s)] || (box_avail&&m.tileMap[posDy(*s)*m.w+x(*s)]==4))
  {
    s->x = posX(*s);
    return 1;
  }
  return 0;
}

int goUp(spr* s, map m, int box_avail)
{
  if(!m.tileMap[posUy(*s)*m.w+x(*s)] || (box_avail&&m.tileMap[posUy(*s)*m.w+x(*s)]==4))
  {
          s->x = posX(*s);
          return 1;
  }
  return 0;
}

int goRight(spr* s, map m, int box_avail)
{
  if(!m.tileMap[y(*s)*m.w+posRx(*s)] || (box_avail && m.tileMap[y(*s)*m.w+posRx(*s)]==4))
  {
          s->y = posY(*s);
          return 1;
  }
  return 0;
}

int goLeft(spr* s, map m, int box_avail)
{
  if(!m.tileMap[y(*s)*m.w+posLx(*s)] || (box_avail && m.tileMap[y(*s)*m.w+posLx(*s)]==4))
  {
    s->y = posY(*s);
    return 1;
  }
  return 0;
}

int want_direction_valid(spr* s, int want_direction, map m, int box_avail)
{
  if(oob(*s, m))
    return 1;
  switch(want_direction)
  {
  case UP:
    return goUp(s, m, box_avail);
  case RIGHT:
    return goRight(s, m, box_avail);
  case DOWN:
    return goDown(s, m, box_avail);
  case LEFT:
    return goLeft(s, m, box_avail);
  default:
    break;
  }
  return 0;
}


void set_ghost_state(ghost* g, int mode);

void update_spr(spr* s, float dt, float speed, map m, int ghost, int box_avail, int ghost_type)
{
  float spd = TLSZ * speed * dt;

  if(s->x >= WINDOW_WIDTH)
    s->x = 0 - s->w;
  else if(s->x+s->w <= 0)
    s->x = WINDOW_WIDTH;

  if(want_direction_valid(s, s->wantDirection, m, box_avail))
  {
    s->direction = s->wantDirection;
    if(ghost_type == CHASE)
      s->mfrm = s->direction;
    s->wantDirection = 0;
  }

  switch(s->direction)
  {
  case UP:
    if(!colU(s->x, s->y+TLSZ/2-TLSZ, m, box_avail))
      s->y -= spd;
    else
    {
      s->y = posY(*s);
      s->cfrmx = 1;
      s->status = NONE;
      s->direction =0;
    }
    break;
  case RIGHT:
    if(!colR(s->x+TLSZ/2+TLSZ, s->y, m, box_avail))
      s->x += spd;
    else
    {
      s->x = posX(*s);
      s->cfrmx = 1;
      s->status = NONE;
      s->direction =0;
    }
    break;
  case DOWN:
    if(!colD(s->x, s->y+TLSZ/2+TLSZ, m, box_avail))
      s->y += spd;
    else
    {
      s->y = posY(*s);
      s->cfrmx = 1;
      s->status = NONE;
      s->direction =0;
    }
    break;
  case LEFT:
    if(!colL(s->x+TLSZ/2-TLSZ, s->y, m, box_avail))
      s->x -= spd;
    else
    {
      s->x = posX(*s);
      s->cfrmx = 1;
      s->status = NONE;
      s->direction =0;
    }
    break;
  default:
    break;
  }
  if(!ghost && ghost_type != HOME)
    animate_spr(s, 22);
}

int dir_opposite(int dir)
{
  if(!dir)
    return dir;
  if(dir > 2)
    return dir - 2;
  else
    return dir + 2;
}

void set_fright(ghost* g, map m)
{
  ghost* current_ghost = g;
  while(current_ghost->nxt!=NULL)
  {
    if(current_ghost->nxt->mode != HOME)
      set_ghost_state(current_ghost->nxt, FRIGHT);
    current_ghost = current_ghost->nxt;
  }
}

int ghost_point(int type)
{
  switch(type)
  {
  case RED:
    return 200;
    break;
  case PINK:
    return 400;
    break;
  case BLUE:
    return 800;
    break;
  case ORANGE:
    return 1600;
    break;
  default:
    break;
  }
  return 0;
}

void update_pacman_spr(spr* s, ghost* g, float dt, map m, int* points)
{
  /*ghost* currentGhost = g;
  while(currentGhost->nxt!=NULL)
  {
    if(posX(currentGhost->nxt->s) == posX(*s) && posY(currentGhost->nxt->s) == posY(*s))
    {
      if(currentGhost->nxt->mode != 3 && currentGhost->nxt->mode != 4)
        exit(0);
      else
      {
        set_ghost_state(currentGhost->nxt, HOME);
        *points += ghost_point(currentGhost->nxt->type);
      }
    }
    currentGhost = currentGhost->nxt;
  }*/
  /*currentGhost = NULL;
  free(currentGhost);*/
  update_spr(s, dt, 11, m, 0, 1, -1);
  switch(s->direction)
  {
  case UP:
    s->angle = 270;
    break;
  case RIGHT:
    s->angle = 0;
    break;
  case DOWN:
    s->angle = 90;
    break;
  case LEFT:
    s->angle = 180;
    break;
  default:
    break;
  }
  if(m.coinMap[(int)((s->y+TLSZ/2)/TLSZ)*m.w+(int)((s->x+TLSZ/2)/TLSZ)]==1)
  {
    m.coinMap[(int)((s->y+TLSZ/2)/TLSZ)*m.w+(int)((s->x+TLSZ/2)/TLSZ)] = 0;
    *points+=10;
  }
  else if(m.coinMap[(int)((s->y+TLSZ/2)/TLSZ)*m.w+(int)((s->x+TLSZ/2)/TLSZ)]==2)
  {
    set_fright(g, m);
    m.coinMap[(int)((s->y+TLSZ/2)/TLSZ)*m.w+(int)((s->x+TLSZ/2)/TLSZ)] = 0;
    *points+=50;
  }

}

void move_spr(spr* s, int newDirection)
{
  s->wantDirection = newDirection;
}

void move_ghost_spr(spr* s, int new_dir)
{
  if(new_dir != s->direction - 2 && new_dir != s->direction + 2)
    s->wantDirection = new_dir;
}

void render_spr(spr* s, GLuint prg, mat4x4 ortho)
{
  mat4x4 m0, m1, t0, t1, f0, f1, f2;
  mat4x4 s0, s1;
  mat4x4_identity(s0);
  mat4x4_scale_aniso(s1, s0, 13.0f/8.0f, 13.0f/8.0f, 0.0f);
  mat4x4_identity(f2);
  mat4x4_translate(t0, s->x  - ((s->w*13.0f/8.0f)-s->w)/2, s->y- ((s->h*13.0f/8.0f)-s->h)/2 + MENU_TILES * TLSZ, 0.0f);
  mat4x4_mul(t1, t0, s1);
  mat4x4_translate(m0, 0.5f*s->w, 0.5f*s->h, 0.0f);
  mat4x4_rotate_Z(m1, m0, s->angle * (PI/180));
  mat4x4_translate(m0, -0.5f*s->w, -0.5f*s->h, 0.0f);
  mat4x4_mul(f0, m1, m0);
  mat4x4_mul(f1, t1, f0);
  mat4x4 texModel;
  mat4x4_translate(texModel, (1.0f/(float)s->nfrmx) * (float)s->cfrmx, 1.0f/(float)s->nfrmy * (s->cfrmy-1), 0.0f);
  glUniformMatrix4fv(glGetUniformLocation(prg, "ortho"), 1, GL_FALSE, &ortho[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(prg, "model"), 1, GL_FALSE, &f1[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(prg, "texModel"), 1, GL_FALSE, &texModel[0][0]);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
