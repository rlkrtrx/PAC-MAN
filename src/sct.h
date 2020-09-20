#ifndef __SCT__
#define __SCT__

typedef struct
{
  GLFWwindow* w;
  GLuint pacPrg, mapPrg, txtPrg;
  spr* pacman;
  ghost* ghosts;
  mat4x4 ortho;
  float *c, *blue, dt, lt, time; // Color, Delta-Time and Last-Time
  map lvl;
  blk b;
  int total, ctotal, *box_map, *pac_map, mode, last_switch, half;
  struct tb high_score_box, player1_box, player2_box, points_box;
  struct font* f;
  struct stack_node* stack_head;
  struct g_node *graph;
  struct g_node **g_map;
  struct g_node_list* g_list;
}sct;


sct Sct(void)
{
  sct s;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  #endif
  s.w = glfwCreateWindow((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
  glfwMakeContextCurrent(s.w);

  if(gl3wInit()) { printf("Failed to initialize OpenGL!\n"); }
  // INITIALIZE PROGRAM VARIABLES AND SUCH...	
  s.c = (float*)malloc(4 * sizeof(float));
  s.c[0] = 0.0f;
  s.c[1] = 0.0f;
  s.c[2] = 0.0f;
  s.c[3] = 1.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  srand(time(NULL));
  float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
  mat4x4_ortho(s.ortho, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.0f, 100.0f);
  s.blue = (float*)malloc(3*sizeof(float));
  s.blue[0] = 0.0f;
  s.blue[1] = 0.0f;
  s.blue[2] = 1.0f;
  s.dt = 0.0f;
  s.lt = 0.0f;
  s.total = 0;
  s.ctotal = 0;
  s.b = Blk();
  s.pacPrg = make_prg(make_shdr("shaders/pacVert.glsl", GL_VERTEX_SHADER), make_shdr("shaders/pacFrag.glsl", GL_FRAGMENT_SHADER));
  s.mapPrg = make_prg(make_shdr("shaders/tileVert.glsl", GL_VERTEX_SHADER), make_shdr("shaders/pacFrag.glsl", GL_FRAGMENT_SHADER));
  s.txtPrg = make_prg(make_shdr("shaders/txtVert.glsl", GL_VERTEX_SHADER), make_shdr("shaders/txtFrag.glsl", GL_FRAGMENT_SHADER));
  s.pacman = Spr("img/pacman.png", WINDOW_WIDTH/2-TLSZ/2, TLSZ*23, TLSZ, TLSZ, 1.0f, 1.0f, 0.0f, 3.0f, 1.0f);
  s.lvl = Map("img/lvl.bmp");
  s.ghosts = init_ghosts(9 * TLSZ, 11 * TLSZ);
  s.pac_map = (int*)malloc(s.lvl.w*s.lvl.h*sizeof(int));
  s.box_map = (int*)malloc(s.lvl.w*s.lvl.h*sizeof(int));
  s.f = init_font_list("img/pac-font.bmp");
  s.high_score_box = init_tb(50, 0, "HIGH SCORE", strlen("HIGH SCORE"), 1, s.f, 2);
  s.player1_box = init_tb(50, 0, "1UP", strlen("1UP"), 1, s.f, TEXT_SF);
  s.player2_box = init_tb(WINDOW_WIDTH-strlen("2UP")*TEXT_SF*8-50, 0, "2UP", strlen("2UP"), 1, s.f, TEXT_SF);
  s.points_box = init_tb(s.player1_box.w/2+s.player1_box.x-strlen("0"), TLSZ, "0", strlen("0"), 1, s.f, TEXT_SF);
  s.g_map = (struct g_node**)malloc(s.lvl.w * s.lvl.h * sizeof(struct g_node*));
  s.g_list = (struct g_node_list*)malloc(sizeof(struct g_node_list));
  s.g_list->next_node = NULL;
  s.graph = ret_map_graph(s.lvl.tileMap, s.g_map, s.lvl.w, s.lvl.h);
  graph_to_list(s.g_list, s.g_map, s.lvl.w, s.lvl.h);
  s.stack_head = (struct stack_node*)malloc(sizeof(struct stack_node));
  s.stack_head->gnode_ptr = NULL;
  s.stack_head->next = NULL;
  printf("Best Direction: ");
  print_direction(shortest_path(s.g_map, 18, 11, 26, 1, s.lvl.w, s.lvl.h, s.stack_head));
  printf("\n");
  //print_direction(shortest_path(s.g_map, 21, 5, 26, 1, s.lvl.w, s.lvl.h, s.stack_head));
  printf("\n");
  if(s.g_map[18+5*s.lvl.w]/*->neighbours[2]*/)
    printf("Exists!\n");
  else
    printf("Exist!\n");
  center_tb(&s.high_score_box, WINDOW_WIDTH);
  return s;	
}

void get_in(sct* s)
{
  if(glfwGetKey(s->w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(s->w, GLFW_TRUE);
  if(glfwGetKey(s->w, GLFW_KEY_W) == GLFW_PRESS)
    move_spr(s->pacman, UP);
  else if(glfwGetKey(s->w, GLFW_KEY_D) == GLFW_PRESS)
    move_spr(s->pacman, RIGHT);
  else if(glfwGetKey(s->w, GLFW_KEY_S) == GLFW_PRESS)
    move_spr(s->pacman, DOWN);
  else if(glfwGetKey(s->w, GLFW_KEY_A) == GLFW_PRESS)
    move_spr(s->pacman, LEFT);
}

void run(sct* s)
{
  while(!glfwWindowShouldClose(s->w))
  {	
    glClearBufferfv(GL_COLOR, 0, s->c);
    s->dt = glfwGetTime() - s->lt;
    s->lt = glfwGetTime();
    get_in(s);
    update_pacman_spr(s->pacman, s->ghosts, s->dt, s->lvl, &s->ctotal);
    //update_ghosts(s->ghosts,s->pac_map, s->box_map, s->g_map, s->g_list, s->stack_head, s->dt, s->lvl);
		render_tex_quad(s->pacPrg, s->lvl.map_vao, s->lvl.map_tex, 0, TLSZ, s->ortho);
    render_coins(s->lvl, s->ortho, s->pacPrg);
    render_moveable_spr(s->pacman, s->ghosts, s->pacPrg, s->ortho);
    render_text_box(s->txtPrg, s->high_score_box, s->ortho);
    render_text_box(s->txtPrg, s->player1_box, s->ortho);
    render_text_box(s->txtPrg, s->player2_box, s->ortho);
    render_text_box(s->txtPrg, s->points_box, s->ortho);
    glfwSwapBuffers(s->w);
    glfwPollEvents();
  }
}

void destroy(sct* s)
{
  free(s->box_map);
  free(s->lvl.tileMap);
  free(s->pac_map);
}

#endif
