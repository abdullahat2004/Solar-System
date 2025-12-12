#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>

float earthRevolutionAngle = 0;
float earthRotationAngle = 0;
float revolutionSpeed = 0.009f;

const float SUN_RADIUS = 1.7f;
const float EARTH_RADIUS = 0.5f;
const float EARTH_ORBIT_RADIUS = 6.0f;

float moonRevolutionAngle = 0;
const float MOON_ORBIT_RADIUS = 1.8f;
const float MOON_RADIUS       = 0.2f;

float camAngleY = 0.0f;
float camAngleX = 20.0f;
float camDistance = 16.0f;

const int STAR_COUNT = 700;
float stars[STAR_COUNT][3];

bool autoEclipseMode = false; 
bool autoLunarMode   = false; 
bool paused          = false; 

void generateStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i][0] = (rand() % 400 - 200) / 10.0f;
        stars[i][1] = (rand() % 400 - 200) / 10.0f;
        stars[i][2] = (rand() % 400 - 200) / 10.0f;
    }
}

void drawStars() {
    glDisable(GL_DEPTH_TEST);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < STAR_COUNT; i++) {
        glColor3f(1, 1, 1);
        glVertex3f(stars[i][0], stars[i][1], stars[i][2]);
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);
}

void initGL() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    generateStars();
}

void drawOrbit(float R) {
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 200; i++) {
        float a = 2 * 3.1416f * i / 200;
        glVertex3f(R * cos(a), 0, R * sin(a));
    }
    glEnd();
}

void drawSun() {
    glColor3f(1.0f, 0.85f, 0.2f);
    glutSolidSphere(SUN_RADIUS, 40, 40);
}

void drawEarth(bool solarEclipse) {
    glPushMatrix();
    glRotatef(earthRotationAngle, 0, 1, 0);

    int slices = 36, stacks = 18;
    for (int i = 0; i < stacks; i++) {
        float lat0 = 3.14159f * (-0.5f + (float)i / stacks);
        float lat1 = 3.14159f * (-0.5f + (float)(i + 1) / stacks);
        float z0 = sin(lat0), zr0 = cos(lat0);
        float z1 = sin(lat1), zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2 * 3.14159f * j / slices;
            float x = cos(lng), y = sin(lng);

            if (x > 0) glColor3f(0.1f, 0.3f, 0.8f);
            else       glColor3f(0.0f, 0.6f, 0.2f);

            glVertex3f(EARTH_RADIUS * x * zr0, EARTH_RADIUS * z0, EARTH_RADIUS * y * zr0);
            glVertex3f(EARTH_RADIUS * x * zr1, EARTH_RADIUS * z1, EARTH_RADIUS * y * zr1);
        }
        glEnd();
    }

    if (solarEclipse) {
        glColor3f(0.05f, 0.05f, 0.05f);
        glPushMatrix();
        glTranslatef(0, EARTH_RADIUS * 1.0f, 0);
        glutSolidSphere(0.25f, 100, 20);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawMoonOrbit() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 120; i++) {
        float a = 2 * 3.1416f * i / 120;
        glVertex3f(MOON_ORBIT_RADIUS * cos(a), 0, MOON_ORBIT_RADIUS * sin(a));
    }
    glEnd();
}

void drawMoon(bool lunarEclipse) {
    glPushMatrix();

    float rad = moonRevolutionAngle * 3.14159f / 180;
    float mx = MOON_ORBIT_RADIUS * cos(rad);
    float mz = MOON_ORBIT_RADIUS * sin(rad);

    glTranslatef(mx, 0, mz);

    if (lunarEclipse)
        glColor3f(0.15f, 0.15f, 0.15f);
    else
        glColor3f(0.75f, 0.75f, 0.75f);

    glutSolidSphere(MOON_RADIUS, 28, 28);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radY = camAngleY * 3.1416f / 180;
    float radX = camAngleX * 3.1416f / 180;
    float camX = camDistance * cos(radX) * sin(radY);
    float camY = camDistance * sin(radX);
    float camZ = camDistance * cos(radX) * cos(radY);
    gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

    drawStars();

    bool lunarEclipse = false;
    bool solarEclipse = false;

    float diff = fabs(earthRevolutionAngle - moonRevolutionAngle);
    if (diff > 180) diff = 360 - diff;

    if (diff < 8)            lunarEclipse = true;
    if (fabs(diff - 180) < 8) solarEclipse = true;

    drawSun();
    drawOrbit(EARTH_ORBIT_RADIUS);

    float rad = earthRevolutionAngle * 3.1416f / 180;
    float ex = EARTH_ORBIT_RADIUS * cos(rad);
    float ez = EARTH_ORBIT_RADIUS * sin(rad);

    glPushMatrix();
    glTranslatef(ex, 0, ez);
    drawEarth(solarEclipse);
    drawMoonOrbit();
    drawMoon(lunarEclipse);
    glPopMatrix();
    glutSwapBuffers();
}


void idleFunc() {

    if (paused) {
        glutPostRedisplay();
        return;
    }

    earthRevolutionAngle += revolutionSpeed;
    if (earthRevolutionAngle >= 360) earthRevolutionAngle -= 360;

    earthRotationAngle += 1.0f;
    if (earthRotationAngle >= 360) earthRotationAngle -= 360;

    moonRevolutionAngle += revolutionSpeed * 1.5f;
    if (moonRevolutionAngle >= 360) moonRevolutionAngle -= 360;

    float diff = fabs(earthRevolutionAngle - moonRevolutionAngle);
    if (diff > 180) diff = 360 - diff;

    if (autoLunarMode && diff < 6) {
        paused = true;
        autoLunarMode = false;
    }

    if (autoEclipseMode && fabs(diff - 180) < 6) {
        paused = true;
        autoEclipseMode = false;
    }

    glutPostRedisplay();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1, 200);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int, int) {
    if (key == GLUT_KEY_LEFT)  camAngleY -= 3;
    if (key == GLUT_KEY_RIGHT) camAngleY += 3;
    if (key == GLUT_KEY_UP)    camAngleX += 3;
    if (key == GLUT_KEY_DOWN)  camAngleX -= 3;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {

    switch(key)
    {
    case '+': camDistance -= 1; break;
    case '-': camDistance += 1; break;

    case 'g':
    case 'G':
        revolutionSpeed = 1.5f;
        paused = false;
        autoEclipseMode = true;
        autoLunarMode = false;
        break;

    case 'h':
    case 'H':
        revolutionSpeed = 1.5f;
        paused = false;
        autoLunarMode = true;
        autoEclipseMode = false;
        break;

    case 'j':
    case 'J':
        revolutionSpeed = 0.009f;
        paused = false;
        autoEclipseMode = false;
        autoLunarMode = false;
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Solar System + Eclipse + Stars + Camera");

    initGL();
    glutDisplayFunc(display);
    glutIdleFunc(idleFunc);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
