/*
 * SpanishVerbs.java
 *
 * Created on June 9, 2006, 9:45 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package Test;

import java.util.*;
import java.io.*;
/**
 *
 * @author vgrassau
 */
public class SpanishVerbs {
    //champs pour la sortie
    private final int LEMME =0;
    private final int MODE =1;
    private final int TEMPS =2;
    private final int PERSONNE =3;
    private final int NOMBRE =4;
    private final int GENRE =5;
    private final int SURFACE =6;
    //champs pour l'entree
    private final int SURFACE_IN =0;
    private final int LEMME_IN =1;
    private final int TEMPS_IN =3;
    private final int PERSONNE_IN =4;
    private final int NOMBRE_IN =5;
    private final int GENRE_IN =6;
    
    
    //les modes
    private final String imperatif = "Imperativo";
    private final String indicatif = "Indicativo";
    private final String subjonctif = "Subjunctivo";
    private final String conditionel = "Condicional";
    //les temps
    private final String present = "Presente";
    private final String futur = "Futuro";
    private final String imparfait = "Imperfecto";
    private final String passe_simple = "Perfecto simple";
    //formes impersonnelles :
    private final String infinitif = "Infinitivo";
    private final String gerondif = "Gerundio";
    private final String participe = "Participio";
    //les traits : 
    private final String[] IInd = {"IInd",indicatif,imparfait};
    private final String[] Perf = {"Perf",indicatif,passe_simple};
    private final String[] PInd = {"PInd",indicatif,present};
    private final String[] Fut = {"Fut",indicatif,futur};
    
    private final String[] ISubj = {"ISubj",subjonctif,imparfait};
    private final String[] Pluperf = {"Pluperf",subjonctif,imparfait};
    private final String[] PSubj = {"PSubj",subjonctif,present};
    private final String[] FSubj = {"FSubj",subjonctif,futur};
    private final String[] Imp = {"Imp",imperatif,""};
    private final String[] Cond = {"Cond",conditionel,""};
       
    private final String[] Inf = {"Inf",infinitif,""};
    private final String[] PresPart = {"PresPart",gerondif,""};
    private final String[] PastPart = {"PastPart",participe,""};
      
    private final String[] Masc = {"Masc","Masculino"};
    private final String[] Fem = {"Fem","Femenino"};
    
    private final String[] Sg = {"Sg","Singular"};
    private final String[] Pl = {"Pl","Plural"};
    
    private final String[][] tempsTraits = {IInd,Perf,PInd,Fut,ISubj,PSubj,FSubj,
                                            Imp,Cond,Inf,PresPart,PastPart,Pluperf};
    
    private File morphFile;
    private File sqlFile;
    private Vector lines;//vetor de tableau de String
    
    /** Creates a new instance of SpanishVerbs */
    public SpanishVerbs(String morphoFile) {
       this.morphFile =new File(morphoFile);
       this.sqlFile = new File(morphFile.getAbsolutePath() + ".sql");
       if ( sqlFile.exists()) {
           sqlFile.delete();
       }
       lines = new Vector();
    
    }
    
    
    private String[] createInput(String splitInput) {
        String[] input = new String[7];
        
        return input;
    }
    
    
    //la ligne contient 7 cases
    public String createInsertSQL(String[] line) {
        StringBuffer sql = new StringBuffer("INSERT INTO `spanish_verbs` VALUES (");
        sql.append("'");
        sql.append(line[LEMME]); 
        sql.append("', '");
        sql.append(line[MODE]);
        sql.append("', '");
        sql.append(line[TEMPS]);
        sql.append("', '");
        sql.append(line[PERSONNE]);
        sql.append("', '");
        sql.append(line[NOMBRE]);
        sql.append("', '");
        sql.append(line[GENRE]);
        sql.append("', '");
        sql.append(line[SURFACE]);
        sql.append("');\n");
       // System.out.println(sql);
        return sql.toString();
    }
    
    /**
     *Construit une ligne que l'on va pouvoir inserer dans une base de donnees
     *@param input line telle qu'elle est donnee dans le fichier
     */
    public String[] constructLine(String[] input) {
        String[] output = new String[7];
        
        for (int i=0; i < output.length; i++) {
            output[i]="";
        }
        //pas de traitement particulier pour le lemme, surface et personne
        output[LEMME]    =input[LEMME_IN].trim();
        output[SURFACE]  =input[SURFACE_IN].trim();
          //temps et mode : 
        trouveModeEtTemps(output,input[TEMPS_IN]);
        
        for (int i=4;i<input.length;i++) {
            String traitInput  = input[i];
            if ( traitInput.equals(Masc[0])){   //genre masculin: 
                output[GENRE] = Masc[1];
            } else if (traitInput.equals(Fem[0])){ //genre feminin: 
                output[GENRE] = Fem[1]; 
            } else  if ( traitInput.equals(Sg[0])) {  //nombre singulier :
                output[NOMBRE] = Sg[1]; 
            } else if (traitInput.equals(Pl[0]) ){ //nombre pluriel: 
                output[NOMBRE] = Pl[1]; 
            } else { //personne
               output[PERSONNE] =traitInput.trim().substring(0,1);
            }
        }
               
        return output;
    }
    
    /**
     *Calcul le temps et le mode selon le trait des fst
     */
    private void trouveModeEtTemps(String[] output,String inputTrait) {
        for ( int i =0 ; i < tempsTraits.length;i++) {
            String[] trait = tempsTraits[i];
            if ( checkTrait(inputTrait,trait)) {
                initTempsMode(output,trait);
            }
        }
    }
    
    
    private void initTempsMode(String[] output, String[] trait){
           output[MODE]  = trait[1];
           output[TEMPS] = trait[2];   
    }
    
    private boolean checkTrait(String inputTrait,String[] trait) {
        if ( inputTrait.equals(trait[0])) { 
            return true;
        } else {
            return false;
        }
            
    }
    
    
    public void generate() {
        //on lit le fichier morhpFile ligne par ligne
        //on decoupe chaque ligne en fonction des "+"
        //on nettoie chaque entree avec trim
        //on genere la requete sql associe pour faire une insertion
        try {
            
            sqlFile.createNewFile();
            FileOutputStream fileOut = new FileOutputStream(sqlFile,true);
              
            if (morphFile == null || !morphFile.exists()) {
              System.err.println("ERROR : " + morphFile  + " n'existe pas !!");
            }
            RandomAccessFile in= new RandomAccessFile(morphFile,"r");
            String line = in.readLine();
            while ( line != null) {
                line = line.trim();
                String[] input = line.split("\\+");
                if ( input.length > 1) {
                    trimInput(input);
                    String[] output = constructLine(input);
                    fileOut.write(createInsertSQL(output).getBytes());
                }
                line = in.readLine();    
            }
            in.close();
        } catch (Exception e) {   
           System.err.println("ERROR : " + e);
           e.printStackTrace();
        }
        
        
    }
    //nettoie l'input
    private void trimInput(String[] input) {
        for (int i=0;i < input.length;i++) {
            input[i] = input[i].trim();
        }
    }
    
    
 static  public void main(String[] args) {
       try {
       String[] files= {"C:/Vowel/Exills/spanish_verbsaa",
                        "C:/Vowel/Exills/spanish_verbsab",
                        "C:/Vowel/Exills/spanish_verbsac",
                        "C:/Vowel/Exills/spanish_verbsad",
                        "C:/Vowel/Exills/spanish_verbsae",
                        "C:/Vowel/Exills/spanish_verbsaf",
                        "C:/Vowel/Exills/spanish_verbsag"
       };
       for (int i=0; i < files.length; i++) {
         SpanishVerbs sv = new SpanishVerbs(files[i]);
         System.out.println("generation du fichier sql pour : " + files[i]);
         sv.generate();
         System.out.println("generation du fichier sql pour : " + files[i] + " ok");
       }
            
           
         
       } catch (Exception e) {
         System.err.println("ERROR : " + e);
         e.printStackTrace();
       }
   }

}
