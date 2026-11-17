#include "eoland.h"

EOLand::EOLand()
    : groundVbo(QOpenGLBuffer::VertexBuffer), vertexCount(0)
{
}

EOLand::~EOLand()
{
    groundVao.destroy();
    groundVbo.destroy();
}

void EOLand::initialize()
{
    initializeOpenGLFunctions();

    // 1. VERTEX SHADER (Procedural 3D Terrain)
    // Identical to IR: Uses noise to push the flat grid up into mountains.
    const char *gVsrc =
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "out vec3 vWorldPos;\n"
        "out float vHeight;\n"
        "uniform mat4 mvpMatrix;\n"

        "float hash(vec2 p) {\n"
        "   return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);\n"
        "}\n"

        "float noise(vec2 p) {\n"
        "   vec2 i = floor(p);\n"
        "   vec2 f = fract(p);\n"
        "   vec2 u = f * f * (3.0 - 2.0 * f);\n"
        "   return mix(mix(hash(i + vec2(0.0,0.0)), hash(i + vec2(1.0,0.0)), u.x),\n"
        "              mix(hash(i + vec2(0.0,1.0)), hash(i + vec2(1.0,1.0)), u.x), u.y);\n"
        "}\n"

        "float fbm(vec2 p) {\n"
        "   float v = 0.0;\n"
        "   float a = 0.5;\n"
        "   mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));\n"
        "   for (int i = 0; i < 5; ++i) {\n"
        "       v += a * noise(p);\n"
        "       p = rot * p * 2.0 + vec2(100.0);\n"
        "       a *= 0.5;\n"
        "   }\n"
        "   return v;\n"
        "}\n"

        // "void main() {\n"
        // "   vec3 pos = position;\n"
        // "   float elevation = fbm(pos.xz * 0.0002) * 800.0;\n"
        // "   pos.y += elevation;\n"

        // "   vWorldPos = pos;\n"
        // "   gl_Position = mvpMatrix * vec4(pos, 1.0);\n"
        // "}\n";
        "void main() {\n"
        "   vec3 pos = position;\n"
        "   float relief = fbm(pos.xz * 0.0002);\n"      // 0..~1
        "   vHeight = relief;\n"
        "   pos.y += (relief - 1.0) * 60.0;\n"           // surface sirf -60 m se 0 m ke beech
        "   vWorldPos = pos;\n"
        "   gl_Position = mvpMatrix * vec4(pos, 1.0);\n"
        "}\n";


    // 2. FRAGMENT SHADER (Optical Biome Mapping + Distance Fog)
    // We color the terrain based on its height to look natural, then fade
    // distant terrain into the horizon color so the ground blends into the
    // sky instead of hard-clipping at the far plane.
    const char *gFsrc =
        "#version 330 core\n"
        "in vec3 vWorldPos;\n"
        "in float vHeight;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "   // Map 0-800m elevation to a 0.0-1.0 percentage\n"
       // "   float heightPercent = clamp(vWorldPos.y / 800.0, 0.0, 1.0);\n"
        "   float heightPercent = clamp(vHeight, 0.0, 1.0);\n"

        "   // Retain the topological grid for scale/speed reference\n"
        "   float gridSize = 100.0;\n"
        "   float gridX = mod(floor(vWorldPos.x / gridSize), 2.0);\n"
        "   float gridZ = mod(floor(vWorldPos.z / gridSize), 2.0);\n"
        "   bool isGridLine = (gridX == gridZ);\n"

        "   // Define Biome Colors\n"
        "   vec3 valleyColor = vec3(0.15, 0.45, 0.15); // Deep green grass\n"
        "   vec3 hillColor   = vec3(0.45, 0.35, 0.20); // Brown dirt/rock\n"
        "   vec3 peakColor   = vec3(0.85, 0.90, 0.95); // White snow\n"

        "   vec3 terrainColor;\n"
        "   // Blend colors based on altitude\n"
        "   if (heightPercent < 0.4) {\n"
        "       // 0% to 40% height: Blend Grass to Rock\n"
        "       terrainColor = mix(valleyColor, hillColor, heightPercent / 0.4);\n"
        "   } else {\n"
        "       // 40% to 100% height: Blend Rock to Snow\n"
        "       terrainColor = mix(hillColor, peakColor, (heightPercent - 0.4) / 0.6);\n"
        "   }\n"

        "   // Darken the procedural grid lines slightly for contrast\n"
        "   if (isGridLine) terrainColor *= 0.85;\n"

        "   // Distance fog: fade the ground into the horizon color instead of\n"
        "   // hard-clipping at the far plane. The camera is always at local\n"
        "   // (X=0, Z=0) in this coordinate scheme, so horizontal distance from\n"
        "   // the camera is just the length of the world XZ position — no extra\n"
        "   // camera-position uniform needed. This matters a lot at high\n"
        "   // altitude, where the visible horizon is tens of kilometres away.\n"
        "   float camDist = length(vWorldPos.xz);\n"
        "   float fogNear = 20000.0;\n"
        "   float fogFar  = 480000.0;\n"
        "   float fog = clamp((camDist - fogNear) / (fogFar - fogNear), 0.0, 1.0);\n"
        "   vec3 horizonColor = vec3(0.65, 0.78, 0.92);\n" // must match sky's horizon color
        "   terrainColor = mix(terrainColor, horizonColor, fog);\n"

        "   fragColor = vec4(terrainColor, 1.0);\n"
        "}\n";

    groundProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, gVsrc);
    groundProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, gFsrc);
    groundProgram.link();

    // 3. GENERATE A DENSE 3D MESH (CPU Side)
    // 500km grid divided into 200x200 segments — enlarged from 50km so the
    // ground still extends to the horizon at high altitude instead of running
    // out before the (also enlarged) far clipping plane.
    QVector<QVector3D> vertices;
    int segments = 200;
    float size = 500000.0f;
    float halfSize = size / 2.0f;
    float step = size / segments;

    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            float x1 = -halfSize + j * step;
            float z1 = -halfSize + i * step;
            float x2 = x1 + step;
            float z2 = z1 + step;

            // Triangle 1
            vertices.append(QVector3D(x1, 0.0f, z1));
            vertices.append(QVector3D(x2, 0.0f, z1));
            vertices.append(QVector3D(x1, 0.0f, z2));

            // Triangle 2
            vertices.append(QVector3D(x2, 0.0f, z1));
            vertices.append(QVector3D(x2, 0.0f, z2));
            vertices.append(QVector3D(x1, 0.0f, z2));
        }
    }

    vertexCount = vertices.size();

    // 4. PUSH TO GPU
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

void EOLand::render(const QMatrix4x4 &projection, const QMatrix4x4 &cameraMatrix, const QVector2D &camEN, float headingRad)
{
    // 1. Create a local copy of the camera matrix just for the ground
    QMatrix4x4 groundMatrix = cameraMatrix;

    // 2. Push the ground 150 meters DOWN on the Y-axis.
    groundMatrix.translate(0.0f, -1.0f, 0.0f);

    groundProgram.bind();

    // 3. Pass our new, offset groundMatrix to the shader
    groundProgram.setUniformValue("mvpMatrix", projection * groundMatrix);

    groundVao.bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    groundVao.release();

    groundProgram.release();
}



// #include "eoland.h"

// #include <QImage>
// #include <QDebug>
// #include <cmath>

// namespace {

// // Default texture location. Change this string, or pass a path to
// // EOLand::initialize(), to point at your own file.
// const char *kDefaultDiffusePath = ":/model/airplane/Model/Airplane/grass_ground_diff_4k.jpg";

// // How many metres one copy of the texture covers on the ground. Smaller =
// // finer grass pattern but more visible repetition. Tune to taste.
// constexpr float TEXTURE_TILE_METERS = 1000.0f;

// // 4096x4096 RGBA + mipmaps is ~85 MB of GPU memory. Scaling down to 2048
// // cuts that to ~21 MB with no visible loss at EO viewing distances.
// constexpr int MAX_TEXTURE_SIZE = 2048;

// // Terrain relief. The surface always stays between -RELIEF_METERS and 0 so
// // no aircraft (simulation ground level = 0 m) is ever buried in the terrain.
// constexpr float RELIEF_METERS = 60.0f;

// // Small extra drop so aircraft sitting at exactly 0 m do not z-fight with
// // the ground.
// constexpr float GROUND_DROP_METERS = 1.0f;

// } // namespace

// EOLand::EOLand()
//     : groundVbo(QOpenGLBuffer::VertexBuffer), vertexCount(0)
// {
// }

// EOLand::~EOLand()
// {
//     if (groundTexture) {
//         groundTexture->destroy();
//     }
//     groundVao.destroy();
//     groundVbo.destroy();
// }

// bool EOLand::loadGroundTexture(const QString &path)
// {
//     QImage img(path);
//     if (img.isNull()) {
//         qWarning() << "[EOLand] ground texture not found / unreadable:" << path
//                    << "- using procedural colours instead";
//         return false;
//     }

//     if (img.width() > MAX_TEXTURE_SIZE || img.height() > MAX_TEXTURE_SIZE) {
//         img = img.scaled(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE,
//                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//     }

//     groundTexture = std::make_unique<QOpenGLTexture>(img, QOpenGLTexture::GenerateMipMaps);
//     groundTexture->setWrapMode(QOpenGLTexture::Repeat);
//     groundTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
//     groundTexture->setMagnificationFilter(QOpenGLTexture::Linear);
//     if (groundTexture->hasFeature(QOpenGLTexture::AnisotropicFiltering)) {
//         // Keeps the texture sharp at grazing angles, which is exactly how an
//         // EO camera sees the ground near the horizon.
//         groundTexture->setMaximumAnisotropy(8.0f);
//     }
//     return true;
// }

// void EOLand::initialize(const QString &diffuseTexturePath)
// {
//     initializeOpenGLFunctions();

//     const bool hasTexture = loadGroundTexture(
//         diffuseTexturePath.isEmpty() ? QString::fromLatin1(kDefaultDiffusePath)
//                                      : diffuseTexturePath);
//     Q_UNUSED(hasTexture);

//     // 1. VERTEX SHADER
//     // The terrain noise is sampled at WORLD-anchored east/north coordinates
//     // (camera position + the vertex offset rotated by the heading), not at
//     // camera-local coordinates, so the ground scrolls and turns with the
//     // aircraft. The texture UVs use the same transform, but with the camera
//     // position reduced modulo the tile size (done on the CPU) so float
//     // precision stays good no matter how far the aircraft has flown.
//     const char *gVsrc = R"GLSL(#version 330 core
// layout(location = 0) in vec3 position;

// out vec3 vWorldPos;   // camera-local position, used for fog distance
// out vec2 vUV;         // texture coordinates (world-anchored)
// out float vHeight;    // 0..1 noise value, used for colour variation

// uniform mat4  mvpMatrix;
// uniform vec2  uCamEN;        // camera east/north metres from a fixed origin
// uniform vec2  uUvOrigin;     // uCamEN modulo the tile size
// uniform float uHeading;      // radians, clockwise from north
// uniform float uTileMeters;
// uniform float uReliefMeters;

// float hash(vec2 p) {
//     return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
// }

// float noise(vec2 p) {
//     vec2 i = floor(p);
//     vec2 f = fract(p);
//     vec2 u = f * f * (3.0 - 2.0 * f);
//     return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
//                mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
// }

// float fbm(vec2 p) {
//     float v = 0.0;
//     float a = 0.5;
//     mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
//     for (int i = 0; i < 5; ++i) {
//         v += a * noise(p);
//         p = rot * p * 2.0 + vec2(100.0);
//         a *= 0.5;
//     }
//     return v;
// }

// void main() {
//     vec3 pos = position;

//     // Mesh offset (nose-aligned frame: +x = right, -z = forward) rotated
//     // into east/north using the heading.
//     float c = cos(uHeading);
//     float s = sin(uHeading);
//     vec2 local = vec2(pos.x * c - pos.z * s,
//                       -(pos.x * s + pos.z * c));

//     float relief = fbm((uCamEN + local) * 0.0002);
//     vHeight = relief;

//     // Surface always lies between -uReliefMeters and 0.
//     pos.y += (relief - 1.0) * uReliefMeters;

//     vWorldPos = pos;
//     vUV = (uUvOrigin + local) / uTileMeters;
//     gl_Position = mvpMatrix * vec4(pos, 1.0);
// }
// )GLSL";

//     // 2. FRAGMENT SHADER
//     // Near the camera: the real texture. Far away: fades to the texture's own
//     // average colour (highest mip level) so the repeating pattern and
//     // shimmering disappear, then into distance fog.
//     const char *gFsrc = R"GLSL(#version 330 core
// in vec3 vWorldPos;
// in vec2 vUV;
// in float vHeight;
// out vec4 fragColor;

// uniform sampler2D uGroundTex;
// uniform int uHasTexture;

// void main() {
//     float h = clamp(vHeight, 0.0, 1.0);
//     // Gentle brightness variation so large areas do not look flat.
//     float shade = mix(0.85, 1.15, h);

//     float camDist = length(vWorldPos.xz);
//     vec3 terrainColor;

//     if (uHasTexture == 1) {
//         vec3 texNear = texture(uGroundTex, vUV).rgb;
//         vec3 texAvg  = textureLod(uGroundTex, vec2(0.5), 20.0).rgb;
//         float farMix = smoothstep(5000.0, 18000.0, camDist);
//         terrainColor = mix(texNear, texAvg, farMix) * shade;
//     } else {
//         // Fallback: old procedural look (green valleys to brown hills).
//         vec3 valleyColor = vec3(0.15, 0.45, 0.15);
//         vec3 hillColor   = vec3(0.45, 0.35, 0.20);
//         terrainColor = mix(valleyColor, hillColor, clamp(h / 0.6, 0.0, 1.0));
//     }

//     // Distance fog into the horizon colour (must match the sky's horizon).
//     float fogNear = 12000.0;
//     float fogFar  = 80000.0;
//     float fog = clamp((camDist - fogNear) / (fogFar - fogNear), 0.0, 1.0);
//     vec3 horizonColor = vec3(0.42, 0.40, 0.36);
//     terrainColor = mix(terrainColor, horizonColor, fog);

//     fragColor = vec4(terrainColor, 1.0);
// }
// )GLSL";

//     if (!groundProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, gVsrc)) {
//         qWarning() << "[EOLand] vertex shader error:" << groundProgram.log();
//     }
//     if (!groundProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, gFsrc)) {
//         qWarning() << "[EOLand] fragment shader error:" << groundProgram.log();
//     }
//     if (!groundProgram.link()) {
//         qWarning() << "[EOLand] shader link error:" << groundProgram.log();
//     }

//     // 3. GENERATE A DENSE 3D MESH (CPU Side)
//     // 500km grid divided into 200x200 segments, so the ground still extends
//     // to the horizon at high altitude.
//     QVector<QVector3D> vertices;
//     int segments = 200;
//     float size = 500000.0f;
//     float halfSize = size / 2.0f;
//     float step = size / segments;

//     for (int i = 0; i < segments; ++i) {
//         for (int j = 0; j < segments; ++j) {
//             float x1 = -halfSize + j * step;
//             float z1 = -halfSize + i * step;
//             float x2 = x1 + step;
//             float z2 = z1 + step;

//             // Triangle 1
//             vertices.append(QVector3D(x1, 0.0f, z1));
//             vertices.append(QVector3D(x2, 0.0f, z1));
//             vertices.append(QVector3D(x1, 0.0f, z2));

//             // Triangle 2
//             vertices.append(QVector3D(x2, 0.0f, z1));
//             vertices.append(QVector3D(x2, 0.0f, z2));
//             vertices.append(QVector3D(x1, 0.0f, z2));
//         }
//     }

//     vertexCount = vertices.size();

//     // 4. PUSH TO GPU
//     groundVao.create();
//     groundVao.bind();

//     groundVbo.create();
//     groundVbo.bind();
//     groundVbo.allocate(vertices.constData(), vertexCount * sizeof(QVector3D));

//     int posLoc = groundProgram.attributeLocation("position");
//     groundProgram.enableAttributeArray(posLoc);
//     groundProgram.setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, 0);

//     groundVao.release();
//     groundVbo.release();
// }

// void EOLand::render(const QMatrix4x4 &projection,
//                     const QMatrix4x4 &cameraMatrix,
//                     const QVector2D &camEN,
//                     float headingRad)
// {
//     // Ground sits just below world Y=0 (simulation ground level).
//     QMatrix4x4 groundMatrix = cameraMatrix;
//     groundMatrix.translate(0.0f, -GROUND_DROP_METERS, 0.0f);

//     groundProgram.bind();
//     groundProgram.setUniformValue("mvpMatrix", projection * groundMatrix);
//     groundProgram.setUniformValue("uCamEN", camEN);
//     groundProgram.setUniformValue("uHeading", headingRad);
//     groundProgram.setUniformValue("uTileMeters", TEXTURE_TILE_METERS);
//     groundProgram.setUniformValue("uReliefMeters", RELIEF_METERS);

//     // Keep the UV origin small: shifting by a whole number of tiles gives
//     // identical texture lookups (wrap mode = Repeat) but avoids float
//     // precision loss when the aircraft is tens of kilometres from the origin.
//     const QVector2D uvOrigin(std::fmod(camEN.x(), TEXTURE_TILE_METERS),
//                              std::fmod(camEN.y(), TEXTURE_TILE_METERS));
//     groundProgram.setUniformValue("uUvOrigin", uvOrigin);

//     if (groundTexture) {
//         groundTexture->bind(0);
//         groundProgram.setUniformValue("uGroundTex", 0);
//         groundProgram.setUniformValue("uHasTexture", 1);
//     } else {
//         groundProgram.setUniformValue("uHasTexture", 0);
//     }

//     groundVao.bind();
//     glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//     groundVao.release();

//     if (groundTexture) {
//         groundTexture->release(0);
//     }
//     groundProgram.release();
// }
