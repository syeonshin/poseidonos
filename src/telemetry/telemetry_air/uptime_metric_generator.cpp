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

#include "uptime_metric_generator.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/logger/logger.h"
#include "src/include/pos_event_id.h"
#include "src/telemetry/telemetry_client/pos_metric.h"

using namespace pos;
using namespace std;

UptimeMetricGenerator::UptimeMetricGenerator(VersionProvider* vp)
: vp(vp)
{
    started = getProcessStartTime();
}

UptimeMetricGenerator::~UptimeMetricGenerator(void)
{
}


int
UptimeMetricGenerator::Generate(POSMetric* m)
{
    time_t current = time(nullptr);
    time_t uptime = current - started;
    string version = vp->GetVersion();

    if (uptime <= 0 || version == "")
    {
        return -1;
    }

    if (m == nullptr)
    {
        return -1;
    }
    m->SetName(TEL05000_COMMON_PROCESS_UPTIME_SECOND);
    m->SetType(POSMetricTypes::MT_COUNT);
    m->SetCountValue(uptime);
    m->AddLabel("version", version);

    return 0;
}

time_t
UptimeMetricGenerator::getProcessStartTime(void)
{
    const string PROC_PATH= "/proc/";
    string pid = to_string(getpid());
    string statPath = PROC_PATH + pid;

    struct stat s;
    int ret = stat(statPath.c_str(), &s);
    if(ret != 0)
    {
        POS_TRACE_ERROR(EID(TELEMETRY_WARNING_MSG), "Faild to get process start time");
        return 0;
    }

    return s.st_mtim.tv_sec;
}