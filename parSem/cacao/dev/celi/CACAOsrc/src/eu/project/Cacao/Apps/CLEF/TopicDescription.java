package eu.project.Cacao.Apps.CLEF;

public class TopicDescription {
    public String what;
    public String about;

    public TopicDescription() {
        what = "";
        about = "";
    }

    public void trim() {
        what = what.trim();
        about = about.trim();
    }
}
