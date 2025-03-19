site web: http://ws.apache.org/axis2/index.html

-------- generer axis2.war -------------
cd webapp
ant create.war

-------- pour utilser le serveur axis standalone -----
copier les fichiers *.aar dans /opt/ParSem/tools/axis2/1.4/repository/services
startAxis

---------
http://choranche:8080/axis2/
login: admin  passwd: axis2


------------ script ~/scripts/startAxis:
#!/bin/sh
echo --------------------------
echo http://localhost:8080/axis2/services/
echo --------------------------
$AXIS2_HOME/bin/axis2server.sh 

------------ modification de  /opt/ParSem/tools/axis2/1.4/bin/axis2server.sh 
# Xms Specify the initial heap size (2600m is the max on linux, 1600m est le
# max sur windows + -server)
java -Xms2600m -classpath $AXIS2_CLASSPATH
org.apache.axis2.transport.SimpleAxis2Server \
-repo $AXIS2_HOME/repository -conf $AXIS2_HOME/conf/axis2.xml $*

--------- configuration ----------
 /opt/ParSem/tools/axis2/1.4/conf/axis2.xml

--------- log ---------
pour le server: /opt/ParSem/tools/axis2/1.4/log4j.properties
log4j.rootCategory=INFO, CONSOLE, LOGFILE
log4j.appender.LOGFILE.File=axis2Server.log

cp /opt/ParSem/tools/axis2/1.4/log4j.properties .../xip/dev/webService/xipWebService
log4j.appender.LOGFILE.File=axis2Client.log


================================== netbeans ================
http://choranche:4848/login.jsf
login: admin      passwd:adminadmin
-------- install axis dans netbeans -----------
cp /opt/ParSem/tools/axis2/1.4/dist/axis2.war /opt/ParSem/tools/glassfish-v2ur2/domains/domain1/autodeploy
--------- conf netbeans ------
tools->options-axis2:
/opt/ParSem/tools/glassfish-v2ur2/domains/domain1/autodeploy/axis2.war
--------- deployer ws ----
Quand on deploie le service en utilisant Axis Web services->deploy to server
=> Copying 1 file to
/opt/ParSem/tools/glassfish-v2ur2/domains/domain1/autodeploy/WEB-INF/services
mais cela ne rend pas le service disponible: il faut utiliser le service WEB
http://choranche:8080/axis2/axis2-admin/upload
ce qui a pour effet de copier le *.aar dans :
/opt/ParSem/tools/glassfish-v2ur2/domains/domain1/applications/j2ee-modules/axis2/WEB-INF/services
c'est ce repertoire qui contient les services actifs:
/opt/ParSem/tools/glassfish-v2ur2/domains/domain1/applications/j2ee-modules/axis2/WEB-INF/services
+ restart de glassfish
------ admin axis ----
http://choranche:8080/axis2/axis2-admin/login
---------------------------------

====================================== MDI =============================
La machine mdi.xrce.xerox.com est de nouveau en marche, son numero IP est le
217.109.185.152, prenable par VNC (password PcypasapT;)
le mote de passe VNC et le mot de passe du user "administrateur" sont
identiques : PcypasapT;

Pour se logger il cliquer sur l'icon en haut a gouche de la fenetre et cliquer sur "send CTRL ALT DEL" 
username: mdi passwd: dsmdi (ne marche plus???)

Avant de vous la redonner a l'exploitation, on va augmenter sa capacite'
memoire et appliquer certains patches de securite'

C'est une machine Windows Server 2003.

-------------------------------------------
scp build/dist/xipWebService.aar
mdi@217.109.185.152:c:/users/grondeau/axis2-1.4/repository/services
passwd: dsmdi
-------------------------------------------
il y a tomcat qui tourne : http://217.109.185.152:8080
==> modif config axis pour utiliser un autre port:
transportReceiver name="http"
        class="org.apache.axis2.transport.http.SimpleHTTPServer">
        <parameter name="port">8081</parameter>

----------------------------------
http://217.109.185.152:8081
----------------------------------
