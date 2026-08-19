package com.seanr.linuxmonitor.controllers;

import com.seanr.linuxmonitor.entities.MonitorSnapshotEntity;
import com.seanr.linuxmonitor.records.MonitorSnapshotRecord;
import com.seanr.linuxmonitor.services.MonitorSnapshotService;
import java.util.List;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController // spring scans for this annotation which tell it that it will send back a .json to
                // the C++ side which is just disgarded (this class handles web requests from C++
                // side)
@RequestMapping("/statistics") // /statistics is the end path url that every method level path gets
                               // appended to
public class MonitorSnapshotController {
    private final MonitorSnapshotService service;

    // Constructor to assign the service once
    public MonitorSnapshotController(MonitorSnapshotService service) {
        this.service = service;
    }

    @PostMapping // This tells spring that the post request comes through here
    // @RequestBody tells spring that the .json is what forms the MonitorSnapshotRecord
    public MonitorSnapshotEntity postSnapshot(@RequestBody MonitorSnapshotRecord snapshot) {
        // trun the record into the entity
        return service.saveSnapshot(snapshot);
    }

    @GetMapping("/latest") // maps this method to /statistics/latest
    // returns the latest monitor stats
    public MonitorSnapshotEntity getLatest() {
        return service.getLatestSnapshot();
    }

    @GetMapping("/all") // maps this method to /statistics/all
    // returns every row in the table
    public List<MonitorSnapshotEntity> getAll() {
        return service.getAllSnapshots();
    }

    // returns a snapshot based off an id
    @GetMapping("/{id}") // maps this method to /statistics/{id}
    // returns a snapshot based off an id
    public MonitorSnapshotEntity getById(@PathVariable Long id) {
        return service.getSnapshotById(id);
    }
}
