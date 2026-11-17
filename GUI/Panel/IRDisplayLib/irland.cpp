#include "irland.h"
#include <QDebug>
#include <cmath>

namespace {
constexpr float RELIEF_METERS = 60.0f;
constexpr float GROUND_DROP_METERS = 1.0f;
} // namespace

IRLand::IRLand()
    : groundVbo(QOpenGLBuffer::VertexBuffer), vertexCount(0)
{
}

IRLand::~IRLand()
{
    groundVao.destroy();
    groundVbo.destroy();
}

void IRLand::initialize()
{
    initializeOpenGLFunctions();

    const char *gVsrc = R"GLSL(#version 330 core
layout(location = 0) in vec3 position;
out vec3 vWorldPos;
out float vHeight;
uniform mat4  mvpMatrix;
uniform vec2  uCamEN;
uniform float uHeading;
uniform float uReliefMeters;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}
float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for (int i = 0; i < 5; ++i) {
        v += a * noise(p);
        p = rot * p * 2.0 + vec2(100.0);
        a *= 0.5;
    }
    return v;
}
void main() {
    vec3 pos = position;
    float c = cos(uHeading);
    float s = sin(uHeading);
    // +x = right, -z = forward → east/north
    vec2 local = vec2(pos.x * c - pos.z * s,
                      -(pos.x * s + pos.z * c));
    float relief = fbm((uCamEN + local) * 0.0002);
    vHeight = relief;
    pos.y += (relief - 1.0) * uReliefMeters;
    vWorldPos = pos;
    gl_Position = mvpMatrix * vec4(pos, 1.0);
}
)GLSL";

    const char *gFsrc = R"GLSL(#version 330 core
in vec3 vWorldPos;
in float vHeight;
out vec4 fragColor;
void main() {
    float heightPercent = clamp(vHeight, 0.0, 1.0);
    float gridSize = 100.0;
    float gridX = mod(floor(vWorldPos.x / gridSize), 2.0);
    float gridZ = mod(floor(vWorldPos.z / gridSize), 2.0);
    bool isGridLine = (gridX == gridZ);
    float baseThermal = mix(0.4, 0.1, heightPercent);

    if (isGridLine) baseThermal -= 0.05;
    vec3 terrainColor = vec3(baseThermal);
    float camDist = length(vWorldPos.xz);
    float fogNear = 20000.0;
    float fogFar  = 480000.0;
    float fog = clamp((camDist - fogNear) / (fogFar - fogNear), 0.0, 1.0);
    vec3 skyColor = vec3(0.05, 0.05, 0.05);
    terrainColor = mix(terrainColor, skyColor, fog);
    fragColor = vec4(terrainColor, 1.0);
}
)GLSL";

    if (!groundProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, gVsrc))
        qWarning() << "[IRLand] vertex shader error:" << groundProgram.log();
    if (!groundProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, gFsrc))
        qWarning() << "[IRLand] fragment shader error:" << groundProgram.log();
    if (!groundProgram.link())
        qWarning() << "[IRLand] link error:" << groundProgram.log();

    QVector<QVector3D> vertices;
    const int segments = 200;
    const float size = 500000.0f;
    const float halfSize = size / 2.0f;
    const float step = size / segments;
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            float x1 = -halfSize + j * step;
            float z1 = -halfSize + i * step;
            float x2 = x1 + step;
            float z2 = z1 + step;
            vertices.append(QVector3D(x1, 0.0f, z1));
            vertices.append(QVector3D(x2, 0.0f, z1));
            vertices.append(QVector3D(x1, 0.0f, z2));
            vertices.append(QVector3D(x2, 0.0f, z1));
            vertices.append(QVector3D(x2, 0.0f, z2));
            vertices.append(QVector3D(x1, 0.0f, z2));
        }
    }
    vertexCount = vertices.size();

    groundVao.create();
    groundVao.bind();
    groundVbo.create();
    groundVbo.bind();
    groundVbo.allocate(vertices.constData(), vertexCount * sizeof(QVector3D));
    int posLoc = groundProgram.attributeLocation("position");
    groundProgram.enableAttributeArray(posLoc);
    groundProgram.setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, 0);
    groundVao.release();
    groundVbo.release();
}

void IRLand::render(const QMatrix4x4 &projection,
                    const QMatrix4x4 &cameraMatrix,
                    const QVector2D &camEN,
                    float headingRad)
{
    QMatrix4x4 groundMatrix = cameraMatrix;
    groundMatrix.translate(0.0f, -GROUND_DROP_METERS, 0.0f);

    groundProgram.bind();
    groundProgram.setUniformValue("mvpMatrix", projection * groundMatrix);
    groundProgram.setUniformValue("uCamEN", camEN);
    groundProgram.setUniformValue("uHeading", headingRad);
    groundProgram.setUniformValue("uReliefMeters", RELIEF_METERS);

    groundVao.bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    groundVao.release();
    groundProgram.release();
}
