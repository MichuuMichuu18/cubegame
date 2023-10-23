#include <GL/freeglut.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <SOIL/SOIL.h>

#define WORLD_SIZE_X 100
#define WORLD_SIZE_Y 20
#define WORLD_SIZE_Z 100
#define CUBE_SIZE 5.0

#define VIEW_DISTANCE 200

int world[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z] = {0};
int selectedBlockX = WORLD_SIZE_X / 2;
int selectedBlockY = 10;
int selectedBlockZ = WORLD_SIZE_Z / 2;
bool blockSelected = true;

GLuint gametexture[6]; // Array to store textures

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y, int seed)
{
    int tmp = hash[(y + seed) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y, int seed)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int, seed);
    int t = noise2(x_int+1, y_int, seed);
    int u = noise2(x_int, y_int+1, seed);
    int v = noise2(x_int+1, y_int+1, seed);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth, int seed)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya, seed) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

void generateWorld() {
    int seed = rand();
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int y = 0; y < WORLD_SIZE_Y; y++) {
            for (int z = 0; z < WORLD_SIZE_Z; z++) {
                int noise = (int)(perlin2d(x, z, 0.1, 2, seed)*12);
                if (y < WORLD_SIZE_Y / 5 - 2 + noise) { world[x][y][z] = 2; }
                else if (y < WORLD_SIZE_Y / 5 - 1 + noise) { world[x][y][z] = 1; }
                else if (y < WORLD_SIZE_Y / 5 + noise) {
                  if (y > 7) { world[x][y][z] = 4; }
                  else { world[x][y][z] = 5; }
                }
                else if (y > 8 && y < WORLD_SIZE_Y / 5 + 1 + noise && rand() % 20 == 1) { world[x][y][z] = 3; }
                
                if (y < 8 && world[x][y][z] == 0) world[x][y][z] = 6;
            }
        }
    }
}

void init() {
    srand(time(NULL));
    glClearColor(0.4, 0.6, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-50, 50, -50, 50, -1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    
}

void selectTexture(int index) {
    glBindTexture(GL_TEXTURE_2D, gametexture[index % 6]);
}

void drawTexturedCube(float x, float y, float z, int blockType) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_TEXTURE_2D);
    selectTexture(blockType);

    glBegin(GL_QUADS);

    // Define texture coordinates for each face of the cube
    // Bottom face
    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Top face
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Front face
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    // Back face
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Left face
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    // Right face
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(0, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawBillboardFace(float x, float y, float z, int blockType) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_TEXTURE_2D);
    selectTexture(blockType);

    glBegin(GL_QUADS);

    glNormal3f(0.0, 1.0, 0.0);

    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, 2);
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE + 2);
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE + 2);
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, 2);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = {0.5, 1.0, 0.5, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat ambientColor[] = {0.4, 0.4, 0.4, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);

    GLfloat diffuseColor[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

    gluLookAt(30, 30, 30, 0, 0, 0, 0, 1, 0);

    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int y = 0; y < WORLD_SIZE_Y; y++) {
            for (int z = 0; z < WORLD_SIZE_Z; z++) {
                int blockType = world[x][y][z];
                float cubeX = x * CUBE_SIZE - selectedBlockX * CUBE_SIZE;
                float cubeY = y * CUBE_SIZE - selectedBlockY * CUBE_SIZE;
                float cubeZ = z * CUBE_SIZE - selectedBlockZ * CUBE_SIZE;
                
                if (
                (cubeX < VIEW_DISTANCE && cubeX > -VIEW_DISTANCE) &&
                (cubeY < VIEW_DISTANCE && cubeY > -VIEW_DISTANCE) &&
                (cubeZ < VIEW_DISTANCE && cubeZ > -VIEW_DISTANCE)
                ){
                if (blockType != 0 && blockType != 3) {
                    drawTexturedCube(cubeX, cubeY, cubeZ, blockType - 1);
                } else if (blockType == 3) {
                    drawBillboardFace(cubeX, cubeY, cubeZ, blockType - 1);
                }
                }
            }
        }
    }
    
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glLineWidth(2.0);
    glutWireCube(CUBE_SIZE);
    glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
        case 'w':
            if (selectedBlockZ > 0) {
                selectedBlockZ--;
            }
            break;
        case 's':
            if (selectedBlockZ < WORLD_SIZE_Z - 1) {
                selectedBlockZ++;
            }
            break;
        case 'a':
            if (selectedBlockX > 0) {
                selectedBlockX--;
            }
            break;
        case 'd':
            if (selectedBlockX < WORLD_SIZE_X - 1) {
                selectedBlockX++;
            }
            break;
        case 'e':
            if (selectedBlockY < WORLD_SIZE_Y - 1) {
                selectedBlockY++;
            }
            break;
        case 'q':
            if (selectedBlockY > 0) {
                selectedBlockY--;
            }
            break;
        case 'p':
            int currentBlockType = world[selectedBlockX][selectedBlockY][selectedBlockZ];
            currentBlockType = (currentBlockType + 1) % 7;
            world[selectedBlockX][selectedBlockY][selectedBlockZ] = currentBlockType;
            break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutCreateWindow("cubegame");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    init();
    
    generateWorld();

    gametexture[0] = SOIL_load_OGL_texture("texture1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    gametexture[1] = SOIL_load_OGL_texture("texture2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    gametexture[2] = SOIL_load_OGL_texture("texture3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    gametexture[3] = SOIL_load_OGL_texture("texture4.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    gametexture[4] = SOIL_load_OGL_texture("texture5.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    gametexture[5] = SOIL_load_OGL_texture("texture6.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

    for (int i = 0; i < 6; i++) {
        glBindTexture(GL_TEXTURE_2D, gametexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (gametexture[i] == 0) { printf("ERROR: Texture loading failed.\n"); return 1; }
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

