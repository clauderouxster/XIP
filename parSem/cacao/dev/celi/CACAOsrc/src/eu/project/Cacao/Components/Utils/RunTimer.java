package eu.project.Cacao.Components.Utils;

public class RunTimer {
	private long startTime;
	
	public RunTimer(){
		startTime = System.currentTimeMillis();
	}
	
	public void reset(){
		startTime = System.currentTimeMillis();
	}
	
	public String getElapsedTimeLabel(){
		long endTime = System.currentTimeMillis();
		long elapsed = (endTime - startTime);
		String elapsedTime="Elapsed time: ";
		if(elapsed>60000){
			int min = (int)(elapsed/60000);
			elapsedTime +=" "+min+" min.";
		}
		else if (elapsed>1000){
			int sec = (int)(elapsed/1000);
			elapsedTime +=" "+sec+" sec.";
		}
		else {
			elapsedTime +=" "+elapsed+" milliSec.";
		}
		return elapsedTime;
	}
	
	public long getElapsedTimeMillis(){
		long endTime = System.currentTimeMillis();
		long elapsed = (endTime - startTime);
		return elapsed;
	}
}
