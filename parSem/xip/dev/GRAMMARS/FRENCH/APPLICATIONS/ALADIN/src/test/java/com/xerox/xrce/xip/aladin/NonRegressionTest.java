/**
 * 
 */
package com.xerox.xrce.xip.aladin;

import java.io.File;
import java.io.FilenameFilter;
import java.io.StringReader;
import java.util.List;

import junit.framework.Assert;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

import com.xerox.jxip.JXip;
import com.xerox.jxip.JXip2;
import com.xerox.xrce.xip.grammars.GrammarsRegressionSuite;
import com.xerox.xrce.xip.grammars.GrammarsRegressionSuite.Files;

import difflib.DiffUtils;
import difflib.Patch;

/**
 * <p>
 * Simple non regression unit tests.
 * </p>
 * 
 * @author Ioan Calapodescu
 * 
 */
@RunWith(GrammarsRegressionSuite.class)
public class NonRegressionTest {

	private static final String grmRootDir = ".";
	private static JXip jxip;
	private static int handler;
	private static int cryptedHandler;

	/**
	 * Initialise the XIP parser and create a encrypted version of the grammar. 
	 * @throws Exception if it's something wrong with XIP
	 */
	@BeforeClass
	public static void loadXip() throws Exception {
		// loading grammar
		jxip = new JXip();
		String relativeGrmPath = "/PRETESTS/aladin.grm";
		File grmFile = new File(grmRootDir, relativeGrmPath);
		handler = jxip.XipGrmFile(grmFile.getAbsolutePath(), 0, true);

		// creating encrypted grammar
		JXip2 jxip2 = new JXip2();
		File cryptedDir = new File(grmRootDir, "target/crypted");
		jxip2.copyAndEncryptGrammar(grmFile.getAbsolutePath(), cryptedDir.getAbsolutePath());
		
		// 'finding' encrypted grammar
		String localPath = new File(grmRootDir).getAbsolutePath();
		if(System.getProperty("os.name").toLowerCase().startsWith("windows")){
			localPath = localPath.substring(2, localPath.length());
		}
		File cryptedDirGrmLocation = new File(cryptedDir, localPath);
		File cryptedGrmFile = new File(cryptedDirGrmLocation, relativeGrmPath);
		cryptedHandler = jxip.XipGrmFile(cryptedGrmFile.getAbsolutePath(), 0, true);
	}

	/**
	 * The unit test file.
	 */
	private final File testFile;

	public NonRegressionTest(File testFile) {
		this.testFile = testFile;
	}

	/**
	 * All the files to test (all .txt files in the TESTS folder).
	 * @return an array of {@link File}
	 */
	@Files 
	public static final File[] listTestFiles() {
		return new File(grmRootDir, "TESTS").listFiles(new FilenameFilter() {
			public boolean accept(File dir, String name) {
				File f = new File(dir, name);
				return f.isFile() && f.canRead() && name.endsWith(".txt");
			}
		});
	}

	/**
	 * Test a file with the reference output (with the normal, non encrypted, grammar).
	 * @throws Exception if a XIP error is thrown
	 */
	@Test
	public void regTest() throws Exception {
		regTest(handler);
	}
	
	/**
	 * Test a file with the reference output (with the normal, non encrypted, grammar).
	 * @throws Exception if a XIP error is thrown
	 */
	@Test
	public void regTestCrypted() throws Exception {
		regTest(cryptedHandler);
	}
	
	/**
	 * Test a file with the reference output (with a given grammar handler).
	 * @throws Exception if a XIP error is thrown
	 */
	private void regTest(int grammarHandler) throws Exception {
		File ref = new File(grmRootDir, "TESTS_REFERENCES/" + testFile.getName() + ".ref");
		if (ref.isFile() && ref.canRead()) {
			String diff = diff(grmRootDir, testFile, ref, grammarHandler);
			boolean ok = diff.isEmpty();
			if (!ok) {
				// saving diff file
				File diffFile = new File(grmRootDir, "TESTS_DIFFS/" + testFile.getName() + ".ref.diff");
				FileUtils.write(diffFile, diff);
			}
			Assert.assertTrue(diff, ok);
		} else {
			Assert.fail("Unable to find a reference " + ref.getAbsolutePath());
		}

	}

	/**
	 * Build a diff representation of two files.
	 * @param grmRootDir - the root directory
	 * @param test - the test file
	 * @param ref - the reference file
	 * @return a String representation in unified diff format
	 * @throws Exception ...
	 */
	private String diff(String grmRootDir, File test, File ref, int grammarHandler) throws Exception {
		// reading file contents
		String unitTestContents = FileUtils.readFileToString(test);
		List<String> unitTestReferenceContents = FileUtils.readLines(ref);

		// executing XIP
		String result = jxip.parseString(grammarHandler, unitTestContents, '0', '1');
		List<String> resultContents = IOUtils.readLines(new StringReader(result));

		// making diff.
		Patch patch = DiffUtils.diff(unitTestReferenceContents, resultContents);

		List<String> udiff = DiffUtils.generateUnifiedDiff(
				ref.getAbsolutePath(), "XIP Parsing of " + test.getAbsolutePath(),
				unitTestReferenceContents, patch, 5);
		StringBuilder builder = new StringBuilder();
		for (String line : udiff) {
			builder.append(line).append("\n");
		}

		// returning diff results
		return builder.toString();
	}

}
