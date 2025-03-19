package JTextEntailment;

public class Pair {
	private String id = null;
	private String entailment = null;
	private String task = null;
	private String text = null;
	private String hypothese = null;
	private long hBeginOffset = 0;
	
	public Pair(String i, String e, String ta, String te, String h, long hBO){
		id = i;
		entailment = e;
		task = ta;
		text = te;
		hypothese = h;
		hBeginOffset = hBO;
		
	}
	public Pair(){
	
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getEntailment() {
		return entailment;
	}

	public void setEntailment(String entailment) {
		this.entailment = entailment;
	}

	public String getTask() {
		return task;
	}

	public void setTask(String task) {
		this.task = task;
	}

	public long getHBeginOffset() {
		return hBeginOffset;
	}

	public void setHBeginOffset(long beginOffset) {
		hBeginOffset = beginOffset;
	}

	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	public String getHypothese() {
		return hypothese;
	}
	public void setHypothese(String hypothese) {
		this.hypothese = hypothese;
	}

}
