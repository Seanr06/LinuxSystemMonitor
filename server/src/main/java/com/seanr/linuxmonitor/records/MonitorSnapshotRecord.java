package com.seanr.linuxmonitor.records;

import java.util.List;

// Mirrors the main data full monitor snapshot
// Uses record which expands for getters, final fields, equals, toString all for free without having
// to build them
// Jackson which is a library in springboot parses the json string and calls getRecordComponents() the fills the constructors by
// matching with the .json
public record MonitorSnapshotRecord(long timeCapture, CoreDataRecord totalCpu,
        List<CoreDataRecord> cores, long memTotal, long memFree, long memAvailable, long cached,
        long buffers, long swapTotal, long swapFree, long active, long inactive, long activeAnon,
        long inactiveAnon, long activeFile, long inactiveFile, long slab, long sreclaimable,
        long sunreclaim, long dirty, long writeback, long anonPages, long mapped, long shmem,
        String kernelInfo, String osInfo, float lastMinuteLoad, float lastFiveMinutesLoad,
        float lastFifteenMinutesLoad, float upTime, float idleTime, long procsRunning,
        long procsBlocked, double ctxtPerSecond, double interruptionsPerSecond) {
}
