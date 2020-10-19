#ifndef __TILE__
#define __TILE__
typedef struct tileNodes{
  int x, y;
  int type; // Type of alignment with neighbouring tiles. Ex. 1 2 3 4 ...
  int angle; // Only need angles that are proportional to 90 degrees.
  int state; // What type of wall the tile is
  int invert;
  struct tileNodes* nextTile;
}tile;

typedef struct{
  int texW, texH;
  GLuint vao,tex;
}renderData;

int get_xy(int* map, int w, int h, int x, int y)
{
  if(!(x>=w||x<0) && !(y>=h||y<0))
    return map[y*w+x];
  else
    return 0;
}

int get_quad_xy(int* map, int w, int h, int x, int y)
{
  if(!(x>=w||x<0) && !(y>=h||y<0))
    return map[y*w+x];
  else
    return 1;
}

int count_side(int* array, int width)
{
  int count = 0;
  for(int i = 0; i < width; i++)
    if(array[i])
      count++;
  return count;
}

int one(int i)
{
  return i > 0;
}

int fright_one(int i)
{
  if(i == 4)
    return 0;
  else
    return i>0;
  return 0;
}

int* create_contact_array(int* map, int w, int h, int x, int y)
{
  int* contact_array = (int*)malloc(4*sizeof(int));
  contact_array[0] = one(get_xy(map, w, h, x, y-1));
  contact_array[1] = one(get_xy(map, w, h, x+1, y));
  contact_array[2] = one(get_xy(map, w, h, x, y+1));
  contact_array[3] = one(get_xy(map, w, h, x-1, y));
  return contact_array;
}

int* create_fright_contact_array(int* map, int w, int h, int x, int y)
{
  int* contact_array = (int*)malloc(4*sizeof(int));
  contact_array[0] = fright_one(get_xy(map, w, h, x, y-1));
  contact_array[1] = fright_one(get_xy(map, w, h, x+1, y));
  contact_array[2] = fright_one(get_xy(map, w, h, x, y+1));
  contact_array[3] = fright_one(get_xy(map, w, h, x-1, y));
  return contact_array;
}

int get_vert_angle(int* map, int w, int h, int x, int y)
{
  int array[4] = { get_xy(map, w, h, x+1, y+1), get_xy(map, w, h, x-1, y+1), get_xy(map, w, h, x-1, y-1), get_xy(map, w, h, x+1, y-1) };
  for(int i = 0; i<4; i++)
    if(!array[i])
      return i*90;
  return -1;
}

void shift_right(int* array, int end)
{
  int saveVal = array[end-1];
  int lastVal;
  for(int i = 0; i < end; i++)
  {
    lastVal = array[i];
    array[i] = saveVal;
    saveVal = lastVal;
  }
}

int compare_arrays(int* a1, int* a2, int l)
{
  for(int i = 0; i < l; i++)
    if(a1[i]!=a2[i])
      return 0;
  return 1;
}

int rotate_array(int* array, int d, int l, int u, int r)
{
  int stride[4] = { d, l, u, r };
  if(count_side(array, 4) != count_side(stride, 4))
  {
    return 0;
  }
  int angle = 0;
  while(!compare_arrays(array, stride, 4))
  {
    shift_right(array, 4);
    angle -= 90;
  }
  return angle;
}

void create_blue_tile_array(tile* lastTile, int* map, int w, int h)
{
  tile* current = lastTile;
  while(current->nextTile!=NULL)
    current = current->nextTile;
  int x = 0, y = 0;
  int* tile_contact_array = create_contact_array(map, w, h, x, y);
  int xinc = 1;
  int yinc = 1;
  int xcling = 1;
  int ycling = 1;
  int i = 0;
  int done = 0;
  int moved = 0;
  while(!done)
  {
    current->nextTile = (tile*)malloc(sizeof(tile));
    current->nextTile->nextTile = NULL;
    current->nextTile->type = count_side(tile_contact_array, 4);
    current->nextTile->x = x * TLSZ;
    current->nextTile->y = y * TLSZ;
    current->nextTile->invert = 0;
    current->nextTile->state = 2;
    if(current->nextTile->type == 1)
    {
      current->nextTile->angle = rotate_array(tile_contact_array, 0, 0, 0, 1);
      current->nextTile->type = 2;
    }
    else if(current->nextTile->type == 2 && tile_contact_array[0] == tile_contact_array[2])
    {
      current->nextTile->angle = rotate_array(tile_contact_array, 0, 1, 0, 1);
      tile_contact_array = create_contact_array(map, w, h, x, y);
      if(tile_contact_array[UP-1]||tile_contact_array[DOWN-1])
      {
        if(yinc == -1)
          current->nextTile->angle += 180;
      }
      else if(tile_contact_array[RIGHT-1]||tile_contact_array[LEFT-1])
        if(xinc == 1)
          current->nextTile->angle += 180;
    }
    else if(current->nextTile->type == 2 && tile_contact_array[0]!=tile_contact_array[2])
    {
      int* rotate_var = tile_contact_array;
      current->nextTile->angle = rotate_array(rotate_var, 0, 1, 1, 0);
    tile_contact_array = create_contact_array(map, w, h, x, y);
      if((tile_contact_array[RIGHT-1] && tile_contact_array[DOWN-1] && xinc == -1) || (tile_contact_array[RIGHT-1] && tile_contact_array[UP-1] && yinc == 1)||(xinc == 1 && tile_contact_array[LEFT-1] && tile_contact_array[UP-1]) || (yinc == -1 && tile_contact_array[LEFT-1] && tile_contact_array[DOWN-1]))
      {
        current->nextTile->type = 1;
        current->nextTile->state = 1;
      }
      else
        current->nextTile->type = 1;
    }
    else if(current->nextTile->type == 3)
    {
      current->nextTile->angle = rotate_array(tile_contact_array, 1, 1, 0, 1);
      if(xinc == 1 && !get_quad_xy(map, w, h, x - 1, y + 1))
        current->nextTile->invert = 1;
      else if(xinc == -1 && !get_quad_xy(map, w, h, x + 1, y - 1))
        current->nextTile->invert = 1;
      else if(yinc == 1 && !get_quad_xy(map, w, h, x - 1, y - 1))
        current->nextTile->invert = 1;
      else if(yinc == -1 && !get_quad_xy(map, w, h, x + 1, y + 1))
        current->nextTile->invert = 1;
    }
    tile_contact_array = create_contact_array(map, w, h, x, y);
    if(tile_contact_array[UP-1] && yinc != 1)
    {
      yinc = -1;
      xinc = 0;
    }
    else if(tile_contact_array[RIGHT-1] && xinc != -1)
    {
      xinc = 1;
      yinc = 0;
    }
    else if(tile_contact_array[DOWN-1] && yinc != -1)
    {
      yinc = 1;
      xinc = 0;
    }
    else if(tile_contact_array[LEFT-1] && xinc != 1)
    {
      xinc = -1;
      yinc = 0;
    }
    if(count_side(tile_contact_array, 4) <= 1 && !moved)
    {
      if(xinc == 1 && get_xy(map, w, h, x, y+2))
      {
        moved = 1;
        current->nextTile->type = 2;
        current->nextTile->angle = rotate_array(tile_contact_array, 0, 1, 0, 0);
        xinc = 0;
        y += 2;
      }
      else if(xinc == -1 && get_xy(map, w, h, x, y - 2))
      {
        moved = 1;
        current->nextTile->type = 2;
        current->nextTile->angle = rotate_array(tile_contact_array, 0, 1, 0, 0);
        xinc = 0;
        y -= 2;
      }
      else
        done = 1;
    }
    else
      moved = 0;
    x+=xinc;
    y+=yinc;
    tile_contact_array = create_contact_array(map, w, h, x, y);
        if(x == 0 && y == 0)
      done = 1;
    current = current->nextTile;
  }
}

void add_blue_box(tile* head, int* map, int w, int h, int* box_x, int *box_y, int *box_w, int *box_h)
{
  int tx = 0, ty = 0, sx, sy;
  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      if(get_xy(map, w, h, i, j) == 3)
      {
        tx = i;
        ty = j;
        i = w;
        j = h;
      }
    }
  }
  sx = tx;
  sy = ty;
  *box_x = sx;
  *box_y = sy;
  tile* current = head;
  while(current->nextTile!=NULL)
    current = current->nextTile;
  int done = 0;
  int yinc = 0;
  int xinc = 1;
  int* contact = create_contact_array(map, w, h, tx, ty);
  int count;
  while(!done)
  {
    contact = create_contact_array(map, w, h, tx, ty);
    count = count_side(contact, 4);
    current->nextTile = (tile*)malloc(sizeof(tile));
    current->nextTile->nextTile = NULL;
    current->nextTile->type = count;
    current->nextTile->state = 3;
    current->nextTile->x = tx * TLSZ;
    current->nextTile->y = ty * TLSZ;
    current->nextTile->invert = 0;
    if(contact[0] != contact[2])
    {
      current->nextTile->angle = rotate_array(contact, 0, 1, 1, 0);
      current->nextTile->type = 1;
    }
    else
    {
      if(get_xy(map, w, h, tx, ty)==4)
        current->nextTile->type = 3;
      else
        current->nextTile->type = 2;
      current->nextTile->angle = rotate_array(contact, 0, 1, 0, 1);
      if(xinc == -1)
        current->nextTile->angle += 180;
      else if(yinc == 1)
        current->nextTile->angle += 180;
    }
    contact = create_contact_array(map, w, h, tx, ty);
    count = count_side(contact, 4);
    if(contact[UP-1] && yinc != 1)
    {
      yinc = -1;
      xinc = 0;
    }
    else if(contact[RIGHT-1] && xinc != -1)
    {
      xinc = 1;
      yinc = 0;
    }
    else if(contact[DOWN-1] && yinc != -1)
    {
      *box_w = *box_x - tx;
      yinc = 1;
      xinc = 0;
    }
    else if(contact[LEFT-1] && xinc != 1)
    {
      *box_h = *box_y - ty;
      xinc = -1;
      yinc = 0;
    }
    tx += xinc;
    ty += yinc;
    if(tx == sx && ty == sy)
      done = 1;
    current = current->nextTile;
  }
}

int tile_info(int* map, int x, int y, int w, int h, int* type, int* angle, int* state)
{
  int* sides = create_contact_array(map, w, h, x, y);
  int product = 0;
  for(int i = 0; i<4; i++)
    product += sides[i];
  *state = get_xy(map, w, h, x, y);
  if(*state == 1)
  {
    switch(product)
    {
    case 1:
      *type = 2;
      *angle = rotate_array(sides, 0, 1, 0, 0);
      break;
    case 2:
      if(sides[0] == sides[2])
      {
        *angle = rotate_array(sides, 0, 1, 0, 1);
        *type = 2;
      }
      else
      {
        *type = 1;
        *angle = rotate_array(sides, 0, 1, 1, 0);
      }
      break;
    case 3:
      *angle = rotate_array(sides, 0, 1, 1, 1);
      *type = 2;
      break;
    case 4:
      if(get_vert_angle(map, w, h, x, y)==-1)
        return 0;
      else
        *type = 3;
      *angle = get_vert_angle(map, w, h, x, y);
      break;
    default:
      break;
    }
  }
  return 1;
}

tile* create_tile_list(int* map, int w, int h, int* box_x, int* box_y, int* box_w, int* box_h)
{
  tile* currentNode = (tile*)malloc(sizeof(tile));
  currentNode->nextTile = NULL;
  tile* returnNode = currentNode;
  int x = 0, y = 0, amo = 0;
  while(y < h)
  {
    while(x < w)
    {
      if(get_xy(map, w, h, x, y)==1)
      {
        currentNode->nextTile = (tile*)malloc(sizeof(tile));
        currentNode->nextTile->nextTile = NULL;
        currentNode->nextTile->x = x*TLSZ;
        currentNode->nextTile->y = y*TLSZ;
        currentNode->nextTile->invert = 0;
        if(tile_info(map, x, y, w, h, &currentNode->nextTile->type, &currentNode->nextTile->angle, &currentNode->nextTile->state))
          currentNode = currentNode->nextTile;
        amo++;
      }
      x++;
    }
    x = 0;
    y++;
  }
  create_blue_tile_array(returnNode, map, w, h);
  add_blue_box(returnNode, map, w, h, box_x, box_y, box_w, box_h);
  return returnNode;
}
//END

int tileSize(tile* tileList)
{
  int i = 0;
  tile* current = tileList;
  while(current->nextTile!=NULL)
  {
    current = current->nextTile;
    i++;
  }
  return i;
}

mat4x4* create_tex_mat_list(tile* tileList)
{
  mat4x4* list = (mat4x4*)malloc(tileSize(tileList) * sizeof(mat4x4));
  tile* current = tileList;
  int i = 0;
  while(current->nextTile!=NULL)
  {
    mat4x4_identity(list[i]);
    mat4x4_translate(list[i], 1.0f/5.0f * (float)(current->nextTile->type + current->nextTile->invert), 1.0f/3.0f * (float)(current->nextTile->state-1), 0.0f);
    i++;
    current = current->nextTile;
  }
  return list;
}

mat4x4* create_model_mat_list(tile* tileList)
{
  mat4x4* list = (mat4x4*)malloc(tileSize(tileList) * sizeof(mat4x4));
  tile * current = tileList;
  int i = 0;
  while(current->nextTile!=NULL)
  {
    mat4x4 m0, m1, m2, m3, f, s0, s, s1;
    mat4x4_identity(s0);
    mat4x4_identity(s);
    mat4x4_translate(m0, current->nextTile->x, current->nextTile->y + MENU_TILES*TLSZ, 0.0f);
    mat4x4_translate(m1, 0.5f*TLSZ, 0.5f*TLSZ, 0.0f);
    mat4x4_rotate_Z(m2, m1, current->nextTile->angle * (PI/180));
    mat4x4_translate(m1, -0.5f*TLSZ, -0.5f*TLSZ, 0.0f);
    mat4x4_mul(m3, m2, m1);
    mat4x4_mul(s0, s, m3);
    mat4x4_mul(s1, m3, s);
    mat4x4_mul(list[i], m0, s1);
    i++;
    current = current->nextTile;
  }
  return list;
}

/* Tiles - 
 * 	 - Position
 *   - Rotation
 *   - Type
 *   - State
 * ____________
 * CreateMapTexture()
 *     Loop(tile_list)
 *     		CurrentBlock = GetTex(tex, x, y, ...)
 *     		....
 */

GLuint GenMapTex(const char* base_tex_path, tile* tile_map, int w, int h)
{
	BMP* base_tex = BMP_ReadFile(base_tex_path);
	int width = BMP_GetWidth(base_tex), height = BMP_GetHeight(base_tex);
	if(!base_tex)
	{
		printf("Failed to load %s!\n", base_tex_path);
		return 0;
	}
	uint32_t* colbuf = (uint32_t*)malloc(w*PX_TILE_WIDTH*h*PX_TILE_HEIGHT*sizeof(uint32_t));
	for(int i = 0; i < w*PX_TILE_WIDTH*h*PX_TILE_HEIGHT; i++)
		colbuf[i] = col_rgb(0, 0, 0); 
	tile* current = tile_map;
	while(current->nextTile!=NULL)
	{
		int base_tex_x = current->nextTile->type+current->nextTile->invert, base_tex_y = current->nextTile->state-1;
		uint32_t* current_tile_tex = make_tex_from_bmp(base_tex, width, height, PX_TILE_WIDTH, PX_TILE_HEIGHT, base_tex_x, base_tex_y);
	  uint32_t* rotated_current_tile_tex = orthogonally_rotate_tex(current_tile_tex, norm((int)current->nextTile->angle/90), PX_TILE_WIDTH, PX_TILE_HEIGHT);
		insert_tex(colbuf, w*PX_TILE_WIDTH, h*PX_TILE_HEIGHT, rotated_current_tile_tex, PX_TILE_WIDTH, PX_TILE_HEIGHT, current->nextTile->x/(int)TLSZ, current->nextTile->y/(int)TLSZ);
		free(current_tile_tex);
		current = current->nextTile;
	}
	BMP_Free(base_tex);
	GLuint gl_tex;
	glGenTextures(1, &gl_tex);
	glBindTexture(GL_TEXTURE_2D, gl_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w*PX_TILE_WIDTH, h*PX_TILE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &colbuf[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(colbuf);
	return gl_tex;
}

void freeList(tile* head)
{
  tile* tmp;
  while (head != NULL)
  {
    tmp = head;
    head = head->nextTile;
    free(tmp);
  }
}

#endif
