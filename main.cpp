#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#pragma comment(lib, "irrKlang.lib") // Esto vincula automáticamente la biblioteca irrKlang
#include "irr/irrKlang.h"

using namespace irrklang;
using namespace std;

ISoundEngine* soundEngine;
bool isSound = false;
#define M_PI 3.14159265358979323846
GLuint buildingTexture;

// Texturas con BitmapLoader
double tolerance = 0.015;
double lightPos[3] = { 50,35,1.95 };

// Colores para modo de iluminación
GLfloat verde[] = { 0.2, 0.9, 0.2, 1.0 }; // Color azul para el cuerpo

// Prototipos
void restartGame();
float randomInRange(float min, float max);

// Variables globales para la posición de la luz y del sol
GLfloat light_position[] = { 0.0, 8.0, 0.0, 10.0 };
GLfloat sun_position[] = { 150.0, 10.0, 0.0 };

// Variables para el movimiento del cubo
float movement_speed = 0.1; // Velocidad de movimiento del cubo
float rotation_speed = 2.0; // Velocidad de rotación del cubo

// Posición y orientación del auto
GLfloat car_position[] = { 0.0, 0.6, 0.0 }; // Posición inicial del auto
GLfloat car_rotation = 0.0; // Rotación inicial del auto
GLfloat car_velocity[3] = { 0.0, 0.0, 0.1 }; // Velocidad inicial en el eje z

string tankHealthBar = ""; // Salud inicial del tanque jugador
int tankHealth = 100; // Salud inicial del tanque jugador
float tankSpeed = 0.1f; // Velocidad inicial del tanque
int municionTanque = 100;
string ammoBar = "";
bool ammo = false;

// Variables para el cronómetro
int startTime = 0;
int currentTime = 0;
int elapsedTime = 0;
int pauseTime = 0; // Tiempo en el que se pausó el temporizador
int duration = 110; // Duración del temporizador en segundos (ejemplo: 1 minuto)
bool timerActive = true; // Variable para controlar si el temporizador está activo o detenido
bool timerActiveInPause = true; // Detenemos el temporizador

float rotTorus = 0;
float velAnimTorus = 2;

float rotLegMonster = 45.0;
float velAnimLeg = 5.0;

float scaleMouthMonster = 1.0;
float velAnimMouth = 0.1;

float rotTankMonster = 360;
float velAnimTank = 0.02;

float translateTankMonster = 0.1;
float velAnimTankTranslate = 0.01;

bool isStaticCamera = true; // Indica si la cámara estática está habilitada
GLfloat camera_distance = 20.0; // Distancia de la cámara al cubo
GLfloat camera_rotation = 0.0; // Rotación de la cámara

bool movingForward = false;
bool movingBackward = false;
bool rotatingLeft = false;
bool rotatingRight = false;
bool shiftPressed = false; // Variable para controlar si la tecla Shift está presionada


int coins = 0; // Contador de enemigos eliminados
int enemiesEliminated = 0; // Contador de enemigos eliminados
bool gameover = false;
bool gamewin = false;
bool loadScreen = true; // Variable para indicar si el juego está en pausa 
bool paused = false; // Variable para indicar si el juego está en pausa 
bool damage = false;
bool health = false;
bool timerRunning = false;
bool timerRunningSpeed = false;
bool timerRunningInstaKill = false;
bool timerRunningKillAll = false;
bool timerRunningMaxAmmo = false;
bool timerRunningDoublePoints = false;
int timerDuration = 1000; // Duración del temporizador en milisegundos (1 segundo)
bool upgradeFlag = false;

bool speed = false;
bool instaKill = false;
bool doublePoints = false;
bool killAllEnemys = false;

float cameraX = 10, cameraY = 10, cameraZ = 10;

bool shooting = false;

// Estructura para representar una bala
struct Bullet {
    float x;
    float y;
    float z;
    float startX;
    float startZ;
    float angle;
    bool active;
    float distanceTravelled;
    bool fromPlayer; // indica si la bala proviene del tanque del jugador
};

// Lista de balas
vector<Bullet> bullets;

// Estructura para representar un objetivo
struct Target {
    float x;
    float z;
    int health;
    float angle;
    bool shooting;
};

vector<Target> targets;

// Estructura para representar un punto de restauración de vida
struct Healer {
    float x;
    float z;
    int healthRestore;
};

vector<Healer> healerpoints;

// Estructura para representar un punto de restauración de munición
struct Municion {
    float x;
    float z;
    int municion;
};

vector<Municion> municiones;

// Estructura para representar un punto de mejora
struct Upgrade {
    float x;
    float z;
    int cost;
    string typeUpgrade;
    int timeUpgrade;
    int health;
    bool active;
};

vector<Upgrade> upgrades;

// Estructura para representar el foco
struct Faro {
    float x;
    float z;
    float lightX;
    float lightZ;
    bool state;
};

struct Building {
    float x;
    float y;
    GLuint textureID;
};

struct Tree {
    float x;
    float y;
};
struct Car {
    float x;
    float y;
    float angle;
};
// Lista de edificios
vector<Building> buildings = {
    { -30.0, 30.0 }, { -30.0, 50.0 }, { -30.0, 70.0 },
    { -30.0, 85.0 }, { -30.0, 100.0 }, { -30.0, 115.0 }, { -30.0, 130.0 },
    { -30.0, 145.0 }, { -30.0, 160.0 }, { -30.0, 175.0 }, { -30.0, 190.0 },

    { -30.0, -30.0 }, { -30.0, -50.0 }, { -30.0, -70.0 },
    { -30.0, -85.0 }, { -30.0, -100.0 }, { -30.0, -115.0 }, { -30.0, -130.0 },
    { -30.0, -145.0 }, { -30.0, -160.0 }, { -30.0, -175.0 }, { -30.0,-190.0 },

    { 30.0, 30.0 }, { 30.0, 50.0 }, { 30.0, 70.0 },
    { 30.0, 85.0 }, { 30.0, 100.0 }, { 30.0, 115.0 }, { 30.0, 130.0 },
    { 30.0, 145.0 }, { 30.0, 160.0 }, { 30.0, 175.0 }, { 30.0,190.0 },

    { 30.0, -30.0 }, { 30.0, -50.0 }, { 30.0, -70.0 },
    { 30.0, -85.0 }, { 30.0, -100.0 }, { 30.0, -115.0 }, { 30.0, -130.0 },
    { 30.0, -145.0 }, { 30.0, -160.0 }, { 30.0, -175.0 }, { 30.0, -190.0 },

    { -40.0, 30.0 }, { -55.0, 30.0 }, { -70.0, 30.0 },
    { -85.0, 30.0 }, { -100.0, 30.0 }, { -115.0, 30.0 }, { -130.0, 30.0 },
    { -145.0, 30.0 }, { -160.0, 30.0 }, { -175.0, 30.0 }, { -190.0, 30.0 },

    { -40.0, -30.0 }, { -55.0, -30.0 }, { -70.0, -30.0 },
    { -85.0, -30.0 }, { -100.0, -30.0 }, { -115.0, -30.0 }, { -130.0, -30.0 },
    { -145.0, -30.0 }, { -160.0, -30.0 }, { -175.0, -30.0 }, { -190.0, -30.0 },

    { 40.0, 30.0 }, { 55.0, 30.0 }, { 70.0, 30.0 },
    { 85.0, 30.0 }, { 100.0, 30.0 }, { 130.0, 30.0 }, { 130.0, 30.0 },
    { 145.0, 30.0 }, { 160.0, 30.0 }, { 175.0, 30.0 }, { 190.0, 30.0 },

    { 40.0, -30.0 }, { 55.0, -30.0 }, { 70.0, -30.0 },
    { 85.0, -30.0 }, { 100.0, -30.0 }, { 130.0, -30.0 }, { 130.0, -30.0 },
    { 145.0, -30.0 }, { 160.0, -30.0 }, { 175.0, -30.0 }, { 190.0, -30.0 },
};
vector<Tree> trees = {
    { -15.0, 8.0 },    { -15.0, 28.0 },
    { -15.0, 48.0 },   { -15.0, 68.0 },
    { -15.0, 88.0 },   { -15.0, 108.0 },
    { -15.0, 128.0 },  { -15.0, 148.0 },
    { -15.0, 168.0 },  { -15.0, 188.0 },

    { -15.0, -8.0 },  { -15.0, -28.0 },
    { -15.0, -48.0 },  {-15.0, -68.0 },
    { -15.0, -88.0 }, { -15.0, -108.0 },
    { -15.0, -128.0 },   { -15.0, -148.0 },
    { -15.0, -168.0 },  { -15.0, -188.0 },

    { 15.0, 8.0 }, { 15.0, 28.0 },
    { 15.0, 48.0 }, { 15.0, 68.0 },
    { 15.0, 88.0 },{ 15.0, 108.0 },
    { 15.0, 128.0 },  { 15.0, 148.0 },
    { 15.0, 168.0 },  { 15.0, 188.0 },

    { 15.0, -8.0 }, { 15.0, -28.0 },
    { 15.0, -48.0 }, { 15.0, -68.0 },
    { 15.0, -88.0 }, { 15.0, -108.0 },
    { 15.0, -128.0 },  { 15.0, -148.0 },
    { 15.0, -168.0 },  { 15.0, -188.0 },

    {28.0, -10.0 },
    {48.0, -10.0 },   {68.0, -10.0 },
    { 88.0, -10.0 },   {108.0, -10.0 },
    {128.0, -10.0 },  {148.0, -10.0 },
    {168.0, -10.0 },  {188.0, -10.0 },

    {-28.0, -10.0 },
    {-48.0, -10.0 },   {-68.0, -10.0 },
    {-88.0, -10.0 },   {-108.0, -10.0 },
    {-128.0, -10.0 },  {-148.0, -10.0 },
    {-168.0, -10.0 },  {-188.0, -10.0 },

    { 28.0, 10.0 },
    { 48.0, 10.0 },   { 68.0, 10.0 },
    { 88.0, 10.0 },   { 108.0, 10.0 },
    { 128.0, 10.0 },  { 148.0, 10.0 },
    { 168.0, 10.0 },  { 188.0, 10.0 },

    { -28.0, 10.0 },
    { -48.0, 10.0 },   { -68.0, 10.0 },
    { -88.0, 10.0 },   { -108.0, 10.0 },
    { -128.0, 10.0 },  { -148.0, 10.0 },
    { -168.0, 10.0 },  { -188.0, 10.0 },
};
vector<Faro> faros = {
    { -7.0, 8.0 },    { -7.0, 28.0 },
    { -7.0, 48.0 },   { -7.0, 68.0 },
    { -7.0, 88.0 },   { -7.0, 108.0 },
    { -7.0, 128.0 },  { -7.0, 148.0 },
    { -7.0, 168.0 },  { -7.0, 188.0 },

    { -7.0, -8.0 },   { -7.0, -28.0 },
    { -7.0, -48.0 },  { -7.0, -68.0 },
    { -7.0, -88.0 },  { -7.0, -108.0 },
    { -7.0, -128.0 }, { -7.0, -148.0 },
    { -7.0, -168.0 }, { -7.0, -188.0 },

    { 7.0, 8.0 },   { 7.0, 28.0 },
    { 7.0, 48.0 },  { 7.0, 68.0 },
    { 7.0, 88.0 },  { 7.0, 108.0 },
    { 7.0, 128.0 }, { 7.0, 148.0 },
    { 7.0, 168.0 }, { 7.0, 188.0 },

    { 7.0, -8.0 },    { 7.0, -28.0 },
    { 7.0, -48.0 },   { 7.0, -68.0 },
    { 7.0, -88.0 },   { 7.0, -108.0 },
    { 7.0, -128.0 },  { 7.0, -148.0 },
    { 7.0, -168.0 },  { 7.0, -188.0 },

    {18.0, -7.0 },   {38.0, -7.0 },
    {48.0, -7.0 },   {68.0, -7.0 },
    {88.0, -7.0 },   {108.0, -7.0 },
    {128.0, -7.0 },  {148.0, -7.0 },
    {168.0, -7.0 },  {188.0, -7.0 },

    {-18.0, -7.0 },   {-38.0, -7.0 },
    {-48.0, -7.0 },   {-68.0, -7.0 },
    {-88.0, -7.0 },   {-108.0, -7.0 },
    {-128.0, -7.0 },  {-148.0, -7.0 },
    {-168.0, -7.0 },  {-188.0, -7.0 },

    { 18.0, 7.0 },  { 38.0, 7.0 },
    { 48.0, 7.0 },  { 68.0, 7.0 },
    { 88.0, 7.0 },  { 108.0, 7.0 },
    { 128.0, 7.0 }, { 148.0, 7.0 },
    { 168.0, 7.0 }, { 188.0, 7.0 },

    { -18.0, 7.0 },   { -38.0, 7.0 },
    { -48.0, 7.0 },   { -68.0, 7.0 },
    { -88.0, 7.0 },   { -108.0, 7.0 },
    { -128.0, 7.0 },  { -148.0, 7.0 },
    { -168.0, 7.0 },  { -188.0, 7.0 },
};
vector<Car> cars = {
    { -4.5, 8.0, 90.0 },{ -4.5, 48.0, 90.0 }, { -4.5, 108.0, 90.0 },{ -4.5, 148.0, 90.0 },

    { -4.5, -18.0, 90.0 }, { -4.5, -78.0, 90.0 }, { -4.5, -88.0, 90.0 }, { -4.5, -188.0, 90.0 },

    { 4.5, 8.0, 90.0 }, { 4.5, 58.0, 90.0 }, { 4.5, 118.0, 90.0 }, { 4.5, 158.0, 90.0 },

    { 4.5, -28.0, 90.0 }, { 4.5, -38.0, 90.0 }, { 4.5, -128.0, 90.0 }, { 4.5, -168.0, 90.0 },

    {8.0 , -4.5},{48.0 , -4.5}, {108.0 , -4.5},{148.0 , -4.5},

    {-18.0 , -4.5}, {-78.0 , -4.5}, {-88.0 , -4.5}, {-188.0 , -4.5},

    {8.0, 4.5}, {58.0, 4.5}, {118.0, 4.5}, {158.0, 4.5},

    {-28.0, 4.5}, {-38.0, 4.5}, {-128.0, 4.5}, {-168.0, 4.5},
};

// Estructura para almacenar datos del modelo
struct Mesh {
    vector<GLfloat> vertices;
    vector<GLuint> indices;
};

vector<Mesh> meshes;
// Estructura para almacenar datos del modelo
struct Model {
    vector<GLfloat> vertices;
    vector<GLuint> indices;
    GLfloat posX, posY, posZ;
};

vector<Model> models;

void loadBuildingTexture(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(image); // Liberar memoria después de cargar la textura
    }
    else {
        cerr << "Error al cargar la textura." << endl;
    }
}

// Función para cargar un modelo OBJ utilizando Assimp
void loadOBJ(const std::string& filePath, GLfloat posX, GLfloat posY, GLfloat posZ) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar el modelo: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Recorrer todos los nodos de la escena y extraer los datos del modelo
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        Model model;
        const aiMesh* aiMesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j) {
            model.vertices.push_back(aiMesh->mVertices[j].x);
            model.vertices.push_back(aiMesh->mVertices[j].y);
            model.vertices.push_back(aiMesh->mVertices[j].z);
        }

        for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
            const aiFace& face = aiMesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                model.indices.push_back(face.mIndices[k]);
            }
        }

        model.posX = posX;
        model.posY = posY;
        model.posZ = posZ;

        models.push_back(model);
    }
}

// Función para cargar un modelo OBJ utilizando Assimp
void loadOBJTank(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar el modelo: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Recorrer todos los nodos de la escena y extraer los datos del modelo
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        Mesh mesh;
        const aiMesh* aiMesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j) {
            mesh.vertices.push_back(aiMesh->mVertices[j].x);
            mesh.vertices.push_back(aiMesh->mVertices[j].y);
            mesh.vertices.push_back(aiMesh->mVertices[j].z);
        }


        for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
            const aiFace& face = aiMesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                mesh.indices.push_back(face.mIndices[k]);
            }
        }

        meshes.push_back(mesh);
    }
}

void drawObjectsBuilding() {
    GLfloat building_color[] = { 0.8, 0.4, 0.2, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, building_color);
    loadBuildingTexture("assets/images/carretera-3.jpg");
    glEnable(GL_TEXTURE_2D);
    for (const auto& model : models) {

        glPushMatrix();
        glTranslatef(model.posX, model.posY, model.posZ);
        glScalef(0.6, 1.0, 0.37);
        //glRotatef(90, 0, 1, 0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, model.vertices.data() + 3);
        glVertexPointer(3, GL_FLOAT, 0, model.vertices.data());
        glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, model.indices.data());

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
}

void drawObjectTank() {

    glRotatef(90, 0, 1, 0);
    // Renderizar cada malla del modelo
    for (const auto& mesh : meshes) {

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY); // Habilitar arreglo de normales
        glNormalPointer(GL_FLOAT, 0, mesh.vertices.data() + 3); // Apuntar a las normales en los datos de vértices

        glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());

        glDisableClientState(GL_NORMAL_ARRAY); // Deshabilitar arreglo de normales
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

// Función para dibujar figuras
void drawCustomGround(float width, float depth, float yPos) {
    // Dibujar el suelo
    glBegin(GL_QUADS);
    glVertex3f(-width, yPos, -depth);
    glVertex3f(-width, yPos, depth);
    glVertex3f(width, yPos, depth);
    glVertex3f(width, yPos, -depth);
    glEnd();
}

// Función para dibujar figuras
void drawCustomWalls() {
    // Entorno
    loadBuildingTexture("assets/images/wall.jpg");
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    // Cara frontal
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, -200.0);
    glTexCoord2f(5.0, 0.0); glVertex3f(200.0, 0.0, -200.0);
    glTexCoord2f(5.0, 1.0); glVertex3f(200.0, 35.0, -200.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 35.0, -200.0);
    // Cara trasera
    glTexCoord2f(0.0, 0.0); glVertex3f(200.0, 35.0, 200.0);
    glTexCoord2f(5.0, 0.0); glVertex3f(-200.0, 35.0, 200.0);
    glTexCoord2f(5.0, 1.0); glVertex3f(-200.0, 0.0, 200.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(200.0, 0.0, 200.0);

    // Cara lateral derecha
    glTexCoord2f(0.0, 0.0); glVertex3f(200.0, 35.0, -200.0);
    glTexCoord2f(5.0, 0.0); glVertex3f(200.0, 35.0, 200.0);
    glTexCoord2f(5.0, 1.0); glVertex3f(200.0, 0.0, 200.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(200.0, 0.0, -200.0);

    // Cara lateral izquierda
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 35.0, 200.0);
    glTexCoord2f(5.0, 0.0); glVertex3f(-200.0, 35.0, -200.0);
    glTexCoord2f(5.0, 1.0); glVertex3f(-200.0, 0.0, -200.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 0.0, 200.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Función de inicialización
void init() {
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearColor(0.01f, 0.01f, 0.1f, 1.0f);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    loadOBJTank("assets/models/tank.obj");

    for (const auto& building : buildings) {
        //drawBuilding(building.x, building.y, 8.0, 20.0);
        loadOBJ("assets/models/city3.obj", building.x, 0.0, building.y);
    }

    soundEngine = createIrrKlangDevice();
    // Verificar si el motor de sonido se ha inicializado correctamente
    if (!soundEngine) {
        cerr << "Error al inicializar el motor de sonido irrKlang." << endl;
    }

}

// Función para cargar y reproducir la música
void playBackgroundMusic() {
    // Cargar la música desde un archivo
    ISound* music = soundEngine->play2D("assets/audio/cyberpunk.mp3", true, false, true);

    // Verificar si la carga fue exitosa
    if (!music) {
        cerr << "Error al cargar la música de fondo." << endl;
    }
}

void playEffectsSound(int effect) {
    switch (effect) {
    case 1:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music1 = soundEngine->play2D("assets/audio/enemy-die-1.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music1) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 2:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/heal-restore-2.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 3:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/disparo-2.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 4:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/choque.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 5:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/instakill.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 6:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/killall.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 7:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/double.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 8:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/speed.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 9:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/error.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 10:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/damage.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 11:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/moving.mp3", true, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 12:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/reload.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 13:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/empty-ammo.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 14:
        isSound = true;
        if (isSound && !gameover && !loadScreen && !gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/max-ammo.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 15:
        isSound = true;
        if (isSound && !gameover && !loadScreen && gamewin)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/dead-song-cod.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    case 16:
        isSound = true;
        if (isSound && !gameover && !loadScreen)
        {
            ISound* music2 = soundEngine->play2D("assets/audio/explosion.mp3", false, false, true);
            isSound = false;
            // Verificar si la carga fue exitosa
            if (!music2) {
                cerr << "Error al cargar la música de fondo." << endl;
            }
        }
        break;
    default:
        break;
    }
}

void ejes() {
    //eje x
    // glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(-100.0, 0.0, 0.0);
    glVertex3f(100.0, 0.0, 0.0);
    glEnd();
    //eje y
    // glColor3f(0.0, 100.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, -100.0, 0.0);
    glVertex3f(0.0, 100.0, 0.0);
    glEnd();
    //eje z
    // glColor3f(0 - 0, 0.0, 100.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -100.0);
    glVertex3f(0.0, 0.0, 100.0);
    glEnd();
}

void drawBullets() {
    for (auto& bullet : bullets) {
        if (bullet.active) {
            glPushMatrix();
            GLfloat color_bullet[] = { 0.8, 0.5, 0.2 };
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_bullet);
            glColor3f(0.8, 0.5, 0.2);
            glTranslatef(bullet.x, 2.4, bullet.z);
            glutSolidSphere(0.15f, 20, 20); // Dibujar esfera como bala
            glPopMatrix();
        }
    }
}

void drawText(const std::string& text, float x, float y) {
    glMatrixMode(GL_PROJECTION);
    double* matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();

    // Ajustar las coordenadas de dibujo del texto en función del tamaño de la ventana
    float windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    float windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float textScale = 0.001f * windowWidth; // Escalar el texto según el tamaño de la ventana
    float adjustedX = x * windowWidth / 760.0f; // 760 es el ancho original de la ventana
    float adjustedY = y * windowHeight / 600.0f; // 600 es la altura original de la ventana
    glRasterPos2f(adjustedX, adjustedY);

    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
    delete[] matrix;
}

// Función para generar un número aleatorio en un rango dado
float randomInRange(float min, float max) {
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

// Función para generar un número aleatorio en un rango dado
void generateRandomTarget() {
    float x = randomInRange(-150.0f, 150.0f);
    float z = randomInRange(-150.0f, 150.0f);
    int health = 5; // salud de y 5
    float angle = randomInRange(0.0f, 360.0f);
    bool shooting = false;
    if (targets.size() < 80) {
        targets.push_back({ x, z, health, angle, shooting });
    }

}
// Mover Solo
void legAnimation(int valor) {
    if (!paused) {
        glutTimerFunc(16, legAnimation, 0);
    }
    rotLegMonster += velAnimLeg;
    if (rotLegMonster > 45 || rotLegMonster < -45) {
        velAnimLeg = -velAnimLeg;
    }
    glutPostRedisplay();
}
void mouthAnimation(int valor) {
    if (!paused) {
        glutTimerFunc(16, mouthAnimation, 0);
    }
    scaleMouthMonster += velAnimMouth;
    if (scaleMouthMonster > 1 || scaleMouthMonster < -1) {
        velAnimMouth = -velAnimMouth;
    }
    glutPostRedisplay();
}
void donaAnimation(int valor) {
    if (!paused) {
        glutTimerFunc(16, donaAnimation, 0);
    }
    rotTorus += velAnimTorus;
    if (rotTorus > 360 || rotTorus < -360) {
        velAnimTorus = -velAnimTorus;
    }
    glutPostRedisplay();
}
void drawTargets() {
    for (const auto& target : targets) {
        glPushMatrix();
        glTranslatef(target.x, 1.5f, target.z);

        GLfloat color_target[] = { 0.05, 0.05, 0.05, 1.0 };
        //GLfloat color_target[] = { 0.29, 0.15, 0.34, 1.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_target);

        glPushMatrix();
        glTranslatef(0, 2.0f, 0);
        glScalef(1.0, 1.0, 1.0);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 3.25f, 0);
        glScalef(1.0, 1.0, 1.0);
        glutSolidSphere(1.2, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 4.25f, 0);
        glScalef(1.0, 1.0, 1.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.5, 1.0, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0.0f, 0);
        glScalef(1.0, 0.8, 1.0);
        glutSolidCube(2.0f);
        glPopMatrix();


        //GLfloat color_head[] = { 0.32, 0.08, 0.08 ,1.0 };
        //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_head);
        // Tentáculos
        glPushMatrix();
        glTranslatef(1.75, -0.5f, 1.75);
        glRotatef(45, 0, 1, 0);
        glScalef(0.5, 0.5, 2.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.75, -0.5f, -1.75);
        glRotatef(45, 0, 1, 0);
        glScalef(0.5, 0.5, 2.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.75, -0.5f, -1.75);
        glRotatef(-45, 0, 1, 0);
        glScalef(0.5, 0.5, 2.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.75, -0.5f, 1.75);
        glRotatef(-45, 0, 1, 0);
        glScalef(0.5, 0.5, 2.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Tentáculos patas
        glPushMatrix();
        glTranslatef(-2.75, -0.75f, 2.75);
        glRotatef(-rotLegMonster, 1, 1, 1);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.75, -0.75f, 2.75);
        glRotatef(rotLegMonster, 1, 1, 1);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.75, -0.75f, -2.75);
        glRotatef(rotLegMonster, 1, 1, 1);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-2.75, -0.75f, -2.75);
        glRotatef(-rotLegMonster, 1, 1, 1);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1.0f);
        glPopMatrix();


        // Ojos circulares negros

        glPushMatrix();
        glTranslatef(0.0, 4.0f, 1.0);
        glScalef(0.5, 0.5, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 4.0f, -1.0);
        glScalef(0.5, 0.5, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.0, 4.0f, 0.0);
        glRotatef(90, 0, 1, 0);
        glScalef(0.5, 0.5, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.0, 4.0f, 0.0);
        glRotatef(90, 0, 1, 0);
        glScalef(0.5, 0.5, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        /*GLfloat color_eyes_square[] = { 1.0, 0.56, 0.22 ,1.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_eyes_square);*/
        GLfloat color_mouths[] = { 0.2, 0.1, 0.1 ,1.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_mouths);
        // Ojos
        glPushMatrix();
        glTranslatef(0.5, 2.5f, 1.5);
        glScalef(1.0, 0.2, 0.1);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.5, 2.5f, -1.5);
        glScalef(1.0, 0.2, 0.1);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.5, 2.5f, 1.5);
        glScalef(1.0, 0.2, 0.1);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.5, 2.5f, -1.5);
        glScalef(1.0, 0.2, 0.1);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.5, 2.5f, 0.5);
        glScalef(0.1, 0.2, 1.0);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.5, 2.5f, 0.5);
        glScalef(0.1, 0.2, 1.0);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.5, 2.5f, -0.5);
        glScalef(0.1, 0.2, 1.0);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.5, 2.5f, -0.5);
        glScalef(0.1, 0.2, 1.0);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Bocas
        glPushMatrix();
        glTranslatef(0.0, 1.5f, 1.5);
        glScalef(0.8, scaleMouthMonster, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 1.5f, -1.5);
        glScalef(0.8, scaleMouthMonster, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.5, 1.5f, 0.0);
        glRotatef(90, 0, 1, 0);
        glScalef(0.8, scaleMouthMonster, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.5, 1.5f, 0.0);
        glRotatef(90, 0, 1, 0);
        glScalef(0.8, scaleMouthMonster, 0.5);
        glutSolidTorus(0.2, 0.8, 10, 10);
        glPopMatrix();

        glPopMatrix();
    }
}

void generateRandomUpgrade() {
    float x = randomInRange(-150.0f, 150.0f);
    float z = randomInRange(-150.0f, 150.0f);
    int costUpgrade = 50;
    int timeUpgrade = 15000;
    vector<string> typeUp = { "speed", "instakill", "doublepoints", "killall" };

    // Obtener un índice aleatorio dentro del rango de índices del array
    int randomIndex = rand() % typeUp.size();

    // Usar el valor aleatorio
    string randomTypeUp = typeUp[randomIndex];

    if (upgrades.size() < 80) {
        // Usar el valor aleatorio
        upgrades.push_back({ x, z, costUpgrade, randomTypeUp, timeUpgrade, 1, false });
    }
}

void drawUpgrade() {
    for (const auto& upgrade : upgrades) {
        glPushMatrix();
        glTranslatef(upgrade.x, 2.0f, upgrade.z);
        glRotatef(rotTorus, 0, 1, 0);
        GLfloat color_upgrade[] = { 0.0, 0.0, 1.0, 0.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_upgrade);
        glColor3f(0.0, 0.0, 1.0);

        glPushMatrix();
        glRotatef(45, 0, 0, 1);
        glTranslatef(1.0, 0.1f, 0.0);
        glColor3f(0.0, 0.5, 0.0);
        glScalef(0.2, 0.75, 0.2);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glRotatef(45, 0, 0, 1);
        glTranslatef(0.1, 1.0f, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glScalef(0.75, 0.2, 0.2);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.5f, 0.0);
        glScalef(1.5, 1.5, 1.5);
        glutSolidTorus(0.2, 1.2, 20, 20);
        glPopMatrix();

        glPopMatrix();
    }
}

void generateRandomHealer() {
    float x = randomInRange(-150.0f, 150.0f);
    float z = randomInRange(-150.0f, 150.0f);
    //int health = rand() % 30 + 10; // salud aleatoria entre 10 y 30
    int health = 20; // salud de 20
    if (healerpoints.size() < 25) {
        healerpoints.push_back({ x, z, health });
    }
}

void drawHealer() {
    for (const auto& healer : healerpoints) {
        glPushMatrix();
        glTranslatef(healer.x, 2.0f, healer.z);
        glRotatef(rotTorus, 0, 1, 0);
        GLfloat color_healer[] = { 0.0, 1.0, 0.0, 0.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_healer);

        glPushMatrix();
        glTranslatef(0.0, 0.5f, 0.0);
        glColor3f(0.0, 0.5, 0.0);
        glScalef(0.2, 0.75, 0.2);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.5f, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glScalef(0.75, 0.2, 0.2);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.5f, 0.0);
        glScalef(1.5, 1.5, 1.5);
        glutSolidTorus(0.2, 1.2, 20, 20);
        glPopMatrix();

        glPopMatrix();
    }
}

void generateRandomMunicion() {
    float x = randomInRange(-180.0f, 180.0f);
    float z = randomInRange(-180.0f, 180.0f);
    int municion = 100; // salud aleatoria entre 10 y 30
    if (municiones.size() < 25) {
        municiones.push_back({ x, z, municion });
    }
}

void drawMunicion() {
    for (const auto& municion : municiones) {
        glPushMatrix();
        glTranslatef(municion.x, 2.0f, municion.z);
        glRotatef(rotTorus, 0, 1, 0);
        GLfloat color_municion[] = { 0.05, 0.1, 0.05, 0.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_municion);
        glColor3f(0.05, 0.1, 0.05);

        glPushMatrix();
        glRotatef(45, 0, 0, 1);
        glTranslatef(0.1f, 1.0f, 0.0);
        glColor3f(0.0, 0.5, 0.0);
        glScalef(0.1, 0.5, 0.1);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glRotatef(45, 0, 0, 1);
        glTranslatef(1.0, 0.1f, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glScalef(0.5, 0.1, 0.1);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.0, 0.25f, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glScalef(0.1, 0.75, 0.1);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.0, 0.25f, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glScalef(0.1, 0.75, 0.1);
        glutSolidCube(3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.5f, 0.0);
        glScalef(1.5, 1.5, 1.5);
        glutSolidTorus(0.2, 1.2, 20, 20);
        glPopMatrix();

        glPopMatrix();
    }
}

void drawBuilding(float x, float z, float width, float height, float depth) {
    // Edificio
    glEnable(GL_TEXTURE_2D);
    //loadBuildingTexture("assets/images/ventanas.jpg");
    //glBindTexture(GL_TEXTURE_2D, buildingTexture);
    GLfloat color_building[] = { 0.8, 0.8, 0.8, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_building);
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(x, 10.0, z); // Posición del edificio
    glScalef(width, height, depth); // Dimensiones del edificio

    glBegin(GL_QUADS);
    // Cara frontal
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // Cara trasera
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

    // Cara izquierda
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);

    // Cara derecha
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // Cara superior
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // Cara inferior
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawBuildings() {
    // Dibujar varios edificios en diferentes posiciones y 
   /* if (buildings.size() < 104) {
        float x = randomInRange(30.0f, 200.0f);
        float z = randomInRange(30.0f, 200.0f);
        buildings.push_back({ x, z });
    }*/
    for (const auto& building : buildings) {
        drawBuilding(building.x, building.y, 8.0, 20.0, 8.0);
    }
}

void drawTree(float x, float z) {
    // Tronco del árbol
    GLfloat color_tree[] = { 0.4, 0.2, 0.1, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_tree);
    glPushMatrix();
    glColor3f(0.4, 0.2, 0.1);
    glTranslatef(x, 1.5, z); // Posición del tronco
    glScalef(1.0, 3.0, 1.0); // Tamaño del tronco
    glutSolidCube(1.0);
    glPopMatrix();

    // Parte superior del árbol (hojas)
    GLfloat color_leaves[] = { 0.0, 0.4, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_leaves);
    glPushMatrix();
    glColor3f(0.0, 0.4, 0.0);
    glTranslatef(x, 4.0, z); // Posición de las hojas
    glScalef(0.98, 1.0, 0.73);
    glutSolidSphere(1.37, 20, 20); // Esfera para representar las hojas
    glPopMatrix();

    GLfloat color_leaves2[] = { 0.0, 0.4, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_leaves);
    glPushMatrix();
    glColor3f(0.0, 0.4, 0.0);
    glTranslatef(x, 5.2, z); // Posición de las hojas
    glScalef(0.97, 1.0, 0.68);
    glutSolidSphere(1.15, 20, 20); // Esfera para representar las hojas
    glPopMatrix();

    GLfloat color_leaves3[] = { 0.0, 0.4, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_leaves);
    glPushMatrix();
    glColor3f(0.0, 0.4, 0.0);
    glTranslatef(x, 6.15, z); // Posición de las hojas
    glScalef(1.0, 1.0, 0.65);
    glutSolidSphere(0.9, 20, 20); // Esfera para representar las hojas
    glPopMatrix();
}

void drawTrees() {
    if (trees.size() < 175) {
        float x = randomInRange(30.0f, 200.0f);
        float z = randomInRange(30.0f, 200.0f);
        trees.push_back({ x, z });
    }
    if (trees.size() < 190) {
        float x = randomInRange(-30.0f, -200.0f);
        float z = randomInRange(-30.0f, -200.0f);
        trees.push_back({ x, z });
    }
    if (trees.size() < 205) {
        float x = randomInRange(-30.0f, -200.0f);
        float z = randomInRange(30.0f, 200.0f);
        trees.push_back({ x, z });
    }
    if (trees.size() < 220) {
        float x = randomInRange(30.0f, 200.0f);
        float z = randomInRange(-30.0f, -200.0f);
        trees.push_back({ x, z });
    }
    // Dibujar varios árboles en diferentes posiciones
    // Continuar la secuencia hasta llegar al valor más cercano a 200
    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y);
    }
}

void drawHeadlight(float x, float z) {
    // Poste
    GLfloat color_light[] = { 0.8, 0.8, 0.8, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_light);
    glPushMatrix();
    glColor3f(0.4, 0.2, 0.1);
    glTranslatef(x, 4.0, z);
    glScalef(0.2, 8.0, 0.2);
    glutSolidCube(1.0);
    glPopMatrix();
    // Contender de la luz
    glPushMatrix();
    glColor3f(0.4, 0.2, 0.1);
    glTranslatef(x, 8.1, z);
    glScalef(1.25, 0.2, 1.25);
    glutSolidCube(1.0);
    glPopMatrix();

    // Luz del faro
    //GLfloat light_position[] = { x, 4.0, z, 1.0 }; // Posición de la luz en la parte superior del árbol
    //GLfloat light_color[] = { 1.0, 1.0, 1.0, 1.0 }; // Color amarillo para el faro
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT1); // Usar otra luz para el faro
    //glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);

    //// Luz del faro
    //GLfloat light_position[] = { x, 4.0, z, 1.0 }; // Posición de la luz en la parte superior del árbol
    //GLfloat light_color[] = { 0.1, 0.1, 0.1, 1.0 }; // Color amarillo para el faro
    //glEnable(GL_LIGHTING);

    //// Generar un nuevo índice de luz para cada faro
    //static int lightIndex = GL_LIGHT2;
    //glEnable(lightIndex); // Usar otra luz para el faro
    //glLightfv(lightIndex, GL_POSITION, light_position);
    //glLightfv(lightIndex, GL_DIFFUSE, light_color);

    //// Incrementar el índice de luz para el siguiente faro
    //lightIndex++;
}

void drawHeadlights() {
    /*if (faros.size() < 100) {
        float x = randomInRange(-150.0f, 150.0f);
        float z = randomInRange(-150.0f, 150.0f);
        faros.push_back({ x, z });
    }*/
    // Dibujar varios árboles en diferentes posiciones
    for (const auto& headlight : faros) {
        drawHeadlight(headlight.x, headlight.z);
    }
}

void drawCar(float x, float z, float angle) {
    // Carrito
    // carcasa
    glPushMatrix();
    glTranslatef(x, 0.6, z);
    glRotatef(angle, 0.0, 1.0, 0.0);
    // Cuerpo del automóvil
    //glColor3f(0.1, 0.1, 0.8); // Color azul para el cuerpo
    GLfloat body_color[] = { 0.1, 0.1, 0.8, 1.0 }; // Color azul para el cuerpo
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, body_color);
    glPushMatrix();
    glScalef(2.0, 0.5, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Techo
    glPushMatrix();
    glTranslatef(-0.25, 0.6, 0.0);
    glScalef(1.5, 0.5, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Parabrisas
    // delantero
    GLfloat windshield_color[] = { 0.9, 0.9, 0.9, 1.0 }; // Color blanco para el parabrisas
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, windshield_color);
    glPushMatrix();
    glTranslatef(0.42, 0.55, 0.0);
    glScalef(0.2, 0.4, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();
    // Trasero
    glPushMatrix();
    glTranslatef(-0.97, 0.55, 0.0);
    glScalef(0.1, 0.4, 1.03);
    glutSolidCube(1.0);
    glPopMatrix();

    // Ventanas laterales
    glColor3f(0.9, 0.9, 0.9); // Color blanco para las Ventanas laterales
    glPushMatrix();
    glTranslatef(-0.25, 0.55, 0.48);
    glScalef(1.5, 0.4, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.25, 0.55, -0.48);
    glScalef(1.5, 0.4, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    // Ruedas
    GLfloat wheel_color[] = { 0.1, 0.1, 0.1, 1.0 }; // Color negro para las ruedas
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wheel_color);
    glPushMatrix();
    glTranslatef(-0.7, -0.25, 0.5);
    glutSolidTorus(0.1, 0.2, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.7, -0.25, 0.5);
    glutSolidTorus(0.1, 0.2, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.7, -0.25, -0.5);
    glutSolidTorus(0.1, 0.2, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.7, -0.25, -0.5);
    glutSolidTorus(0.1, 0.2, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

void drawCars() {
    for (const auto& car : cars) {
        drawCar(car.x, car.y, car.angle);
    }
}

void drawSun() {
    // Dibujar el sol
    GLfloat sun_color[] = { 1.0, 1.0, 0.0, 1.0 }; // Color amarillo
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sun_color);
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(sun_position[0], sun_position[1], sun_position[2]);
    // //glColor3f(1.0, 1.0, 0.0); // Color amarillo
    glutSolidSphere(5.0, 20, 20); // Sol como una esfera
    glPopMatrix();
}

void drawTank() {

    glPushMatrix();
    glTranslatef(car_position[0], car_position[1], car_position[2]);
    glRotatef(car_rotation, 0.0, 1.0, 0.0);
    GLfloat tank_color[] = { 0.05, 0.1, 0.05,0.0 }; // Verde oscuro para el cuerpo del tanque
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tank_color);
    drawObjectTank();
    //// Cuerpo del tanque
    //GLfloat tank_color[] = { 0.05, 0.1, 0.05,0.0 }; // Verde oscuro para el cuerpo del tanque
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tank_color);
    //glPushMatrix();
    //glColor3f(0.05, 0.1, 0.05);
    //glTranslatef(0.0, 0.0, 0.0); // Posición del cuerpo del tanque
    //glScalef(3.0, 1.0, 1.5); // Dimensiones del cuerpo del tanque
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Torreta
    //GLfloat turret_color[] = { 0.1, 0.2, 0.1, 0.0 }; // Verde medio para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, turret_color);
    //glPushMatrix();
    //glColor3f(0.1, 0.2, 0.1);
    //glTranslatef(-0.25, 0.75, 0.0); // Posición de la torreta en la parte superior del tanque
    //glScalef(1.75, 0.5, 1.25); // Dimensiones de la torreta
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-0.25, 1.0, 0.0); // Posición de la torreta en la parte superior del tanque
    //glScalef(1.5, 0.25, 1.0); // Dimensiones de la torreta
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Cañón
    //glColor3f(0.0, 0.5, 0.0); // Verde claro para el cañón
    //glPushMatrix();
    //glTranslatef(1.5, 0.7, 0.0); // Posición del cañón en la torreta
    //glScalef(2.2, 0.2, 0.2); // Dimensiones del cañón
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glColor3f(0.0, 0.5, 0.0); // Verde claro para el cañón
    //glPushMatrix();
    //glTranslatef(2.75, 0.7, 0.0); // Posición del cañón en la torreta
    //glScalef(0.6, 0.3, 0.3); // Dimensiones del cañón
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Orugas
    //GLfloat wheel_color[] = { 0.0, 0.0, 0.0, 0.0 }; // Negro para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wheel_color);
    //glPushMatrix();
    //glColor3f(0.0, 0.0, 0.0);
    //// Definir la posición y dimensiones de las orugas
    //// Izquierda
    //glPushMatrix();
    //glTranslatef(0.0, 0.45, -0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, -0.45, -0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.45, 0.0, -0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(1.45, 0.0, -0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();
    //// Derecha
    //glPushMatrix();
    //glTranslatef(0.0, 0.45, 0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(0.0, -0.45, 0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.45, 0.0, 0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(1.45, 0.0, 0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Ruedas
    //GLfloat inner_wheel_color[] = { 0.07, 0.07, 0.07, 0.0 }; // Verde medio para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, inner_wheel_color);
    //glPushMatrix();
    //glColor3f(0.07, 0.07, 0.07);
    //glTranslatef(-1.30, 0.3, 0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(1.30, 0.3, 0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.80, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(-0.80, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.30, 0.3, -0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(1.30, 0.3, -0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.80, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(-0.80, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    glPopMatrix();
}

void drawSuelo(float width, float depth, float yPos) {
    // Carretera
    glEnable(GL_TEXTURE_2D);
    loadBuildingTexture("assets/images/carretera-3.jpg");
    glBegin(GL_QUADS);
    // Cara inferior
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.02, 200.0);
    glTexCoord2f(25.0, 0.0); glVertex3f(200.0, 0.02, 200.0);
    glTexCoord2f(25.0, 10.0); glVertex3f(200.0, 0.02, -200.0);
    glTexCoord2f(0.0, 10.0); glVertex3f(-200.0, 0.02, -200.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void drawSky() {
    // Cielo
    glEnable(GL_TEXTURE_2D);
    loadBuildingTexture("assets/images/clouds-textures-red.jpg");
    glBegin(GL_QUADS);
    GLfloat color_sky[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_sky);
    glColor3f(1.0, 1.0, 1.0); // Color blanco
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 35.0, -200.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(200.0, 35.0, -200.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 35.0, 200.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 35.0, 200.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawGround() {
    GLfloat color_general_ground[] = { 0.6, 0.6, 0.6, 1.0 }; // Color gris para el suelo
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_general_ground);
    glColor3f(0.6, 0.6, 0.6);
    drawCustomWalls();
    drawSuelo(100.0, 100.0, 0.0);
    // Llamar a la función para dibujar un suelo más pequeño en una posición elevada
    GLfloat color_floor[] = { 0.1, 0.1, 0.1, 1.0 }; // Color gris para el suelo
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_floor);
    glColor3f(0.1, 0.1, 0.1);
    drawCustomGround(5.0, 200.0, 0.1);
    drawCustomGround(200.0, 5.0, 0.05);
}

void monsterAnimation(int valor) {
    glutTimerFunc(8, monsterAnimation, 0);
    rotTankMonster += velAnimTank;
    if (rotTankMonster > 360) {
        velAnimTank = -velAnimTank;
    }
    glutPostRedisplay();
}
void tankAnimation(int valor) {
    glutTimerFunc(16, tankAnimation, 0);
    translateTankMonster += velAnimTankTranslate;
    if (translateTankMonster > 0.5 || translateTankMonster < -0.5) {
        velAnimTankTranslate = -velAnimTankTranslate;
    }
    glutPostRedisplay();
}
void drawMonsterLoader() {
    glPushMatrix();
    /*glTranslatef(0, -6, 0);
    glScalef(0.4, 0.4, 0.4);
    glRotatef(rotTankMonster * 0.3, 1.0, 1.0, 0.0);*/

    glTranslatef(7, 5, -4);
    glScalef(1.0, 1.0, 1.0);
    glRotatef(rotTankMonster, 0.0, 1.0, 1.0);

    GLfloat color_target[] = { 0.05, 0.05, 0.05, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_target);

    glPushMatrix();
    glTranslatef(0, 2.0f, 0);
    glScalef(1.0, 1.0, 1.0);
    glutSolidCube(3.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 3.25f, 0);
    glScalef(1.0, 1.0, 1.0);
    glutSolidSphere(1.2, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 4.25f, 0);
    glScalef(1.0, 1.0, 1.0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.5, 1.0, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.0f, 0);
    glScalef(1.0, 0.8, 1.0);
    glutSolidCube(2.0f);
    glPopMatrix();


    //GLfloat color_head[] = { 0.32, 0.08, 0.08 ,1.0 };
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_head);
    // Tentáculos
    glPushMatrix();
    glTranslatef(1.75, -0.5f, 1.75);
    glRotatef(45, 0, 1, 0);
    glScalef(0.5, 0.5, 2.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.75, -0.5f, -1.75);
    glRotatef(45, 0, 1, 0);
    glScalef(0.5, 0.5, 2.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.75, -0.5f, -1.75);
    glRotatef(-45, 0, 1, 0);
    glScalef(0.5, 0.5, 2.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.75, -0.5f, 1.75);
    glRotatef(-45, 0, 1, 0);
    glScalef(0.5, 0.5, 2.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tentáculos patas
    glPushMatrix();
    glTranslatef(-2.75, -0.75f, 2.75);
    glRotatef(-45, 1, 1, 1);
    glScalef(0.5, 1.5, 0.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.75, -0.75f, 2.75);
    glRotatef(45, 1, 1, 1);
    glScalef(0.5, 1.5, 0.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.75, -0.75f, -2.75);
    glRotatef(45, 1, 1, 1);
    glScalef(0.5, 1.5, 0.5);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.75, -0.75f, -2.75);
    glRotatef(-45, 1, 1, 1);
    glScalef(0.5, 1.5, 0.5);
    glutSolidCube(1.0f);
    glPopMatrix();


    // Ojos circulares negros

    glPushMatrix();
    glTranslatef(0.0, 4.0f, 1.0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 4.0f, -1.0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0, 4.0f, 0.0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.0, 4.0f, 0.0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    /*GLfloat color_eyes_square[] = { 1.0, 0.56, 0.22 ,1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_eyes_square);*/
    GLfloat color_mouths[] = { 0.2, 0.1, 0.1 ,1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_mouths);
    // Ojos
    glPushMatrix();
    glTranslatef(0.5, 2.5f, 1.5);
    glScalef(1.0, 0.2, 0.1);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5, 2.5f, -1.5);
    glScalef(1.0, 0.2, 0.1);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5, 2.5f, 1.5);
    glScalef(1.0, 0.2, 0.1);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5, 2.5f, -1.5);
    glScalef(1.0, 0.2, 0.1);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5, 2.5f, 0.5);
    glScalef(0.1, 0.2, 1.0);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.5, 2.5f, 0.5);
    glScalef(0.1, 0.2, 1.0);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5, 2.5f, -0.5);
    glScalef(0.1, 0.2, 1.0);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.5, 2.5f, -0.5);
    glScalef(0.1, 0.2, 1.0);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Bocas

    glPushMatrix();
    glTranslatef(0.0, 1.5f, 1.5);
    glScalef(0.8, 0.8, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 1.5f, -1.5);
    glScalef(0.8, 0.8, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5, 1.5f, 0.0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.8, 0.8, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.5, 1.5f, 0.0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.8, 0.8, 0.5);
    glutSolidTorus(0.2, 0.8, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void drawTankLoader() {

    glPushMatrix();
    /*glTranslatef(0, 6, 0);
    glScalef(0.8, 0.8, 0.8);
    glRotatef(rotTankMonster, 0.0, 1.0, 0.0);*/

    glTranslatef(translateTankMonster - 8, -6, 10.0);
    glScalef(2.5, 2.5, 2.5);
    glRotatef(25, 0.0, 1.0, 1.0);
    GLfloat tank_color[] = { 0.05, 0.1, 0.05,0.0 }; // Verde oscuro para el cuerpo del tanque
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tank_color);
    drawObjectTank();

    //// Cuerpo del tanque
    //GLfloat tank_color[] = { 0.05, 0.1, 0.05,0.0 }; // Verde oscuro para el cuerpo del tanque
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tank_color);
    //glPushMatrix();
    //glColor3f(0.05, 0.1, 0.05);
    //glTranslatef(0.0, 0.0, 0.0); // Posición del cuerpo del tanque
    //glScalef(3.0, 1.0, 1.5); // Dimensiones del cuerpo del tanque
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Torreta
    //GLfloat turret_color[] = { 0.1, 0.2, 0.1, 0.0 }; // Verde medio para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, turret_color);
    //glPushMatrix();
    //glColor3f(0.1, 0.2, 0.1);
    //glTranslatef(-0.25, 0.75, 0.0); // Posición de la torreta en la parte superior del tanque
    //glScalef(1.75, 0.5, 1.25); // Dimensiones de la torreta
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-0.25, 1.0, 0.0); // Posición de la torreta en la parte superior del tanque
    //glScalef(1.5, 0.25, 1.0); // Dimensiones de la torreta
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Cañón
    //glColor3f(0.0, 0.5, 0.0); // Verde claro para el cañón
    //glPushMatrix();
    //glTranslatef(1.5, 0.7, 0.0); // Posición del cañón en la torreta
    //glScalef(2.2, 0.2, 0.2); // Dimensiones del cañón
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glColor3f(0.0, 0.5, 0.0); // Verde claro para el cañón
    //glPushMatrix();
    //glTranslatef(2.75, 0.7, 0.0); // Posición del cañón en la torreta
    //glScalef(0.6, 0.3, 0.3); // Dimensiones del cañón
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Orugas
    //GLfloat wheel_color[] = { 0.0, 0.0, 0.0, 0.0 }; // Negro para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wheel_color);
    //glPushMatrix();
    //glColor3f(0.0, 0.0, 0.0);
    //// Definir la posición y dimensiones de las orugas
    //// Izquierda
    //glPushMatrix();
    //glTranslatef(0.0, 0.45, -0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, -0.45, -0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.45, 0.0, -0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(1.45, 0.0, -0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();
    //// Derecha
    //glPushMatrix();
    //glTranslatef(0.0, 0.45, 0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(0.0, -0.45, 0.85);
    //glScalef(3.0, 0.1, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.45, 0.0, 0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(1.45, 0.0, 0.85);
    //glScalef(0.1, 0.8, 0.25);
    //glutSolidCube(1.0);
    //glPopMatrix();

    //// Ruedas
    //GLfloat inner_wheel_color[] = { 0.07, 0.07, 0.07, 0.0 }; // Verde medio para la torreta
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, inner_wheel_color);
    //glPushMatrix();
    //glColor3f(0.07, 0.07, 0.07);
    //glTranslatef(-1.30, 0.3, 0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(1.30, 0.3, 0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.80, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(-0.80, 0.0, 0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-1.30, 0.3, -0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(1.30, 0.3, -0.90); // Posición de la primera rueda de la derecha
    //glScalef(0.2, 0.2, 0.2); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.5, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.0, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(0.80, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    //glPushMatrix();
    //glTranslatef(-0.80, 0.0, -0.90); // Posición de la tercera rueda de la derecha
    //glScalef(0.8, 0.8, 0.4); // Dimensiones de la rueda
    //glutSolidTorus(0.2, 0.4, 10, 10); // Rueda grande
    //glPopMatrix();
    glPopMatrix();
}

void drawLoadScreen() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Cambiar el color de fondo a rojo
    GLfloat text_color_1[] = { 0.0, 0.3, 0.0, 1.0 }; // Color blanco
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, text_color_1);
    glColor3f(0.6, 0.6, 0.6);
    drawText("         _                    ____         ", 220, 450);
    drawText("        / /\\                   |      |   ", 220, 430);
    drawText("       / /  \\                  |      |   ", 220, 410);
    drawText("      / / /\\ \\                 |      |            (     )           ____________           __________", 220, 390);
    drawText("     / / / \\ \\ \\               |      |             ___          /                        |         /                     \\", 220, 370);
    drawText("    / / /   \\ \\ \\              |      |            |      |        /   ---------   |       /                        \\", 220, 350);
    drawText("   / / /__/  /\\ \\            |      |            |      |      /                           |       |                           |", 220, 330);
    drawText("  / / /___/ /\\ \\ \\          |      |            |      |      |      ___________|      |             |             |", 220, 310);
    drawText(" / /______/\\ \\ \\         |      |            |      |      |     |___________       |             |             |", 220, 290);
    drawText("/ / /_       __\\ \\_\\       |      |            |      |      |                           |       |             |             |", 220, 270);
    drawText("\\_\\___\\  /___/_/       |___|            |___|       \\_____________|       |______ |             /", 220, 250);
    drawText("                                                                                                                   \\_____/", 220, 230);
    drawText("ANNIHILIATION - TANK OPERATION", 300, 210);
    //drawText("|-----------------------------|", 310, 330);
    glPushMatrix();
    GLfloat text_color_2[] = { 0.0, 0.5, 0.0, 1.0 }; // Color blanco
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, text_color_2);
    glScalef(0.1, 0.1, 0.1);
    //drawText("--------------------------------", 290, 120);
    drawText("Presiona [ ENTER ] para comenzar", 307.5, 100);
    //drawText("--------------------------------", 290, 80);
    glPopMatrix();

    //GLfloat text_color_3[] = { 0.0, 0.4, 0.0, 1.0 }; // Color blanco
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, text_color_3);
    //drawText("..../''''''''''''''''''''|=========[]", 260, 530);
    //drawText("/''''''''''''''''''''''''''''''''''''''''''\\", 260, 510);
    //drawText("\\(@) (@) (@) (@) (@)/", 260, 490);

    //drawText("[]=========|''''''''''''''''''''\\....", 420, 530);
    //drawText("/''''''''''''''''''''''''''''''''''''''''''\\", 440, 510);
    //drawText("\\(@) (@) (@) (@) (@)/", 440, 490);
}

void setupLighting() {
    /*static GLfloat angle = 0.0;
    light_position[0] = 15.0 * cos(angle);
    light_position[1] = 15.0 * sin(angle);
    angle += 0.1;*/

    GLfloat light_position[] = { 0.0, 8.0, 0.0, 10.0 }; // Posición de la luz en la parte superior del árbol
    GLfloat light_color[] = { 0.1, 0.1, 0.1, 0.1 };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1); // Usar otra luz para el faro
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void updateCarPosition(float dx) {
    // Convertir el ángulo de rotación a radianes
    float angle_rad = car_rotation * M_PI / 180.0;

    // Calcular los componentes x e z de la dirección del frente del auto
    float front_x = cos(angle_rad);
    float front_z = -sin(angle_rad); // Se utiliza el signo negativo debido a la convención de orientación en OpenGL

    // Actualizar la posición del auto en la dirección del frente
    car_position[0] += dx * front_x;
    car_position[2] += dx * front_z;
}

void updateCarRotation(float dr) {
    // Actualizar la rotación del auto
    car_rotation += dr;
}

void updateBulletPosition() {
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.x += 0.2f * cos(bullet.angle * M_PI / 180.0f);
            bullet.z += 0.2f * -sin(bullet.angle * M_PI / 180.0f);
            bullet.distanceTravelled = sqrt(pow(bullet.x - bullet.startX, 2) + pow(bullet.z - bullet.startZ, 2));
        }
    }
}

//void updateCameraPosition() {
//    // Actualizar la posición de la cámara en función de la posición del cubo
//    GLfloat camera_offset = 10.0; // Distancia desde el cubo a la cámara
//    GLfloat camera_height = 5.0; // Altura de la cámara sobre el cubo
//    GLfloat camera_x = car_position[0] + camera_offset * -cos(car_rotation * M_PI / 180.0);
//    GLfloat camera_y = car_position[1] + camera_height;
//    GLfloat camera_z = car_position[2] + camera_offset * sin(car_rotation * M_PI / 180.0);
//    gluLookAt(camera_x, camera_y, camera_z, car_position[0], car_position[1], car_position[2], 0.0, 1.0, 0.0);
//}
void updateCameraPosition() {
    // Ajustar la posición de la cámara en función de la posición del tanque
    GLfloat camera_offset = 10.0; // Distancia desde el tanque a la cámara
    GLfloat camera_height = 5.0; // Altura de la cámara sobre el tanque

    // Calcular la posición de la cámara detrás del tanque
    GLfloat camera_x = car_position[0] - camera_offset * cos(car_rotation * M_PI / 180.0);
    GLfloat camera_y = car_position[1] + camera_height;
    GLfloat camera_z = car_position[2] + camera_offset * sin(car_rotation * M_PI / 180.0);

    // Calcular la posición a la que la cámara está mirando (puede ajustar este valor según sea necesario)
    GLfloat lookAt_x = car_position[0] + 2.5 * cos(car_rotation * M_PI / 180.0);
    GLfloat lookAt_y = car_position[1] + 2.5;
    GLfloat lookAt_z = car_position[2] + 2.5 * sin(car_rotation * M_PI / 180.0);

    gluLookAt(camera_x, camera_y, camera_z, lookAt_x, lookAt_y, lookAt_z, 0.0, 1.0, 0.0);
}

void infoTextRender() {
    // Mostrar pantalla de Game Over
    glDisable(GL_LIGHTING);
    glColor3f(0.6, 0.6, 0.6);
    drawText("ACCION  ---  [TECLA]", 630, 570);
    drawText("---- ---- ---- ----", 630, 550);
    drawText("Adelante           [ W ]", 630, 530);
    drawText("Reversa            [ S ]", 630, 510);
    drawText("Izquierda          [ A ]", 630, 490);
    drawText("Derecha            [ D ]", 630, 470);
    drawText("Disparar            [ESPACIO]", 630, 450);
    drawText("Velocidad         [SHIFT]", 630, 430);
    drawText("Vista                 [C]", 630, 410);
    drawText("Pausar              [ESC]", 630, 390);
    drawText("Menu                CLK DER", 630, 370);
    glEnable(GL_LIGHTING);
}

void creditsTextRender() {
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    drawText("[        CREDITOS        ]", 630, 220);
    drawText("---- ---- ---- ----", 630, 200);
    drawText("[  Joseph Santamaria  ]", 630, 180);
    drawText("[   Diego Chancusig    ]", 630, 160);
    drawText("[       Joel Cuenca       ]", 630, 140);
    drawText("[        Belen Taco        ]", 630, 120);
    drawText("[       Bryan Gualpa      ]", 630, 100);
    drawText("---- ---- ---- ----", 630, 80);
    glEnable(GL_LIGHTING);
}

void timerCallbackDamage(int value) {
    damage = false; // Establecer damage como false cuando el temporizador expire
    timerRunning = false;
}
void timerCallbackHealth(int value) {
    health = false;
    timerRunning = false;
}
void timerCallbackCoins(int value) {
    upgradeFlag = false;
    timerRunning = false;
}
void timerCallbackSpeed(int value) {
    speed = false;
    timerRunningSpeed = false;
}
void timerCallbackInstaKill(int value) {
    instaKill = false;
    timerRunningInstaKill = false;
}
void timerCallbackKillAll(int value) {
    killAllEnemys = false;
    timerRunningKillAll = false;
}
void timerCallbackMaxAmmo(int value) {
    ammo = false;
    timerRunningMaxAmmo = false;
}
void timerCallbackDoubleCoins(int value) {
    doublePoints = false;
    timerRunningDoublePoints = false;
}
void resetTimer() {
    // Reiniciamos el temporizador
    startTime = glutGet(GLUT_ELAPSED_TIME) - elapsedTime * 1000; // Restauramos el tiempo inicial considerando la pausa
    pauseTime = 0; // Reiniciamos el tiempo de pausa
    timerActive = true; // Activamos el temporizador nuevamente
}

void pauseTimer() {
    // Pausamos el temporizador
    timerActive = false;
    pauseTime = glutGet(GLUT_ELAPSED_TIME);
}

// Función de dibujo
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    int remainingTime = 0;
    if (loadScreen)
    {
        gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Vista desde arriba
        drawMonsterLoader();
        drawTankLoader();
        drawLoadScreen();
        //creditsTextRender();
    }
    else {
        if (paused) {
            glDisable(GL_LIGHTING);
            glColor3f(1.0, 1.0, 1.0);
            glColor3f(1.0, 1.0, 1.0);
            drawText("En pausa", 350, 330);
            creditsTextRender();
            glEnable(GL_LIGHTING);
        }
        if (gameover) {
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Cambiar el color de fondo a rojo
            gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Vista desde arriba
        }
        if (enemiesEliminated == 40) {
            gamewin = true;
            timerActive = true; // Detenemos el temporizador
        }
        if (gamewin)
        {
            glClearColor(0.16, 0.83, 0.52, 1.0f); // Cambiar el color de fondo a verde
            gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Vista desde arriba
            // Mostrar pantalla de Game Over
            glDisable(GL_LIGHTING);
            glColor3f(1.0, 1.0, 1.0);
            glColor3f(0.16, 0.83, 0.52);
            drawText("¡¡¡¡¡¡Felicidades, ganaste!!!!!", 330, 350);
            drawText("Presiona [R] para Reiniciar", 320, 320);
            glEnable(GL_LIGHTING);
        }

        if (isStaticCamera) {
            // Cámara estática
            //gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Vista desde arriba
            gluLookAt(car_position[0], car_position[1] + 10.0f, car_position[2] - 20.0f, car_position[0], car_position[1], car_position[2], 0.0f, 1.0f, 0.0f);
        }
        else {
            updateCameraPosition();
        }

        setupLighting();
        drawGround();
        drawTargets(); // Dibujar objetivos
        drawTrees();
        drawSun();
        drawTank();
        //drawBuildings();
        drawObjectsBuilding();
        drawBullets();
        drawHealer();
        drawMunicion();
        drawUpgrade();
        drawSky();
        drawHeadlights();
        drawCars();
        //ejes();

        if (loadScreen)
        {
            drawLoadScreen();
            creditsTextRender();
        }
        else {
            glDisable(GL_LIGHTING);
            // Mostrar la vida del tanque en la pantalla
            glColor3f(0.16, 0.83, 0.52);
            string healthText = "Vida del tanque  " + tankHealthBar + "  " + to_string(tankHealth);
            drawText(healthText, 30, 570);
            // Mostrar el número de enemigos eliminados en la pantalla
            glColor3f(0.2, 0.3, 0.2);
            string ammoText = "Municiones         " + ammoBar + "  " + to_string(municionTanque);
            drawText(ammoText, 30, 550);
            // Mostrar el número de enemigos eliminados en la pantalla
            glColor3f(0.42, 0.65, 0.82);
            string enemiesText = "Enemigos eliminados  " + to_string(enemiesEliminated);
            drawText(enemiesText, 30, 530);
            // Mostrar el número de enemigos eliminados en la pantalla
            glColor3f(1.0, 0.95, 0.0);
            string coinsText = "Dinero  " + to_string(coins) + " $";
            drawText(coinsText, 30, 510);

            // Mostrar el número de enemigos eliminados en la pantalla
            glColor3f(1.0, 1.0, 1.0);

            // Calculamos el tiempo transcurrido si el temporizador está activo
            if (timerActive && !gameover && !gamewin) {
                elapsedTime = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000; // Convertimos de milisegundos a segundos
            }

            // Si el temporizador está pausado, calculamos el tiempo transcurrido hasta la pausa
            if (!timerActive && pauseTime != 0) {
                elapsedTime = ((pauseTime - startTime) / 1000); // Ajustamos el tiempo transcurrido con la pausa
            }

            remainingTime = duration - elapsedTime;
            // Dibujamos el tiempo restante en la ventana

            string timeString = "Tiempo restante  " + to_string(remainingTime) + "  segundos";

            // Verificamos si el tiempo ha llegado a cero
            if (remainingTime <= 0) {
                timerActive = false; // Detenemos el temporizador
                timerActiveInPause = false;
            }

            drawText(timeString, 30, 490);
            drawText("------------------------------------------", 30, 140);
            drawText("[  Misiones  ]", 30, 120);
            drawText("Elimina a 40 objetivos antes de que se acabe el tiempo", 30, 100);
            drawText("------------------------------------------", 30, 80);
            glEnable(GL_LIGHTING);
        }

        for (auto& upgrade : upgrades) {
            glDisable(GL_LIGHTING);
            glColor3f(0.16, 0.83, 0.52);
            if (upgrade.typeUpgrade == "speed" && speed) {
                string speedText = "Velocidad extra activada [15s]";
                drawText(speedText, 330, 100);
                if (!timerRunningSpeed) {
                    glutTimerFunc(upgrade.timeUpgrade, timerCallbackSpeed, 0); // Iniciar temporizador
                    timerRunningSpeed = true;
                }
            }
            if (upgrade.typeUpgrade == "instakill" && instaKill) {
                string speedText = "Muerte instantanea [15s]";
                drawText(speedText, 350, 120);
                if (!timerRunningInstaKill) {
                    glutTimerFunc(upgrade.timeUpgrade, timerCallbackInstaKill, 0); // Iniciar temporizador
                    timerRunningInstaKill = true;
                }
            }
            if (upgrade.typeUpgrade == "killall" && killAllEnemys) {
                string speedText = "Kaboom, muerte a todos los enemigos.";
                drawText(speedText, 330, 140);
                if (!timerRunningKillAll) {
                    glutTimerFunc(3000, timerCallbackKillAll, 0); // Iniciar temporizador
                    timerRunningKillAll = true;
                }
            }
            if (upgrade.typeUpgrade == "doublepoints" && doublePoints) {
                string speedText = "Puntos dobles x2 [15s]";
                drawText(speedText, 350, 160);
                if (!timerRunningDoublePoints) {
                    glutTimerFunc(upgrade.timeUpgrade, timerCallbackDoubleCoins, 0); // Iniciar temporizador
                    timerRunningDoublePoints = true;
                }
            }
            glEnable(GL_LIGHTING);
        }
        glDisable(GL_LIGHTING);
        if (gameover) {
            if (remainingTime <= 0) {
                // Mostrar pantalla de Game Over
                glColor3f(1.0, 0.0, 0.0);
                drawText("¡Se te acabo el tiempo!", 330, 350);
                drawText("Presiona [R] para Reiniciar", 320, 320);
            }
            else {
                // Mostrar pantalla de Game Over
                glColor3f(1.0, 0.0, 0.0);
                drawText("Moriste", 350, 350);
                drawText("Presiona [R] para Reiniciar", 315, 320);
            }
        }
        if (damage && !gameover) {
            glColor3f(1.0, 1.0, 1.0);
            string TankDamageText = "Perdiendo 1 de vida por milisegundo";
            drawText(TankDamageText, 350, 530);
            if (!timerRunning) {
                glutTimerFunc(timerDuration, timerCallbackDamage, 0); // Iniciar temporizador
                timerRunning = true;
            }
        }
        if (health && !gameover) {
            glColor3f(0.0, 1.0, 0.0);
            for (auto& healer : healerpoints) {
                // Verificar colisión de objetivo con tanque jugador
                string healthRestoreText = "+ " + to_string(healer.healthRestore) + " de salud";
                drawText(healthRestoreText, 350, 550);
            }
            if (!timerRunning) {
                glutTimerFunc(timerDuration, timerCallbackHealth, 0); // Iniciar temporizador
                timerRunning = true;
            }
        }
        if (upgradeFlag && !gameover) {
            glColor3f(1.0, 1.0, 1.0);
            string UpgradeCoinsText = "No tienes suficientes creditos para la mejora. 50$ Necesarios.";
            drawText(UpgradeCoinsText, 250, 510);
            if (!timerRunning) {
                glutTimerFunc(2000, timerCallbackCoins, 0); // Iniciar temporizador
                timerRunning = true;
            }
        }
        if (!ammo && !gameover && municionTanque <= 0) {
            glColor3f(1.0, 0.0, 0.0);
            string speedText = "No tienes municiones.";
            drawText(speedText, 330, 490);
        }
        if (ammo && !gameover) {
            glColor3f(0.2, 0.8, 0.);
            string speedText = "Municion restaurada al maximo.";
            drawText(speedText, 325, 180);
            if (!timerRunningMaxAmmo) {
                glutTimerFunc(3000, timerCallbackMaxAmmo, 0); // Iniciar temporizador
                timerRunningMaxAmmo = true;
            }
        }
        if (!loadScreen) {
            infoTextRender();
        }
        glEnable(GL_LIGHTING);
    }
    glutSwapBuffers();
}

// Función para actualizar la posición del sol
void updateSunPosition(int value) {
    static int simulated_time = 0;
    simulated_time++;

    GLfloat angle = (simulated_time % 1440) / 1440.0 * 2 * M_PI;

    light_position[0] = 100.0 * cos(angle);
    light_position[1] = 100.0 * sin(angle);

    sun_position[0] = 50.0 * cos(angle);
    sun_position[1] = 50.0 * sin(angle);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glutPostRedisplay();
    glutTimerFunc(1000, updateSunPosition, 0); // Llama a la función cada segundo
}

// Función de redimensionamiento de ventana
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Variables de estado para las teclas 'w' 'a' 's' y 'd'
bool key_w_pressed = false;
bool key_s_pressed = false;
bool key_a_pressed = false;
bool key_d_pressed = false;
bool key_shift_pressed = false;

void updateCarDirection() {
    // Verificar si se presionaron ambas teclas 'w' y 'a' simultáneamente
    if (key_w_pressed && key_a_pressed) {
        // Realizar alguna acción cuando ambas teclas se presionan simultáneamente
        // Girar hacia la izquierda al mismo tiempo
        updateCarPosition(movement_speed);
        updateCarRotation(rotation_speed);
    }
    else if (key_w_pressed && key_d_pressed) {
        // Realizar alguna acción cuando ambas teclas se presionan simultáneamente
        // Girar hacia la derecha al mismo tiempo
        updateCarPosition(movement_speed);
        updateCarRotation(-rotation_speed);
    }
    else if (key_s_pressed && key_a_pressed) {
        // Realizar alguna acción cuando ambas teclas se presionan simultáneamente
        // Girar hacia la izquierda al mismo tiempo
        updateCarPosition(-movement_speed);
        updateCarRotation(-rotation_speed);
    }
    else if (key_s_pressed && key_d_pressed) {
        // Realizar alguna acción cuando ambas teclas se presionan simultáneamente
        // Girar hacia la derecha al mismo tiempo
        updateCarPosition(-movement_speed);
        updateCarRotation(rotation_speed);
    }
    if (key_w_pressed && !key_d_pressed && !key_s_pressed && !key_a_pressed) {
        // Realizar alguna acción cuando se presiona la tecla 'w'
        updateCarPosition(movement_speed);
    }
    if (key_s_pressed && !key_w_pressed && !key_d_pressed && !key_a_pressed) {
        // Realizar alguna acción cuando se presiona la tecla 's'
        updateCarPosition(-movement_speed);
    }
    if (key_a_pressed && !key_d_pressed && !key_s_pressed && !key_w_pressed) {
        // Realizar alguna acción cuando se presiona la tecla 'a'
        updateCarRotation(rotation_speed);
    }
    if (key_d_pressed && !key_w_pressed && !key_s_pressed && !key_a_pressed) {
        // Realizar alguna acción cuando se presiona la tecla 'd'
        updateCarRotation(-rotation_speed);
    }

}

void update(int value) {
    updateCarDirection();
    // Calcular la velocidad actual del 

    float nextX = car_position[0];
    float nextY = car_position[2];

    if (movingForward && !loadScreen && !gameover) {
        //playEffectsSound(10);
        updateCarPosition(movement_speed);
        key_w_pressed = true;
    }
    if (movingBackward && !loadScreen && !gameover) {
        updateCarPosition(-movement_speed);
        key_s_pressed = true;
    }
    if (rotatingLeft && !loadScreen && !gameover) {
        key_a_pressed = true;
    }
    if (rotatingRight && !loadScreen && !gameover) {
        key_d_pressed = true;
    }
    if (shiftPressed && !loadScreen && !gameover) {
        key_shift_pressed = true;
    }

    // Generar objetivos aleatorios con cierta probabilidad
    if (rand() % 10 == 0 && !loadScreen) { // Probabilidad del 1%
        if (!gamewin)
        {
            generateRandomTarget();
        }
        //playEffectsSound(1);
    }
    // Actualizar movimiento de los objetivos
    for (auto& target : targets) {
        float dx = car_position[0] - target.x;
        float dz = car_position[2] - target.z;
        target.angle = atan2(dx, dz) * 180.0f / 3.14159f; // Calcular ángulo hacia el tanque

        float distance = sqrt(dx * dx + dz * dz);
        if (distance > 1.0f) { // Mover el objetivo hacia el tanque si está lejos
            target.x += 0.3f * sin(target.angle * 3.14159 / 180.0f);
            target.z += 0.3f * cos(target.angle * 3.14159 / 180.0f);
        }
    }

    // Generar puntos de restauración de vida aleatorios con cierta probabilidad
    if (rand() % 300 == 0 && !loadScreen) {
        generateRandomHealer();
    }

    if (rand() % 150 == 0 && !loadScreen) {
        generateRandomUpgrade();
    }

    if (rand() % 150 == 0 && !loadScreen) {
        generateRandomMunicion();
    }

    // Detección de colisión con las casas
    bool collided = false;
    for (const auto& building : buildings) {
        if ((fabs(nextX - building.x) < 5.0f) && (fabs(nextY - building.y) < 5.0f)) {
            car_position[0] = nextX * 0.99;
            car_position[2] = nextY * 0.99;
            // Colisión con una 
            playEffectsSound(4);
            collided = true;
            break;
        }
    }
    for (const auto& tree : trees) {
        if ((fabs(nextX - tree.x) < 1.8f) && (fabs(nextY - tree.y) < 1.8f)) {
            car_position[0] = nextX * 0.99;
            car_position[2] = nextY * 0.99;
            playEffectsSound(4);
            collided = true;
            break;
        }
    }
    for (const auto& car : cars) {
        if ((fabs(nextX - car.x) < 3.0f) && (fabs(nextY - car.y) < 1.2f)) {
            car_position[0] = nextX * 0.99;
            car_position[2] = nextY * 0.99;
            playEffectsSound(4);
            collided = true;
            break;
        }
    }


    if (!collided) {
        car_position[0] = nextX;
        car_position[2] = nextY;
    }
    //float currentSpeed = shiftPressed ? 2.0f * tankSpeed : tankSpeed;

    // Actualizar movimiento de las balas
    for (auto& target : targets) {
        // Verificar colisión de objetivo con tanque jugador
        if (fabs(car_position[0] - target.x) < 4.5f && fabs(car_position[2] - target.z) < 4.5f) {
            if (!gamewin)
            {
                // Si el objetivo colisiona con el tanque, reducir la salud del 
                playEffectsSound(10);
                damage = true;
                tankHealth -= 1;

                if (!tankHealthBar.empty()) { // Verifica si la cadena no está vacía
                    tankHealthBar.pop_back(); // Elimina el último carácter
                }
                // Eliminar el objetivo 
                // target.health = 0; // Marcar como muerto para ser eliminado más tarde
                if (gameover || tankHealth <= 0 || tankHealthBar.empty()) {
                    tankHealth = 0;
                }
            }
            break;
        }
    }
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.x += 3.0f * cos(bullet.angle * M_PI / 180.0f);
            bullet.z += 3.0f * -sin(bullet.angle * M_PI / 180.0f);
            bullet.distanceTravelled = sqrt(pow(bullet.x - bullet.startX, 2) + pow(bullet.z - bullet.startZ, 2));

            // Verificar colisión de bala con objetivos
            for (auto& target : targets) {
                // Verificar colisión de objetivo con tanque jugador
                if (fabs(bullet.x - target.x) < 1.5f && fabs(bullet.z - target.z) < 1.5f) {
                    if (instaKill) {
                        target.health -= 4;
                    }
                    else {
                        target.health -= 1;
                    }
                    bullet.active = false;
                    if (target.health <= 0 && !gameover) {
                        enemiesEliminated++;
                        if (doublePoints) {
                            coins += 20;
                        }
                        else {
                            coins += 10;
                        }
                        playEffectsSound(1);
                    }
                    break;
                }

            }
            // Desactivar balas que han viajado más allá de una distancia máxima
            if (bullet.distanceTravelled > 30.0f) {
                bullet.active = false;
            }
        }
    }
    for (auto& healer : healerpoints) {
        // Verificar colisión de objetivo con tanque jugador
        if (fabs(car_position[0] - healer.x) < 4.5f && fabs(car_position[2] - healer.z) < 4.5f) {
            // heal-restore
            health = true;
            playEffectsSound(2);
            // Si el objetivo colisiona con el tanque, reducir la salud del 
            if (tankHealth <= 100) {
                tankHealth += healer.healthRestore;

                // Determinar cuántas barras adicionales se deben agregar
                int additionalBars = (healer.healthRestore + tankHealthBar.size()) > 100 ? (100 - tankHealthBar.size()) : healer.healthRestore;

                // Agregar las barras adicionales
                tankHealthBar.append(additionalBars, '|');
            }
            if (tankHealth >= 101) {
                tankHealth = 100;
            }
            // Eliminar el objetivo
            healer.healthRestore = 0; // Marcar como muerto para ser eliminado más tarde

            if (gameover || tankHealth <= 0) {
                tankHealth = 0;
            }
        }

    }
    for (auto& upgrade : upgrades) {
        // Verificar colisión de objetivo con tanque jugador
        if (fabs(car_position[0] - upgrade.x) < 4.5f && fabs(car_position[2] - upgrade.z) < 4.5f) {
            if (coins >= upgrade.cost && !gameover)
            {
                cout << "upgrade.typeUpgrade: " << upgrade.typeUpgrade << endl;
                if (upgrade.typeUpgrade == "speed") {
                    playEffectsSound(8);
                    upgrade.active = true;
                    //cout << "Aumento de velocidad"<< endl;
                    speed = true;
                    coins -= upgrade.cost;

                    movement_speed = 1.0f;
                    updateBulletPosition();
                }
                if (upgrade.typeUpgrade == "instakill") {
                    playEffectsSound(5);
                    upgrade.active = true;
                    instaKill = true;
                    coins -= upgrade.cost;
                }
                if (upgrade.typeUpgrade == "killall") {
                    playEffectsSound(6);
                    upgrade.active = true;
                    killAllEnemys = true;
                    coins -= upgrade.cost;
                    for (auto& target : targets) {
                        target.health = 0;
                    }
                }
                if (upgrade.typeUpgrade == "doublepoints") {
                    playEffectsSound(7);
                    upgrade.active = true;
                    doublePoints = true;
                    coins -= upgrade.cost;
                }

                upgrade.health = 0; // Marcar como muerto para ser eliminado más tarde
            }
            else {
                playEffectsSound(9);
                upgradeFlag = true;
                upgrade.health = 0; // Marcar como muerto para ser eliminado más tarde
            }
        }

    }
    for (auto& municion : municiones) {
        // Verificar colisión de objetivo con tanque jugador
        if (fabs(car_position[0] - municion.x) < 4.5f && fabs(car_position[2] - municion.z) < 4.5f) {
            // heal-restore
            ammo = true;
            playEffectsSound(12);
            // Si el objetivo colisiona con el tanque, reducir la salud del 
            if (municionTanque <= 100) {
                municionTanque += municion.municion;

                // Determinar cuántas barras adicionales se deben agregar
                int additionalBars = (municion.municion + ammoBar.size()) > 100 ? (100 - ammoBar.size()) : municion.municion;

                // Agregar las barras adicionales
                ammoBar.append(additionalBars, '|');
            }
            if (municionTanque >= 101) {
                municionTanque = 100;
            }
            // Eliminar el objetivo
            municion.municion = 0; // Marcar como muerto para ser eliminado más tarde
            playEffectsSound(14);
        }

    }
    // Eliminar objetivos con salud menor o igual a cero
    targets.erase(remove_if(targets.begin(), targets.end(), [](const Target& t) { return t.health <= 0; }), targets.end());
    healerpoints.erase(remove_if(healerpoints.begin(), healerpoints.end(), [](const Healer& t) { return t.healthRestore <= 0; }), healerpoints.end());
    upgrades.erase(remove_if(upgrades.begin(), upgrades.end(), [](const Upgrade& t) { return t.health <= 0; }), upgrades.end());
    municiones.erase(remove_if(municiones.begin(), municiones.end(), [](const Municion& t) { return t.municion <= 0; }), municiones.end());

    // Verificar si el tanque jugador ha perdido toda su salud
    if (tankHealth <= 0 || !timerActiveInPause) {
        // Reiniciar posición del tanque a (0, 0, 0)
        tankHealth = 0;
        playEffectsSound(16);
        gameover = true; // Marcar el juego como terminado
    }

    glutPostRedisplay();

    if (!paused) {
        glutTimerFunc(16, update, 0); // 60 FPS
    }

    // Movimiento con velocidad del tanque
    if (!speed && !loadScreen && !gameover) {
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
            movement_speed = 0.35f; // Si se presiona shift, la velocidad es mayor
            updateBulletPosition();

            if (GetAsyncKeyState('W') & 0x8000) {
                updateCarPosition(movement_speed);
            }
            if (GetAsyncKeyState('S') & 0x8000) {
                updateCarPosition(-movement_speed);
            }
            if (GetAsyncKeyState('A') & 0x8000) {
                updateCarRotation(rotation_speed);
            }
            if (GetAsyncKeyState('D') & 0x8000) {
                updateCarRotation(-rotation_speed);
            }
            if (GetAsyncKeyState('S') & 0x8000 && GetAsyncKeyState('D') & 0x8000) {
                updateCarPosition(-movement_speed);
                updateCarRotation(rotation_speed);
            }
            if (GetAsyncKeyState('S') & 0x8000 && GetAsyncKeyState('A') & 0x8000) {
                updateCarPosition(-movement_speed);
                updateCarRotation(-rotation_speed);
            }
        }
        else {
            movement_speed = 0.4f;
        }
    }
}

void keyDown(unsigned char key, int x, int y) {
    switch (key) {
    case 'W':
    case 'w':
        movingForward = true;
        break;
    case 'S':
    case 's':
        movingBackward = true;
        break;
    case 'A':
    case 'a':
        rotatingLeft = true;
        break;
    case 'D':
    case 'd':
        rotatingRight = true;
        break;
    case 'C':
    case 'c':
        isStaticCamera = !isStaticCamera; // Alternar entre la cámara estática y la cámara en tercera persona
        break;
    case '4':
        cameraX -= 0.5;
        break;
    case '5':
        cameraX += 0.5;
        break;
    case '6':
        cameraY += 0.5;
        break;
    case '7':
        cameraY -= 0.5;
        break;
    case '+':
        cameraZ += 0.5;
        break;
    case '-':
        cameraZ -= 0.5;
        break;
    case ' ':
        // Presionar barra espaciadora para disparar
        if (!gameover && !loadScreen && municionTanque > 0 && !gamewin && !paused) {
            municionTanque -= 1;
            if (!ammoBar.empty()) { // Verifica si la cadena no está vacía
                ammoBar.pop_back(); // Elimina el último carácter
            }
            bullets.push_back({ car_position[0], car_position[1], car_position[2], car_position[0], car_position[2], car_rotation, true, 0.0f });
            playEffectsSound(3);
        }
        else if (!gameover && !loadScreen && !gamewin && !paused) {
            playEffectsSound(13);
        }
        break;
    case 'R':
    case 'r':
        if (gameover || gamewin) {
            if (ammoBar.size() > 0) {
                ammoBar.clear();
            }
            if (tankHealthBar.size() > 0) {
                tankHealthBar.clear();
            }
            restartGame();
            gameover = false;
        }
        break;
    case 13:
        if (loadScreen) {
            restartGame();
            loadScreen = false;
        }
        break;
    case 27: // Tecla Esc para salir del juego
        //exit(0);
        paused = !paused; // Cambiar el estado de pausa
        if (timerActive) {
            pauseTimer(); // Pausamos el temporizador si está activo
        }
        else {
            resetTimer(); // Reanudamos el temporizador si está pausado
        }
        if (!paused) {
            // Si se despausa el juego, reiniciar el temporizador
            glutTimerFunc(16, update, 0); // 60 FPS
        }
        if (!paused) {
            glutTimerFunc(16, legAnimation, 0);
        }
        if (!paused) {
            glutTimerFunc(16, mouthAnimation, 0);
        }
        if (!paused) {
            glutTimerFunc(16, donaAnimation, 0);
        }
        break;
    default:
        break;
    }
}
void keyUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'W':
    case 'w':
        movingForward = false;
        key_w_pressed = false;
        break;
    case 'S':
    case 's':
        movingBackward = false;
        key_s_pressed = false;
        break;
    case 'A':
    case 'a':
        rotatingLeft = false;
        key_a_pressed = false;
        break;
    case 'D':
    case 'd':
        rotatingRight = false;
        key_d_pressed = false;
        break;
    default:
        break;
    }
}

// Función del menú
void menu(int value) {
    switch (value) {
    case 1:
        if (glutGet(GLUT_WINDOW_WIDTH) != glutGet(GLUT_SCREEN_WIDTH) ||
            glutGet(GLUT_WINDOW_HEIGHT) != glutGet(GLUT_SCREEN_HEIGHT)) {
            glutFullScreen(); // Maximizar la pantalla
        }
        else {
            glutReshapeWindow(800, 600); // Restaurar a ventana normal
        }
        break;
    case 2:
        glutReshapeWindow(800, 600); // Cambiar la resolución a 800x600
        break;
    case 3:
        glutReshapeWindow(1024, 768); // Cambiar la resolución a 1024x768
        break;
    case 4:
        glutReshapeWindow(1280, 720); // Cambiar la resolución a 1280x720
        break;
    case 5:
        glutReshapeWindow(1920, 1080); // Cambiar la resolución a 1920x1080
        break;
    case 6:
        exit(0);
        break;
    }
}

void restartGame() {
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    //glClearColor(0.6f, 0.6f, 0.7f, 1.0f); // Cambiar el color de fondo a rojo
    car_position[0] = 0.0f;
    car_position[2] = 0.0f;
    car_rotation = 0.0f;
    startTime = glutGet(GLUT_ELAPSED_TIME);
    timerActive = true; // Activamos el temporizador nuevamente
    timerActiveInPause = true;
    tankHealth = 100;
    tankHealthBar.append(tankHealth, '|');
    enemiesEliminated = 0;
    coins = 0;
    municionTanque = 100;
    ammoBar.append(municionTanque, '|');
    movingForward = false;
    movingBackward = false;
    rotatingLeft = false;
    rotatingRight = false;
    shooting = false;
    gameover = false;
    gamewin = false;
    bullets.clear();
    targets.clear();
    healerpoints.clear();
    upgrades.clear();
    municiones.clear();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH) - 20, glutGet(GLUT_SCREEN_HEIGHT) -140); // Tamaño inicial de la ventana
    glutCreateWindow("Alien Annihilation - Operacion Tanque");
    glutPositionWindow(0, 40);
    glutSetIconTitle("logo-tank-game.ico");
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    init(); // Inicializamos OpenGL
    glutDisplayFunc(display); // Función de dibujo
    glutReshapeFunc(reshape); // Función de redimensionamiento de ventana
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    //glutSpecialFunc(handleSpecialKeypress); // Teclas especiales

    // Crear menú
    glutCreateMenu(menu);
    glutAddMenuEntry("Pantalla Completa / Restaurar", 1);
    glutAddMenuEntry("800x600", 2);
    glutAddMenuEntry("1024x768", 3);
    glutAddMenuEntry("1280x720", 4);
    glutAddMenuEntry("1920x1080", 5);
    glutAddMenuEntry("Salir", 6);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    // Inicia la simulación del tiempo
    glutTimerFunc(0, update, 0); // Call update() initially
    glutTimerFunc(16, legAnimation, 0);
    glutTimerFunc(16, mouthAnimation, 0);
    glutTimerFunc(16, donaAnimation, 0);
    glutTimerFunc(16, tankAnimation, 0);
    glutTimerFunc(8, monsterAnimation, 0);
    //glutTimerFunc(1000, updateSunPosition, 0);
    playBackgroundMusic();

    glutMainLoop();

    soundEngine->drop();
}