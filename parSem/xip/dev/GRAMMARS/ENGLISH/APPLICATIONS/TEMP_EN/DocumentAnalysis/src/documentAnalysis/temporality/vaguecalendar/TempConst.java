/*
 * TempConst.java
 *
 * Created on December 1, 2006, 10:50 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar;

import java.util.HashMap;

/**
 *
 * @author xtannier
 */
public class TempConst {
    
    
    public static HashMap<String, String> mapHours;
    public static HashMap<String, String> mapDaysOfWeek;
    public static HashMap<String, String> mapMonths;
    public static HashMap<String, String> mapMods;
    
    
    static
    {
        mapHours = new HashMap<String, String>();
        mapHours.put("NI","night");
        mapHours.put("AF","afternoon");
        mapHours.put("MO","morning");
        mapHours.put("DT","day-time");
        mapHours.put("EV","evening");
        mapHours.put("MI","mid-day");
        
        mapDaysOfWeek = new HashMap<String, String>();
        mapDaysOfWeek.put("1","Monday");
        mapDaysOfWeek.put("2","Tuesday");
        mapDaysOfWeek.put("3","Wednesday");
        mapDaysOfWeek.put("4","Thursday");
        mapDaysOfWeek.put("5","Friday");
        mapDaysOfWeek.put("6","Saturday");
        mapDaysOfWeek.put("7","Sunday");
        mapDaysOfWeek.put("0","Sunday");
        mapDaysOfWeek.put("W","Week-end");
        
        mapMonths = new HashMap<String, String>();
        mapMonths.put("1","January");
        mapMonths.put("2","February");
        mapMonths.put("3","March");
        mapMonths.put("4","April");
        mapMonths.put("5","May");
        mapMonths.put("6","June");
        mapMonths.put("7","July");
        mapMonths.put("8","August");
        mapMonths.put("9","September");
        mapMonths.put("01","January");
        mapMonths.put("02","February");
        mapMonths.put("03","March");
        mapMonths.put("04","April");
        mapMonths.put("05","May");
        mapMonths.put("06","June");
        mapMonths.put("07","July");
        mapMonths.put("08","August");
        mapMonths.put("09","September");        
        mapMonths.put("10","October");
        mapMonths.put("11","November");
        mapMonths.put("12","December");
        mapMonths.put("WI","Winter");
        mapMonths.put("SU","Summer");
        mapMonths.put("FA","Fall");
        mapMonths.put("SP","Spring");
        mapMonths.put("H1","first half of the year");
        mapMonths.put("H2","second half of the year");
        mapMonths.put("Q1","first quarter");
        mapMonths.put("Q2","second quarter");
        mapMonths.put("Q3","third quarter");
        mapMonths.put("Q4","fourth quarter");
        
        mapMods = new HashMap<String, String>();
        mapMods.put("EXACT", "exactly");
        mapMods.put("APPROX", "about");
        mapMods.put("LESS_THAN", "less than");
        mapMods.put("MORE_THAN", "more than");
        mapMods.put("EQUAL_OR_LESS", "less than");
        mapMods.put("EQUAL_OR_MORE", "more than");
        mapMods.put("START", "beginning");
        mapMods.put("MID", "middle");
        mapMods.put("END", "end");
    }
    
    
    /**
     * Returns an English representation of the unit from a formal one.
     * @param hour the formal representation of the unit.
     * @return an English representation of the unit.
     */
    public static String hourToString(String hour){
        return mapHours.get(hour);
    }

    /**
     * Returns an English representation of the unit from a formal one.
     * @param dayOfWeek the formal representation of the unit.
     * @return an English representation of the unit.
     */
    public static String dayOfWeekToString(String dayOfWeek){
        return mapDaysOfWeek.get(dayOfWeek);
    }
    
    /**
     * Returns an English representation of the unit from a formal one.
     * @param month the formal representation of the unit.
     * @return an English representation of the unit.
     */
    public static String monthToString(String month){
        return mapMonths.get(month);
    }
    
    /**
     * Returns an English representation of the mod from a formal one.
     * @param mod the formal representation of the mod.
     * @return an English representation of the mod.
     */
    public static String modToString(String mod){
        return mapMods.get(mod.toUpperCase());
    }
    
}
