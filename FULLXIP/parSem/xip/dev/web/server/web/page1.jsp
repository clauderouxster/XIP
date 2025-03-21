
<%@ include file="banner.jspf" %>

<%@ page import="com.xerox.jxip.JXip" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="java.lang.*" %>
<%@ page import="java.util.*" %>

<%@ page errorPage="errorPage.jsp" %>

<%-- To specify the encoding of the response --%>
<%@ page contentType="text/html; charset=iso-8859-1" %>

<%-- To set the source encoding of the page itself --%>
<%@ page pageEncoding="iso-8859-1" %> 

<%-- declaration part --%>
<%!
//Constant
private static int sizeMax = 2000;
private static String startParsingButtonStr = "Start parsing";
private static String clearTextButtonStr = "Clear text";
private static String defaultExampleButtonStr = "Predefined example";
private static String encoding = "iso-8859-1";
private static int colSize = 70;
//Variable
private JXip jxip = null;
private String tomCatDir = null;
private ServletContext servletContext = null;
private Integer handlerEnglishGram = null;
private Integer handlerFrenchGram = null;
private Integer handlerSpanishGram = null;
private Integer handlerPortugueseGram = null;
private Integer handlerItalianGram = null;
private Integer handlerGermanGram = null;
private String inputStr = null;
private String submitParam = null;
private String defaultExample = null;
private String outputStr = "";
private int gHandler = -1;
// language selected
private boolean englishSelected = false;
private boolean frenchSelected = false;
private boolean spanishSelected = false;
private boolean portugueseSelected = false;
private boolean italianSelected = false;
private boolean germanSelected = false;
// option selected
private boolean option2Selected = false;
private boolean option3Selected = false;
private boolean option4Selected = false;
private boolean option5Selected = false;
private boolean option6Selected = false;
private boolean option7Selected = false;


public void jspInit() {
         //System.out.println("jspInit running");
         fullInit();
}

public void jspDestroy() {
         //System.out.println("jsp destroy running");
}

private synchronized void logInfo(int gHandler, String input, char xmlFlag, int displayMode, javax.servlet.http.HttpServletRequest request) {
         try {
                 File outFile = new File(tomCatDir + "/logs/xipweb.txt");
                 FileWriter out = new FileWriter(outFile, true);
                 Date today = new Date();
                 SimpleDateFormat formatter = new SimpleDateFormat("d MMM yyyy HH:mm:ss");
                 String date = formatter.format(today);
                 out.write("----------------------------  " + date + "  -----------------------------\n");
                 out.write("remoteAddr: " +request.getRemoteAddr() + "\n");
                 //out.write("remote host = " + request.getRemoteHost() + "\n");
                 out.write("gHandler: " + gHandler + "\n");
                 out.write("xmlFlag: " + (int)xmlFlag + "\n");
                 out.write("displayMode: " + displayMode + "\n");
                 out.write(input + "\n");
                 out.close();
         } catch (Exception ex) {
                 System.out.println("logInfo: " + ex.toString());
                 ex.printStackTrace();
         }
}

private synchronized void logExcept(String mes) {
         try {
                 File outFile = new File(tomCatDir + "/logs/xipweb.txt");
                 FileWriter out = new FileWriter(outFile, true);
                 Date today = new Date();
                 SimpleDateFormat formatter = new SimpleDateFormat("d MMM yyyy HH:mm:ss");
                 String date = formatter.format(today);
                 out.write("-------- Exception  " + date + "  --------------\n");
                 out.write(mes + "\n");
                 out.close();
         } catch (Exception ex) {
                 System.out.println("logExcept: " + ex.toString());
                 ex.printStackTrace();
         }
}

private String[] french_entities = {"DATE", "PERSONNE", "PERIODE", "CELEB", "VILLE",
         "PAYS", "REGION", "LIEU", "QUARTIER", "BATIMENT", "BATIMENTORG", "LOCORG", "ORG", "BATIMENTORG", "LABEL", "EMAIL", "URL"};
         
private String[] english_entities = {"CAR_MODEL","ENTITE","LOCATION","URL","PHONE","EMAIL","TEMPEXPR","YEAR","HOUR",
"DATE","COMPLEXNUM","MONEY","PERCENT","UNIT","PERSON","EVENT","LOCORG","LAW","ORGANISATION","PROFESSION", "PRODUCT", "SHIP"};
         
private synchronized boolean isEntity(String subStr, String []entities) {
         for (int index = 0; index < entities.length; index++) {
                 String entity = entities[index];
                 if (subStr.regionMatches(0, entity, 0, entity.length())) {
                         return true;
                 }
         }
         return false;
}

private synchronized void loadGram() {
         //System.out.println("loadGram()");
         String grmRoot = tomCatDir + "/webapps/xip/grammar";
         String grmFile = "";
         handlerEnglishGram = null;
         handlerFrenchGram = null;
         handlerSpanishGram = null;
         handlerPortugueseGram = null;
         handlerItalianGram = null;
         handlerGermanGram = null;
         try {
                 // load the english entity grammar
                 grmFile = grmRoot + File.separator + "english" + File.separator + "entity" + File.separator + "gram_gen_entit.grm";
                 System.out.println("Loading grm file: " + grmFile);
                 gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerEnglishGram = null;
                 } else {
                         handlerEnglishGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerEnglishGram", handlerEnglishGram);
          
                 // load the french grammar
                 grmFile = grmRoot + File.separator + "french" + File.separator + "entity" + File.separator + "french_entity.grm";
                 System.out.println("Loading grm file: " + grmFile);
                 gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerFrenchGram = null;
                 } else {
                         handlerFrenchGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerFrenchGram", handlerFrenchGram);
                 
                 // load the portuguese grammar
                 grmFile = grmRoot + File.separator + "portuguese" + File.separator + "basic" + File.separator + "portuguese.grm";
                 System.out.println("Loading grm file: " + grmFile);
                 gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerPortugueseGram = null;
                 } else {
                         handlerPortugueseGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerPortugueseGram", handlerPortugueseGram);
                 
                 // load the spanish grammar
                 grmFile = grmRoot + File.separator + "spanish" + File.separator + "basic" + File.separator + "spanish.grm";
                 System.out.println("Loading grm file: " + grmFile);
                 gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerSpanishGram = null;
                 } else {
                         handlerSpanishGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerSpanishGram", handlerSpanishGram);
                 
                 // load the italian grammar
                 grmFile = grmRoot + File.separator + "italian" + File.separator + "basic" + File.separator + "italian.grm";
                 System.out.println("Loading grm file: " + grmFile);
                 gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerItalianGram = null;
                 } else {
                         handlerItalianGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerItalianGram", handlerItalianGram);
                 
                 // load the german grammar
                 grmFile = grmRoot + File.separator + "german" + File.separator + "basic" + File.separator + "german.grm";
                 //System.out.println("Loading grm file: " + grmFile);
                 //gHandler = jxip.XipGrmFile(grmFile , 0, true);
                 gHandler = 32;  //PB avec la gram german: il faut utiliser jxipPy.jar et ca ne marche pas (a voir pourquoi)
                 if (gHandler == -1) {
                         System.out.println("loading grammar error: " + grmFile);
                         handlerGermanGram = null;
                 } else {
                         handlerGermanGram = new Integer(gHandler);
                 }
                 servletContext.setAttribute("handlerGermanGram", handlerGermanGram);
         } catch (Exception ex) {
                 logExcept(ex.toString());
                 System.out.println("loadGram: " + ex.toString());
                 ex.printStackTrace();
         }
}

private synchronized void initGramHandler() throws Exception {
         //System.out.println("initGramHandler()");
         handlerEnglishGram = (Integer)servletContext.getAttribute("handlerEnglishGram");
         handlerFrenchGram = (Integer)servletContext.getAttribute("handlerFrenchGram");
         handlerSpanishGram = (Integer)servletContext.getAttribute("handlerSpanishGram");
         handlerPortugueseGram = (Integer)servletContext.getAttribute("handlerPortugueseGram");
         handlerItalianGram = (Integer)servletContext.getAttribute("handlerItalianGram");
         handlerGermanGram = (Integer)servletContext.getAttribute("handlerGermanGram");
         if ((handlerEnglishGram == null) 
                 || (handlerFrenchGram == null)
                 || (handlerSpanishGram == null)
                 || (handlerPortugueseGram == null)
                 || (handlerItalianGram == null)
                 || (handlerGermanGram == null)
                 ) {
                 loadGram();
         }
         handlerEnglishGram = (Integer)servletContext.getAttribute("handlerEnglishGram");
         handlerFrenchGram = (Integer)servletContext.getAttribute("handlerFrenchGram");
         handlerSpanishGram = (Integer)servletContext.getAttribute("handlerSpanishGram");
         handlerPortugueseGram = (Integer)servletContext.getAttribute("handlerPortugueseGram");
         handlerItalianGram = (Integer)servletContext.getAttribute("handlerItalianGram");
         handlerGermanGram = (Integer)servletContext.getAttribute("handlerGermanGram");
         //System.out.println("handlerEnglishGram = " + handlerEnglishGram );
         //System.out.println("handlerFrenchGram = " + handlerFrenchGram );
         //System.out.println("handlerSpanishGram = " + handlerSpanishGram );
         //System.out.println("handlerPortugueseGram = " + handlerPortugueseGram );
         //System.out.println("handlerItalianGram = " + handlerItalianGram );
         //System.out.println("handlerGermanGram = " + handlerGermanGram );
         if (handlerEnglishGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerEnglishGram == null");
         }
         if (handlerFrenchGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerFrenchGram == null");
         }
         if (handlerSpanishGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerSpanishGram == null");
         }
         if (handlerPortugueseGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerPortugueseGram == null");
         }
         if (handlerItalianGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerItalianGram == null");
         }
         if (handlerGermanGram == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> handlerGermanGram == null");
         }
}

private synchronized void getParam(javax.servlet.http.HttpServletRequest request) {
         //System.out.println("getParam()");
         //System.out.println("characterEncoding = " + request.getCharacterEncoding());
         inputStr = request.getParameter("input");
         if (inputStr == null) {
                 inputStr = "";
         }
         submitParam = request.getParameter("submitParam");
         if (submitParam == null) {
                 submitParam = "";
         }
         defaultExample = request.getParameter("defaultExample");
         if (defaultExample == null) {
                 defaultExample = "";
         }
}

private synchronized void getLanguageOption(javax.servlet.http.HttpServletRequest request) {
         //System.out.println("getLanguageOption()");
         englishSelected = false;
         frenchSelected = false;
         spanishSelected = false;
         portugueseSelected = false;
         italianSelected = false;
         germanSelected = false;
         String language = request.getParameter("language");
         if (language != null) {
                 //out.print("language = " + language + "<br>");
                 if (language.compareTo("english") == 0) {
                         gHandler = handlerEnglishGram.intValue();
                         englishSelected = true;
                         //System.out.println("getLanguageOption()  English gHandler = " + gHandler );
                 } else if (language.compareTo("french") == 0) {
                         gHandler = handlerFrenchGram.intValue();
                         frenchSelected = true;
                         //System.out.println("getLanguageOption()  French gHandler = " + gHandler );
                 } else if (language.compareTo("spanish") == 0) {
                         gHandler = handlerSpanishGram.intValue();
                         spanishSelected = true;
                         //System.out.println("getLanguageOption()  Spanish gHandler = " + gHandler );
                 } else if (language.compareTo("portuguese") == 0) { 
                         gHandler = handlerPortugueseGram.intValue();
                         portugueseSelected = true;
                         //System.out.println("getLanguageOption()  Portuguese gHandler = " + gHandler );
                 } else if (language.compareTo("italian") == 0) {
                         gHandler = handlerItalianGram.intValue();
                         italianSelected = true;
                         //System.out.println("getLanguageOption()  Italian gHandler = " + gHandler );
                 } else if (language.compareTo("german") == 0) {
                         gHandler = handlerGermanGram.intValue();
                         germanSelected = true;
                         //System.out.println("getLanguageOption()  German gHandler = " + gHandler );
                 } else {
                         gHandler = handlerEnglishGram.intValue();
                         englishSelected = true;
                 }
         } else {
                 //default value
                 gHandler = handlerEnglishGram.intValue();
                 englishSelected = true;
                 language = "";
         }
}

private synchronized void getOptionSelected(javax.servlet.http.HttpServletRequest request) {
         //System.out.println("getOptionSelected()");
         option2Selected = false;
         option3Selected = false;
         option4Selected = false;
         option5Selected = false;
         option6Selected = false;
         option7Selected = false;
         String[] options = request.getParameterValues("option");
         if (options != null) {
                 for (int ind = 0; ind <options.length; ind++) {
                         String option = options[ind];
                         //out.print(option + "<br>");
                         if (option.compareTo("option2") == 0) {
                                 option2Selected = true;
                         } else if (option.compareTo("option3") == 0) {
                                 option3Selected = true;
                         } else if (option.compareTo("option4") == 0) {
                                 option4Selected = true;
                         } else if (option.compareTo("option5") == 0) {
                                 option5Selected = true;
                         } else if (option.compareTo("option6") == 0) {
                                 option6Selected = true;
                         } else if (option.compareTo("option7") == 0) {
                                 option7Selected = true;
                         } else {
                                 //default value 
                                 option4Selected = true;
                         }
                 }
         } else {
                 //default value 
                 option4Selected = true;
         }
}

private synchronized void fullInit() {
         //System.out.println("fullInit()");
         tomCatDir = System.getProperty("catalina.home");
         //System.out.println("++++++++++++ tomCatDir: " + tomCatDir);
         try {
                 jxip = new JXip();
                 servletContext = getServletContext();
                 servletContext.setAttribute("jxip",jxip);
                 // dsiplay the xip version in catalina.out
                 System.out.println("\n------- " + jxip.GetVersion() + "-------\n");
         } catch (Exception ex) {
                 jxip = null;
                 System.out.println("jXip init: " + ex.toString());
                 ex.printStackTrace();
         }
}

private synchronized void initVar() throws Exception {
         //System.out.println("initVar()");
         servletContext = getServletContext();
         jxip = (JXip)servletContext.getAttribute("jxip");
         if (jxip == null) {
                 fullInit();
         }
         if (jxip == null) {
                 throw new Exception("<br><br><h1>STOP and ReSTART tomCat </h1><br><br> jxip == null");
         }
         inputStr = null;
         submitParam = null;
         defaultExample = null;
         outputStr = "";
         gHandler = -1;
}

private String englishDefaultInput = "";
private String frenchDefaultInput = "";
private String spanishDefaultInput = "";
private String portugueseDefaultInput = "";
private String italianDefaultInput = "";
private String germanDefaultInput = "";

private void initDefaultInput() {
         //English
         englishDefaultInput = "If our value per text is nominally estimated at one dollar, then we produce 2 ";
         englishDefaultInput += "million dollars per hour this year we will have to do four text ";
         englishDefaultInput += "files per month:  thus upping our productivity from one million. \n";
         englishDefaultInput += "The Goal of Project Gutenberg is to Give Away One Trillion Etext ";
         englishDefaultInput += "Files by the December 31, 2001. ";
         englishDefaultInput += "This is ten thousand titles each to one hundred million readers, ";
         englishDefaultInput += "which is 10% of the expected number of computer users by the end ";
         englishDefaultInput += "of the year 2001. \n";
         englishDefaultInput += "We need your donations more than ever!";
         englishDefaultInput += "All donations should be made to \"Project Gutenberg IBC\", and are ";
         englishDefaultInput += "tax deductible to the extent allowable by law (\"IBC\" is Illinois ";
         englishDefaultInput += "Benedictine College).  (Subscriptions to our paper newsletter go ";
         englishDefaultInput += "to IBC, too). \n";
         englishDefaultInput += "When all other email fails try our Michael S. Hart, Executive Director: \n";
         englishDefaultInput += "paul.hart@vmd.cso.uiuc.edu. \n";
         englishDefaultInput += "This \"Small Print!\" by Charles B. Kramer. \n";
         englishDefaultInput += "Internet (72600.2026@compuserve.com); TEL: (212-254-5093). \n";
         englishDefaultInput += "Czechoslovakia has been superseded by the Czech Republic and Slovakia. \n";
         englishDefaultInput += "United States Security Treaty. \n";
         englishDefaultInput += "African Development Bank. \n";
         englishDefaultInput += "August 1945, abrogated by Federal Constitution of 1949 and Provisional Constitution of 1950, restored 5 July 1959. \n";
         englishDefaultInput += "2 grams, 5 ounces, 69 milliliters, 30.48 centimeters,144 inches , 3 miles, 2 kilometers, 100 degrees. \n";
         englishDefaultInput += "Warsaw Pact, Antarctic Treaty Consultative Meeting. \n";
         englishDefaultInput += "Colombo Plan CSCE Conference, Pan Africanist Congress of Azania. \n";
         englishDefaultInput += "http://www.strummersite.com. \n";
         //French
         frenchDefaultInput = "L'usine, qui devrait être implantée à Eloyes (Vosges) représente un investissement ";
         frenchDefaultInput += "d'environ 3,7 milliards de yens (148 milliards de francs).\n"; 
         frenchDefaultInput += "Elle fabriquera, dans un premier temps, le produit liquide qui entre dans ";
         frenchDefaultInput += "le processus des photocopies ainsi que des pièces détachées pour la filiale ";
         frenchDefaultInput += "de Minolta en RFA.\n"; 
         frenchDefaultInput += "A partir de 1992, cette usine devrait se lancer dans la fabrication ";
         frenchDefaultInput += "de photocopieurs et d'imprimantes.\n"; 
         frenchDefaultInput += "Lors de son ouverture, elle emploiera une centaine de personnes.\n";
         frenchDefaultInput += "C'est en 1293 qu'il se rend pour la première fois à Paris.\n";
         frenchDefaultInput += "De décembre 1999 , l'hiver 1936, le 16-05-06, mercredi 11.\n";
         frenchDefaultInput += "Dans les années 60, Les années 80 et début 90, Au IIIe siècle ap. J-C, L'époque médiévale, L'âge du bronze.\n";
         frenchDefaultInput += "Aux troisième et deuxième millénaires avant notre ère, Au néolithique.\n";
         frenchDefaultInput += "Les émirats Arabes Unis, les pays d'Afrique subsaharienne, Corenc-le-haut.\n";
         frenchDefaultInput += "Tepanakert Allaer, chef-lieu de la région de Telinka, la région Centre, la vallée de l'Emblève.\n";
         frenchDefaultInput += "Un café de la rue Mazarine, à Paris, Le café Chez Mina.\n";
         frenchDefaultInput += "France Bigaud, le match France-Belgique, M. de Pourceaugnac, Véronique Pellerin de Radio France.\n";
         frenchDefaultInput += "Le ministre aux Affaires Etrangères angolais, Jooo Tyyy, Le maire de Grenoble George D.\n";
         frenchDefaultInput += "L'URL de wikipedia est: http://fr.wikipedia.org.\n";
         frenchDefaultInput += "Pour nous contacter: xxxx.yyy@wandoo.fr.\n";
         frenchDefaultInput += "\n";
         //Spanish
         spanishDefaultInput = "Cuando era adolescente, prefirió maravillarse con las matemáticas y con un primitivo ";
         spanishDefaultInput += "ordenador que jugar al béisbol o salir con chicas. De aquellos juegos con las sumas y ";
         spanishDefaultInput += "las restas y al cabo de los años, William Gates III, nacido en Seattle (Estados Unidos) ";
         spanishDefaultInput += "en 1955, se ha convertido en el rey Midas de los años 90. A los 16 años, creó su ";
         spanishDefaultInput += "primera empresa, aunque unos meses después la tuvo que cerrar porque sus clientes ";
         spanishDefaultInput += "descubrieron que la empresa estaba al mando de un mocoso adolescente. A los 19, ";
         spanishDefaultInput += "junto con su amigo Paul Allen, bautizó a la que hoy es la empresa informática más ";
         spanishDefaultInput += "boyante del planeta: Microsoft. Desde entonces \"el mago\" Bill no ha dado ni un ";
         spanishDefaultInput += "respiro a su cabeza -- su varita mágica particular --, aunque él jure y perjure que no es ";
         spanishDefaultInput += "ningún genio, tan sólo un empresario que intenta, y vaya si lo consigue, hacer dinero.";
         //Portuguese
         portugueseDefaultInput = "A câmara formalizou duas acções judiciais no Tribunal. ";
         portugueseDefaultInput += "Numa, a Anacom e as operadoras são intimadas a informar sobre as empresas que estão ";
         portugueseDefaultInput += "a cobrar a taxa e quais os valores em causa. ";
         portugueseDefaultInput += "Na outra, também dirigida à Anacom, discute-se o regulamento.";
         //Italian
         italianDefaultInput = "La Acqua Marcia può evitare il fallimento.\n";
         italianDefaultInput += "La finanziaria di Vincenzo Romagnoli è stata ammessa al concordato preventivo;"; 
         italianDefaultInput += " gli stessi giudici della sezione fallimentare del Tribunale di Roma, che martedì avevano rifiutato ";
         italianDefaultInput += "di concedere l'amministrazione controllata, hanno ritenuto possibile ieri un accordo con i creditori.\n";
         italianDefaultInput += "La Consob ha sospeso a tempo indeterminato i titoli ordinari e le azioni di risparmio.\n";
         italianDefaultInput += "I tre magistrati, Carlo Atzeni (presidente), Paolo Celotti e Emilio Norelli, hanno preso ";
         italianDefaultInput += "atto dell'esistenza in bilancio di beni patrimoniali superiori al totale delle esposizioni: l'attivo ";
         italianDefaultInput += " sarebbe pari a 600 miliardi, contro 460 miliardi di passivo. In base a questi dati, spiega la società, ";
         italianDefaultInput += " è possibile il pagamento dei creditori anche al di là dei minimi previsti dalla legge";
         italianDefaultInput += " (40% dei debiti chirografari e 100%  dei privilegiati).";
         //German
         germanDefaultInput = "Die Vereinigten Staaten senden sieben Abgeordnete nach Berlin.\n";
         germanDefaultInput += "Der Ausschuss von Schroeder tagt in Berlin.\n";
         germanDefaultInput += "Der neue Bundeskanzler beruft sein Kabinett.\n";
}
%>
<%-- end of declaration part --%>

<%-- build the page --%>
<%
request.setCharacterEncoding(encoding);
initVar();
initGramHandler();
initDefaultInput();
getParam(request);
getLanguageOption(request);
getOptionSelected(request);
%>

<form action="/xip/page1.jsp" method="POST">
    <table border="0">
        <tr>
            <h1>XIP evaluation </h1>
        </tr>
        
        <tr>
            This user interface is intended for evaluation purposes only.<br><br>
        </tr>
        <tr>
            <p>
                <b>Select the input language:</b>
                <select name="language" size=1>
                    <option <% if (englishSelected) {out.print(" selected ");} %> value="english"> English </option>
                    <option <% if (frenchSelected) {out.print(" selected ");} %> value="french"> French </option>
                    <option <% if (spanishSelected) {out.print(" selected ");} %> value="spanish"> Spanish </option>
                    <option <% if (italianSelected) {out.print(" selected ");} %> value="italian"> Italian </option>
                    <option <% if (portugueseSelected) {out.print(" selected ");} %> value="portuguese"> Portuguese </option>
                    <!-- LANG_LIMIT
             <option <% if (germanSelected) {out.print(" selected ");} %> value="german"> German </option>
-->
                </select>
            </p>
        </tr>
        
        <tr>
            <p><b>Select predefined example</b>	
                <% 
                out.print("<input type=\"submit\" name=\"defaultExample\" value=\"" + defaultExampleButtonStr + "\">");
                %>
            </p>
        </tr>
        
        <tr>
            <%
            out.print("<p><b> Or </b>");
             out.print("<input type=\"submit\" name=\"submitParam\" value=\"" + clearTextButtonStr + "\"> ");
             out.print("<b> and enter some sentences to parse</b> (size limited to " + sizeMax + " bytes):</p> ");
            %>
        </tr>
        
        <tr>
            <TEXTAREA COLS=70 ROWS=5 NAME="input" id="xipInput"><%
                 if (submitParam != null) {
                         if (submitParam.compareTo(clearTextButtonStr) == 0) {
                                 //the reset text button has been pushed
                                 inputStr = "";
                         }	
                 }
                 if (defaultExample != null) {
                         if (defaultExample.compareTo(defaultExampleButtonStr) == 0) {
                                 if (englishSelected ) {
                                         inputStr = englishDefaultInput;
                                 }
                                 if (frenchSelected ) {
                                         inputStr = frenchDefaultInput;
                                 }
                                 if (spanishSelected ) {
                                         inputStr = spanishDefaultInput;
                                 }
                                 if (portugueseSelected ) {
                                         inputStr = portugueseDefaultInput;
                                 }
                                 if (italianSelected ) {
                                         inputStr = italianDefaultInput;
                                 }
                                 if (germanSelected ) {
                                         inputStr = germanDefaultInput;
                                 }
                         }
                 }
                 if (inputStr != null) {
                         out.print(inputStr);
                 } else {
                         out.print("");
                 }
            %></TEXTAREA>
        </tr>
        
        <tr>
            <p>
            <br><b>Parsing options:</b><br>
            <input type="radio" name="option" value="option2" <% if (option2Selected) {out.print(" checked ");}%> > Reduce output
                   <input type="radio" name="option" value="option3" <% if (option3Selected) {out.print(" checked ");}%> > Full output
                   <input type="radio" name="option" value="option4" <% if (option4Selected) {out.print(" checked ");}%> > Tree
                   <input type="radio" name="option" value="option5" <% if (option5Selected) {out.print(" checked ");}%> > Large tree
                   <% 
                   if ((frenchSelected) || (englishSelected)){
                           out.print("<input type=\"radio\" name=\"option\" value=\"option6\"");
                           if (option6Selected) {
                                   out.print(" checked ");
                           }
                           out.print("> Entity extraction");
                   }
                   %>
                   <input type="radio" name="option" value="option7" <% if (option7Selected) {out.print(" checked ");}%> > XML output
                   </p>
        </tr>
        
        <tr>
            <b>Press to</b>	
            <% 
            out.print("<input type=\"submit\" name=\"submitParam\" value=\"" + startParsingButtonStr + "\">");
            %>
        </tr>   
    </table> 
</form>

<%
if (submitParam != null) {
     if (submitParam.compareTo(startParsingButtonStr) == 0) {
             //the start parsing button has been pushed
             //out.print("inputStr.length() = " + inputStr.length() + "<br><br>");
             //out.print("inputStr = " + inputStr + "<br><br>");
             //out.print("gHandler = " + gHandler + "<br><br>");
             if (inputStr.length() > sizeMax) {
                     out.print("<h3>Too large input data: the input size is limited (to " + sizeMax + " bytes) in this XIP evaluation version</h3>");
             } else {
                     int displayMode = 0;
                     char xmlFlag = 0;
                     if (option2Selected) {
                             displayMode =  32; //DISPLAY_reduce 32
                     }
                     if (option3Selected) {
                             displayMode = 64;  //DISPLAY_FULL 64
                     }
                     if (option4Selected) {
                             //tree
                             displayMode = 262144; // DISPLAY_TREE 262144
                     }
                     if (option5Selected) {
                             //large tree
                             displayMode = 1048576; // DISPLAY_TREE_COLUMN
                     }
                     if (option6Selected) {
                             //entity extraction
                             displayMode = 0; 
                     }
                     if (option7Selected) {
                             // xml output
                             displayMode = 64;  //DISPLAY_FULL 64
                             xmlFlag = 1; 
                     }
                     //out.print("displayMode = " + displayMode + "<br><br>");
                     if (inputStr.length() > 0) {
                             jxip.XipSetDisplayMode(gHandler, displayMode, colSize);
                             // set Const.DISPLAY_CHUNK_TREE
                             jxip.XipAddFlagDisplay(gHandler, 2147483648L , colSize);
                             //logInfo(gHandler, inputStr, xmlFlag, displayMode, request);
                             try {
                                     //Use this function call otherwise somme input char will be not well processed 
                                     // ex: "matemáticas" with spanish grammar
                                     outputStr = jxip.parseStringCharSet(gHandler, inputStr, encoding, xmlFlag, (char)1);
                             } catch (Exception ex) {
                                     logExcept(ex.toString());
                                     ex.printStackTrace();
                                     outputStr = "Internal Error!!!";
                             }
                             if (outputStr == null) {
                                     outputStr = "";
                             }
                             if (option6Selected) {
                                     //entity extraction
                                     String[] arrayStr =  outputStr.split("\n");
                                     outputStr = "";
                                     String []entities = null;
                                     if (englishSelected) {
                                             entities = english_entities;
                                     } else if (frenchSelected) {
                                             entities = french_entities;
                                     }
                                     for (int ind =0; ind < arrayStr.length; ind++) {
                                             String subStr = arrayStr[ind];
                                             //out.print("<br> -- " + subStr);
                                             if (isEntity(subStr, entities)) {
                                                     outputStr += subStr + "<br>";
                                             }
                                     }	
                             }
                             if (option3Selected || option7Selected)  {
                                     // required when option 3 or 7 is set
                                     outputStr = outputStr.replaceAll("<","&lt;");
                                     outputStr = outputStr.replaceAll(">","&gt;");
                             }
                             if (englishSelected ) {
                                     // the html page des1 or des2 have a jsp suffix because on the xerox external web site we use 
                                     // an old version of httpd which don't take in account in work2.propertie /xip/*
                                     out.print("<b>Documentation: </b><a href=\"/xip/des1.jsp\">Dependencies nomenclature</a> ");
                             } else if (frenchSelected ) {
                                     out.print("<b>Documentation: </b><a href=\"/xip/des2.jsp\">Dependencies nomenclature</a> ");
                             } else if (italianSelected ) {
                                     out.print("<b>Documentation: </b><a href=\"/xip/itaDoc.pdf\">Dependencies nomenclature</a> ");
                             } else if (spanishSelected ) {
                                     out.print("<b>Documentation: </b><a href=\"/xip/spaDoc.pdf\">Dependencies nomenclature</a> ");
                             } else if (portugueseSelected ) {
                             } else if (germanSelected ) {
                             }
                             out.print("<br><br><b>XIP output:</b><br>");
                             if (spanishSelected || portugueseSelected || germanSelected || italianSelected){
                                     out.print("<br><br>NOTE: this is a preliminary version of the grammar !<br><br>");
                             }
                             if (portugueseSelected) {
                                     out.print("<br>This grammar is developped in collaboration with L2F laboratory, INESC-ID in Lisbon (Portugal)<br>");
                             }
                             // id="xipOutput" will be used by httpUnit tests
			     // IMPORTANT: Don't remove the <pre> before <pre id=xipOutpute> : it's used by the xip WEB service
                             out.print("<pre><pre id=\"xipOutput\">" + outputStr + "</pre></pre>");
                     }
             }
     }
}
out.print("<br><br>");
%>

<%@ include file="footer.jspf" %>

