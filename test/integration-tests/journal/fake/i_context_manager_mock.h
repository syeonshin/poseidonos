#include <gmock/gmock.h>

#include <list>
#include <string>
#include <vector>

#include "src/allocator/i_context_manager.h"

namespace pos
{
class IContextManagerMock : public IContextManager
{
public:
    using IContextManager::IContextManager;
    MOCK_METHOD(int, FlushContexts, (EventSmartPtr callback, bool sync), (override));

    virtual void UpdateOccupiedStripeCount(StripeId lsid) {}
    virtual SegmentId AllocateFreeSegment(void) { return 0; }
    virtual SegmentId AllocateGCVictimSegment(void) { return 0; }
    virtual SegmentId AllocateRebuildTargetSegment(void) { return 0; }
    virtual int ReleaseRebuildSegment(SegmentId segmentId) { return 0; }
    virtual int MakeRebuildTarget(void) { return 0; }
    virtual int StopRebuilding(void) { return 0; }
    virtual bool NeedRebuildAgain(void) { return true; }
    virtual uint32_t GetRebuildTargetSegmentCount(void) { return 0; }
    virtual int GetNumOfFreeSegment(bool needLock) { return 0; }
    virtual GcMode GetCurrentGcMode(void) { return MODE_NO_GC; }
    virtual int GetGcThreshold(GcMode mode) { return 0; }
    virtual uint64_t GetStoredContextVersion(int owner) { return 0; }
    IContextManagerMock(void)
    {
        ON_CALL(*this, FlushContexts).WillByDefault(::testing::Invoke(this,
        &IContextManagerMock::_FlushContexts));
    }

private:
    int _FlushContexts(EventSmartPtr callback, bool sync)
    {
        if (callback != nullptr)
        {
            bool result = callback->Execute();
            if (result == false)
            {
                return -1;
            }
        }
        return 0;
    }
};

} // namespace pos
