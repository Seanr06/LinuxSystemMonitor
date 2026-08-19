package com.seanr.linuxmonitor.entities;

import jakarta.persistence.*; // Access to annotations like @Entity
import java.util.List;

@Entity // Tells Hibernate that this is a class that it needs to handle
@Table(name = "monitor_snapshot") // Names the postgres table monitor_snapshot
public class MonitorSnapshotEntity {
    @Id // Tells hibernate to treat private Long id as an entries unique identifier
    @GeneratedValue(strategy = GenerationType.IDENTITY) // Produces a unique identifier by
                                                        // incrementing
    private Long id; // ID for the entity

    private long timeCapture; // time it was captured

    @Embedded // Ensures that the fields in CoreDataEntity class which is annotated as
              // @Embeddable
              // also gets its own spots in the table instead of getting its own separate
              // table
    private CoreDataEntity totalCpu; // Applies to totalCpu

    @ElementCollection // Multiple @Embeddable so this tells hibernate to treat the list of cores as
                       // its own table
    @OrderColumn(name = "core_index") // Adds an extra integer column to that side table section in
                                      // order to remember the order of the cores because the cores cant actually fit
                                      // inside the main datatable
    private List<CoreDataEntity> cores;

    private long memTotal;
    private long memFree;
    private long memAvailable;
    private long cached;
    private long buffers;
    private long swapTotal;
    private long swapFree;
    private long memActive;
    private long inactive;
    private long activeAnon;
    private long inactiveAnon;
    private long activeFile;
    private long inactiveFile;
    private long slab;
    private long sreclaimable;
    private long sunreclaim;
    private long dirty;
    private long writeback;
    private long anonPages;
    private long mapped;
    private long shmem;
    private String kernelInfo;
    private String osInfo;
    private float lastMinuteLoad;
    private float lastFiveMinutesLoad;
    private float lastFifteenMinutesLoad;
    private float upTime;
    private float idleTime;
    private long procsRunning;
    private long procsBlocked;
    private double ctxtPerSecond;
    private double interruptionsPerSecond;

    // Setters and getters for the entity
    public Long getId() {
        return id;
    }

    public long getTimeCapture() {
        return timeCapture;
    }

    public void setTimeCapture(long timeCapture) {
        this.timeCapture = timeCapture;
    }

    public CoreDataEntity getTotalCpu() {
        return totalCpu;
    }

    public void setTotalCpu(CoreDataEntity totalCpu) {
        this.totalCpu = totalCpu;
    }

    public List<CoreDataEntity> getCores() {
        return cores;
    }

    public void setCores(List<CoreDataEntity> cores) {
        this.cores = cores;
    }

    public long getMemTotal() {
        return memTotal;
    }

    public void setMemTotal(long memTotal) {
        this.memTotal = memTotal;
    }

    public long getMemFree() {
        return memFree;
    }

    public void setMemFree(long memFree) {
        this.memFree = memFree;
    }

    public long getMemAvailable() {
        return memAvailable;
    }

    public void setMemAvailable(long memAvailable) {
        this.memAvailable = memAvailable;
    }

    public long getCached() {
        return cached;
    }

    public void setCached(long cached) {
        this.cached = cached;
    }

    public long getBuffers() {
        return buffers;
    }

    public void setBuffers(long buffers) {
        this.buffers = buffers;
    }

    public long getSwapTotal() {
        return swapTotal;
    }

    public void setSwapTotal(long swapTotal) {
        this.swapTotal = swapTotal;
    }

    public long getSwapFree() {
        return swapFree;
    }

    public void setSwapFree(long swapFree) {
        this.swapFree = swapFree;
    }

    public long getMemActive() {
        return memActive;
    }

    public void setMemActive(long memActive) {
        this.memActive = memActive;
    }

    public long getInactive() {
        return inactive;
    }

    public void setInactive(long inactive) {
        this.inactive = inactive;
    }

    public long getActiveAnon() {
        return activeAnon;
    }

    public void setActiveAnon(long activeAnon) {
        this.activeAnon = activeAnon;
    }

    public long getInactiveAnon() {
        return inactiveAnon;
    }

    public void setInactiveAnon(long inactiveAnon) {
        this.inactiveAnon = inactiveAnon;
    }

    public long getActiveFile() {
        return activeFile;
    }

    public void setActiveFile(long activeFile) {
        this.activeFile = activeFile;
    }

    public long getInactiveFile() {
        return inactiveFile;
    }

    public void setInactiveFile(long inactiveFile) {
        this.inactiveFile = inactiveFile;
    }

    public long getSlab() {
        return slab;
    }

    public void setSlab(long slab) {
        this.slab = slab;
    }

    public long getSreclaimable() {
        return sreclaimable;
    }

    public void setSreclaimable(long sreclaimable) {
        this.sreclaimable = sreclaimable;
    }

    public long getSunreclaim() {
        return sunreclaim;
    }

    public void setSunreclaim(long sunreclaim) {
        this.sunreclaim = sunreclaim;
    }

    public long getDirty() {
        return dirty;
    }

    public void setDirty(long dirty) {
        this.dirty = dirty;
    }

    public long getWriteback() {
        return writeback;
    }

    public void setWriteback(long writeback) {
        this.writeback = writeback;
    }

    public long getAnonPages() {
        return anonPages;
    }

    public void setAnonPages(long anonPages) {
        this.anonPages = anonPages;
    }

    public long getMapped() {
        return mapped;
    }

    public void setMapped(long mapped) {
        this.mapped = mapped;
    }

    public long getShmem() {
        return shmem;
    }

    public void setShmem(long shmem) {
        this.shmem = shmem;
    }

    public String getKernelInfo() {
        return kernelInfo;
    }

    public void setKernelInfo(String kernelInfo) {
        this.kernelInfo = kernelInfo;
    }

    public String getOsInfo() {
        return osInfo;
    }

    public void setOsInfo(String osInfo) {
        this.osInfo = osInfo;
    }

    public float getLastMinuteLoad() {
        return lastMinuteLoad;
    }

    public void setLastMinuteLoad(float lastMinuteLoad) {
        this.lastMinuteLoad = lastMinuteLoad;
    }

    public float getLastFiveMinutesLoad() {
        return lastFiveMinutesLoad;
    }

    public void setLastFiveMinutesLoad(float lastFiveMinutesLoad) {
        this.lastFiveMinutesLoad = lastFiveMinutesLoad;
    }

    public float getLastFifteenMinutesLoad() {
        return lastFifteenMinutesLoad;
    }

    public void setLastFifteenMinutesLoad(float lastFifteenMinutesLoad) {
        this.lastFifteenMinutesLoad = lastFifteenMinutesLoad;
    }

    public float getUpTime() {
        return upTime;
    }

    public void setUpTime(float upTime) {
        this.upTime = upTime;
    }

    public float getIdleTime() {
        return idleTime;
    }

    public void setIdleTime(float idleTime) {
        this.idleTime = idleTime;
    }

    public long getProcsRunning() {
        return procsRunning;
    }

    public void setProcsRunning(long procsRunning) {
        this.procsRunning = procsRunning;
    }

    public long getProcsBlocked() {
        return procsBlocked;
    }

    public void setProcsBlocked(long procsBlocked) {
        this.procsBlocked = procsBlocked;
    }

    public double getCtxtPerSecond() {
        return ctxtPerSecond;
    }

    public void setCtxtPerSecond(double ctxtPerSecond) {
        this.ctxtPerSecond = ctxtPerSecond;
    }

    public double getInterruptionsPerSecond() {
        return interruptionsPerSecond;
    }

    public void setInterruptionsPerSecond(double interruptionsPerSecond) {
        this.interruptionsPerSecond = interruptionsPerSecond;
    }
}
