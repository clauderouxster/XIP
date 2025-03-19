/**
 * 
 */
package eu.project.Cacao.Connectors.OAIPMH;

import java.net.*;
import java.text.Normalizer.Form;
import java.io.*;

/**
 * 
 * Class for simple HTTP GET request
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class HTTPRequest {

	private String requestStr;
	private String urlStr;
	private String responseStr;

	/**
	 * 
	 * Class constructor
	 * 
	 * @param req
	 *            request String
	 * @param url
	 *            target URL
	 * @throws MalformedURLException
	 *             In case of bad URL
	 * @throws IOException
	 *             In case of network problem
	 */
	HTTPRequest(String req, String url) throws MalformedURLException,
			IOException {
		try {
			requestStr = req;
			urlStr = url;

			URL repositoryUrl = new URL(urlStr + "?" + requestStr);
			URLConnection conn = repositoryUrl.openConnection();
			
			conn.setReadTimeout(0);
			//BufferedReader in = new BufferedReader(new InputStreamReader(
			//		repositoryUrl.openStream()));
			BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
			
			responseStr = "";
			String str;

			while ((str = in.readLine()) != null) {
				responseStr += str+" ";
			}
			responseStr = java.text.Normalizer.normalize(responseStr, Form.NFC); 
			in.close();
		} catch (MalformedURLException e) {
			throw (e);
		} catch (IOException e) {
			throw (e);
		}
	}

	/**
	 * 
	 * Return the response received from the server via HTTP
	 * 
	 * @return The response
	 */
	public String getResponse() {
		return responseStr;
	}
}
