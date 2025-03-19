
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
