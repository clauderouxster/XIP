/*
 * EventFrame.java
 *
 * Created on July 5, 2007, 3:30 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.autoCalendar;

import common.Options;
import common.TypedPropertyException;
import documentAnalysis.temporality.TemporalConsistencyException;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSpinner;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;


/**
 * A frame for events
 */
public final class EventFrame extends JFrame implements ActionListener{
    
    
    private JPanel container = null;//Déclaration de l’objet JPanel
    private FlowLayout layout = null ;//Déclaration de notre layout
    private int areaLength;
    private int frameHeight;
    private int frameWidth;
    private int minimumHeight;
    private Vector<CalendarEvent> events;
    private Options options;
    
    private static final long serialVersionUID = 1L;
    
    public EventFrame(Options options) {
        this.areaLength = 30;
        this.minimumHeight = 65;
        this.frameHeight = 200;
        this.frameWidth = 400;
        this.setTitle("Events suggestion."); //On donne un titre à l’application
        this.setSize(this.frameWidth, this.minimumHeight); //On donne une taille à notre fenêtre
        this.setLocationRelativeTo(null); //On centre la fenêtre sur l’écran
        this.setResizable(false); //On interdit la redimensionnement de l’écran
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //On dit à l’application de se fermer lors du clic sur la croix
        this.options = options;
        // container
        layout = new FlowLayout() ; //Instanciation du layout
        layout.setAlignment(FlowLayout.LEFT) ;
        
        this.setContentPane(this.getContainer()); //On lui dit de mettre le panel en fond
    }
    
    private JPanel getContainer(){
        container = new JPanel() ; //On crée notre objet
        container.setLayout(layout);
        
        // text field to enter new events
        JLabel quickAddLabel = new JLabel("Quick add:");
        JTextField quickAdd = new JTextField(this.areaLength - 7);
        quickAdd.setName("QuickAddArea");
        container.add(quickAddLabel);
        container.add(quickAdd);
        // button
        JButton quickAddButton = new JButton("GO");
        quickAddButton.setName("QuickAdd");
        container.add(quickAddButton);
        quickAddButton.addActionListener(this);
        // separator
        JSeparator sepStart1 = new JSeparator();
        JSeparator sepStart2 = new JSeparator();
        Dimension sepStartDimension = new Dimension();
        sepStartDimension.setSize(this.getSize().getWidth(), 10);
        sepStart1.setPreferredSize(sepStartDimension);
        sepStart2.setPreferredSize(sepStartDimension);
        this.container.add(sepStart1);
        this.container.add(sepStart2);
        return container ;
    }
    
    public void addEvent(CalendarEvent event, int number) {
        int value;
        JLabel labelWhat = new JLabel("What:  ");
        JTextArea what = new JTextArea(event.getWhat().toString(), 1, this.areaLength);
        container.add(labelWhat);
        container.add(what);
        
        JLabel labelWhen = new JLabel("When:  ");
        container.add(labelWhen);
        JSeparator sepWhen = new JSeparator();
        Dimension sepWhenDimension = new Dimension();
        sepWhenDimension.setSize(this.getSize().getWidth(), 0);
        sepWhen.setPreferredSize(sepWhenDimension);
        container.add(sepWhen);
        
        
        // START
        JLabel labelFrom = new JLabel("    From ");
        container.add(labelFrom);
        // month
        JTextField startMonth = new JTextField(event.getStart().getMonth().getValue());
        container.add(startMonth);
        // day
        JLabel labelSlash2 = new JLabel("/");
        JTextField startDay = new JTextField(event.getStart().getDayOfMonth().getValue());
        container.add(labelSlash2);
        container.add(startDay);
        // year
        JLabel labelSlash1 = new JLabel("/");
        JTextField startYear = new JTextField(event.getStart().getYear().toString(), 4);
        container.add(labelSlash1);
        container.add(startYear);
        // hour
        JLabel labelAt1 = new JLabel(" at ");
        value = event.getStart().getHour().getIntValue();
        if (value == -1)
            value = 8;
        JSpinner startHour = new JSpinner(new SpinnerNumberModel(value,
                0, 23, 1));
        
        container.add(labelAt1);
        container.add(startHour);
        // minute
        JLabel labelColon1 = new JLabel(":");
        value = event.getStart().getMinute().getIntValue();
        if (value == -1)
            value = 0;
        JSpinner startMinute = new JSpinner(new SpinnerNumberModel(value,
                0, 59, 5));
        container.add(labelColon1);
        container.add(startMinute);
        
        JSeparator sepFrom = new JSeparator();
        Dimension sepFromDimension = new Dimension();
        sepFromDimension.setSize(this.getSize().getWidth(), 0);
        sepFrom.setPreferredSize(sepFromDimension);
        container.add(sepFrom);
        
        // END
        JLabel labelTo = new JLabel("    To     ");
        container.add(labelTo);
        // month
        JTextField endMonth = new JTextField(event.getEnd().getMonth().getValue());
        container.add(endMonth);
        // day
        JLabel labelSlash4 = new JLabel("/");
        JTextField endDay = new JTextField(event.getEnd().getDayOfMonth().getValue());
        container.add(labelSlash4);
        container.add(endDay);
        // year
        JLabel labelSlash3 = new JLabel("/");
        JTextField endYear = new JTextField(event.getEnd().getYear().toString(), 4);
        container.add(labelSlash3);
        container.add(endYear);
        // hour
        JLabel labelAt2 = new JLabel(" at ");
        value = event.getEnd().getHour().getIntValue();
        if (value == -1)
            value = 18;
        JSpinner endHour = new JSpinner(new SpinnerNumberModel(value,
                0, 23, 1));
//            JTextField startHour = new JTextField(event.getStart().getHour().getValue());
        container.add(labelAt2);
        container.add(endHour);
        // minute
        JLabel labelColon2 = new JLabel(":");
        value = event.getEnd().getMinute().getIntValue();
        if (value == -1)
            value = 0;
        JSpinner endMinute = new JSpinner(new SpinnerNumberModel(value,
                0, 59, 5));
        container.add(labelColon2);
        container.add(endMinute);
        
        JSeparator sepTo = new JSeparator();
        Dimension sepToDimension = new Dimension();
        sepToDimension.setSize(this.getSize().getWidth(), 0);
        sepTo.setPreferredSize(sepToDimension);
        container.add(sepTo);
//            JLabel sepEnd = new JLabel("             ");
//            container.add(sepEnd);
        
        
        JLabel labelWho = new JLabel("Who:  ");
        JTextArea who = new JTextArea(event.getWith().toString(), 1, this.areaLength);
        container.add(labelWho);
        container.add(who);
        
        JLabel labelWhere = new JLabel("Where:");
        JTextArea where = new JTextArea(event.getWhere().toString(), 1, this.areaLength);
        container.add(labelWhere);
        container.add(where);
        
        JButton buttonCalcul = new JButton("Add this event");
//            buttonCalcul.setPreferredSize(new Dimension(125,25));
        buttonCalcul.addActionListener(this);
        buttonCalcul.setName("" + number);
        container.add(buttonCalcul);
        
        JSeparator sepEvent = new JSeparator();
        Dimension sepEventDimension = new Dimension();
        sepEventDimension.setSize(this.getSize().getWidth(), 10);
        sepEvent.setPreferredSize(sepEventDimension);
        container.add(sepEvent);
        
//            JLabel sepAfterButton = new JLabel("                                                                ");
//            container.add(sepAfterButton);
        
        
        
    }
    
    public void actionPerformed(ActionEvent e){
        if (e.getSource() instanceof JButton){
            JButton button = (JButton)e.getSource();
            if (button.getName().equalsIgnoreCase("QuickAdd")){
                Component[] components = this.container.getComponents();
                for (int i = 0 ; i < components.length ; ++i) {
                    if (components[i] instanceof JTextField) {
                        if (components[i].getName().equalsIgnoreCase("QuickAddArea")) {
                            String text = ((JTextField)components[i]).getText();
                            try {
                                this.options.setProperty("INQUICKADD", "1");
                                this.options.setInputString(text + " .");
                                this.options.getParser().parse(this.options);
                                this.options.setProperty("INQUICKADD", "0");
                                System.out.println("\n\n\nEnter your text:");
                                break;
                            } catch (TypedPropertyException ex) {
                                ex.printStackTrace();
                                System.exit(-1);
                            } catch (TemporalConsistencyException ex) {
                                ex.printStackTrace();
                                System.exit(-1);
                            }
                        }
                    }
                }
            } else {
                int number = Integer.parseInt(button.getName());
//            JPopupMenu popUp = new JPopupMenu("Event creation");
//            popUp.setLabel("Creation of event " + number + "...");
                JOptionPane.showMessageDialog(this, new String("Creation of event " + number + "..."),
                        "Event Creation", JOptionPane.INFORMATION_MESSAGE);
            }
        }
    }
    
    public void showEvents(Vector<CalendarEvent> events){
        this.events = events;
        this.setSize(new Dimension(this.frameWidth, this.frameHeight * events.size() + this.minimumHeight));
        this.setContentPane(this.getContainer()); //On lui dit de mettre le panel en fond
        int i = 0;
        for (CalendarEvent event : events) {
            ++i;
            this.addEvent(event, i);
        }
        this.validate();
        this.toFront();
    }
}


