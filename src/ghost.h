#ifndef __GHOST__
#define __GHOST__

ghost* init_ghosts(float x, float y) // Creates a pointer to a ghost structure. Fully initialized. 
{
  ghost* g = (ghost*)malloc(sizeof(ghost));  
  g->nxt = NULL;
  ghost* crt = g;
  int ctype = 1;
  while(ctype<=4)
  {
    crt->nxt = (ghost*)malloc(sizeof(ghost));
    crt->nxt->nxt = NULL;
    crt->nxt->s = Spr("img/ghost.png", x + ctype * 2 * TLSZ - TLSZ/2, y, TLSZ, TLSZ, 1.0f, 1.0f, 0.0f, 8.0f, 5.0f);
    crt->nxt->s->cfrmx = 1;
    crt->nxt->s->cfrmy = ctype;
    crt->nxt->s->direction = RIGHT;
    crt->nxt->s->wantDirection = NONE;//rand()%4+1;
    crt->nxt->type = ctype;
    crt->nxt->mode = CHASE;
    crt->nxt->last_contact = (int*)calloc(4, sizeof(int));
    crt->nxt->last_x = 0;
    crt->nxt->last_y = 0;
    ctype++;
    crt = crt->nxt;
  }
  return g;
}

int return_contact(int* map, int w, int h, spr* s, int dir)
{
  switch(dir)
  {
  case UP:
    return one(get_xy(map, w, h, posX(*s)/TLSZ, posY(*s)/TLSZ-1));
    break;
  case RIGHT:
    return one(get_xy(map, w, h, posX(*s)/TLSZ+1, posY(*s)/TLSZ));
    break;
  case DOWN:
    return one(get_xy(map, w, h, posX(*s)/TLSZ, posY(*s)/TLSZ+1));
    break;
  case LEFT:
    return one(get_xy(map, w, h, posX(*s)/TLSZ-1, posY(*s)/TLSZ));
    break;
  default:
    break;
  }
  return 0;
}

void render_ghosts(ghost* head, GLuint prg, mat4x4 ortho)
{
  ghost* c_ghost = head; 
  while(c_ghost->nxt!=NULL)
  {
    render_spr(c_ghost->nxt->s, prg, ortho);
    c_ghost = c_ghost->nxt;
  }
}

void render_moveable_spr(spr* s, ghost* g, GLuint prg, mat4x4 ortho)
{
  glUseProgram(prg);
  ghost* crt = g;
  while(crt->nxt != NULL)
  {
    glBindTexture(GL_TEXTURE_2D, crt->nxt->s->tex);
    glBindVertexArray(crt->nxt->s->vao);

    render_spr(crt->nxt->s, prg, ortho);
    crt = crt->nxt;
  }
  glBindTexture(GL_TEXTURE_2D, s->tex);
  glBindVertexArray(s->vao);
  render_spr(s, prg, ortho);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
}

void print_table(int* table, int* map, int w, int h)
{
  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      if(table[j*w+i])
      {
        if(table[j*w+i]>9)
          printf(" %d", table[j*w+i]);
        else 
          printf(" %d ", table[j*w+i]);
      }
      else
        printf("   ");
    }
    printf("\n\n");
  }
}

struct node
{
  int x, y, id, dir;
  struct node* next_node;
};

int get_dir_x(int x, int y, int dir)
{
  switch(dir)
  {
  case RIGHT:
    return x+1;
  case LEFT: 
    return x-1;
  default:
    return x;
  }
  return x;
}

int get_dir_y(int x, int y, int dir)
{
  switch(dir)
  {
  case UP:
    return y-1;
  case DOWN:
    return y+1;
  default:
    return y;
  }
  return y;
}

void add_node(struct node* head, int x, int y, int dir)
{
  struct node* current = head;
  while(current->next_node!=NULL)
    current = current->next_node;
  current->next_node = (struct node*)malloc(sizeof(struct node));
  current->next_node->next_node = NULL;
  current->next_node->x = x;
  current->next_node->y = y;
  current->next_node->id = current->id+1;
  current->next_node->dir = dir;
}

void free_list(struct node* head)
{
  struct node* tmp;
  while (head != NULL)
  {
    tmp = head;
    head = head->next_node;
    free(tmp);
  }
}

void del_node(struct node* head, int id)
{
  struct node* current = head;
  while(current->next_node->id != id)
    current = current->next_node;
  if(current->next_node->next_node == NULL)
    free(current->next_node);
  else
  {
    struct node* delete = current->next_node;
    current->next_node = current->next_node->next_node;
    free(delete);
  }
}

int get_random_dir(ghost* g, int* map, int w, int h)
{
  int reverse_direction = dir_opposite(g->s->direction);
  int dif = 0;

  for(int i = 0; i < 4; i++)
    if(return_contact(map, w, h, g->s, i+1) != g->last_contact[i])
    {
      dif = 1;
      break;
    }

  if(!dif)
    return 0;
  int i = rand()%4;
  while(return_contact(map, w, h, g->s, i+1)!=0 || i == reverse_direction-1){ i = rand()%4; }
  int current_direction = i+1;
  for(int j = 0; j < 4; j++)
    g->last_contact[j] = return_contact(map, w, h, g->s, i+1);
  return current_direction;
}

void set_path(ghost* g, struct g_node** map, struct g_node_list* list, struct stack_node* stack_head, int w, int h, int destination_x, int destination_y)
{
  g->s->wantDirection = shortest_path(map, x(*g->s), y(*g->s), destination_x, destination_y, w, h, stack_head, list);
  print_direction(g->s->wantDirection);
  printf(" - %d %d\n", x(*g->s), y(*g->s));
}

void set_ghost_direction(ghost* g, map m, struct g_node** g_map, struct g_node_list* list, struct stack_node* stack_head)
{
  int d;
  switch(g->mode)
  {
  case CHASE:
    if((g_map[x(*g->s)+y(*g->s)*m.w]!=NULL && (x(*g->s)!=g->last_x || y(*g->s)!=g->last_y)))
    {
      set_path(g, g_map, list, stack_head, m.w, m.h, 1, 1);
      g->last_x = x(*g->s);
      g->last_y = y(*g->s);
    }
    break;
  case HOME:
    break;
  case FRIGHT:
    if(g->s->wantDirection != 0 || (x(*g->s) != g->last_x && y(*g->s)!=g->last_y && g_map[x(*g->s)+y(*g->s)*m.w]!=NULL && (d = get_random_dir(g, m.tileMap, m.w, m.h))))
    {
      g->s->wantDirection = d;
      g->last_x = x(*g->s);
      g->last_y = y(*g->s);
    }
    return;
    break;
  default:
    return;
    break;
  }
}

void set_ghost_state(ghost* g, int mode)
{
  g->mode = mode;
  switch(mode)
  {
    case FRIGHT:
      g->s->direction = dir_opposite(g->s->direction);
      g->s->cfrmx = 0;
      g->s->cfrmy = 5;
      g->s->mfrm = 1;
      break;
    case CHASE:
      g->s->cfrmy = g->type;
      g->s->cfrmx = 0;
      g->s->mfrm = g->s->direction;
      break;
    case HOME:
      g->s->cfrmx = 0;
      g->s->mfrm = 3;
      g->s->cfrmy = 5;
      break;
    default:
      break;
  }
}

void update_ghost_state(ghost* g)
{
  switch(g->mode)
  {
  case CHASE:
    animate_ghost_spr(g->s, 12);
    break;
  case FRIGHT:
    animate_ghost_spr(g->s, 12);
    break;
  case HOME:
    g->s->cfrmx = 3 + g->s->direction;
    break;
  default:
    break;
  } 
}

void update_ghosts(ghost* head, int* table, int* box_map, struct g_node** g_map, struct g_node_list* g_list, struct stack_node* stack_head, float dt, map m)
{
  ghost* c_ghost = head;
  while(c_ghost->nxt!=NULL)
  {
    set_ghost_direction(c_ghost->nxt, m, g_map, g_list, stack_head);
    update_ghost_state(c_ghost->nxt);
    update_spr(c_ghost->nxt->s, dt, c_ghost->nxt->mode == FRIGHT ? 5 : 8, m, 1, 1, c_ghost->nxt->mode); 
    c_ghost = c_ghost->nxt;
  }
}

#endif
