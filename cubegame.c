#include <GL/freeglut.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <SOIL/SOIL.h> // Using SOIL for texture loading

#define WORLD_SIZE_X 100
#define WORLD_SIZE_Y 20
#define WORLD_SIZE_Z 100

#define CUBE_SIZE 5.0

int world[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z] = {0};

int selectedBlockX = WORLD_SIZE_X/2;
int selectedBlockY = 6;
int selectedBlockZ = WORLD_SIZE_Z/2;
bool blockSelected = true;

// Function to generate and populate the world
void generateWorld() {
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int y = 0; y < WORLD_SIZE_Y; y++) {
            for (int z = 0; z < WORLD_SIZE_Z; z++) {
                // Generate a random block type (replace with your block types)
                if (y < WORLD_SIZE_Y/3-2) { world[x][y][z] = 2; }
                else if (y < WORLD_SIZE_Y/3-1) { world[x][y][z] = 1; }
                else if (y < WORLD_SIZE_Y/3) { world[x][y][z] = 4; }
                else if (y < WORLD_SIZE_Y/3+1 && rand() % 20 == 1) { world[x][y][z] = 3; }
            }
        }
    }
}

GLuint textureID1;
GLuint textureID2;
GLuint textureID3;
GLuint textureID4;

// Function to initialize OpenGL
void init() {
    srand(time(NULL));
    glClearColor(0.4, 0.6, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-50, 50, -50, 50, -1000, 1000); // Increased render distance
    glMatrixMode(GL_MODELVIEW);
}

void selectTexture(int index){
    if (index % 4 == 0) {
        glBindTexture(GL_TEXTURE_2D, textureID1);
    } else if (index % 4 == 1) {
        glBindTexture(GL_TEXTURE_2D, textureID2);
    } else if (index % 4 == 2) {
        glBindTexture(GL_TEXTURE_2D, textureID3);
    } else if (index % 4 == 2) {
        glBindTexture(GL_TEXTURE_2D, textureID4);
    } else {
        glBindTexture(GL_TEXTURE_2D, textureID4);
    }
}

// Function to render a single textured cube with alternating textures
void drawTexturedCube(float x, float y, float z, int index) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_TEXTURE_2D);
    selectTexture(index);

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

void drawBillboardFace(float x, float y, float z, int index) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_TEXTURE_2D);
    selectTexture(index);

    glBegin(GL_QUADS);
    // Calculate texture coordinates for the billboarded face
    glNormal3f(0.0, 1.0, 0.0);
    
    // Define texture coordinates to create a perspective effect from the top-right corner
    glTexCoord2f(0, 0);
    glVertex3f(-CUBE_SIZE / 2, -CUBE_SIZE / 2, 2);  // Bottom-left corner
    glTexCoord2f(1, 0);
    glVertex3f(CUBE_SIZE / 2, -CUBE_SIZE / 2, -CUBE_SIZE+2);   // Bottom-right corner
    glTexCoord2f(1, 1);
    glVertex3f(CUBE_SIZE / 2, CUBE_SIZE / 2, -CUBE_SIZE+2);    // Top-right corner
    glTexCoord2f(0, 1);
    glVertex3f(-CUBE_SIZE / 2, CUBE_SIZE / 2, 2);   // Top-left corner
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


// Function to render the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
              
    // Set the light position
    GLfloat lightPosition[] = {0.5, 1.0, 0.5, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    // Set the ambient color of the light
    GLfloat ambientColor[] = {0.4, 0.4, 0.4, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    
    // Set the diffuse color of the light
    GLfloat diffuseColor[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    
    // Set the camera position and orientation
    gluLookAt(30, 30, 30, 0, 0, 0, 0, 1, 0); // Increased camera distance

    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int y = 0; y < WORLD_SIZE_Y; y++) {
            for (int z = 0; z < WORLD_SIZE_Z; z++) {
                int blockType = world[x][y][z];
                float cubeX = x * CUBE_SIZE - selectedBlockX * CUBE_SIZE;
                float cubeY = y * CUBE_SIZE - selectedBlockY * CUBE_SIZE;
                float cubeZ = z * CUBE_SIZE - selectedBlockZ * CUBE_SIZE;
                
                if (blockType != 0 && blockType != 3) {
                    drawTexturedCube(cubeX, cubeY, cubeZ, blockType - 1);
                } else if (blockType == 3) {
                    drawBillboardFace(cubeX, cubeY, cubeZ, blockType - 1);
                }
            }
        }
    }
    
    // Disable lighting after rendering cubes
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    
    // Draw a wireframe cube at the selected block's position
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glLineWidth(2.0);
    //glTranslatef(selectedBlockX * CUBE_SIZE - WORLD_SIZE_X/2, selectedBlockY * CUBE_SIZE - WORLD_SIZE_Y/2, selectedBlockZ * CUBE_SIZE - WORLD_SIZE_Z/2);
    glutWireCube(CUBE_SIZE);
    glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
        case 'w': // Move forward
            if (selectedBlockZ > 0) {
                selectedBlockZ--;
                
            }
            break;
        case 's': // Move backward
            if (selectedBlockZ < WORLD_SIZE_Z - 1) {
                selectedBlockZ++;
            }
            break;
        case 'a': // Move left
            if (selectedBlockX > 0) {
                selectedBlockX--;
            }
            break;
        case 'd': // Move right
            if (selectedBlockX < WORLD_SIZE_X - 1) {
                selectedBlockX++;
            }
            break;
        case 'e': // Move up
            if (selectedBlockY < WORLD_SIZE_Y - 1) {
                selectedBlockY++;
            }
            break;
        case 'q': // Move down
            if (selectedBlockY > 0) {
                selectedBlockY--;
            }
            break;
        case 'p': // Place or modify a block at the selected position
            int currentBlockType = world[selectedBlockX][selectedBlockY][selectedBlockZ];
            currentBlockType = (currentBlockType + 1) % 5;
            world[selectedBlockX][selectedBlockY][selectedBlockZ] = currentBlockType;
            break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("cubegame");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha blending
    init();
    
    generateWorld();

    // Load the textures
    textureID1 = SOIL_load_OGL_texture(
        "texture1.png", // Replace with your texture file name
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    textureID2 = SOIL_load_OGL_texture(
        "texture2.png", // Replace with your texture file name
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    textureID3 = SOIL_load_OGL_texture(
        "texture3.png", // Replace with your texture file name
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    textureID4 = SOIL_load_OGL_texture(
        "texture4.png", // Replace with your texture file name
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    glBindTexture(GL_TEXTURE_2D, textureID1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, textureID2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, textureID3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, textureID4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Check if texture loading was successful
    if (textureID1 == 0 || textureID2 == 0 || textureID3 == 0 || textureID4 == 0) {
        printf("Texture loading failed.\n");
        return 1;
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

