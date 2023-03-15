#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "src/allocator/context_manager/context_io_manager.h"
#include "src/allocator/context_manager/context_manager.h"
#include "src/allocator/context_manager/rebuild_ctx/rebuild_ctx.h"
#include "src/allocator/context_manager/segment_ctx/segment_ctx.h"
#include "src/include/address_type.h"
#include "src/journal_manager/journal_manager.h"
#include "src/meta_file_intf/mock_file_intf.h"
#include "test/integration-tests/allocator/address/allocator_address_info_tester.h"
#include "test/integration-tests/allocator/allocator_it_common.h"
#include "test/unit-tests/allocator/context_manager/allocator_ctx/allocator_ctx_mock.h"
#include "test/unit-tests/allocator/context_manager/allocator_file_io_mock.h"
#include "test/unit-tests/allocator/context_manager/block_allocation_status_mock.h"
#include "test/unit-tests/allocator/context_manager/context_io_manager_mock.h"
#include "test/unit-tests/allocator/context_manager/context_manager_mock.h"
#include "test/unit-tests/allocator/context_manager/context_replayer_mock.h"
#include "test/unit-tests/allocator/context_manager/gc_ctx/gc_ctx_mock.h"
#include "test/unit-tests/allocator/context_manager/rebuild_ctx/rebuild_ctx_mock.h"
#include "test/unit-tests/allocator/context_manager/segment_ctx/segment_ctx_mock.h"
#include "test/unit-tests/array_models/interface/i_array_info_mock.h"
#include "test/unit-tests/bio/volume_io_mock.h"
#include "test/unit-tests/event_scheduler/event_mock.h"
#include "test/unit-tests/event_scheduler/event_scheduler_mock.h"
#include "test/unit-tests/journal_manager/checkpoint/checkpoint_manager_mock.h"
#include "test/unit-tests/journal_manager/checkpoint/checkpoint_meta_flush_completed_mock.h"
#include "test/unit-tests/journal_manager/checkpoint/dirty_map_manager_mock.h"
#include "test/unit-tests/journal_manager/checkpoint/log_group_releaser_mock.h"
#include "test/unit-tests/journal_manager/config/journal_configuration_mock.h"
#include "test/unit-tests/journal_manager/journal_writer_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/buffer_write_done_notifier_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/callback_sequence_controller_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/i_journal_log_buffer_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/journal_log_buffer_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/log_buffer_io_context_factory_mock.h"
#include "test/unit-tests/journal_manager/log_buffer/log_write_context_factory_mock.h"
#include "test/unit-tests/journal_manager/log_write/buffer_offset_allocator_mock.h"
#include "test/unit-tests/journal_manager/log_write/journal_event_factory_mock.h"
#include "test/unit-tests/journal_manager/log_write/journal_volume_event_handler_mock.h"
#include "test/unit-tests/journal_manager/log_write/log_write_handler_mock.h"
#include "test/unit-tests/journal_manager/replay/replay_handler_mock.h"
#include "test/unit-tests/journal_manager/status/journal_status_provider_mock.h"
#include "test/unit-tests/lib/bitmap_mock.h"
#include "test/unit-tests/meta_file_intf/async_context_mock.h"
#include "test/unit-tests/meta_file_intf/meta_file_intf_mock.h"
#include "test/unit-tests/meta_service/i_meta_updater_mock.h"
#include "test/unit-tests/telemetry/telemetry_client/telemetry_client_mock.h"
#include "test/unit-tests/telemetry/telemetry_client/telemetry_publisher_mock.h"
#include "test/unit-tests/allocator/address/allocator_address_info_mock.h"

using namespace ::testing;
using testing::NiceMock;
using testing::_;
using testing::Return;

namespace pos
{
class ContextManagerIntegrationTestFixture : public testing::Test
{
public:
    ContextManagerIntegrationTestFixture(void) {};
    virtual ~ContextManagerIntegrationTestFixture(void) {};

    virtual void SetUp(void);
    virtual void TearDown(void);

protected:
    const uint32_t numOfSegment = 10;
    const uint32_t validBlockCount = 0;
    const uint32_t maxOccupiedStripeCount = 128;
    const uint32_t numOfStripesPerSegment = 10;
    const uint32_t arrayId = 0;
    const uint32_t numLogGroups = 2;

    PartitionLogicalSize partitionLogicalSize;

    JournalManager* journal;
    SegmentInfoData* segInfoDataForSegCtx;
    SegmentCtx* segCtx;
    ContextManager* ctxManager;
    CheckpointHandler* cpHandler;

    NiceMock<MockIArrayInfo>* arrayInfo;

    NiceMock<MockJournalConfiguration>* config;
    NiceMock<MockJournalStatusProvider>* statusProvider;
    NiceMock<MockLogWriteHandler>* logWriteHandler;
    NiceMock<MockJournalWriter>* journalWriter;
    NiceMock<MockLogWriteContextFactory>* logWriteContextFactory;
    NiceMock<MockLogBufferIoContextFactory>* logBufferIoContextFactory;
    NiceMock<MockJournalEventFactory>* journalEventFactory;
    NiceMock<MockJournalVolumeEventHandler>* volumeEventHandler;
    NiceMock<MockIJournalLogBuffer>* logBuffer;
    NiceMock<MockBufferOffsetAllocator>* bufferAllocator;
    NiceMock<MockLogGroupReleaser>* logGroupReleaser;
    NiceMock<MockCheckpointManager>* checkpointManager;

    NiceMock<MockDirtyMapManager>* dirtyMapManager;
    NiceMock<MockLogBufferWriteDoneNotifier>* logFilledNotifier;
    NiceMock<MockCallbackSequenceController>* callbackSequenceController;
    NiceMock<MockReplayHandler>* replayHandler;

    NiceMock<MockTelemetryPublisher>* tp;
    NiceMock<MockTelemetryClient>* tc;
    NiceMock<MockContextManager>* contextManager;
    NiceMock<MockIMetaUpdater>* metaUpdater;
    NiceMock<MockEventScheduler>* eventScheduler;

    NiceMock<MockAllocatorCtx>* allocCtx;
    NiceMock<MockRebuildCtx>* reCtx;
    NiceMock<MockGcCtx>* gcCtx;
    NiceMock<MockBlockAllocationStatus>* blockAllocStatus;
    NiceMock<MockContextIoManager>* ioManager;

    NiceMock<MockAllocatorAddressInfo>* addrInfo;
    NiceMock<MockCheckpointMetaFlushCompleted>* checkpointFlushCompleted;

    const int ALLOCATOR_META_ID = 1000;

private:
    void _InitializePartitionSize(void);
};

void
ContextManagerIntegrationTestFixture::SetUp(void)
{
    _InitializePartitionSize();

    arrayInfo = new NiceMock<MockIArrayInfo>;
    config = new NiceMock<MockJournalConfiguration>;

    EXPECT_CALL(*arrayInfo, GetSizeInfo(_)).WillRepeatedly(Return(&partitionLogicalSize));
    ON_CALL(*config, GetNumLogGroups).WillByDefault(Return(numLogGroups));
    EXPECT_CALL(*config, IsEnabled()).WillRepeatedly(Return(true));

    statusProvider = new NiceMock<MockJournalStatusProvider>;
    logWriteHandler = new NiceMock<MockLogWriteHandler>;
    journalWriter = new NiceMock<MockJournalWriter>;
    logWriteContextFactory = new NiceMock<MockLogWriteContextFactory>;
    logBufferIoContextFactory = new NiceMock<MockLogBufferIoContextFactory>;
    journalEventFactory = new NiceMock<MockJournalEventFactory>;
    volumeEventHandler = new NiceMock<MockJournalVolumeEventHandler>;
    logBuffer = new NiceMock<MockIJournalLogBuffer>;
    bufferAllocator = new NiceMock<MockBufferOffsetAllocator>;
    logGroupReleaser = new NiceMock<MockLogGroupReleaser>;
    checkpointManager = new NiceMock<MockCheckpointManager>;

    dirtyMapManager = new NiceMock<MockDirtyMapManager>;
    logFilledNotifier = new NiceMock<MockLogBufferWriteDoneNotifier>;
    callbackSequenceController = new NiceMock<MockCallbackSequenceController>;
    replayHandler = new NiceMock<MockReplayHandler>;

    tp = new NiceMock<MockTelemetryPublisher>;
    tc = new NiceMock<MockTelemetryClient>;
    contextManager = new NiceMock<MockContextManager>;
    metaUpdater = new NiceMock<MockIMetaUpdater>;

    allocCtx = new NiceMock<MockAllocatorCtx>();
    reCtx = new NiceMock<MockRebuildCtx>();
    gcCtx = new NiceMock<MockGcCtx>();
    blockAllocStatus = new NiceMock<MockBlockAllocationStatus>();
    ioManager = new NiceMock<MockContextIoManager>;
    addrInfo = new NiceMock<MockAllocatorAddressInfo>;
    eventScheduler = new NiceMock<MockEventScheduler>;

    ON_CALL(*addrInfo, GetArrayId).WillByDefault(Return(arrayId));
    segInfoDataForSegCtx = new SegmentInfoData[numOfSegment];
    for (int i = 0; i < numOfSegment; i++)
    {
        segInfoDataForSegCtx[i].Set(validBlockCount,
            maxOccupiedStripeCount, SegmentState::SSD);
    }

    segCtx = new SegmentCtx(tp, reCtx, addrInfo, gcCtx, segInfoDataForSegCtx);
    segCtx->SetEventScheduler(eventScheduler);

    journal = new JournalManager(config, statusProvider,
        logWriteContextFactory, logBufferIoContextFactory, journalEventFactory, logWriteHandler,
        volumeEventHandler, journalWriter,
        logBuffer, bufferAllocator, logGroupReleaser, checkpointManager,
        nullptr, dirtyMapManager, logFilledNotifier,
        callbackSequenceController, replayHandler, arrayInfo, tp);

    ctxManager = new ContextManager(tp, allocCtx, segCtx, reCtx,
        gcCtx, blockAllocStatus, ioManager, nullptr, nullptr, 0);

    cpHandler = new CheckpointHandler(ALLOCATOR_META_ID);;
    cpHandler->Init(nullptr, nullptr, ctxManager, nullptr);

    EXPECT_CALL(*contextManager, GetSegmentCtx()).WillRepeatedly(Return(segCtx));

    checkpointFlushCompleted =
        new NiceMock<MockCheckpointMetaFlushCompleted>(cpHandler, ALLOCATOR_META_ID);
}

void
ContextManagerIntegrationTestFixture::TearDown(void)
{
    if (nullptr != arrayInfo)
    {
        delete arrayInfo;
    }

    if (nullptr != tc)
    {
        delete tc;
    }

    if (nullptr != journal)
    {
        delete journal;
    }

    if (nullptr != contextManager)
    {
        delete contextManager;
    }

    if (nullptr != ioManager)
    {
        delete ioManager;
    }

    if (nullptr != addrInfo)
    {
        delete addrInfo;
    }

    if (nullptr != eventScheduler)
    {
        delete eventScheduler;
    }

    delete cpHandler;
}

void
ContextManagerIntegrationTestFixture::_InitializePartitionSize(void)
{
    partitionLogicalSize.minWriteBlkCnt = 0;
    partitionLogicalSize.blksPerChunk = 4;
    partitionLogicalSize.blksPerStripe = 16;
    partitionLogicalSize.chunksPerStripe = 4;
    partitionLogicalSize.stripesPerSegment = 2;
    partitionLogicalSize.totalStripes = 300;
    partitionLogicalSize.totalSegments = 300;
}

TEST_F(ContextManagerIntegrationTestFixture, DISABLED_GetRebuildTargetSegment_FreeUserDataSegment)
{
    // Constant
    const int TEST_SEG_CNT = 1;
    const int TEST_TRIAL = 100;

    // AllocatorAddressInfo (Mock)
    NiceMock<MockAllocatorAddressInfo>* allocatorAddressInfo = new NiceMock<MockAllocatorAddressInfo>();
    EXPECT_CALL(*allocatorAddressInfo, GetblksPerStripe).WillRepeatedly(Return(BLK_PER_STRIPE));
    EXPECT_CALL(*allocatorAddressInfo, GetchunksPerStripe).WillRepeatedly(Return(CHUNK_PER_STRIPE));
    EXPECT_CALL(*allocatorAddressInfo, GetnumWbStripes).WillRepeatedly(Return(WB_STRIPE));
    EXPECT_CALL(*allocatorAddressInfo, GetnumUserAreaStripes).WillRepeatedly(Return(USER_STRIPE));
    EXPECT_CALL(*allocatorAddressInfo, GetblksPerSegment).WillRepeatedly(Return(USER_BLOCKS));
    EXPECT_CALL(*allocatorAddressInfo, GetstripesPerSegment).WillRepeatedly(Return(STRIPE_PER_SEGMENT));
    EXPECT_CALL(*allocatorAddressInfo, GetnumUserAreaSegments).WillRepeatedly(Return(TEST_SEG_CNT));
    EXPECT_CALL(*allocatorAddressInfo, IsUT).WillRepeatedly(Return(true));

    // WbStripeCtx (Mock)
    NiceMock<MockAllocatorCtx>* allocatorCtx = new NiceMock<MockAllocatorCtx>();

    // GcCtx (Mock)
    NiceMock<MockGcCtx>* gcCtx = new NiceMock<MockGcCtx>();

    // ContextReplayer (Mock)
    NiceMock<MockContextReplayer>* contextReplayer = new NiceMock<MockContextReplayer>();

    // BlockAllocationStatus (Mock)
    NiceMock<MockBlockAllocationStatus>* blockAllocStatus = new NiceMock<MockBlockAllocationStatus>();

    // TelemetryPublisher (Mock)
    NiceMock<MockTelemetryPublisher>* telemetryPublisher = new NiceMock<MockTelemetryPublisher>();

    // RebuildCtx (Mock)
    NiceMock<MockRebuildCtx>* rebuildCtx = new NiceMock<MockRebuildCtx>();

    // SegmentCtx (Real)
    SegmentCtx* segmentCtx = new SegmentCtx(nullptr, rebuildCtx, allocatorAddressInfo, gcCtx);

    // Start Test
    for (int i = 0; i < TEST_TRIAL; ++i)
    {
        int nanoSec = std::rand() % 100;
        std::thread th1(&SegmentCtx::GetRebuildTargetSegment, segmentCtx);
        std::this_thread::sleep_for(std::chrono::nanoseconds(nanoSec));
        VirtualBlks blksToInvalidate = {
            .startVsa = {
                .stripeId = 0,
                .offset = 0},
            .numBlks = 1};
        std::thread th2(&SegmentCtx::InvalidateBlks, segmentCtx, blksToInvalidate, false);
        th1.join();
        th2.join();

        VirtualBlks blksToInvalidate2 = {
            .startVsa = {
                .stripeId = 3,
                .offset = 0},
            .numBlks = 1};
        std::thread th3(&SegmentCtx::InvalidateBlks, segmentCtx, blksToInvalidate2, false);
        std::this_thread::sleep_for(std::chrono::nanoseconds(nanoSec));
        std::thread th4(&SegmentCtx::GetRebuildTargetSegment, segmentCtx);
        th3.join();
        th4.join();
    }

    // Clean up
    delete allocatorAddressInfo;
}

TEST_F(ContextManagerIntegrationTestFixture, DISABLED_FlushContexts_FlushRebuildContext)
{
    NiceMock<MockAllocatorAddressInfo> allocatorAddressInfo;
    NiceMock<MockTelemetryPublisher> telemetryPublisher;

    NiceMock<MockAllocatorFileIo>* segmentCtxIo = new NiceMock<MockAllocatorFileIo>;
    NiceMock<MockAllocatorFileIo>* allocatorCtxIo = new NiceMock<MockAllocatorFileIo>;
    NiceMock<MockAllocatorFileIo>* rebuildCtxIo = new NiceMock<MockAllocatorFileIo>;

    ContextIoManager ioManager(&allocatorAddressInfo, &telemetryPublisher, eventScheduler, segmentCtxIo, allocatorCtxIo, rebuildCtxIo);

    ON_CALL(*eventScheduler, EnqueueEvent).WillByDefault([&](EventSmartPtr event) {
        event->Execute();
    });

    // Conditional variable to control the test timing
    std::mutex mut;
    std::condition_variable cv;
    bool rebuildFlushCompleted = false;
    std::atomic<int> numFlushedContexts(0);

    // Test set-up for testing FlushContexts
    EventSmartPtr checkpointCallback(new MockEvent());
    EXPECT_CALL(*(MockEvent*)(checkpointCallback.get()), Execute).Times(1);

    // Expects flushing allocator contexts to wait for rebuild context flush done
    AsyncMetaFileIoCtx* allocCtxFlush = new AsyncMetaFileIoCtx();
    auto allocCtxHeader = new AllocatorCtxHeader();
    allocCtxFlush->SetIoInfo(MetaFsIoOpcode::Write, 0, 0, (char*)allocCtxHeader);
    EXPECT_CALL(*allocatorCtxIo, Flush)
        .WillOnce([&](FnAllocatorCtxIoCompletion callback, ContextSectionBuffer externalBuf)
        {
            std::thread allocCtxFlushCallback([&]
            {
                {
                    std::unique_lock<std::mutex> lock(mut);
                    cv.wait(lock, [&] { return (rebuildFlushCompleted == true); });
                }

                numFlushedContexts++;
                cv.notify_all();
            });
            allocCtxFlushCallback.detach();
            return 0;
        });
    AsyncMetaFileIoCtx* segCtxFlush = new AsyncMetaFileIoCtx();
    auto segCtxHeader = new SegmentCtxHeader();
    segCtxFlush->SetIoInfo(MetaFsIoOpcode::Write, 0, sizeof(CtxHeader), (char*)segCtxHeader);
    EXPECT_CALL(*segmentCtxIo, Flush)
        .WillOnce([&](FnAllocatorCtxIoCompletion callback, ContextSectionBuffer externalBuf)
        {
            std::thread segCtxFlushCallback([&]
            {
                {
                    std::unique_lock<std::mutex> lock(mut);
                    cv.wait(lock, [&] { return (rebuildFlushCompleted == true); });
                }

                numFlushedContexts++;
                cv.notify_all();
            });
            segCtxFlushCallback.detach();
            return 0;
        });

    // When 1. Flushing contexts started by CheckpointHandler
    ioManager.FlushContexts(checkpointCallback, false, INVALID_CONTEXT_SECTION_BUFFER);

    // Test set-up for testing FlushRebuildContext
    // Expects flushing segment context to be completed right away
    AsyncMetaFileIoCtx* rebuildCtxFlush = new AsyncMetaFileIoCtx();
    auto rebuildCtxHeader = new RebuildCtxHeader();
    rebuildCtxFlush->SetIoInfo(MetaFsIoOpcode::Write, 0, sizeof(CtxHeader), (char*)rebuildCtxHeader);
    EXPECT_CALL(*rebuildCtxIo, Flush)
        .WillOnce([&](FnAllocatorCtxIoCompletion callback, ContextSectionBuffer externalBuf)
        {
            callback();

            rebuildFlushCompleted = true;
            cv.notify_all();
            return 0;
        });

    // When 2. Flushing rebuild contexts started
    // ioManager.FlushRebuildContext(nullptr, false);

    // Wait for all flush completed
    {
        std::unique_lock<std::mutex> lock(mut);
        cv.wait(lock, [&] { return (numFlushedContexts == 2); });
    }

    // Then, checkpointCallback should be called
}

TEST(ContextManagerIntegrationTest, UpdateSegmentContext_testIfSegmentOverwritten)
{
    // Given
    NiceMock<MockAllocatorAddressInfo> addrInfo;
    uint32_t numSegments = 10;
    ON_CALL(addrInfo, GetnumUserAreaSegments).WillByDefault(Return(numSegments));

    NiceMock<MockTelemetryPublisher> telemetryPublisher;
    NiceMock<MockEventScheduler> eventScheduler;

    NiceMock<MockContextIoManager>* ioManager = new NiceMock<MockContextIoManager>;
    NiceMock<MockAllocatorCtx>* allocatorCtx = new NiceMock<MockAllocatorCtx>;
    NiceMock<MockRebuildCtx>* rebuildCtx = new NiceMock<MockRebuildCtx>;
    NiceMock<MockGcCtx>* gcCtx = new NiceMock<MockGcCtx>;
    NiceMock<MockBlockAllocationStatus>* blockAllocStatus = new NiceMock<MockBlockAllocationStatus>();
    NiceMock<MockContextReplayer>* contextReplayer = new NiceMock<MockContextReplayer>;
    NiceMock<MockTelemetryPublisher> tp;
    SegmentCtx* segmentCtx = new SegmentCtx(&tp, rebuildCtx, &addrInfo, gcCtx);
    ON_CALL(addrInfo, IsUT).WillByDefault(Return(true));

    std::mutex allocatorLock;
    ON_CALL(*allocatorCtx, GetCtxLock).WillByDefault(ReturnRef(allocatorLock));

    ContextManager contextManager(&telemetryPublisher, allocatorCtx, segmentCtx, rebuildCtx,
        gcCtx, blockAllocStatus, ioManager, contextReplayer, &addrInfo, ARRAY_ID);
    contextManager.Init();
    segmentCtx->SetEventScheduler(&eventScheduler);

    // Set valid block count of each segments to 32 for test
    uint32_t maxValidBlkCount = 32;
    ON_CALL(addrInfo, GetblksPerSegment).WillByDefault(Return(maxValidBlkCount));
    ON_CALL(addrInfo, GetstripesPerSegment).WillByDefault(Return(STRIPE_PER_SEGMENT));

    for (SegmentId segId = 0; segId < numSegments; segId++)
    {
        VirtualBlks blks = {
            .startVsa = {
                .stripeId = segId * STRIPE_PER_SEGMENT,
                .offset = 0},
            .numBlks = maxValidBlkCount};
        segmentCtx->ValidateBlks(blks);
    }

    // When : All stripes in each segment is occupied
    ON_CALL(addrInfo, GetstripesPerSegment).WillByDefault(Return(STRIPE_PER_SEGMENT));
    for (StripeId lsid = 0; lsid < STRIPE_PER_SEGMENT * numSegments; lsid++)
    {
        segmentCtx->UpdateOccupiedStripeCount(lsid);
    }

    // Then: State of occupied segments must be SSD
    SegmentState expectedState = SegmentState::SSD;
    for (SegmentId segId = 0; segId < numSegments; segId++)
    {
        SegmentState actualState = segmentCtx->GetSegmentState(segId);
        EXPECT_EQ(expectedState, actualState);
    }

    // When: All of segments is overwritten
    for (SegmentId segId = 0; segId < numSegments; segId++)
    {
        VirtualBlkAddr startVsa = {
            .stripeId = segId * STRIPE_PER_SEGMENT,
            .offset = 0,
        };
        VirtualBlks blks = {
            .startVsa = startVsa,
            .numBlks = maxValidBlkCount,
        };

        EXPECT_CALL(eventScheduler, EnqueueEvent);
        segmentCtx->InvalidateBlks(blks, false);
    }

    // Then: State of overwritten segments must be FREE and occupied stripe count is zero
    expectedState = SegmentState::FREE;
    int expectedOccupiedCount = 0;
    for (SegmentId segId = 0; segId < numSegments; segId++)
    {
        SegmentState actualState = segmentCtx->GetSegmentState(segId);
        EXPECT_EQ(expectedState, actualState);

        int actualOccupiedCount = segmentCtx->GetOccupiedStripeCount(segId);
        EXPECT_EQ(expectedOccupiedCount, actualOccupiedCount);
    }
}
} // namespace pos
