// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "OutputResource.h"

template<class T>
void OutputResourceList<T>::ReleaseDeviceDependentResources(Output<T> const& output)
{
    // Iterate in reverse order of creation.
    for (auto* p = m_last; p != nullptr; p = p->m_prev)
    {
        p->ReleaseDeviceDependentResources(output);
    }
}

template<class T>
void OutputResourceList<T>::CreateDeviceDependentResources(Output<T> const& output)
{
    // Iterate in order of creation.
    for (auto* p = m_first; p != nullptr; p = p->m_next)
    {
        p->CreateDeviceDependentResources(output);
    }
}

template<class T>
void OutputResourceList<T>::HandleRasterizationScaleChanged(Output<T> const& output)
{
    // Iterate in order of creation.
    for (auto* p = m_first; p != nullptr; p = p->m_next)
    {
        p->HandleRasterizationScaleChanged(output);
    }
}

// The ctor adds OutputResource to the OutputResourceList.
template<class T>
OutputResource<T>::OutputResource(std::shared_ptr<OutputResourceList<T>> const& resourceList) noexcept :
    m_resourceList(resourceList)
{
    if (m_resourceList->m_first != nullptr)
    {
        // Non-empty list: insert after the last.
        // This is so we invoke handlers in the order items were created.
        m_resourceList->m_last->m_next = this;
        m_prev = m_resourceList->m_last;
        m_resourceList->m_last = this;
    }
    else
    {
        // Empty list: this is now the first and last node.
        m_resourceList->m_first = this;
        resourceList->m_last = this;
    }
}

// The dtor removes the OutputResource from the OutputResourceList.
template<class T>
OutputResource<T>::~OutputResource()
{
    OutputResource*& forwardLink = m_prev != nullptr ?
        m_prev->m_next :
        m_resourceList->m_first;
    forwardLink = m_next;

    OutputResource*& backLink = m_next != nullptr ?
        m_next->m_prev :
        m_resourceList->m_last;
    backLink = m_prev;
}

// Explicit template instantiation.
template class OutputResourceList<winrt::Compositor>;
template class OutputResource<winrt::Compositor>;

template class OutputResourceList<winrt::WUC::Compositor>;
template class OutputResource<winrt::WUC::Compositor>;
