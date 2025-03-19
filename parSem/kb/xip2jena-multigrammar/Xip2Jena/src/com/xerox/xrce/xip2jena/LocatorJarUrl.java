/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.util.Locator;
import com.hp.hpl.jena.util.TypedStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import org.apache.log4j.Logger;


public class LocatorJarUrl implements Locator {
    static Logger log = Logger.getLogger(LocatorJarUrl.class) ;

    @Override
    public TypedStream open(String filenameOrURI) {
        URL url;
        try {
            url = new URL(filenameOrURI);
        } catch (MalformedURLException ex) {
            log.debug("not a URL", ex);
            return null;
        }

        if ( ! acceptByScheme(filenameOrURI) ) {
            log.debug("scheme not handled");
            return null;
        }

        InputStream stream;
        try {
            stream = url.openStream();
        } catch (IOException ex) {
            // We found the resource but failed to open it.  This exception should
            // really propagate up, but the interface doesn't allow it.
            log.warn("Found resource " + url + " but failed to open it.", ex);
            return null;
        }
        if (stream == null) return null;
        return new TypedStream(stream);
    }

    @Override
    // Why the $&#$ did they put this in the interface?
    public String getName() {
        return LocatorJarUrl.class.getName();
    }

    static final String[] schemeNames = {"jar:"};
    
    // Copied from LocatorURL.java.  I would have derived this class from LocatorURL
    // and inherited acceptByScheme, but I can't because LocatorURL defines
    // schemeNames as static final.
    private boolean acceptByScheme(String filenameOrURI)
    {
        String uriSchemeName = getScheme(filenameOrURI) ;
        if ( uriSchemeName == null )
            return false ;
        uriSchemeName = uriSchemeName.toLowerCase() ;
        for ( int i = 0 ; i < schemeNames.length ; i++ )
        {
            if ( uriSchemeName.equals(schemeNames[i]) )
                return true ;
        }
        return false ;
    }

    // copied from LocatorURL.java, see comment above.
    private String getScheme(String uri)
    {
        int ch = uri.indexOf(':') ;
        if ( ch < 0 )
            return null ;

        // Includes the :
        return uri.substring(0,ch+1) ;
    }


}
