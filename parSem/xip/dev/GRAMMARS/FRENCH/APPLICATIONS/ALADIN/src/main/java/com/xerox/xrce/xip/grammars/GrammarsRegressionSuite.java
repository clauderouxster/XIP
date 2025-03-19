/**
 * 
 */
package com.xerox.xrce.xip.grammars;
 
import java.io.File;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.junit.runner.Runner;
import org.junit.runner.notification.RunNotifier;
import org.junit.runners.BlockJUnit4ClassRunner;
import org.junit.runners.Suite;
import org.junit.runners.model.FrameworkMethod;
import org.junit.runners.model.InitializationError;
import org.junit.runners.model.Statement;
import org.junit.runners.model.TestClass;

/**
 * @author Ioan Calapodescu
 * 
 */
public class GrammarsRegressionSuite extends Suite {
	@Retention(RetentionPolicy.RUNTIME)
	@Target(ElementType.METHOD)
	public static @interface Files { 
	}

	private class TestClassRunnerForGrammars extends BlockJUnit4ClassRunner {

		private final File file;

		TestClassRunnerForGrammars(Class<?> type, File file) throws InitializationError {
			super(type);
			this.file = file;
		}

		@Override
		public Object createTest() throws Exception {
			return getTestClass().getOnlyConstructor().newInstance(file);
		}

		@Override
		protected String getName() {
			return String.format("[%s]", file.getAbsolutePath());
		}

		@Override
		protected String testName(final FrameworkMethod method) {
			return String.format("%s[%s]", method.getName(), file.getAbsolutePath());
		}

		@Override
		protected void validateConstructor(List<Throwable> errors) {
			validateOnlyOneConstructor(errors);
		}

		@Override
		protected Statement classBlock(RunNotifier notifier) {
			return childrenInvoker(notifier);
		}
	}
	private final ArrayList<Runner> runners= new ArrayList<Runner>();

	/**
	 * Only called reflectively. Do not use programmatically.
	 */
	public GrammarsRegressionSuite(Class<?> klass) throws Throwable {
		super(klass, Collections.<Runner>emptyList());
		File[] files = getFiles(getTestClass());
		for (int i= 0; i < files.length; i++) {
			runners.add(new TestClassRunnerForGrammars(getTestClass().getJavaClass(),files[i]));
		}
	}

	@Override
	protected List<Runner> getChildren() {
		return runners;
	}

	private File[] getFiles(TestClass klass) throws Throwable {
		return (File[]) getFilesMethod(klass).invokeExplosively(null);
	}

	private FrameworkMethod getFilesMethod(TestClass testClass)
			throws Exception {
		List<FrameworkMethod> methods= testClass.getAnnotatedMethods(Files.class);
		for (FrameworkMethod each : methods) {
			int modifiers= each.getMethod().getModifiers();
			if (Modifier.isStatic(modifiers) && Modifier.isPublic(modifiers))
				return each;
		}

		throw new Exception("No public static files method on class " + testClass.getName());
	}
}
