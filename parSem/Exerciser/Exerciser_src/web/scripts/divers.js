MOUSE_X =0;
MOUSE_Y =0;
DELTA_X = 10;
DELTA_Y = 20;

function ns(e) {
	x=e.x; y=e.y;
        MOUSE_X=e.pageX;
        MOUSE_Y=e.pageY;
}

function ie() {
	var f=window.event;

        MOUSE_X=f.x;
        MOUSE_Y=f.y;

  

	
}


function initMouseEvent() {
	
        if (navigator.appName.substring(0,3) == "Net") {
 	    document.captureEvents(Event.MOUSEMOVE); document.onmousemove = ns;
	} else {

            document.onmousemove=ie;

	}
}



function error(id,vi,pos) {

    var layer = document.getElementById(id);
    if (layer != null) {
        layer.style.position=pos;
        layer.style.visibility=vi;
        layer.style.left=MOUSE_X + DELTA_X;
        layer.style.top=MOUSE_Y + DELTA_Y;
    } else {
  
    }
    

 }




/* hide or expand a div html bloc 
   id : id for the div 
 
*/
function expand(id,vi,position) {
   
    var layer = document.getElementById(id);
    if (layer != null) {
        layer.style.position=position;
        layer.style.visibility=vi;
    } else {
  
    }
}

function check(id, id_) {

     var c  = document.getElementById(id);
     var c2 = document.getElementById(id_);



     var checked = c.checked;



     if ( checked ) {
         c2.style.color = "#FFFFFF";           
         c2.style.background = "#008040";
         c2.style.textDecoration ="none";
  
    } else {
      
       c2.style.color="#008040";  
       c2.style.textDecoration ="underline";
       c2.style.background = "transparent";     
  
    } 

    
    
    
}


 function openPopup(page,title){
          window.open(page,title,"menubar=no, status=no, scrollbars=yes, width=800, height=600");
     }



