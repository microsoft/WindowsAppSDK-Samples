// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once
#include "SettingCollection.h"

template <class T>
class Output;
using LiftedOutput = Output<winrt::Compositor>;
using SystemOutput = Output<winrt::WUC::Compositor>;

template <class T>
class OutputResource;
using LiftedOutputResource = OutputResource<winrt::Compositor>;
using SystemOutputResource = OutputResource<winrt::WUC::Compositor>;

// Linked list of all active OutputResource objects.
//
// An OutputResource is an object that owns device-dependent resources or needs to be notified
// of rasterization scale changes. An OutputResourceList is used to track the OutputResource
// objects associated with a particular Output object.
//
// Note: Unlike std::list or other standard containers, this list does not "own" or otherwise
//       affect the lifetime of the objects in the list. It merely keeps track of the objects
//       that do exist. Each OutputResource adds itself to the list in its constructor and
//       removes itself from the list in its destructor.
template <class T>
class OutputResourceList
{
public:

    // Invokes ReleaseDeviceDependentResources on each OutputResource in reverse order of creation.
    void ReleaseDeviceDependentResources(Output<T> const& output);

    // Invokes EnsureInitialized on each OutputResource in order of creation.
    void EnsureInitialized(Output<T> const& output);

    // Invokes OnSettingChanged on each OutputResource in order of creation.
    void OnSettingChanged(Output<T> const& output, SettingId id);

private:
    friend class OutputResource<T>;

    OutputResource<T>* m_first = nullptr;
    OutputResource<T>* m_last = nullptr;
};

// OutputResource is the base class for objects that own device-dependent resources or that
// need to be notified of rasterization scale changes.
template<class T>
class OutputResource
{
public:
    // The ctor adds OutputResource to the OutputResourceList.
    OutputResource(std::shared_ptr<OutputResourceList<T>> const& resourceList) noexcept;

    // The dtor removes the OutputResource from the OutputResourceList.
    ~OutputResource();

    // Not copyable.
    OutputResource(OutputResource<T> const&) = delete;
    void operator=(OutputResource<T> const&) = delete;

    // Not moveable.
    OutputResource(OutputResource<T>&&) = delete;
    void operator=(OutputResource<T>&&) = delete;

    virtual void ReleaseDeviceDependentResources(Output<T> const&)
    {
    }

    virtual void EnsureInitialized(Output<T> const&)
    {
    }

    virtual void OnSettingChanged(Output<T> const&, SettingId)
    {
    }

private:
    friend class OutputResourceList<T>;
    std::shared_ptr<OutputResourceList<T>> m_resourceList;
    OutputResource<T>* m_prev = nullptr;
    OutputResource<T>* m_next = nullptr;
};
using LiftedOutputResourceList = OutputResourceList<winrt::Compositor>;
using SystemOutputResourceList = OutputResourceList<winrt::WUC::Compositor>;
