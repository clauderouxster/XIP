var IB=new Object;
IB.ColTexte="black";IB.ColFond="#FFFF00";IB.ColContour="navy";IB.NbPixel=1;
var maxDivInd = 5;
var divIds = ["nlpdiv1","nlpdiv2","nlpdiv3","nlpdiv4", "nlpdiv5", "nlpdiv6"];
var divVisited = [0,0,0,0,0,0];

var nlinkOtagPattern = "<A onClick=\"AffBulle('$ID',$DIVIND,event);return true;\"> <FONT COLOR=\"RED\"><I>";
var nlinkCtagPattern = "</I></FONT></A>";

var popupPattern = "<TABLE border=0 cellspacing=0 cellpadding="+IB.NbPixel+"><TR bgcolor='"+IB.ColContour+"'><TD ><TABLE border=0 cellpadding=2 cellspacing=0 bgcolor='"+IB.ColFond+"'><TR><TD ><DIV style='font-family:arial;color:"+IB.ColTexte+";font-size:12px;font-weight:normal'> $CONTENT <BR> <A onClick=\"HideBulle($DIVIND);return true\"><U>Close</U></A> </DIV></TD></TR></TABLE></TD></TR></TABLE>&nbsp;";

var nodeInfoTab = new Array();

function getParentId(id) {
    var nodeinfo = nodeInfoTab[id];
    var regex = /<NLPL id=([^\ \,\)\!\;\<\>\/]+)>/;
    nodeinfo.match(regex);
    return RegExp.$1;
}


var posX=0;posY=0;
var xOffset=0;yOffset=35;
function AffBulle(id, divind, event) {
    var mareg, listids, other_id, clean_id, i;
    var divid = divIds[divind];
    var next_divind = divind + 1;
    if (divind > maxDivInd) {
        return true;
    } else if (divind == 0) {
        yOffset = 35;
        HideBulle(0);
    } else {
        yOffset=10;
    }
    if (event.ctrlKey==1) {
       id = getParentId(id);
    }
    //alert(id);
    var nodeinfo = nodeInfoTab[id];
    if (!nodeinfo)
        return;
    mareg = /<NLPL[\ ]+id=[^\ \,\)\!\;\<\>\/]+>/g;
    var nlplinks = nodeinfo.match(mareg);
    for(i=0; nlplinks && (i<nlplinks.length); i++) {
        nlplinks[i].match(/id=([^\ \,\)\!\;\<\>\/]+)>/);
        nlinkOtag = nlinkOtagPattern.replace("$ID",RegExp.$1);
        nlinkOtag = nlinkOtag.replace(/\$DIVIND/g,next_divind);
        nodeinfo = nodeinfo.replace(nlplinks[i],nlinkOtag);
    }
    nodeinfo = nodeinfo.replace(/<\/NLPL>/g,nlinkCtagPattern);
    // alert(nodeinfo); return true;
    contenu= popupPattern.replace("$CONTENT",nodeinfo);
    contenu= contenu.replace("$DIVIND",divind);
    // alert(contenu);

    var finalPosX=posX-xOffset;
    if (finalPosX<0) finalPosX=0;
    if (document.layers) {
        document.layers[divid].document.write(contenu);
        document.layers[divid].document.close();
        document.layers[divid].top=posY+yOffset;
        document.layers[divid].left=finalPosX;
        document.layers[divid].visibility="show";
    }
    if (document.all) {
        document.all[divid].innerHTML=contenu;
        document.all[divid].style.top=posY+yOffset;
        document.all[divid].style.left=finalPosX;//f.x-xOffset;
        document.all[divid].style.visibility="visible";
    } else if (document.getElementById) {
        document.getElementById(divid).innerHTML=contenu;
        document.getElementById(divid).style.top=posY+yOffset;
        document.getElementById(divid).style.left=finalPosX;
        document.getElementById(divid).style.visibility="visible";
        //document.getElementById(divid).onClick="\"HideBulle();return true;\""
     }
}

function getMousePos(e) {
  if (document.all) {
  posX=event.x+document.body.scrollLeft; 
  posY=event.y+document.body.scrollTop;
  }
  else {
  posX=e.pageX; 
  posY=e.pageY; 
  }
}

function setDivVisited(divind) {
    divVisited[divind] = 1;
    var i;
    for(i=0; i < divind; i++) {
       divVisited[divind] = 0;
    }
    // document.getElementById(divIds[divind]).style.visibility="visible";

}

function HideBulle(divind) {
    //if (divind==0)
     //   alert("HideBulle: " + divind + ", Visited: " + divVisited[divind]);
    if (divind < divIds.length) {
        var leid = divIds[divind];
        if (document.layers) {document.layers[leid].visibility="hide";}
        if (document.all) {document.all[leid].style.visibility="hidden";}
        else if (document.getElementById){document.getElementById(leid).style.visibility="hidden";}
            window.status="";
        HideBulle(divind+1);
        divVisited[divind] = 0;
    }
}

function HideBulle2(divind) {
    alert("HideBulle2: " + divind + ", Visited: " + divVisited[divind]);
    if ( (divind < divIds.length) && (divVisited[divind]==1) && (divind>0) ) {
        HideBulle(divind);
    }
}


function InitBulle(divind) {
    var divid = divIds[divind];
	//if (document.layers) {
	//	window.captureEvents(Event.MOUSEMOVE);window.onMouseMove=getMousePos;
	//	document.write("<LAYER name='" + divid + "' top=0 left=0 visibility='hide'></LAYER>");
	//}
	//if (document.all) {
	//	document.write("<DIV id='" + divid + "' //style='position:absolute;top:0;left:0;visibility:hidden'></DIV>");
	//	document.onmousemove=getMousePos;
	//}
	//else
    if (document.getElementById) {
	        document.onmousemove=getMousePos;
	        document.write("<DIV id='" + divid + "' style='position:absolute;top:0;left:0;visibility:hidden'></DIV>");
	}

}


for (divind in divIds) {
    InitBulle(divind);
}




