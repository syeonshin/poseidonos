/*
 *   BSD LICENSE
 *   Copyright (c) 2021 Samsung Electronics Corporation
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Samsung Electronics Corporation nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SMART_COLLECTOR_H_
#define SMART_COLLECTOR_H_

#include <string>

#include "src/lib/singleton.h"

struct spdk_nvme_cpl;
struct spdk_nvme_health_information_page;
struct spdk_nvme_ctrlr;

namespace pos
{
class TelemetryPublisher;
class TelemetryClient;

enum class SmartReturnType
{
    SUCCESS = 0,
    SEND_ERR,
    RESPONSE_ERR,
    CTRL_NOT_EXIST
};

class SmartCollector
{
public:
    SmartCollector(void);
    virtual ~SmartCollector(void);
    void PublishSmartDataToTelemetry(void);
    SmartReturnType CollectPerCtrl(spdk_nvme_health_information_page* payload, spdk_nvme_ctrlr* ctrlr);

private:
    static void CompleteSmartLogPage(void* arg, const spdk_nvme_cpl* cpl);
    TelemetryClient* telemetryClient = nullptr;
    TelemetryPublisher* publisher = nullptr;
};
using SmartCollectorSingleton = Singleton<SmartCollector>;
} // namespace pos

#endif // SMART_COLLECTOR_H_
