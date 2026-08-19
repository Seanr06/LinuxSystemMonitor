package com.seanr.linuxmonitor.services;

import com.seanr.linuxmonitor.entities.MonitorSnapshotEntity;
import com.seanr.linuxmonitor.records.MonitorSnapshotRecord;
import com.seanr.linuxmonitor.repositories.MonitorSnapshotRepository;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

@Service // Tells springboot that this class actually handles the logic behind my service
         // and that this class will be injected elsewhere
public class MonitorSnapshotService {
    private final MonitorSnapshotRepository repository;

    // Grab the repository so that JPA and my own find method can be used by the
    // controller
    public MonitorSnapshotService(MonitorSnapshotRepository repository) {
        // Can only be assigned once because repository is final
        this.repository = repository;
    }

    // Creates a new entity for the table and save it in the entire postgre data
    // table
    public MonitorSnapshotEntity saveSnapshot(MonitorSnapshotRecord snapshotRecord) {
        MonitorSnapshotEntity entity = new MonitorSnapshotEntity();

        // Records use record.field() instead of .getField()
        entity.setTimeCapture(snapshotRecord.timeCapture());
        entity.setTotalCpu(coreRecToEntry(snapshotRecord.totalCpu()));
        entity.setCores(coreListRecToEntry(snapshotRecord.cores()));
        entity.setMemTotal(snapshotRecord.memTotal());
        entity.setMemFree(snapshotRecord.memFree());
        entity.setMemAvailable(snapshotRecord.memAvailable());
        entity.setCached(snapshotRecord.cached());
        entity.setBuffers(snapshotRecord.buffers());
        entity.setSwapTotal(snapshotRecord.swapTotal());
        entity.setSwapFree(snapshotRecord.swapFree());
        entity.setMemActive(snapshotRecord.active());
        entity.setInactive(snapshotRecord.inactive());
        entity.setActiveAnon(snapshotRecord.activeAnon());
        entity.setInactiveAnon(snapshotRecord.inactiveAnon());
        entity.setActiveFile(snapshotRecord.activeFile());
        entity.setInactiveFile(snapshotRecord.inactiveFile());
        entity.setSlab(snapshotRecord.slab());
        entity.setSreclaimable(snapshotRecord.sreclaimable());
        entity.setSunreclaim(snapshotRecord.sunreclaim());
        entity.setDirty(snapshotRecord.dirty());
        entity.setWriteback(snapshotRecord.writeback());
        entity.setAnonPages(snapshotRecord.anonPages());
        entity.setMapped(snapshotRecord.mapped());
        entity.setShmem(snapshotRecord.shmem());
        entity.setKernelInfo(snapshotRecord.kernelInfo());
        entity.setOsInfo(snapshotRecord.osInfo());
        entity.setLastMinuteLoad(snapshotRecord.lastMinuteLoad());
        entity.setLastFiveMinutesLoad(snapshotRecord.lastFiveMinutesLoad());
        entity.setLastFifteenMinutesLoad(snapshotRecord.lastFifteenMinutesLoad());
        entity.setUpTime(snapshotRecord.upTime());
        entity.setIdleTime(snapshotRecord.idleTime());
        entity.setProcsRunning(snapshotRecord.procsRunning());
        entity.setProcsBlocked(snapshotRecord.procsBlocked());
        entity.setCtxtPerSecond(snapshotRecord.ctxtPerSecond());
        entity.setInterruptionsPerSecond(snapshotRecord.interruptionsPerSecond());

        // comes from the jpa interface to save the entry and now that entry exists as a
        // postgre
        // row
        return repository.save(entity);
    }

    // Grabs the latest data
    public MonitorSnapshotEntity getLatestSnapshot() {
        Optional<MonitorSnapshotEntity> latest = repository.findTopByOrderByTimeCaptureDesc();
        // uses a lambda to detect if the find failed and call a new exception
        return latest.orElseThrow(
                () -> new ResponseStatusException(HttpStatus.NOT_FOUND, "No snapshots found"));
    }

    // turns a coredata record in an entry
    public com.seanr.linuxmonitor.entities.CoreDataEntity coreRecToEntry(
            com.seanr.linuxmonitor.records.CoreDataRecord coreDataRecords) {
        com.seanr.linuxmonitor.entities.CoreDataEntity coreData =
                new com.seanr.linuxmonitor.entities.CoreDataEntity();

        coreData.setUsagePercent(coreDataRecords.usagePercent());
        coreData.setUserTime(coreDataRecords.userTime());
        coreData.setNice(coreDataRecords.nice());
        coreData.setSystem(coreDataRecords.system());
        coreData.setIdle(coreDataRecords.idle());
        coreData.setIowait(coreDataRecords.iowait());
        coreData.setIrq(coreDataRecords.irq());
        coreData.setSoftirq(coreDataRecords.softirq());
        coreData.setSteal(coreDataRecords.steal());
        coreData.setActive(coreDataRecords.active());
        coreData.setTotal(coreDataRecords.total());

        return coreData;
    }

    // Turns all the coredata records into actual entries
    private List<com.seanr.linuxmonitor.entities.CoreDataEntity> coreListRecToEntry(
            List<com.seanr.linuxmonitor.records.CoreDataRecord> coreDataRecords) {
        List<com.seanr.linuxmonitor.entities.CoreDataEntity> coreDataEntries =
                new ArrayList<com.seanr.linuxmonitor.entities.CoreDataEntity>();
        for (int i = 0; i < coreDataRecords.size(); i++) {
            coreDataEntries.add(coreRecToEntry(coreDataRecords.get(i)));
        }
        return coreDataEntries;
    }

    // get every row in the data tables
    // comes from the jpa interface
    public List<MonitorSnapshotEntity> getAllSnapshots() {
        return repository.findAll();
    }

    // Get the snapshot by an id
    public MonitorSnapshotEntity getSnapshotById(Long id) {
        Optional<MonitorSnapshotEntity> snapshot = repository.findById(id);
        // uses a lambda to detect if the find failed and call a new exception
        return snapshot.orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND,
                "No snapshot found with id " + id));
    }
}
