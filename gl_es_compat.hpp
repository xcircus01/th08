#pragma once
//
// gl_es_compat.hpp — GLES-safe replacements for the small set of desktop-only
// legacy GL calls used by the D3D8 compatibility layer (src/modern/linux/d3d8_compat.cpp).
//
// OpenGL ES (1.x and later) has no immediate mode (glBegin/glEnd/glVertex*/
// glTexCoord*) and no attribute stack (glPushAttrib/glPopAttrib). Both are
// used by the desktop Linux backend. This header provides drop-in
// equivalents built on client-side vertex arrays and manual state
// save/restore, which are valid on:
//   - desktop OpenGL 1.x/compat profile (current Linux port)
//   - OpenGL ES 1.1 "Common" profile (Android via SDL2/EGL)
//
// Nothing here requires shaders, so it also does not change behavior on the
// existing Linux build. It only removes calls that GLES cannot express.
//
#ifdef TH08_DESKTOP_GL
#include <GL/gl.h>
#else
#include <GLES/gl.h>  // Android (SDL2 creates a GLES1.1-capable EGL context)
#endif

// -----------------------------------------------------------------------
// GlStateGuard — replacement for glPushAttrib(GL_ALL_ATTRIB_BITS)/glPopAttrib().
//
// The two call sites that used push/pop attrib (ReadBackbuffer's full-screen
// blit and the framebuffer-to-window Present blit) only ever touch this
// fixed, known set of state. Every other draw call goes through
// LinuxDirect3DDevice8::PrepareState(), which unconditionally reprograms all
// of this before the next real draw — so a full attribute stack was never
// required, just isolation around these two blits.
// -----------------------------------------------------------------------
struct GlStateGuard
{
    GLboolean alphaTest, blend, cullFace, depthTest, lighting, scissorTest;
    GLboolean depthMask;
    GLboolean texture2D;
    GLint boundTexture;
    GLint texEnvMode;
    GLint minFilter, magFilter;

    GlStateGuard()
    {
        alphaTest = glIsEnabled(GL_ALPHA_TEST);
        blend = glIsEnabled(GL_BLEND);
        cullFace = glIsEnabled(GL_CULL_FACE);
        depthTest = glIsEnabled(GL_DEPTH_TEST);
        lighting = glIsEnabled(GL_LIGHTING);
        scissorTest = glIsEnabled(GL_SCISSOR_TEST);
        texture2D = glIsEnabled(GL_TEXTURE_2D);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
        glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texEnvMode);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    }

    ~GlStateGuard()
    {
        (alphaTest ? glEnable : glDisable)(GL_ALPHA_TEST);
        (blend ? glEnable : glDisable)(GL_BLEND);
        (cullFace ? glEnable : glDisable)(GL_CULL_FACE);
        (depthTest ? glEnable : glDisable)(GL_DEPTH_TEST);
        (lighting ? glEnable : glDisable)(GL_LIGHTING);
        (scissorTest ? glEnable : glDisable)(GL_SCISSOR_TEST);
        glDepthMask(depthMask);
        (texture2D ? glEnable : glDisable)(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, boundTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texEnvMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    GlStateGuard(const GlStateGuard &) = delete;
    GlStateGuard &operator=(const GlStateGuard &) = delete;
};

// -----------------------------------------------------------------------
// DrawTexturedQuad — replacement for the two `glBegin(GL_TRIANGLE_STRIP) /
// glTexCoord2f / glVertex2f x4 / glEnd()` full-screen blit blocks.
//
// u0/v0/u1/v1 select the texture-coordinate corners so callers keep the same
// flip behavior the original code had (ReadBackbuffer vs. Present flip the Y
// axis oppositely).
// -----------------------------------------------------------------------
inline void DrawTexturedQuad(float width, float height,
                              float u0, float v0, float u1, float v1)
{
    const GLfloat positions[8] = {
        0.0f,  0.0f,
        width, 0.0f,
        0.0f,  height,
        width, height,
    };
    const GLfloat texcoords[8] = {
        u0, v0,
        u1, v0,
        u0, v1,
        u1, v1,
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, positions);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// -----------------------------------------------------------------------
// DrawFvfVertices — replacement for the per-vertex glBegin/.../glEnd loop in
// LinuxDirect3DDevice8::Draw(), the main game geometry path (sprites,
// bullets, background, UI — everything).
//
// Builds interleaved-free position/color/texcoord client arrays from the
// already-decoded per-vertex values and issues a single glDrawArrays call.
//
// Known trade-off: the original loop called an EXT_fog_coord entry point
// (g_fogCoordf) per vertex when linear vertex fog was enabled. That
// extension does not exist under GLES at all, so g_fogCoordf is already
// always NULL on Android (SDL_GL_GetProcAddress simply won't find it) and
// vertex fog is already a no-op there before this change. This array-based
// path does not attempt to preserve that desktop-only fog path either, so
// on the existing Linux build, scenes that rely on per-vertex linear fog
// (D3DRS_FOGENABLE + D3DRS_FOGVERTEXMODE = D3DFOG_LINEAR) will stop applying
// fog. That render state is rarely hit in TH08; verify against the fog
// ledger/effects list before shipping this on Linux too.
// -----------------------------------------------------------------------
struct FvfVertex
{
    GLfloat x, y, z;
    GLubyte r, g, b, a;
    GLfloat u, v;
};

inline void DrawFvfVertices(GLenum mode, const FvfVertex *vertices, unsigned count, bool hasTexture)
{
    if (count == 0) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(FvfVertex), &vertices[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(FvfVertex), &vertices[0].r);

    if (hasTexture)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(FvfVertex), &vertices[0].u);
    }

    glDrawArrays(mode, 0, static_cast<GLsizei>(count));

    if (hasTexture) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}
