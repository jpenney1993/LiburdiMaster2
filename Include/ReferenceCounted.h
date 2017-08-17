#pragma once

#include <QDebug>


namespace Vreo
{




struct IReferenceCounted
{
    virtual void                            addRef() throw() = 0;
    virtual void                            release() throw() = 0;

protected:
    virtual ~IReferenceCounted() throw() { }
};



template<class T>
class ReferenceCountedPtr
{
public:
    ReferenceCountedPtr() throw()
    : m_p(nullptr)
    {
    }

    ReferenceCountedPtr(T* p, bool addRef) throw()
    : m_p(p)
    {
        if (m_p && addRef) m_p->addRef();
    }

    ReferenceCountedPtr(const ReferenceCountedPtr<T>& other) throw()
    : m_p(other.get())
    {
        if (m_p) m_p->addRef();
    }

    ReferenceCountedPtr& operator=(const ReferenceCountedPtr<T>& other) throw()
    {
        T* pTemp = m_p;
        m_p = other.m_p;
        if (pTemp) pTemp->release();
        if (m_p) m_p->addRef();

        return *this;
    }

    ~ReferenceCountedPtr() throw()
    {
        if (m_p) m_p->release();
    }

    operator T*() const throw()
    {
        return m_p;
    }

    T& operator*() const
    {
        Q_ASSERT(m_p != nullptr);
        return *m_p;
    }

    T** operator&() throw()
    {
        Q_ASSERT(m_p == nullptr);
        return &m_p;
    }

    T* operator->() const throw()
    {
        Q_ASSERT(m_p != nullptr);
        return m_p;
    }

    operator bool() const throw()
    {
        return (m_p != nullptr);
    }

    bool operator!() const throw()
    {
        return (m_p == nullptr);
    }

    T* get() throw()
    {
        return m_p;
    }

    void swap(ReferenceCountedPtr<T>& other) throw()
    {
        T* pTemp = m_p;
        m_p = other.m_p;
        other.m_p = pTemp;
    }

    void release() throw()
    {
        T* pTemp = m_p;
        if (pTemp)
        {
            m_p = nullptr;
            pTemp->release();
        }
    }

    void attach(T* p) throw()
    {
        if (m_p)
        {
            m_p->release();
        }

        m_p = p;
    }

    T* detach() throw()
    {
        T* pt = m_p;
        m_p = nullptr;
        return pt;
    }

private:
    T*                          m_p;
};



}
