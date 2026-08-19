package com.seanr.linuxmonitor.repositories;

import com.seanr.linuxmonitor.entities.MonitorSnapshotEntity;
import java.util.Optional;
import org.springframework.data.jpa.repository.JpaRepository;

// This interface gives a bunch of free crud methods that come from JpaRepository
public interface MonitorSnapshotRepository extends JpaRepository<MonitorSnapshotEntity, Long> {
    // This is for finding the latest entry in the table

    // Drived query method
    // spring automatically parses the method name
    // findTop to restrict the results to row 1 and retrieve the data by reading
    // OrderByTimeCaptureDesc orders the result by the time that the data was
    // captured in decending order
    // Uses optinal so that findTopByOrderByTimeCaptureDesc doesnt return null and
    // instead just returns .empty()
    Optional<MonitorSnapshotEntity> findTopByOrderByTimeCaptureDesc();
}
