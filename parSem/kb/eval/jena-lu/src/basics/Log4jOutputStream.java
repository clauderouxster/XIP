package basics;

// imports
import java.io.IOException;
import java.io.OutputStream;

import org.apache.log4j.Logger;

/**
 * Simple class to redirect an OutputStream to a log4j Logger.
 * 
 * @author tlu
 * @version 1.0
 */
public class Log4jOutputStream extends OutputStream {

    private final Logger log;

    public Log4jOutputStream(Logger log) {
        this.log = log;
    }

    public void write(int b) throws IOException {
        log.info("" + b);
    }

    public void write(byte[] b, int start, int len) throws IOException {
        if ((len == 2) && (start == 0) && (b[0] == '\r') && (b[1] == '\n')) {
            return;
        }
        log.info(new String(b, start, len));
    }

    public void write(byte[] b) throws IOException {
        log.info(new String(b));
    }

    public void flush() throws IOException {
        return;
    }

    public void close() throws IOException {
        return;
    }
}
