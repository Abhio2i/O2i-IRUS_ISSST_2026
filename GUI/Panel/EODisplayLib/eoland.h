#ifndef EOLAND_H
#define EOLAND_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

class EOLand : protected QOpenGLFunctions
{
public:
    EOLand();
    ~EOLand();

    void initialize();
    void render(const QMatrix4x4 &projection, const QMatrix4x4 &cameraMatrix,  const QVector2D &camEN = QVector2D(0.0f, 0.0f), float headingRad = 0.0f);

private:
    QOpenGLShaderProgram groundProgram;
    QOpenGLVertexArrayObject groundVao;
    QOpenGLBuffer groundVbo;

    int vertexCount; // Tracks the size of our generated 3D mesh
};

#endif // EOLAND_H


// #ifndef EOLAND_H
// #define EOLAND_H

// #include <QOpenGLFunctions>
// #include <QOpenGLShaderProgram>
// #include <QOpenGLVertexArrayObject>
// #include <QOpenGLBuffer>
// #include <QOpenGLTexture>
// #include <QMatrix4x4>
// #include <QVector>
// #include <QVector2D>
// #include <QVector3D>
// #include <QString>
// #include <memory>

// class EOLand : protected QOpenGLFunctions
// {
// public:
//     EOLand();
//     ~EOLand();

//     // diffuseTexturePath: tileable ground texture (jpg/png). Accepts a Qt
//     // resource path (":/textures/grass_ground_diff_4k.jpg") or a plain file
//     // path. Empty string = use the default path in eoland.cpp. If the file
//     // cannot be loaded, the old procedural green/brown colouring is used as
//     // a fallback, so the display never goes blank.
//     // Must be called with the GL context current (i.e. from initializeGL()).
//     void initialize(const QString &diffuseTexturePath = QString());

//     // camEN      : camera position in metres (east, north) from a FIXED
//     //              origin. Makes the ground scroll as the aircraft moves.
//     // headingRad : sensor heading in radians (clockwise from north), so the
//     //              ground rotates correctly when the aircraft turns.
//     // Both default to 0, which reproduces the old "ground never moves"
//     // behaviour, so an old call render(projection, cameraMatrix) still works.
//     void render(const QMatrix4x4 &projection,
//                 const QMatrix4x4 &cameraMatrix,
//                 const QVector2D &camEN = QVector2D(0.0f, 0.0f),
//                 float headingRad = 0.0f);

// private:
//     bool loadGroundTexture(const QString &path);

//     QOpenGLShaderProgram groundProgram;
//     QOpenGLVertexArrayObject groundVao;
//     QOpenGLBuffer groundVbo;
//     std::unique_ptr<QOpenGLTexture> groundTexture; // null = no texture loaded

//     int vertexCount; // Tracks the size of our generated 3D mesh
// };

// #endif // EOLAND_H
