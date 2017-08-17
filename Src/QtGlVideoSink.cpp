#include "Logger.h"
#include "QtGlVideoSink.h"


#include <GL/glu.h>


namespace Vreo
{


void QtGlVideoSink::addRef() throw()
{
    m_refs++;
}


void QtGlVideoSink::release() throw()
{
    if (!--m_refs)
    {
        delete this;
    }
}


QtGlVideoSink::~QtGlVideoSink()
{
    if (m_texture)
    {
        ::glDeleteTextures(1, &m_texture);
    }
}


QtGlVideoSink::QtGlVideoSink(QWidget* parent)
: QGLWidget(parent)
, m_refs(1)
, m_texture(0)
{

}


bool QtGlVideoSink::writeFrame(const VideoFrame& frm)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    m_currentFrame = frm;

    QMetaObject::invokeMethod(this, "updateGL");

    return true;
}


void QtGlVideoSink::initializeGL()
{
    ::glClearColor(0.0, 0.0, 0.0, 1.0);
    ::glGenTextures(1, &m_texture);
}


void QtGlVideoSink::paintGL()
{
    std::lock_guard<std::mutex> _lk(m_lock);

    ::glClear(GL_COLOR_BUFFER_BIT);

    if (!m_currentFrame.isValid())
    {
        return;
    }

    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();

    ::glEnable(GL_TEXTURE_RECTANGLE_ARB);

    ::glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texture);

    uint32_t w = m_currentFrame.format().width;
    uint32_t h = m_currentFrame.format().height;

    ::glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, m_currentFrame.data());

    GLfloat k = GLfloat(w) / h;

    ::glBegin(GL_QUADS);
        ::glTexCoord2f(0, 0);
        ::glVertex2f(1.0 * k, 1.0);
        ::glTexCoord2f(w, 0);
        ::glVertex2f(-1.0 * k, 1.0);
        ::glTexCoord2f(w, h);
        ::glVertex2f(-1.0 * k, -1.0);
        ::glTexCoord2f(0, h);
        ::glVertex2f(1.0 * k, -1.0);
    ::glEnd();

    ::glDisable(GL_TEXTURE_RECTANGLE_ARB);
}


void QtGlVideoSink::resizeGL(int w, int h)
{
    ONEVIEW_LOG_INFO("resizeGL %dx%d", w, h);

    if (h == 0) h = 1;
    GLfloat aspect = (GLfloat)w / (GLfloat)h;

    ::glViewport(0, 0, w, h);

    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();

    if (w >= h)
    {
        // aspect >= 1, set the height from -1 to 1, with larger width
        ::gluOrtho2D(1.0 * aspect, -1.0 * aspect, -1.0, 1.0);
    }
    else
    {
        // aspect < 1, set the width to -1 to 1, with larger height
        ::gluOrtho2D(1.0, -1.0, -1.0 / aspect, 1.0 / aspect);
    }
}


}
