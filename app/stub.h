vector3d* zpoint_lookup(char *name);
matrix3d* zmatrix_lookup(char *name);

int zcolor_alias(int index, char *name); 
int zpoint_alias(char *name, vector3d *point); 
int zmatrix_alias(char *name, matrix3d *matrix); 
int zcolor_modify(int index, char *name, int inverse_index, bool adjusted,
          int p_r, int p_g, int p_b, int p_a,
          int d_r, int d_g, int d_b, int d_a);
int edge_color(int c);
void translate_color(int c, ZCOLOR *zcp, ZCOLOR *zcs);
void render_line(vector3d *vp1, vector3d *vp2, int c);
void render_triangle(vector3d *vp1, vector3d *vp2, vector3d *vp3, int c);
int above_line(vector3d *vp1, vector3d *vp2, vector3d *vp3);
void render_quad(vector3d *vp1, vector3d *vp2, vector3d *vp3, vector3d *vp4, int c);
void render_five(vector3d *vp1, vector3d *vp2, vector3d *vp3, vector3d *vp4, int c);

int init_zimage(ZIMAGE *zp, int rows, int cols);
void zAddTriangle(ZIMAGE *zp, ZPOINT *p1, ZPOINT *p2, ZPOINT *p3, ZCOLOR *zcp, ZCOLOR *zcs);
void zAddLine(ZIMAGE *zp, ZPOINT *pc1, ZPOINT *pc2, ZCOLOR *zcp, ZCOLOR *zcs, int replace);
