#ifndef IRLAND_H
#define IRLAND_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

class IRLand : protected QOpenGLFunctions
{
public:
    IRLand();
    ~IRLand();

    void initialize();

    // ground scroll + heading
    void render(const QMatrix4x4 &projection,
                const QMatrix4x4 &cameraMatrix,
                const QVector2D &camEN = QVector2D(0.0f, 0.0f),
                float headingRad = 0.0f);

private:
    QOpenGLShaderProgram groundProgram;
    QOpenGLVertexArrayObject groundVao;
    QOpenGLBuffer groundVbo;
    int vertexCount;
};

#endif // IRLAND_H
