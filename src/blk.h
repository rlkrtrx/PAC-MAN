#ifndef __BLK__
#define __BLK__
typedef struct
{
  GLuint vao;
  float* col;
}blk;
blk Blk()
{
  blk b;
  b.vao = make_quad(TLSZ, TLSZ, 1, 1);
  b.col = (float*)malloc(4*sizeof(float));
  b.col[0] = 0.0f;
  b.col[1] = 0.0f;
  b.col[2] = 0.0f;
  b.col[3] = 1.0f;
  return b;
}
void render_blk(GLuint prg, float x, float y, blk b, mat4x4 ortho)
{
  mat4x4 mdl;
  mat4x4_identity(mdl);
  mat4x4_translate(mdl, x, y, 0.0f);
  glUseProgram(prg);
  glBindVertexArray(b.vao);
  glUniformMatrix4fv(glGetUniformLocation(prg, "ortho"), 1, GL_FALSE, &ortho[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(prg, "model"), 1, GL_FALSE, &mdl[0][0]);
  glUniform4fv(glGetUniformLocation(prg, "inCol"), 1, &b.col[0]);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
} 
#endif
