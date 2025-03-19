# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# User specific aliases and functions
export PATH=$PATH:$HOME/bin

export PATH=.:/cacao/scripts:$PATH

export JAVA_HOME=/cacao/tools/jdk/1.6.0_13
export PATH=$JAVA_HOME/bin:$PATH

export CATALINA_HOME=/cacao/tools/tomcat/6.0.18
export PATH=$CATALINA_HOME/bin:$PATH

alias tom="cd $CATALINA_HOME"
alias cacao="cd /cacao"

