package com.seanr.linuxmonitor.records;

// Mirrors CoreData on the local side
// Uses record keyword just like MonitorSnapshot
public record CoreDataRecord(float usagePercent, long userTime, long nice, long system, long idle,
        long iowait, long irq, long softirq, long steal, long active, long total) {
}
