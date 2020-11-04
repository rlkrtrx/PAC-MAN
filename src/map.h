#ifndef __MAP__
#define __MAP__

typedef struct
{
  GLuint vrtobj, coin_vao, amo, tex, dot_amo, pellet_amo, max_points, map_tex, map_vao, *coin_tex;
  int *tileMap, *coinMap, w, h;
  //mat4x4* mtcs, *tmtcs;
  tile* tm;
  int box_x, box_y, box_w, box_h;
}map;

void free_tile_list(tile* head)
{
  tile* tmp;
  while (head != NULL)
  {
    tmp = head;
    head = head->nextTile;
    free(tmp);
  }
}

GLuint make_tile_inst(float w, float h, mat4x4* mtcs, mat4x4* tmtcs, int amount, int ctfrmx, int ctfrmy)
{
  GLuint vao, b;
  float vrt[16] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    w, 0.0f, 1.0f/(float)ctfrmx, 0.0f,
    w, h, 1.0f/(float)ctfrmx, 1.0f/(float)ctfrmy,
    0.0f, h, 0.0f, 1.0f/(float)ctfrmy
  };
  int ind[6] = {
    0, 1, 3,
    3, 2, 1
  };
  GLuint b0, b1, b2;
  b0 = make_buff(vrt, sizeof(vrt), GL_ARRAY_BUFFER);
  b1 = make_buff(ind, sizeof(ind), GL_ELEMENT_ARRAY_BUFFER);
  GLuint* va = (GLuint*)malloc(sizeof(GLuint));
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, b0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
  b = make_inst(mtcs, amount, 2);
  b2 = make_inst(tmtcs, amount, 7);
  glBindVertexArray(0);
  return vao;
}

void render_inst(GLuint prg, GLuint vao, GLuint mapTex, GLuint amount, mat4x4 ortho, float color[3])
{
  glUseProgram(prg);
  glBindTexture(GL_TEXTURE_2D, mapTex);
  glBindVertexArray(vao);
  glUniformMatrix4fv(glGetUniformLocation(prg, "prj"), 1, GL_FALSE, &ortho[0][0]);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, amount);	
  glBindVertexArray(0);
}

map Map(const char* mapBmpPath)
{
  map m;
  BMP* img = BMP_ReadFile(mapBmpPath);
  m.w = BMP_GetWidth(img);
  m.h = BMP_GetHeight(img);
  m.tileMap = (int*)malloc(m.w * m.h * sizeof(int));
  m.coinMap = (int*)malloc(m.w * m.h * sizeof(int));
  m.coin_vao = make_quad(TLSZ, TLSZ, 1.0f, 1.0f);
  int amo = 0;
  UCHAR r,g,b;
  m.dot_amo = 0;
  m.pellet_amo = 0;
  for(int y = 0; y < m.h; y++)
  {
    for(int x = 0; x < m.w; x++)
    {
      BMP_GetPixelRGB(img, x, y, &r, &g, &b);	
      if(r == 0 && g == 0 && b == 255)
      {
        amo ++;
        m.tileMap[y * m.w + x] = 1;
        m.coinMap[y * m.w + x] = 0;
      }
      else if(r == 0 && g == 0 && b == 200)
      {
        amo++;
        m.tileMap[y * m.w + x] = 2;
        m.coinMap[y * m.w + x] = 0;
      }
      else if(r == 0 && g == 0 && b == 150)
      {
        amo++;
        m.tileMap[y * m.w + x] = 3;
        m.coinMap[y * m.w + x] = 0;
      }
      else if(r == 255 && g == 255 && b == 255)
      {
        amo++;
        m.tileMap[y * m.w + x] = 4; 
        m.coinMap[y * m.w + x] = 0;
      }
      else if(r == 255 && g == 255 && b==0)
      {
        m.dot_amo++;
        m.coinMap[y * m.w + x] = 1;
        m.tileMap[y * m.w + x] = 0;
      }
      else if(r == 200 && g == 200 && b==200)
      {
        m.coinMap[y * m.w + x] = 2;
        m.tileMap[y * m.w + x] = 0;
        m.pellet_amo++;
      }
      else
      {
        m.tileMap[y * m.w + x] = 0;
        m.coinMap[y * m.w + x] = 0;
      }
    }
  }
	BMP_Free(img);
  m.amo = amo;
  m.tm = create_tile_list(m.tileMap, m.w, m.h, &m.box_x, &m.box_y, &m.box_w, &m.box_h);
	m.map_tex = GenMapTex("img/tile.bmp", m.tm, 28, 31);
	m.map_vao = make_quad(TLSZ*28, TLSZ*31, 1, 1);
  free_tile_list(m.tm);
  m.tex = make_tex("img/tile.bmp", GL_RGB);
	m.coin_tex = (GLuint*)malloc(2*sizeof(GLuint));
	BMP* img_bmp = BMP_ReadFile("img/tile.bmp");
	uint32_t* buf1 = make_tex_from_bmp(img_bmp, BMP_GetWidth(img_bmp), BMP_GetHeight(img_bmp), PX_TILE_WIDTH, PX_TILE_HEIGHT, 4, 0);
	m.coin_tex[0] = gen_tex_from_map(make_tex_from_bmp(img_bmp, BMP_GetWidth(img_bmp), BMP_GetHeight(img_bmp), PX_TILE_WIDTH, PX_TILE_HEIGHT, 4, 2), PX_TILE_WIDTH, PX_TILE_HEIGHT); 
	m.coin_tex[1] = gen_tex_from_map(make_tex_from_bmp(img_bmp, BMP_GetWidth(img_bmp), BMP_GetHeight(img_bmp), PX_TILE_WIDTH, PX_TILE_HEIGHT, 4, 0), PX_TILE_WIDTH, PX_TILE_HEIGHT); 
  m.max_points = m.dot_amo*10 + m.pellet_amo*50;
  return m;
}

void render_coins(map m, mat4x4 proj, GLuint p)
{
  mat4x4 mdl;
  glUseProgram(p); 
	glBindTexture(GL_TEXTURE_2D, m.coin_tex[0]);
  glBindVertexArray(m.coin_vao);
  for(int y = 0; y < m.h; y++)
  {
    for(int x = 0; x < m.w; x++)
    {
      if(m.coinMap[y*m.w+x])
      {
				glBindTexture(GL_TEXTURE_2D, m.coin_tex[m.coinMap[y*m.w+x]-1]);
        mat4x4_translate(mdl, x * TLSZ, y * TLSZ + TLSZ * MENU_TILES, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(p, "ortho"), 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(p, "model"), 1, GL_FALSE, &mdl[0][0]); 
				mat4x4_identity(mdl);
        glUniformMatrix4fv(glGetUniformLocation(p, "texModel"), 1, GL_FALSE, &mdl[0][0]); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }
  glBindVertexArray(0);
  glBindTexture(0, 0);
}

int get_tile(map m, int x, int y)
{
  return m.tileMap[y*m.w+x];
}

struct g_node
{
  uint16_t x, y;
  uint32_t d, c; // Distance and Change, respectively.
  int parent;
  int visited;
  struct g_node** neighbours;
};

struct g_node_list {
  struct g_node* gnode_ptr;
  struct g_node_list* next_node;
};

void add_g_node(struct g_node_list* list, struct g_node* ptr)
{
  struct g_node_list* current_node = list;
  while(current_node->next_node != NULL)
    current_node = current_node->next_node;
  current_node->next_node = (struct g_node_list*)malloc(sizeof(struct g_node_list));
  current_node->next_node->next_node = NULL;
  current_node->next_node->gnode_ptr = ptr;
}

struct g_node* init_graph_node(int x, int y)
{
  struct g_node* ret_g = (struct g_node*)malloc(sizeof(struct g_node));
  ret_g->neighbours = (struct g_node**)malloc(4 * sizeof(struct g_node*));
  for(int i = 0; i < 4; i++)
    ret_g->neighbours[i] = NULL;
  ret_g->x = x;
  ret_g->y = y;
  ret_g->d = 0xFFFF;
  ret_g->c = 0xFFFF; 
  ret_g->parent = -1;
  ret_g->visited = 0;
  return ret_g;
}

int greater(int val)
{
  return val>0;
}

struct g_node* ret_map_graph(int* map, struct g_node** g_map, int w, int h)
{
  struct g_node* root = NULL;
  struct g_node* left_node = NULL;
  struct g_node** top_nodes = (struct g_node**)malloc(w*sizeof(struct g_node));
  int intersect = 0;
  for(int i = 0; i < w; i++)
    top_nodes[i] = NULL;
  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      int* contact_array = create_contact_array(map, w, h, i, j);
      int count = 0;
      struct g_node* current;
      for(int i = 0; i < 4; i++)
        count += contact_array[i];
      switch(greater(map[j*w+i]))
      {
      case TRUE:
        top_nodes[i] = NULL;
        left_node = NULL;
        break;
      case FALSE:
        if((contact_array[0]!=contact_array[1] && contact_array[0] != contact_array[3])&&(contact_array[2]!=contact_array[1] && contact_array[2] != contact_array[3])) //Check if one finds themselves at an intersection
          continue;
        if(!root)
        {
          root = init_graph_node(i, j);
          current = root;
        }
        else
          current = init_graph_node(i, j);
        if(left_node)
        {
          left_node->neighbours[RIGHT-1] = current;
          current->neighbours[LEFT-1] = left_node;

        }
        if(top_nodes[i])
        {
          top_nodes[i]->neighbours[DOWN-1] = current; 
          current->neighbours[UP-1] = top_nodes[i];
        }
        g_map[j*w+i] = current;
        //Check for a left node and connect if one finds themselves at an intersection
        left_node = current; //Assign left node to g for the next intersection
        top_nodes[i] = current; //Assign top node to g for the next row
        break;
      default:
        
        break;
      }
      free(contact_array);
    }
    left_node = NULL;
  }
  refresh();
  return root;
}

void graph_to_list(struct g_node_list* list, struct g_node** g_map, int w, int h)
{
  for(int i = 0; i < w*h; i++)
    if(g_map[i]!=NULL)
      add_g_node(list, g_map[i]);
}

struct stack_node
{
  struct g_node* gnode_ptr;
  struct stack_node* next;
};

void push(struct stack_node* head, struct g_node* ptr)
{
  struct stack_node* current = head;
  while(current->next && current->next->gnode_ptr->d+current->next->gnode_ptr->c < ptr->d+ptr->c)
    current = current->next;
  struct stack_node* new_node = (struct stack_node*)malloc(sizeof(struct stack_node));
  new_node->next = NULL;
  printf("ptr data: %d %d %d\n", ptr->x, ptr->y, ptr->parent);
  new_node->gnode_ptr = ptr;
  printf("ptr data: %d %d %d\n", ptr->x, ptr->y, ptr->parent);
  if(current->next)
    new_node->next = current->next;  
  current->next = new_node;
}

void pop(struct stack_node* head)
{
  struct stack_node* temp = head->next;
  head->next = head->next->next;
  free(temp);
}

void clear_list(struct g_node_list* list)
{
  struct g_node_list* current = list;
  while(current->next_node!=NULL)
  {
    current->next_node->gnode_ptr->d = 0xFFFF;
    current->next_node->gnode_ptr->c = 0xFFFF;
    current->next_node->gnode_ptr->parent = -1;
    current->next_node->gnode_ptr->visited = 0;
    current = current->next_node;
  }
}

int distance(int x1, int y1, int x2, int y2)
{
  return (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
}

int perpendicular_distance(struct g_node* root, struct g_node* new_node)
{
  return root->x == new_node->x ? abs(root->y - new_node->y) : abs(root->x - new_node->x);
}

void proc_neighbour(struct g_node* root, struct g_node* neighbour, struct g_node* dest)
{
  neighbour->c = root->x == neighbour->x ? abs(root->y - neighbour->y) : abs(root->x - neighbour->x);
  neighbour->d = distance(neighbour->x, neighbour->y, dest->x, dest->y)+neighbour->c;
  neighbour->parent = (root->x == neighbour->x ? (root->y - neighbour->y < 0 ? UP : DOWN) : (root->x - neighbour->x < 0 ? LEFT : RIGHT))-1;  
  neighbour->visited = 1;
}

void list_stack(struct stack_node* head)
{
  struct stack_node* current_node = head;
  while(current_node->next)
  {
    if(current_node->next->gnode_ptr)
      printf(" - %d %d %d %d - ", current_node->next->gnode_ptr->x, current_node->next->gnode_ptr->y, current_node->next->gnode_ptr->d, current_node->next->gnode_ptr->c);
    current_node = current_node->next;
  }
  printf("\n");
}

void clear_stack(struct stack_node* head)
{
  struct stack_node* current = head;
  while(head->next != NULL)
    pop(head);
}

void print_map(struct g_node** g_map, int* map, int w, int h)
{
  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      if(g_map[i+j*w])
        printf("X ");
      else if(map[i+j*w])
        printf("%d ", map[i+j*w]);
      else
        printf("  ");
    }
    printf("\n");
  }
}

int shortest_path(struct g_node** g_map, int si, int sj, int di, int dj, int w, int h, struct stack_node* global, struct g_node_list* g_list)
{
  struct g_node *start_node, *end_node;
  if(!(start_node = g_map[sj*w+si]) || !(end_node = g_map[dj*w+di]) || (si == di && sj == dj))
    return 0;
  start_node->d = distance(si, sj, di, dj);
  start_node->c = 0;
  start_node->visited = 1;
  start_node->parent = -1;
  push(global, start_node);
  printf("%d %d\n", si, sj);
  printf("%d\n", start_node->parent);

  struct g_node** check_node = &global->next->gnode_ptr;
  while(global->next->gnode_ptr != end_node && global->next != NULL)
  {
    struct g_node* check = global->next->gnode_ptr; 
    pop(global);

    if(!check)
      continue;
    for(int i = 0; i < 4; i++)
    {
      if((check->neighbours[i] == NULL) || (check->neighbours[i]->visited))
        continue;
      printf("X %d Y %d Visited %d\n", check->neighbours[i]->x, check->neighbours[i]->y, check->neighbours[i]->visited);

      proc_neighbour(check, check->neighbours[i], end_node);

      push(global, check->neighbours[i]);
    }
  }  

  printf("%d %d\n", di, dj);

  struct g_node* current = end_node->neighbours[end_node->parent];
  printf("assignéd %d %d %d\n", current->parent, current->x, current->y);
  while(current->parent != -1 && current->neighbours[current->parent] != start_node)
  {
    printf("loop... %d %d\n", current->x, current->neighbours[current->parent]->y);
    current = current->neighbours[current->parent];
  }
  clear_list(g_list);
  clear_stack(global);
  return current->x == start_node->x ? (start_node->y - current->y < 0 ? DOWN : UP) : (start_node->x - current->x > 0 ? LEFT : RIGHT); 
}


#endif
