#include <stdio.h>
#include "H.h"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint VBO;
GLuint gWorldLocation;

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                        \n\
{                                                                                     \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\                                                                                   \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

    Scale += 0.003f;

    Matrix4f World;
    //Матрица 4 на 4. Чтобы фигура двигалась горизонтально v2 и v3 устанавливаем в 0
    World.m[0][0] = cosf(Scale); World.m[0][1] = -sinf(Scale); World.m[0][2] = 0.0f; World.m[0][3] = 0.0f;
    World.m[1][0] = sinf(Scale); World.m[1][1] = cosf(Scale);  World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;         World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;         World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}


static void CreateVertexBuffer()
{
    Vector3f V[3];
    V[0] = Vector3f(-0.5f, -0.5f, 0.0f);
    V[1] = Vector3f(0.5f, -0.5f, 0.0f);
    V[2] = Vector3f(0.0f, 0.5f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    //Создание объекта шейдера
    GLuint ShaderObj = glCreateShader(ShaderType);

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);//Компилирование шейдера. 
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    //В случае ошибки, отображаются все ошибки, обнаруженные компилятором 
    if (!success) 
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
    //Присоединяем скомпилированные объект шейдера к объекту программы
    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();    //Создание программного объекта

    if (ShaderProgram == 0) 
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);// линковка шейдера
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);// проверка на программыне ошибки
    }

    glValidateProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    //назначение шейдеров для конвейера
    glUseProgram(ShaderProgram);
    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lesson 7");

    glutDisplayFunc(RenderSceneCB);

    glutIdleFunc(RenderSceneCB);

    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    glutMainLoop();
}