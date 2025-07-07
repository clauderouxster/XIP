/* $Id: tk-commands.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* (c) 2001 by the Xerox Corporation.  All rights reserved */

/* These Tcl commands come from John Maxwell's xle-interpreter.tcl.
   This file is read in during the initialization of the Tcl interface
   to XLE. To make the application independent of the local file
   system, we define the Tcl commands in this header file. They
   will be fed as arguments to Tcl_Eval in the tk_define_commands()
   routine in tk-inspect.c.
*/

#define INSPECT "proc inspect {address {raw \"\"} {spawner \"\"}} {\n\
    # because this procedure may be called from the debugger\n\
    # xle-initialization-script\n\
    wm withdraw \".\"\n\
    if {$raw == 1} {\n\
	;# force the raw inspector to be used\n\
	set window [inspect-raw $address]\n\
    } else {switch [type $address]  {\n\
	AVPair {set window [show-avpair $address]}\n\
	NET {set window [inspect-network $address]}\n\
	STATE {set window [inspect-state $address]}\n\
	InternalSolution {set window [display-internal-solution $address]}\n\
	default {set window [inspect-raw $address]}\n\
    }}\n\
    return $window\n\
}"

#define INSPECT_NETWORK "proc inspect-network {address} {\n\
    set self [inspect-raw $address]\n\
    if {[get_field $address \"virtual\"] != \"T\"} {return $self}\n\
    set panel $self.panel\n\
    return $self\n\
}"

#define INSPECT_STATE "proc inspect-state {address} {\n\
    set self [inspect-raw $address]\n\
    if {[get_field $address \"virtual\"] != \"T\"} {return $self}\n\
    set panel $self.panel\n\
    button $panel.realize -text realize -command \"realize-state $address\"\n\
    pack $panel.realize -in $panel -anchor nw\n\
    return $self\n\
}"

#define REALIZE_STATE "proc realize-state {address} {\n\
    set self .${address}+\n\
    realize_state $address\n\
    recompute-inspector $self 1\n\
}"

#define INSPECT_RAW "proc inspect-raw {address} {\n\
    set fields [get_field $address *fields*]\n\
    if {$fields == \"\"} {return \"\"}\n\
    set self .${address}+\n\
    ;# add \"+\" to distinguish from non-raw inspector\n\
    if {[winfo exists $self]==0} {\n\
	toplevel $self -class Inspector\n\
	wm title $self [get-label $address]\n\
    } elseif {[winfo children $self] != \"\"} {\n\
	;# if the window has children it already exists\n\
	;# otherwise, it is being recomputed\n\
	focus-window $self\n\
	return $self\n\
    }\n\
    inspector-panel $self 1\n\
    foreach field $fields {\n\
	display-field $field [get_field $address $field] $self top\n\
    }\n\
    return $self\n\
}"

#define FOCUS_WINDOW "proc focus-window {window {noflash \"\"}} {\n\
#    global olwm\n\
    wm deiconify $window\n\
#    if {$olwm == 0} {raise $window}\n\
    if {$noflash == \"\"} {$window.panel.recompute flash}\n\
}"

#define INSPECTOR_PANEL "proc inspector-panel {parent {raw ""}} {\n\
    set self $parent.panel\n\
    frame $self\n\
    pack $self -in $parent -anchor nw\n\
    button $self.kill -text \"kill\" -command \"destroy $parent\"\n\
    bind $self.kill <Control-Button-1> \"kill-descendents $parent\"\n\
    bind $self.kill <Button-3> {puts \"destroys the window\"}\n\
    button $self.recompute -text \"recompute\" \
	-command \"recompute-inspector $parent $raw\"\n\
    bind $self.recompute <Button-3> \
      {puts \"refreshes the display to reflect a changed data structure\"}\n\
    set address [get-address $parent]\n\
    button $self.address -text \"address\" -command \"puts $address\"\n\
    bind $self.address <Button-3> \
      {puts \"prints out the address of the underlying data structure\"}\n\
    button $self.done -text \"done\" -command \"set debugging done\"\n\
    bind $self.done <Button-3> \
 {puts \"returns you to the debugger if you got to here from the debugger\"}\n\
    pack $self.kill $self.recompute $self.address $self.done \
	-in $self -anchor nw -side left\n\
    if {$raw == \"\"} {\n\
	set  address [get-address $parent]\n\
	button $self.raw -text \"raw\" -command \"inspect-raw $address\"\n\
	bind $self.raw <Button-3> \
   {puts \"gives you a non-specialized, standard inspector\
 on the data structure\"}\n\
	pack $self.raw -in $self -anchor nw -side left\n\
    }\n\
}"

#define RECOMPUTE_INSPECTOR "proc recompute-inspector {self {raw \"\"}} {\n\
    set address [get-address $self]\n\
    foreach w [winfo children $self] {destroy $w}\n\
    update idletasks\n\
    inspect $address $raw\n\
}"

#define DISPLAY_FIELD "proc display-field {name value self {side} {cutoff 2}} {\n\
    if {$value == \"\"} {return}\n\
    frame $self.a$name\n\
    pack $self.a$name -anchor w\n\
    label $self.a${name}label -text $name\n\
    frame $self.a${name}value\n\
    pack $self.a${name}label -in $self.a$name -side left\n\
    pack $self.a${name}value -in $self.a$name\n\
    set rest \"\"\n\
    if {[llength $value] > [expr {$cutoff + 1}]} {\n\
	set rest [lrange $value $cutoff end]\n\
	set value [lrange $value 0 [expr {$cutoff - 1}]]\n\
    }\n\
    set n 1\n\
    foreach item $value {\n\
	set label [get-label $item]\n\
	set button $self.a${name}value.$n\n\
	incr n\n\
	button $button -text $label\n\
	inspect-button $button $item\n\
	pack $button -anchor w -side $side\n\
    }\n\
    if {$rest !=\"\"} {\n\
	set button $self.a${name}value.rest\n\
	if {[winfo exists $button]} {return}\n\
	set length [llength $rest]\n\
	button $button -text \"...($length)\"\
	    -command \"display-rest ${self}${name} $name $rest\"\n\
	bind $button <Button-3> {puts \"displays the rest\"}\n\
	pack $button -anchor w -side $side\n\
    }\n\
}"

#define DISPLAY_REST "proc display-rest {self name args} {\n\
    set self \"${self}rest\"\n\
    if {[winfo exists $self]==0} {\n\
	toplevel $self -class Inspector\n\
	wm title $self $self\n\
    } elseif {[winfo children $self] != \"\"} {\n\
	;# if the window has children it already exists\n\
	;# otherwise, it is being recomputed\n\
	focus-window $self\n\
	return $self\n\
    }\n\
    set panel $self.panel\n\
    frame $panel\n\
    pack $panel -in $self -anchor nw\n\
    button $panel.kill -text \"kill\" -command \"destroy $self\"\n\
    bind $panel.kill <Button-3> {puts \"destroys the window\"}\n\
    button $panel.recompute -text \"recompute\"\n\
    bind $panel.recompute <Button-3> {puts \"does nothing\"}\n\
    button $panel.done -text \"done\" -command \"set debugging done\"\n\
    bind $panel.done <Button-3>\
 {puts \"returns you to the debugger if you got to here from the debugger\"}\n\
    pack $panel.kill $panel.recompute $panel.done \
	-in $panel -anchor nw -side left\n\
    display-field $name $args $self top 10\n\
}"

#define TYPE "proc type {address} {\n\
    string range $address \
	[expr {[string first ( $address]+1}] \
	     [expr {[string first ) $address]-1}]\n\
}"

#define GET_TOPLEVEL "proc get-toplevel {window} {\n\
# get the name of the top-level window\n\
    set tail [string range $window 1 end]\n\
    set top [string range $window 0 [string first . $tail]]\n\
    if {$top == \"\"} {set top [string range $window 0 end]}\n\
    return $top\n\
}"

#define GET_ADDRESS "proc get-address {window} {\n\
# get the address being inspected\n\
    set address [get-toplevel $window]\n\
    set last [expr {[string length $address] - 1}]\n\
    if {[string index $address $last] == \"+\"} {\n\
	set address [string range $address 1 [expr {$last-1}]]\n\
    } else {set address [string range $address 1 end]}\n\
    return $address\n\
}"

#define GET_LABEL "proc get-label {address {root \"\"}} {\n\
    set label \"\"\n\
    switch [type $address] {\n\
        AVPair {set label [get-avpair-label $address $root]}\n\
	Clause {\n\
	    set label [truncate [print-clause $address $root]]\n\
	}\n\
    }\n\
    if {$label == \"\"} {set label [get_field $address *label*]}\n\
    if {$label == \"\"} {set label $address}\n\
    return $label\n\
}"

#define TRUNCATE "proc truncate {string} {\n\
    if {[string length $string] > 30} {\n\
	set string [string range $string 0 26]\n\
	append string \" ...\"\n\
    } else {return $string}\n\
}"

#define INSPECT_BUTTON "proc inspect-button {button address} {\n\
    set top [get-toplevel $button]\n\
    bind $button <Button-1> \"inspect $address 0 $top\"\n\
    bind $button <M4-Button-1> \"inspect-and-destroy $address 0 $top\"\n\
    bind $button <Control-Button-1> \"inspect $address 1 $top\"\n\
    bind $button <M4-Control-Button-1>\
 \"inspect-and-destroy $address 1 $top\"\n\
    bind $button <Meta-Button-1> \"puts $address\"\n\
    bind $button <Button-3> {\n\
	puts \"\"\n\
	puts \"<Button-1> creates an inspector for this value\"\n\
	inspect-button-doc\n\
    }\n\
}"

#define INSPECT_BUTTON_DOC "proc inspect-button-doc {} {\n\
	puts \"<Control-Button-1>\
 creates a non-specialized (raw) inspector for this value\"\n\
	puts \"<Meta-Button-1> prints the address of the data structure\"\n\
}"

#define INSPECT_AND_DESTROY "proc inspect-and-destroy {address raw top} {\n\
    set window [inspect $address $raw $top]\n\
    if {$window != $top} {update idletasks; destroy $top}\n\
}"

#define BREAKPOINT "proc breakpoint {} {\n\
    set max [expr {[info level] -1}]\n\
    set current $max\n\
    show $current\n\
    while (1) {\n\
	puts -nonewline stderr \"#$current: \"\n\
	gets stdin line\n\
	while {![info complete $line]} {\n\
	    puts -nonewline stderr \"? \"\n\
	    append line \n[gets stdin]\n\
	}\n\
	switch -- $line {\n\
	    + {\n\
		if {$current < $max} {\n\
		    show [incr current]\n\
		}\n\
	    }\n\
	    - {\n\
		if {$current > 0 } {\n\
		    show [incr current -1]\n\
		}\n\
	    }\n\
	    C {puts stderr \"Resuming execution\";return}\n\
	    ? {show $current}\n\
	    default {\n\
		catch {uplevel #$current $line } result\n\
		puts stderr $result\n\
	    }\n\
	}\n\
    }\n\
}"

#define SHOW "proc show {current} {\n\
    if {$current > 0 } {\n\
	set info [info level $current]\n\
	set proc [lindex $info 0]\n\
	puts stderr \"$current: Procedure $proc \
		{[info args $proc]}\"\n\
	set index 0\n\
	foreach arg [info args $proc] {\n\
	    puts stderr \
		    \"\t$arg = [lindex $info [incr index]]\"\n\
	}\n\
    } else {\n\
	puts stderr \"Top level\"\n\
    }\n\
}"

#define KILL_ALL "proc kill-all {} {\n\
    foreach child [winfo children .] {\n\
	destroy $child\n\
    }\n\
    update idletasks\n\
}"

