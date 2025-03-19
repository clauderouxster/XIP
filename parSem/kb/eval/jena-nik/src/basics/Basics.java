
/* This class is part of the ActiveMath project.
Copyright D.F.K.I. GmbH, Saarbruecken, Germany
Please refer to the License that was in the distribution.
Using this software presupposes you agree with it.
 */
package basics;

import java.io.File;
import java.io.FileInputStream;
import java.io.PrintStream;
import java.util.Date;
import java.util.Properties;
import org.apache.log4j.Appender;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.apache.log4j.RollingFileAppender;

/**
 * Basic class to support a generic management of the project, such as logging
 * system.
 * 
 *  Notice, This class is just for personal usage, if we want to deliver our software, we should remove
 *  or rewrite this class. 
 *  
 *  This class provide the facility of using log4j as logging mechanism. It's easy to rewrite, but this is
 *  not the major job.
 *  
 * @version 1.0
 */
public class Basics {

    private static File projectHome = null;
    private static File logDirectory = null;

    // public static void setup() throws Exception {
    // computeActiveMathHome();
    // System.setProperty("activemath.home", projectHome.getAbsolutePath());
    // System.setProperty("tomcat.home", projectHome.getAbsolutePath());
    // System.setProperty("catalina.base", projectHome.getAbsolutePath());
    // System.setProperty("catalina.home", projectHome.getAbsolutePath());
    // }
    static java.io.File getProjectHome() {
        return projectHome;
    }

    // /**
    // * Computes the activemath-home from the jar that loaded this class or the
    // * class file.
    // */
    // private static void computeActiveMathHome() throws MalformedURLException,
    // UnsupportedEncodingException {
    // if (System.getProperty("activemath.home") != null) {
    // projectHome = new File(System.getProperty("activemath.home"));
    // System.out.println("ActiveMath home is: " + projectHome);
    // return;
    // }
    // URLClassLoader loader = (URLClassLoader) Basics.class.getClassLoader();
    //
    // // find the URL of my class-bytecode-file
    // URL myURL = loader
    // .findResource("org/activemath/config/startup/Basics.class");
    //
    // // System.out.println("myURL=" + myURL);
    // String myURLstring = myURL.toExternalForm();
    // int p = myURLstring.indexOf(".jar!");
    //
    // if (myURLstring.startsWith("jar:") && p > 0) {
    // // remove after the "!" thereby obtaining the jar
    // myURL = new URL(myURLstring.substring("jar:".length(), p
    // + ".jar".length()));
    // } else {
    // // we're just loaded from a class... go a few steps higher!
    // myURL = new URL(myURL, "../../../../");
    // }
    //
    // String jarFilename = myURL.getFile();
    // // " " comes out as "%20", so we need to decode for the filename:
    // // jarFilename = URLDecoder.decode(jarFilename, "ISO-8859-1"); // since
    // JDK
    // 1.4
    // jarFilename = URLDecoder.decode(jarFilename);
    // File jarLocation = new File(jarFilename);
    //
    // // if we start from Eclipse, jarFilename comes out as
    // // ".../activemath/classes/"
    // // if we start from script jarFilename comes out as
    // // ".../activemath/lib/activemath.jar
    // // go one up
    // projectHome = jarLocation.getParentFile();
    // if (!(new File(projectHome, "bin")).exists()) {
    // // go up anotherone
    // projectHome = projectHome.getParentFile();
    // }
    // System.out.println("ActiveMath home is: " + projectHome);
    // }
    static File getLogDirectory() {
        if (logDirectory != null) {
            return logDirectory;
        }
        logDirectory = new File(getProjectHome(), "logs");
        return logDirectory;
    }

    /**
     * Moves the previous log, cleans old log-files, and sets the standard
     * output and standard error to both the logfile and the console.
     * 
     * @param logName
     *            the name of the logile, without path or extension, i.e.
     *            logName can be fileMbase to have the log go into
     *            ACTIVEMATH_HOME/logs/fileMBase.log
     * @throws java.io.IOException 
     */
    public static void useLog(String logName) throws java.io.IOException {
        putLastLogInArchive(logName);
        openNewLog(logName);
        deleteOldLogs();
    }

    /**
     * Setup logging:
     * <ul>
     * <li> init log4j and make it write to the logfile specified
     * <li> redirect System.out and System.err to log4j
     * </ul>
     */
    static void openNewLog(String logName) throws java.io.IOException {
        String logFilePath = "logs/" + logName + ".log";
        // configure and init log4j
        final String log4jConfigfile = "conf/log4j.properties";
        System.setProperty("log4j.defaultInitOverride ", "true");
        Properties log4jProps = new Properties();
        log4jProps.load(new FileInputStream(new File(log4jConfigfile)));
        log4jProps.put("log4j.appender.logfile.File", logFilePath);
        PropertyConfigurator.configure(log4jProps);
        // rollover logfile and start new file
        Appender appender = Logger.getRootLogger().getAppender("logfile");
        if ((appender != null) && (appender instanceof RollingFileAppender)) {
            ((RollingFileAppender) appender).rollOver();
        }
        // redirect stdout and stderr to log4j
      //  System.setOut(new PrintStream(new Log4jOutputStream(Logger.getLogger("stdout"))));
      //  System.setErr(new PrintStream(new Log4jOutputStream(Logger.getLogger("stderr"))));
        System.out.println("=================================================================");
        System.out.println("Starting application...");
        System.out.println("Logging goes to " + logFilePath);
    }

    /**
     * Removes the .log files olders than two weeks.
     */
    private static void deleteOldLogs() {
        logDirectory = getLogDirectory();
        String[] logFiles = logDirectory.list();
        long twoWeeksAgo = System.currentTimeMillis() - (1000L * 60L * 60L * 24L * 14L);
        for (int i = 0; i < logFiles.length; i++) {
            File file = new File(logDirectory, logFiles[i]);
            if (file.lastModified() < twoWeeksAgo) {
                file.delete();
            }
        }
    }

    /**
     * Moves the current log-file to an "Old.&lt;logName&gt;_&lt;date&gt;.log
     */
    private static void putLastLogInArchive(String logName) {
        logDirectory = getLogDirectory();
        File logFile = new File(logDirectory, logName + ".log");
        if (logFile.isFile()) {
            File newFileName = new File(logFile, "Old." + logName + "_" + new Date(logFile.lastModified()) + ".log");
            logFile.renameTo(newFileName);
        }
    }
}
