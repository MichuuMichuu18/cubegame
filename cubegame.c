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
#define ANIM_SPEED 0.02
#define ANIM_STRENGTH 0.5

int world[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z] = {-1};
int selectedBlockX = WORLD_SIZE_X / 2;
int selectedBlockY = 10;
int selectedBlockZ = WORLD_SIZE_Z / 2;
bool blockSelected = true;

float anim = 0.0;

GLuint textureAtlas;

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
                int noise = (int)(perlin2d(x, z, 0.1, 2, seed) * 12);

                if (y < WORLD_SIZE_Y / 5 - 2 + noise) {
                    world[x][y][z] = 6; //stone
                } else if (y < WORLD_SIZE_Y / 5 - 1 + noise) {
                    world[x][y][z] = 1; //dirt
                } else if (y < WORLD_SIZE_Y / 5 + noise) {
                    if (y > 7) {
                        world[x][y][z] = 5; //grass
                    } else {
                        world[x][y][z] = 4; //sand
                    }
                } else if (y > 8 && y < WORLD_SIZE_Y / 5 + 1 + noise && rand() % 20 == 1) {
                    world[x][y][z] = 7; //flower
                } else if (y < 8 && world[x][y][z] == 0) {
                    world[x][y][z] = 3; //water
                }
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

void calculateTextureCoordinates(int blockType, float* texCoords) {
    int atlasSize = 4; // Assuming a 4x4 atlas
    int row = (blockType - 1) / atlasSize; // Calculate row from the adjusted division
    int col = (blockType - 1) % atlasSize; // Calculate column from the adjusted remainder

    float tileSize = 1.0 / atlasSize;
    float startU = col * tileSize;
    float startV = (atlasSize - 1 - row) * tileSize; // Adjusting for the correct order

    texCoords[0] = startU;
    texCoords[1] = startV;
    texCoords[2] = startU + tileSize;
    texCoords[3] = startV;
    texCoords[4] = startU + tileSize;
    texCoords[5] = startV + tileSize;
    texCoords[6] = startU;
    texCoords[7] = startV + tileSize;
}

void drawTexturedCube(float x, float y, float z, int blockType) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glBegin(GL_QUADS);

    // Define texture coordinates for each face of the cube
    // Bottom face
    glNormal3f(0.0, -1.0, 0.0);
    float texCoords[8];
    calculateTextureCoordinates(blockType, texCoords);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Top face
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Front face
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    // Back face
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);

    // Left face
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    // Right face
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE / 2);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE / 2);

    glEnd();

    glPopMatrix();
}

void drawBillboardFace(float x, float y, float z, int blockType) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glBegin(GL_QUADS);

    glNormal3f(0.0, 1.0, 0.0);
    float texCoords[8];
    calculateTextureCoordinates(blockType, texCoords);
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, 2.5);
    glTexCoord2f(texCoords[2], texCoords[3]);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE + 2.5);
    glTexCoord2f(texCoords[4], texCoords[5]);
    glVertex3f(CUBE_SIZE / 2+cos(anim+x*0.3)*ANIM_STRENGTH, CUBE_SIZE / 2, -CUBE_SIZE + 2.5 +sin(anim+x*0.3)*ANIM_STRENGTH);
    glTexCoord2f(texCoords[6], texCoords[7]);
    glVertex3f(-CUBE_SIZE / 2+sin(anim+z*0.3)*ANIM_STRENGTH, CUBE_SIZE / 2, 2.5 +cos(anim+z*0.3)*ANIM_STRENGTH);

    glEnd();

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = {0.5, 1.0, 0.5, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat ambientColor[] = {0.4, 0.5, 0.6, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);

    GLfloat diffuseColor[] = {1.0, 0.9, 0.8, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

    gluLookAt(1, 1, 1, -1, -1, -1, 0, 1, 0);

    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int y = 0; y < WORLD_SIZE_Y; y++) {
            for (int z = 0; z < WORLD_SIZE_Z; z++) {
                int blockType = world[x][y][z];
                float cubeX = x * CUBE_SIZE - selectedBlockX * CUBE_SIZE;
                float cubeY = y * CUBE_SIZE - selectedBlockY * CUBE_SIZE;
                float cubeZ = z * CUBE_SIZE - selectedBlockZ * CUBE_SIZE;
                
                if ((cubeX < VIEW_DISTANCE && cubeX > -VIEW_DISTANCE) &&
                    (cubeY < VIEW_DISTANCE && cubeY > -VIEW_DISTANCE) &&
                    (cubeZ < VIEW_DISTANCE && cubeZ > -VIEW_DISTANCE)){
                    glBindTexture(GL_TEXTURE_2D, textureAtlas);
                    glEnable(GL_TEXTURE_2D);
                    if (blockType != 0 && blockType != 7) {
                        drawTexturedCube(cubeX, cubeY, cubeZ, blockType);
                    } else if (blockType == 7) {
                        drawBillboardFace(cubeX, cubeY, cubeZ, blockType);
                    }
                    glDisable(GL_TEXTURE_2D);
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
            currentBlockType = (currentBlockType + 1) % 8;
            world[selectedBlockX][selectedBlockY][selectedBlockZ] = currentBlockType;
            break;
    }

    glutPostRedisplay();
}

void updateAnimation() {
    anim += ANIM_SPEED;
    if (anim > 3.0) anim = -anim;
    glutPostRedisplay();
    glutTimerFunc(16, updateAnimation, 0); // Call every 16 milliseconds (about 60 frames per second)
}

int main(int argc, char** argv) {
    generateWorld();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(700, 700);
    glutCreateWindow("cubegame");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glDisable(GL_MULTISAMPLE);
    init();
    
    textureAtlas = SOIL_load_OGL_texture("textures.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (textureAtlas == 0) { printf("ERROR: Texture loading failed.\n"); return 1; }

    glutDisplayFunc(display);
    glutTimerFunc(0, updateAnimation, 0);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

