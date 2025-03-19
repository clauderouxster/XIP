/*
 *  Xerox Research Centre Europe - Grenoble Laboratory
 *  Copyright (C) 2011 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 */
package xrce.taskSync;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.util.ArrayList;
import java.util.Iterator;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.TaskContainer;
import org.apache.tools.ant.property.LocalProperties;

/**
 *
 * @author grondeau
 */
public class TaskSync extends Task implements TaskContainer {

    private String lockFileName;
    private ArrayList<Task> nestedTasks = new ArrayList<Task>();
    static int MAXRETRY = 60;
    private int retryCnt;

    /**
     * Add a nested task Locked.
     *
     * @param nestedTask Nested task to execute sequential and synchronized
     */
    @Override
    public void addTask(Task nestedTask) {
        nestedTasks.add(nestedTask);
    }

    public void setLockName(String lockName) {
        this.lockFileName = lockName;
    }

    private void doTask() {
        //log("doTask");
        // Execute all nested Tasks.
        LocalProperties localProperties = LocalProperties.get(getProject());
        localProperties.enterScope();
        try {
            for (Iterator i = nestedTasks.iterator(); i.hasNext();) {
                Task nestedTask = (Task) i.next();
                nestedTask.perform();
            }
        } finally {
            localProperties.exitScope();
        }
    }

    private void waitOneMn() {
        log("waiting 1mn");
        try {
            Thread.sleep(1000 * 60); //wait 1 mn
        } catch (Exception ex) {
        }
    }

    private void waitNoFile(File macOsFileLock) {
        //wait until file deleted by an other process
        while (true) {
            if (macOsFileLock.exists()) {
                retryCnt++;
                if (retryCnt > MAXRETRY) {
                    //log("MAXRETRY done");
                    break;
                }
                waitOneMn();
            } else {
                //log("No file");
                break;
            }
        }
    }

    @Override
    public void execute() throws BuildException {
        //as fileChannel.lock() does not run on mac OS, I had a sync using the fact a file exist or not
        // this will be used only on mac os
        if (lockFileName == null) {
            throw new BuildException("lockFileName attribute required.");
        }
        try {
            File fileLock = new File(lockFileName);
            waitNoFile(fileLock);
            fileLock.createNewFile();
            doTask();
            fileLock.delete();
        } catch (Exception ex) {
            throw new BuildException(ex.getMessage());
        } finally {
            try {
            } catch (Exception ex) {
                //if delete can't be done(file locked), it will done later by the ant process locking this file
            }
        }
    }

    /**
     * cette version est OK sur linux(32/64) et win32/64 mais nook sur macOs
     * fileChannel.lock() does not run on mac OS. - first, blocks until it can
     * retrieve the lock. - then, Execute all nestedTasks. - then, release the
     * lock
     *
     * @throws BuildException if one of the nested tasks fails.
     */
    //  @Override
    public void executeNotUsed() throws BuildException {
        File lockFile;
        RandomAccessFile randomAccessFile = null;
        FileChannel fileChannel = null;
        if (lockFileName == null) {
            throw new BuildException("lockFileName attribute required.");
        }
        try {
            lockFile = new File(lockFileName);
            log("try to get a lock on the file: " + lockFile);
            randomAccessFile = new RandomAccessFile(lockFile, "rw");
            fileChannel = randomAccessFile.getChannel();
            // blocks until it can retrieve the lock.
            FileLock fileLock = fileChannel.lock();
            if (fileLock != null) {
                log("now tasks are synchronized");
                doTask();
                fileLock.release(); //release lock on file
                log("tasks are done");
            } else {
                throw new BuildException("fileChannel.lock() error");
            }
        } catch (IOException ex) {
            throw new BuildException(ex.getMessage());
        } finally {
            try {
                if (fileChannel != null) {
                    fileChannel.close();
                }
                if (randomAccessFile != null) {
                    randomAccessFile.close();
                }
                //if (lockFile != null) {
                //    lockFile.delete();
                //}
            } catch (Exception ex) {
            }
        }
    }
}