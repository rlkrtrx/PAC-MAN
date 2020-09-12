#ifndef __NGIN__
#define __NGIN__

GLuint make_buff(void* data, GLsizei size, GLenum type)
{
  GLuint b;
  glGenBuffers(1, &b);
  glBindBuffer(type, b);
  glBufferData(type, size, data, GL_STATIC_DRAW);
  glBindBuffer(type, 0);
  return b;
}

GLuint make_quad(float w, float h, int ctfrmx, int ctfrmy)
{
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
  GLuint v, b0, b1;
  glGenVertexArrays(1, &v);
  glBindVertexArray(v);
  b0 = make_buff(vrt, sizeof(vrt), GL_ARRAY_BUFFER);
  b1 = make_buff(ind, sizeof(ind), GL_ELEMENT_ARRAY_BUFFER);
  glBindBuffer(GL_ARRAY_BUFFER, b0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return v;
}

void render_tex_quad(GLuint prg, GLuint quad, GLuint tex, int x, int y, mat4x4 ortho)
{
	glUseProgram(prg);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBindVertexArray(quad);
	mat4x4 identity;
	mat4x4_identity(identity);

	mat4x4 model;
	mat4x4_translate(model, 0, MENU_TILES*TLSZ, 0);

	glUniformMatrix4fv(glGetUniformLocation(prg, "ortho"), 1, GL_FALSE, &ortho[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(prg, "model"), 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(prg, "texModel"), 1, GL_FALSE, &identity[0][0]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  rewind(file);
  char *data = (char*)calloc(length + 1, sizeof(char));
  fread(data, 1, length, file);
  fclose(file);
  return data;
}

GLuint make_shdr(const char* dataPath, GLenum type)
{
  const char* src = read_file(dataPath);	
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, NULL); 
  glCompileShader(s);

  int success;
  char infoLog[512];

  glGetShaderiv(s, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(s, 512, NULL, infoLog);
    printf("ERROR: CMP ERROR - %s\n", infoLog);
  }
  return s;
}

GLuint make_prg(GLuint vrt, GLuint frg)
{
  GLuint p = glCreateProgram();
  glAttachShader(p, vrt);
  glAttachShader(p, frg);
  glLinkProgram(p);

  int success;
  char infoLog[512];

  glGetProgramiv(p, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(p, 512, NULL, infoLog);
    printf("ERROR: CMP ERROR - %s\n", infoLog);
  }

  return p;
}

GLuint make_tex(const char* texPath, GLenum type)
{
  GLuint t;
  glGenTextures(1, &t);
  glBindTexture(GL_TEXTURE_2D, t);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*_MIPMAP_NEAREST*/);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  int w, h, n;
  unsigned char* data = stbi_load(texPath, &w, &h, &n, 0);
  if(data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    printf("Failed to load image %s\n", texPath);
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D,0);
  return t;
}

GLuint make_inst(mat4x4* mtcs, int amount, int offset)
{
  GLuint b;
  glGenBuffers(1, &b);
  glBindBuffer(GL_ARRAY_BUFFER, b);
  glBufferData(GL_ARRAY_BUFFER, amount * sizeof(mat4x4), mtcs, GL_STATIC_DRAW);

  for(int i = 0; i < 4; i++)
  {
    glEnableVertexAttribArray(i+offset);
    glVertexAttribPointer(i+offset, 4, GL_FLOAT, GL_FALSE, 4*sizeof(vec4), (void*)(i*sizeof(vec4))); 
    glVertexAttribDivisor(i+offset, 1);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return b;
}

uint32_t col_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	return (r << 24) | (g << 16) | (b << 8) | 255;
}

uint32_t col_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r << 24) | (g << 16) | (g << 8) | a;
}

uint32_t* make_tex_from_bmp(BMP* img, int width, int height, int tile_width, int tile_height, int x, int y)
{
	uint32_t* col_map = (uint32_t*)malloc(tile_width*tile_height*sizeof(uint32_t));
	UCHAR r, g, b;
	for(int j = y*tile_height; j < y*tile_height+tile_height; j++)
	{
		for(int i = x*tile_width; i < x*tile_width+tile_width; i++)
		{
			int map_x = i-x*tile_width, map_y = j-y*tile_height;
			BMP_GetPixelRGB(img, i, j, &r, &g, &b);
			col_map[map_y*tile_width+map_x] = col_rgb(r, g, b);
		}
	}
	return &col_map[0];
}

GLuint gen_tex_from_map(uint32_t* data, int width, int height)
{
	GLuint gl_tex;
	glGenTextures(1, &gl_tex);
	glBindTexture(GL_TEXTURE_2D, gl_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return gl_tex;
}

uint32_t* invert_tex(uint32_t* map, int w, int h)
{
	uint32_t* inverted_map = (uint32_t*)malloc(w*h*sizeof(uint32_t));	
	for(int j = 0; j < h; j++)
		for(int i = 0; i < w; i++)
			inverted_map[i+j*w] = map[j*w+(w-i)];
	free(map);
	return &inverted_map[0];
}

uint32_t* orthogonally_rotate_tex(uint32_t* map, int num_rots, int w, int h)
{
	if(num_rots == 0)
		return map;
	uint32_t* rotated_tex = (uint32_t*)malloc(w*h*sizeof(uint32_t));	
	for(int j = 0; j < w; j++)
	{
		for(int i = 0; i < w; i++)
			rotated_tex[i*w+(w-1-j)] = map[j*w+i];
	}
	rotated_tex = orthogonally_rotate_tex(rotated_tex, num_rots-1, w, h);
	return &rotated_tex[0];
}

void insert_tex(uint32_t* base, int base_w, int base_h, uint32_t* tex, int tex_w, int tex_h, int x, int y)
{
	for(int j = 0; j < tex_h; j++)
	{
		for(int i = 0; i < tex_w; i++)
		{
			int base_x = i + x * tex_w, base_y = j*base_w + y*base_w*tex_h;
			base[base_x+base_y] = tex[i+j*tex_w];
		}
	}
}

int norm(int val)
{
	if(val<0)
		return 360+val;
	return val;
}

#endif
