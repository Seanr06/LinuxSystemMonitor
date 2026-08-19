package com.seanr.linuxmonitor.entities;

import jakarta.persistence.*;

@Embeddable // Tells hibernate that this class will not have an identity and is not a separate entry
            // so it can never get its own table
public class CoreDataEntity {
    private float usagePercent;

    private long userTime;
    private long nice;
    private long system;
    private long idle;
    private long iowait;
    private long irq;
    private long softirq;
    private long steal;

    private long active;
    private long total;

    // Setters and getters
    public float getUsagePercent() {
        return usagePercent;
    }

    public void setUsagePercent(float usagePercent) {
        this.usagePercent = usagePercent;
    }

    public long getUserTime() {
        return userTime;
    }

    public void setUserTime(long userTime) {
        this.userTime = userTime;
    }

    public long getNice() {
        return nice;
    }

    public void setNice(long nice) {
        this.nice = nice;
    }

    public long getSystem() {
        return system;
    }

    public void setSystem(long system) {
        this.system = system;
    }

    public long getIdle() {
        return idle;
    }

    public void setIdle(long idle) {
        this.idle = idle;
    }

    public long getIowait() {
        return iowait;
    }

    public void setIowait(long iowait) {
        this.iowait = iowait;
    }

    public long getIrq() {
        return irq;
    }

    public void setIrq(long irq) {
        this.irq = irq;
    }

    public long getSoftirq() {
        return softirq;
    }

    public void setSoftirq(long softirq) {
        this.softirq = softirq;
    }

    public long getSteal() {
        return steal;
    }

    public void setSteal(long steal) {
        this.steal = steal;
    }

    public long getActive() {
        return active;
    }

    public void setActive(long active) {
        this.active = active;
    }

    public long getTotal() {
        return total;
    }

    public void setTotal(long total) {
        this.total = total;
    }
}
