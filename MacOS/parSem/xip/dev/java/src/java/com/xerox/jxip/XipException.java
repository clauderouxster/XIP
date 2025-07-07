/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

/**
 * Description of the Class
 *
 * @author grondeau @created 17 January 2006
 */
public class XipException extends Exception {

    private String message;
    private String fileName;
    private String timeStamp;
    private long line;
    private long position;

    /**
     * Constructor for the XipException object
     */
    public XipException() {
        //System.out.println("XipException()");
        message = "";
    }

    /**
     * Constructor for the XipException object
     *
     * @param msg
     */
    public XipException(String msg) {
        super(msg);
        //System.out.println("XipException(msg)");
        message = msg;
        fileName = "";
        timeStamp = "";
        line = 0;
        position = 0;
    }

    /**
     * Constructor for the XipException object
     *
     * @param msg
     * @param fileN
     * @param timeS
     * @param lineN
     * @param pos
     */
    public XipException(String msg, String fileN, String timeS, long lineN, long pos) {
        super(msg);
        //System.out.println("XipException(...): " + msg + " " + fileN + " " + lineN);
        message = msg;
        fileName = fileN;
        timeStamp = timeS;
        line = lineN;
        position = pos;
    }

    /**
     * Constructor for the XipException object
     *
     * @param msg
     * @param fileN
     * @param lineN
     * @param pos
     */
    public XipException(String msg, String fileN, long lineN, long pos) {
        super(msg);
        //System.out.println("XipException(...): " + msg + " " + fileN + " " + lineN);
        message = msg;
        fileName = fileN;
        line = lineN;
        position = pos;
    }

    /**
     * Gets the message attribute of the XipException object
     *
     * @return The message value
     */
    @Override
    public synchronized String getMessage() {
        return message;
    }

    /**
     * Gets the fileName attribute of the XipException object
     *
     * @return The fileName value
     */
    public synchronized String getFileName() {
        return fileName;
    }

//    /**
//     *  Gets the timeStamp attribute of the XipException object
//     *
//     * @return    The timeStamp value
//     */
//    public synchronized String getTimeStamp() {
//        return timeStamp;
//    }
    /**
     * Gets the line attribute of the XipException object
     *
     * @return The line value
     */
    public synchronized long getLine() {
        return line;
    }

    /**
     * Gets the position attribute of the XipException object
     *
     * @return The position value
     */
    public synchronized long getPosition() {
        return position;
    }

    /**
     *
     *
     * @return
     */
    @Override
    public synchronized String toString() {
        String mess = "";
        mess += message;
        if (!fileName.equals("")) {
            mess += "   Error in file: " + fileName;
            mess += "   (line: " + line + ")";
        }
        return mess;
    }
}