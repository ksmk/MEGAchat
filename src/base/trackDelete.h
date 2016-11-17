#ifndef TRACKDELETE_H
#define TRACKDELETE_H

namespace karere
{
/** @brief Used to keep track of deletion of a lambda-captured object
  * pointer/reference - the instance may get deleted before the lambda is called
  * e.g. an attribute is fetched
  */
class DeleteTrackable
{
public:
    struct SharedData
    {
        bool mDeleted = false;
        uint32_t mRefCount = 0;
    };
    class Handle
    {
    protected:
        SharedData* mData;
        Handle(SharedData* shared)
        : mData(shared) { mData->mRefCount++; }
        Handle& operator=(Handle& other) = delete;
    public:
        Handle(const Handle& other): Handle(other.mData){}
        ~Handle()
        {
            if (--(mData->mRefCount) <= 0)
                delete mData;
        }
        bool deleted() const { return mData->mDeleted; }
        void throwIfDeleted() const
        {
            if (mData->mDeleted)
                throw std::runtime_error("TrackDelete: Instance has been deleted");
        }
        friend class DeleteTrackable;
    };

protected:
    Handle mSharedDataHandle;
public:
    Handle getDelTracker() const { return Handle(mSharedDataHandle.mData); }
    Handle weakHandle() const { return getDelTracker(); }
    DeleteTrackable(): mSharedDataHandle(new SharedData()){}
    ~DeleteTrackable() { mSharedDataHandle.mData->mDeleted = true; }
};

template <class T>
class WeakReferenceable
{
protected:
public:
    struct WeakRefSharedData
    {
        T* mPtr;
        int mRefCount = 0;
        WeakRefSharedData(T* aPtr): mPtr(aPtr){}
    };

    class WeakRefHandle
    {
    protected:
        WeakRefSharedData* mData;
        WeakRefHandle(WeakRefSharedData* data)
        : mData(data)
        {
            if (mData)
                mData->mRefCount++;
        }
        void unref()
        {
            if (!mData)
                return;
            if (--(mData->mRefCount) <= 0)
                delete mData;
        }
    public:
        friend class WeakReferenceable;
        WeakRefHandle(): mData(nullptr) {}
        WeakRefHandle(const WeakRefHandle& other): WeakRefHandle(other.mData){}
        WeakRefHandle& operator=(const WeakRefHandle& other)
        {
            unref();
            mData = other.mData;
            if (mData)
                mData->mRefCount++;
            return *this;
        }
        ~WeakRefHandle() { unref(); }
        bool isValid() const { return (mData && (mData->mPtr != nullptr)); }
        void reset()
        {
            unref();
            mData = nullptr;
        }
        void throwIfInvalid() const
        {
            if (!isValid())
                throw std::runtime_error("WeakRefHandle::isValid: Handle is invalid or referenced object has been deleted");
        }
        static WeakRefHandle invalid() { return WeakRefHandle(); }
        T& operator*()
        {
            throwIfInvalid();
            return mData->mPtr;
        }
        const T& operator*() const
        {
            throwIfInvalid();
            return mData->mPtr;
        }
        T* weakPtr()
        {
            return mData ? mData->mPtr : nullptr;
        }
        const T* weakPtr() const
        {
            return mData ? mData->mPtr : nullptr;
        }
        T* operator->() { return weakPtr(); }
        const T* operator->() const { return weakPtr(); }
    };
protected:
    WeakRefHandle mWeakRefHandle;
public:
    WeakReferenceable(T* target): mWeakRefHandle(new WeakRefSharedData(target)){}
    ~WeakReferenceable()
    {
        assert(mWeakRefHandle.isValid());
        mWeakRefHandle.mData->mPtr = nullptr;
    }
    WeakRefHandle getWeakHandle() const { return mWeakRefHandle; }
};
}

#endif